#include "Stereo_Enhancer.hpp"

StereoEnhancer::StereoEnhancer()
{
	mWidth = STEREO_WIDTH_DEFAULT;
	Reset();
}

void StereoEnhancer::ApplyDsp(float *inbuffer, float *outbuffer, unsigned int length, int channels)
{
	float mid, side;

	while (length--)
	{
		float left = *inbuffer++;
		float right = *inbuffer++;

		mid = sumGainCurrent  * (left + right);
		side = diffGainCurrent * (right - left);

		*outbuffer++ = mid - side; // Left output
		*outbuffer++ = mid + side; // Right output
	}
}

//void StereoEnhancer::GetSamples(float *inbuffer, float *outbuffer, unsigned int length, int channels)
//{
//	for (unsigned int sample = 0; sample < length; sample++)
//	{
//		for (unsigned int ch = 0; ch < (unsigned int)channels; ch++)
//		{
//			*outbuffer++ = ApplyDsp(*inbuffer++, ch);
//		}
//	}
//}

void StereoEnhancer::process(float *inbuffer, float *outbuffer, unsigned int length, int channels)
{
	//* PARAMETERS SMOOTHING */
 //	while (length && (sumGainCurrent != sumGainTarget || diffGainCurrent != diffGainTarget))
	//{
	//		/* Fixed increment smoothing */
	//		 sumGain interpolation
	//		if (sumGainCurrent < sumGainTarget)
	//		{
	//			sumGainCurrent += sumGainDelta;
	//			if (sumGainCurrent >= sumGainTarget)
	//				sumGainCurrent = sumGainTarget;
	//		}
	//		else
	//		{
	//			sumGainCurrent -= sumGainDelta;
	//			if (sumGainCurrent <= sumGainTarget)
	//				sumGainCurrent = sumGainTarget;
	//		}
	//		// diffGain interpolation
	//		if (diffGainCurrent < diffGainTarget)
	//		{
	//			diffGainCurrent += diffGainDelta;
	//			if (diffGainCurrent >= diffGainTarget)
	//				diffGainCurrent = diffGainTarget;
	//		}
	//		else
	//		{
	//			diffGainCurrent -= diffGainDelta;
	//			if (diffGainCurrent <= diffGainTarget)
	//				diffGainCurrent = diffGainTarget;
	//		}

	if (mInterpolationSamples)
	{
		// Parameters increment
 		float sumGainDelta = (sumGainTarget - sumGainCurrent) / mInterpolationSamples;
		float diffGainDelta = (diffGainTarget - diffGainCurrent) / mInterpolationSamples;

		while (length)
		{
			if (--mInterpolationSamples)
			{
				sumGainCurrent += sumGainDelta;
				diffGainCurrent += diffGainDelta;

				// ApplyDsp on 1 sample
				ApplyDsp(inbuffer, outbuffer, 1, channels);
			}
			else
			{
				sumGainCurrent = sumGainTarget;
				diffGainCurrent = diffGainTarget;
				break;
			}
			inbuffer += channels;
			outbuffer += channels;
			--length;
		}
	}

	ApplyDsp(inbuffer, outbuffer, length, channels);

	sumGainCurrent = sumGainTarget;
	diffGainCurrent = diffGainTarget;
}

void StereoEnhancer::DontProcess(float *inbuffer, float *outbuffer, unsigned int length, int channels)
{
	memcpy(outbuffer, inbuffer, sizeof(float)*length*channels);
}

void StereoEnhancer::Reset()
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

void StereoEnhancer::setBypass(float value)
{
	mBypass = value;
}

float StereoEnhancer::getBypass()
{
	return mBypass;
}
