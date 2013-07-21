#if !defined BGFGSeg
#define BGFGSeg

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "VideoProcessor.h"

class Segmentor : public FrameProcessor {
	
	cv::Mat gray;			// current gray-level image
	cv::Mat background;		// accumulated background
	cv::Mat backImage;		// background image
	cv::Mat foreground;		// foreground image
	double learningRate;    // learning rate in background accumulation
	int threshold;			// threshold for foreground extraction

	// The Mixture of Gaussian object used with all default parameters
	cv::BackgroundSubtractorMOG mog;

  public:

	Segmentor() : threshold(10), learningRate(0.01) {}

	// Set the threshold used to declare a foreground
	void setThreshold(int t) {

		threshold = t;
	}

	// Set the learning rate
	void setLearningRate(double r) {

		learningRate = r;
	}

	// processing method
	void process(cv::Mat &frame, cv::Mat &output) {

		// ======== TASK: BACKGROUND MODELLING AND SUBTRACTION FOR DISPLAYING MOVING OBJECTS ========

		method1_MovingAverage(frame, output);			//Method 1: Moving Average Method for Background Modelling
		//method2_MixtureOfGaussian(frame, output);		//Method 2: Mixture of Gaussian (MOG) Method for Background Modelling
	}

	// === METHOD 1: Moving Average Method for Background Modelling ===
	void method1_MovingAverage(cv::Mat &frame, cv::Mat &output) {

		// convert to gray-level image
		cv::cvtColor(frame, gray, CV_BGR2GRAY); 

		// initialize background to 1st frame
		if (background.empty())
			gray.convertTo(background, CV_32F);
		
		// convert background to 8U
		background.convertTo(backImage, CV_8U);

		// compute difference between current image and background
		cv::absdiff(backImage, gray, foreground);

		// apply threshold to foreground image
		cv::threshold(foreground, output, threshold, 255, cv::THRESH_BINARY_INV);

		// accumulate background
		cv::accumulateWeighted(gray, background, learningRate, output);
	}

	// === METHOD 2: Mixture of Gaussian (MOG) Method for Background Modelling ===
	void method2_MixtureOfGaussian(cv::Mat &frame, cv::Mat &output) {

		// The Mixture of Gaussian object used with all default parameters
		mog(frame, foreground, 0.01);

		Mat src;
		// Complement the image
		cv::threshold(foreground, output, 128, 255, cv::THRESH_BINARY_INV);
	}
};

#endif
