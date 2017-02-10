#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../../FMOD_PLUGINS_LIB/fmod.hpp"

extern "C" {
	F_EXPORT FMOD_DSP_DESCRIPTION* F_CALL FMODGetDSPDescription();
}

const float STEREO_WIDTH_MAX = 4.0f;
const float STEREO_WIDTH_MIN = 0.0f;
const float STEREO_WIDTH_DEFAULT = 1.0f;
#define INTERPOLATION_SAMPLES 256

enum
{
	STEREO_ENHANCER_STEREO_WIDTH_PARAM = 0,
	STEREO_ENHANCER_NUM_PARAMETERS
};

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspcreate(FMOD_DSP_STATE *dsp_state);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dsprelease(FMOD_DSP_STATE *dsp_state);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspreset(FMOD_DSP_STATE *dsp_state);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspprocess(FMOD_DSP_STATE *dsp_state, unsigned int length, const FMOD_DSP_BUFFER_ARRAY *inbufferarray, FMOD_DSP_BUFFER_ARRAY *outbufferarray, FMOD_BOOL inputsidle, FMOD_DSP_PROCESS_OPERATION op);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspsetparamfloat(FMOD_DSP_STATE *dsp_state, int index, float value);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspsetparamint(FMOD_DSP_STATE *dsp_state, int index, int value);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspsetparambool(FMOD_DSP_STATE *dsp_state, int index, FMOD_BOOL value);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspsetparamdata(FMOD_DSP_STATE *dsp_state, int index, void *data, unsigned int length);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspgetparamfloat(FMOD_DSP_STATE *dsp_state, int index, float *value, char *valuestr);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspgetparamint(FMOD_DSP_STATE *dsp_state, int index, int *value, char *valuestr);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspgetparambool(FMOD_DSP_STATE *dsp_state, int index, FMOD_BOOL *value, char *valuestr);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspgetparamdata(FMOD_DSP_STATE *dsp_state, int index, void **value, unsigned int *length, char *valuestr);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_shouldiprocess(FMOD_DSP_STATE *dsp_state, FMOD_BOOL inputsidle, unsigned int length, FMOD_CHANNELMASK inmask, int inchannels, FMOD_SPEAKERMODE speakermode);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_sys_register(FMOD_DSP_STATE *dsp_state);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_sys_deregister(FMOD_DSP_STATE *dsp_state);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_sys_mix(FMOD_DSP_STATE *dsp_state, int stage);

static bool FMOD_Stereo_Enhancer_Running = false;
static FMOD_DSP_PARAMETER_DESC p_width;

FMOD_DSP_PARAMETER_DESC *FMOD_Stereo_Enhancer_dspparam[STEREO_ENHANCER_NUM_PARAMETERS] =
{
	&p_width
};

FMOD_DSP_DESCRIPTION FMOD_Stereo_Enhancer_Desc =
{
	FMOD_PLUGIN_SDK_VERSION,
	"sL Stereo Enhancer", // name
	0x0001, // plug-in version
	1,      // number of input buffers to process
	1,      // number of output buffers to process
	FMOD_Stereo_Enhancer_dspcreate,
	FMOD_Stereo_Enhancer_dsprelease,
	FMOD_Stereo_Enhancer_dspreset,
	0, // FMOD_x_dspread   
	FMOD_Stereo_Enhancer_dspprocess,
	0, // WTF is this!?
	STEREO_ENHANCER_NUM_PARAMETERS,
	FMOD_Stereo_Enhancer_dspparam,
	FMOD_Stereo_Enhancer_dspsetparamfloat,
	0, // FMOD_x_dspsetparamint,
	0, // FMOD_x_dspsetparambool,
	0, // FMOD_x_dspsetparamdata,
	FMOD_Stereo_Enhancer_dspgetparamfloat,
	0, // FMOD_x_dspgetparamint,
	0, // FMOD_x_dspgetparambool,
	0, // FMOD_x_dspgetparamdata,
	FMOD_Stereo_Enhancer_shouldiprocess,
	0, // userdata
	FMOD_Stereo_Enhancer_sys_register,
	FMOD_Stereo_Enhancer_sys_deregister,
	FMOD_Stereo_Enhancer_sys_mix
};

extern "C"
{
	F_EXPORT FMOD_DSP_DESCRIPTION* F_CALL FMODGetDSPDescription()
	{
		FMOD_DSP_INIT_PARAMDESC_FLOAT(p_width, "Stereo Width", "", "Default = 1. Mono = 0", STEREO_WIDTH_MIN, STEREO_WIDTH_MAX, STEREO_WIDTH_DEFAULT);
		return &FMOD_Stereo_Enhancer_Desc;
	}
}

