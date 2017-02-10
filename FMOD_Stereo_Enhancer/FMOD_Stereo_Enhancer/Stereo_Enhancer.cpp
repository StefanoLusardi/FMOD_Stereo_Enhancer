#include "Stereo_Enhancer.hpp"

StereoEnhancer::StereoEnhancer()
{
	m_width = STEREO_WIDTH_DEFAULT;
	reset();
}

void StereoEnhancer::read(float *inbuffer, float *outbuffer, unsigned int length, int channels)
{
	// Note: buffers are interleaved
	//float gain = m_currentWidth;

	//if (m_interpolationSamplesLeft)
	//{
	//	float target = m_targetWidth;
	//	float delta = (target - gain) / m_interpolationSamplesLeft;
	//	while (length)
	//	{
	//		if (--m_interpolationSamplesLeft)
	//		{
	//			gain += delta;
	//			for (int i = 0; i < channels; ++i)
	//			{
	//				*outbuffer++ = *inbuffer++ * gain;
	//			}
	//		}
	//		else
	//		{
	//			gain = target;
	//			break;
	//		}
	//		--length;
	//	}
	//}

	//unsigned int samples = length * channels;
	//while (samples--)
	//{
	//	*outbuffer++ = *inbuffer++ * gain;
	//}

	//m_currentWidth = gain;


	float m, s;
	unsigned int stereoLength = (length * channels) >> 1;

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
	{
		tmp = 1.0f / (1.0f + m_width);
	}
	else
	{
		tmp = 1.0f / 2.0f;
	}

	diffGain = m_width * tmp;
	sumGain = tmp;
}

float StereoEnhancer::getWidth()
{
	return m_width;
}
