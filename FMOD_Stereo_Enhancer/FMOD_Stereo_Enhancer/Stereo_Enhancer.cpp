#include "Stereo_Enhancer.hpp"

StereoEnhancer::StereoEnhancer()
{
	mWidth = STEREO_WIDTH_DEFAULT;
	reset();
}

void StereoEnhancer::ApplyDsp(float *inbuffer, float *outbuffer, unsigned int length, int channels)
{
	float mid, side;
	unsigned int stereoLength = (length * channels) >> 1;

	while (stereoLength--)
	{
		float left = *inbuffer++;
		float right = *inbuffer++;

		mid = sumGainCurrent  * (left + right);
		side = diffGainCurrent * (right - left);

		*outbuffer++ = mid - side;
		*outbuffer++ = mid + side;
	}
}

void StereoEnhancer::process(float *inbuffer, float *outbuffer, unsigned int length, int channels)
{
	/* PARAMETERS SMOOTHING */
	while (length && (sumGainCurrent != sumGainTarget || diffGainCurrent != diffGainTarget))
	{
		// Parameters increment
		float sumGainDelta  = 0.1f;
		float diffGainDelta = 0.1f;

		// sumGain interpolation
		if (sumGainCurrent < sumGainTarget)
		{
			sumGainCurrent += sumGainDelta;
			if (sumGainCurrent >= sumGainTarget)
				sumGainCurrent = sumGainTarget;
		}
		else
		{
			sumGainCurrent -= sumGainDelta;
			if (sumGainCurrent <= sumGainTarget)
				sumGainCurrent = sumGainTarget;
		}

		// diffGain interpolation
		if (diffGainCurrent < diffGainTarget)
		{
			diffGainCurrent += diffGainDelta;
			if (diffGainCurrent >= diffGainTarget)
				diffGainCurrent = diffGainTarget;
		}
		else
		{
			diffGainCurrent -= diffGainDelta;
			if (diffGainCurrent <= diffGainTarget)
				diffGainCurrent = diffGainTarget;
		}

		// ApplyDsp on 2 samples
		ApplyDsp(inbuffer, outbuffer, 2, channels);
		inbuffer++;
		outbuffer++;
		length--;

	}

	ApplyDsp(inbuffer, outbuffer, length, channels);
}

void StereoEnhancer::reset()
{
	sumGainCurrent  = sumGainTarget;
	diffGainCurrent = diffGainTarget;
	mInterpolationSamples = 0;
	setWidth(mWidth);
}

void StereoEnhancer::setWidth(float width)
{
	mWidth = width;
	float tmp;

	if (width > 1.0f)
		tmp = 1.0f / (1.0f + mWidth); // Stereo
	else
		tmp = 1.0f / 2.0f; // Mono

	diffGainTarget = mWidth * tmp;
	sumGainTarget = tmp;

	mInterpolationSamples = INTERPOLATION_SAMPLES;
}

float StereoEnhancer::getWidth()
{
	return mWidth;
}
