#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../../FMOD_Lib/fmod.hpp"

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
	STEREO_ENHANCER_BYPASS_PARAM,
	STEREO_ENHANCER_NUM_PARAMETERS
};

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_Create(FMOD_DSP_STATE *dsp_state);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_Release(FMOD_DSP_STATE *dsp_state);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_Reset(FMOD_DSP_STATE *dsp_state);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_Process(FMOD_DSP_STATE *dsp_state, unsigned int length, const FMOD_DSP_BUFFER_ARRAY *inbufferarray, FMOD_DSP_BUFFER_ARRAY *outbufferarray, FMOD_BOOL inputsidle, FMOD_DSP_PROCESS_OPERATION op);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_SetParamFloat(FMOD_DSP_STATE *dsp_state, int index, float value);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_GetParamFloat(FMOD_DSP_STATE *dsp_state, int index, float *value, char *valuestr);
//FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_SetParamBool(FMOD_DSP_STATE *dsp_state, int index, FMOD_BOOL value);
//FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_GetParamBool(FMOD_DSP_STATE *dsp_state, int index, FMOD_BOOL *value, char *valuestr);
//FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_ShouldIProcess(FMOD_DSP_STATE *dsp_state, FMOD_BOOL inputsidle, unsigned int length, FMOD_CHANNELMASK inmask, int inchannels, FMOD_SPEAKERMODE speakermode);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_SysRegister(FMOD_DSP_STATE *dsp_state);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_SysDeregister(FMOD_DSP_STATE *dsp_state);
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_SysMix(FMOD_DSP_STATE *dsp_state, int stage);

static bool FMOD_Stereo_Enhancer_Running = false;
static FMOD_DSP_PARAMETER_DESC p_width;
static FMOD_DSP_PARAMETER_DESC p_bypass;

FMOD_DSP_PARAMETER_DESC *FMOD_Stereo_Enhancer_dspparam[STEREO_ENHANCER_NUM_PARAMETERS] =
{
	&p_width,
	&p_bypass
};

FMOD_DSP_DESCRIPTION FMOD_Stereo_Enhancer_Desc =
{
	FMOD_PLUGIN_SDK_VERSION,
	"sL Stereo Enhancer", 
	0x00020000, 
	1,      
	1,      
	FMOD_Stereo_Enhancer_Create,
	FMOD_Stereo_Enhancer_Release,
	FMOD_Stereo_Enhancer_Reset,
	0,   
	FMOD_Stereo_Enhancer_Process,
	0, 
	STEREO_ENHANCER_NUM_PARAMETERS,
	FMOD_Stereo_Enhancer_dspparam,
	FMOD_Stereo_Enhancer_SetParamFloat,
	0, 
	0, //FMOD_Stereo_Enhancer_SetParamBool,
	0, 
	FMOD_Stereo_Enhancer_GetParamFloat,
	0, 
	0, //FMOD_Stereo_Enhancer_GetParamBool,
	0, 
	0,
	0,
	FMOD_Stereo_Enhancer_SysRegister,
	FMOD_Stereo_Enhancer_SysDeregister,
	FMOD_Stereo_Enhancer_SysMix
};

extern "C"
{
	F_EXPORT FMOD_DSP_DESCRIPTION* F_CALL FMODGetDSPDescription()
	{
		FMOD_DSP_INIT_PARAMDESC_FLOAT(
			p_width, 
			"Stereo Width", 
			"", 
			"Default = 1. Mono = 0", 
			STEREO_WIDTH_MIN, 
			STEREO_WIDTH_MAX, 
			STEREO_WIDTH_DEFAULT, 
			false);

		FMOD_DSP_INIT_PARAMDESC_FLOAT(
			p_bypass, 
			"Bypass",
			"",
			"Bypass the audio processing.",
			0,
			1, 
			0,
			true
			);
		return &FMOD_Stereo_Enhancer_Desc;
	}
}

