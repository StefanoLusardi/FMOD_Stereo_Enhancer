#include "Stereo_Enhancer.hpp"

StereoEnhancer::StereoEnhancer()
{
	m_width = STEREO_WIDTH_DEFAULT;
	reset();
}

void StereoEnhancer::read(float *inbuffer, float *outbuffer, unsigned int length, int channels)
{
	float m, s;
	unsigned int stereoLength = (length * channels) >> 1;

	// TODO: apply smoothing changes to sumGain and diffGain

	while (stereoLength--)
	{
		float left  = *inbuffer++;
		float right = *inbuffer++;
		
		m = sumGain  * (left  + right);
		s = diffGain * (right - left);
		
		*outbuffer++ = m - s;
		*outbuffer++ = m + s;
	}
}

void StereoEnhancer::reset()
{
	m_currentWidth = m_targetWidth;
	m_interpolationSamplesLeft = 0;
}

void StereoEnhancer::setWidth(float width)
{
	m_width = width;
	float tmp;

	if (1.0f + width > 2.0f)
		tmp = 1.0f / (1.0f + m_width);
	else
		tmp = 1.0f / 2.0f;

	diffGain = m_width * tmp;
	sumGain = tmp;
}

float StereoEnhancer::getWidth()
{
	return m_width;
}