class StereoEnhancer
{
public:
	StereoEnhancer();

	void  read(float *inbuffer, float *outbuffer, unsigned int length, int channels);
	void  reset();
	void  setWidth(float width);
	float getWidth();

private:
	float m_width;
	float m_targetWidth;
	float m_currentWidth;
	float sumGain;
	float diffGain;
	int   m_interpolationSamplesLeft;
};

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspcreate(FMOD_DSP_STATE *dsp_state)
{
	dsp_state->plugindata = (StereoEnhancer *)FMOD_DSP_STATE_MEMALLOC(dsp_state, sizeof(StereoEnhancer), FMOD_MEMORY_NORMAL, "StereoEnhancer");
	if (!dsp_state->plugindata)
	{
		return FMOD_ERR_MEMORY;
	}
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dsprelease(FMOD_DSP_STATE *dsp_state)
{
	StereoEnhancer *state = (StereoEnhancer *)dsp_state->plugindata;
	FMOD_DSP_STATE_MEMFREE(dsp_state, state, FMOD_MEMORY_NORMAL, "StereoEnhancer");
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspprocess(FMOD_DSP_STATE *dsp_state, unsigned int length, const FMOD_DSP_BUFFER_ARRAY *inbufferarray, FMOD_DSP_BUFFER_ARRAY *outbufferarray, FMOD_BOOL /*inputsidle*/, FMOD_DSP_PROCESS_OPERATION op)
{
	StereoEnhancer *state = (StereoEnhancer *)dsp_state->plugindata;

	if (op == FMOD_DSP_PROCESS_QUERY)
	{
		if (outbufferarray && inbufferarray)
		{
			outbufferarray[0].bufferchannelmask[0] = inbufferarray[0].bufferchannelmask[0];
			outbufferarray[0].buffernumchannels[0] = inbufferarray[0].buffernumchannels[0];
			outbufferarray[0].speakermode = inbufferarray[0].speakermode;
		}
	}
	else
	{
		state->read(inbufferarray[0].buffers[0], outbufferarray[0].buffers[0], length, inbufferarray[0].buffernumchannels[0]); // input and output channels count match for this effect
	}

	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspreset(FMOD_DSP_STATE *dsp_state)
{
	StereoEnhancer *state = (StereoEnhancer *)dsp_state->plugindata;
	state->reset();
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspsetparamfloat(FMOD_DSP_STATE *dsp_state, int index, float value)
{
	StereoEnhancer *state = (StereoEnhancer *)dsp_state->plugindata;

	switch (index)
	{
	case STEREO_ENHANCER_STEREO_WIDTH_PARAM:
		state->setWidth(value);
		return FMOD_OK;
	}

	return FMOD_ERR_INVALID_PARAM;
}

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_dspgetparamfloat(FMOD_DSP_STATE *dsp_state, int index, float *value, char *valuestr)
{
	StereoEnhancer *state = (StereoEnhancer *)dsp_state->plugindata;

	switch (index)
	{
	case STEREO_ENHANCER_STEREO_WIDTH_PARAM:
		*value = state->getWidth();
		if (valuestr)
		{
			sprintf(valuestr, "%.1f", state->getWidth());
		}
		return FMOD_OK;
	}

	return FMOD_ERR_INVALID_PARAM;
}

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_shouldiprocess(FMOD_DSP_STATE * /*dsp_state*/, FMOD_BOOL inputsidle, unsigned int /*length*/, FMOD_CHANNELMASK /*inmask*/, int /*inchannels*/, FMOD_SPEAKERMODE /*speakermode*/)
{
	if (inputsidle)
	{
		return FMOD_ERR_DSP_DONTPROCESS;
	}

	return FMOD_OK;
}


FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_sys_register(FMOD_DSP_STATE * /*dsp_state*/)
{
	FMOD_Stereo_Enhancer_Running = true;
	// called once for this type of dsp being loaded or registered (it is not per instance)
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_sys_deregister(FMOD_DSP_STATE * /*dsp_state*/)
{
	FMOD_Stereo_Enhancer_Running = false;
	// called once for this type of dsp being unloaded or de-registered (it is not per instance)
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_sys_mix(FMOD_DSP_STATE * /*dsp_state*/, int /*stage*/)
{
	// stage == 0 , before all dsps are processed/mixed, this callback is called once for this type.
	// stage == 1 , after all dsps are processed/mixed, this callback is called once for this type.
	return FMOD_OK;
}