class StereoEnhancer
{
public:
	StereoEnhancer();
	void  ApplyDsp(float * inbuffer, float * outbuffer, unsigned int length, int channels);
	void  process(float *inbuffer, float *outbuffer, unsigned int length, int channels);
	void  DontProcess(float * inbuffer, float * outbuffer, unsigned int length, int channels);
	void  Reset();
	void  setWidth(float width);
	float getWidth();
	void  setBypass(float value);
	float getBypass();

private:
	float mWidth;
	float sumGainTarget;
	float sumGainCurrent;
	float diffGainTarget;
	float diffGainCurrent;
	int   mInterpolationSamples;
	bool  mBypass;
};

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_Create(FMOD_DSP_STATE *dsp_state)
{
	dsp_state->plugindata = (StereoEnhancer *)FMOD_DSP_STATE_MEMALLOC(dsp_state, sizeof(StereoEnhancer), FMOD_MEMORY_NORMAL, "StereoEnhancer");
	if (!dsp_state->plugindata)
	{
		return FMOD_ERR_MEMORY;
	}
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_Release(FMOD_DSP_STATE *dsp_state)
{
	StereoEnhancer *state = (StereoEnhancer *)dsp_state->plugindata;
	FMOD_DSP_STATE_MEMFREE(dsp_state, state, FMOD_MEMORY_NORMAL, "StereoEnhancer");
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_Process(FMOD_DSP_STATE *dsp_state, unsigned int length, const FMOD_DSP_BUFFER_ARRAY *inbufferarray, FMOD_DSP_BUFFER_ARRAY *outbufferarray, FMOD_BOOL /*inputsidle*/, FMOD_DSP_PROCESS_OPERATION op)
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
		// Check if bypass is active before processing audio data.
		if (state->getBypass())
		{
			state->DontProcess(inbufferarray[0].buffers[0], outbufferarray[0].buffers[0], length, inbufferarray[0].buffernumchannels[0]);
			return FMOD_OK;
		}
		else
		{
			state->process(inbufferarray[0].buffers[0], outbufferarray[0].buffers[0], length, inbufferarray[0].buffernumchannels[0]); // input and output channels count match for this effect
			return FMOD_OK;
		}
	}
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_Reset(FMOD_DSP_STATE *dsp_state)
{
	StereoEnhancer *state = (StereoEnhancer *)dsp_state->plugindata;
	state->Reset();
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_SetParamFloat(FMOD_DSP_STATE *dsp_state, int index, float value)
{
	StereoEnhancer *state = (StereoEnhancer *)dsp_state->plugindata;

	switch (index)
	{
	case STEREO_ENHANCER_STEREO_WIDTH_PARAM:
		state->setWidth(value);
		return FMOD_OK;

	case STEREO_ENHANCER_BYPASS_PARAM:
		state->setBypass(value);
		return FMOD_OK;
	}
	return FMOD_ERR_INVALID_PARAM;
}

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_GetParamFloat(FMOD_DSP_STATE *dsp_state, int index, float *value, char *valuestr)
{
	StereoEnhancer *state = (StereoEnhancer *)dsp_state->plugindata;

	switch (index)
	{
	case STEREO_ENHANCER_STEREO_WIDTH_PARAM:
		*value = state->getWidth();
		if (valuestr)
			sprintf(valuestr, "%.1f", state->getWidth());
		return FMOD_OK;

	case STEREO_ENHANCER_BYPASS_PARAM:
		*value = state->getBypass();
		if (valuestr)
			sprintf(valuestr, "%.1f", state->getBypass());
		return FMOD_OK;
	}
	return FMOD_ERR_INVALID_PARAM;
}

//FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_SetParamBool(FMOD_DSP_STATE *dsp_state, int index, FMOD_BOOL value)
//{
//	StereoEnhancer *state = (StereoEnhancer *)dsp_state->plugindata;
//
//	switch (index)
//	{
//	case STEREO_ENHANCER_BYPASS_PARAM:
//		state->setBypass(value);
//		return FMOD_OK;
//	}
//	return FMOD_ERR_INVALID_PARAM;
//}
//
//FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_GetParamBool(FMOD_DSP_STATE *dsp_state, int index, FMOD_BOOL *value, char *valuestr)
//{
//	StereoEnhancer *state = (StereoEnhancer *)dsp_state->plugindata;
//
//	switch (index)
//	{
//	case STEREO_ENHANCER_BYPASS_PARAM:
//		*value = state->getBypass();
//		if (valuestr)
//			sprintf(valuestr, "%.1f", state->getBypass());
//		return FMOD_OK;
//	}
//	return FMOD_ERR_INVALID_PARAM;
//}
//
//FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_ShouldIProcess(FMOD_DSP_STATE *dsp_state, FMOD_BOOL inputsidle, unsigned int length, FMOD_CHANNELMASK inmask, int inchannels, FMOD_SPEAKERMODE speakermode)
//{
//	/* This function gets called only before a Read_Callback and not before a Process_Callback */
//  	StereoEnhancer *state = (StereoEnhancer *)dsp_state->plugindata;
//	if (inputsidle || state->getBypass())
//		return FMOD_ERR_DSP_DONTPROCESS;
//
//	return FMOD_OK;
//} 
 
FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_SysRegister(FMOD_DSP_STATE * /*dsp_state*/)
{
	FMOD_Stereo_Enhancer_Running = true;
	// called once for this type of dsp being loaded or registered (it is not per instance)
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_SysDeregister(FMOD_DSP_STATE * /*dsp_state*/)
{
	FMOD_Stereo_Enhancer_Running = false;
	// called once for this type of dsp being unloaded or de-registered (it is not per instance)
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMOD_Stereo_Enhancer_SysMix(FMOD_DSP_STATE * /*dsp_state*/, int /*stage*/)
{
	// stage == 0 , before all dsps are processed/mixed, this callback is called once for this type.
	// stage == 1 , after all dsps are processed/mixed, this callback is called once for this type.
	return FMOD_OK;
}

