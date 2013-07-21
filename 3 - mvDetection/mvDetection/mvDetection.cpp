//=================================================
// Project: mvDetection (Assignment III)
// Coded by: Sagun Man SINGH SHRESTHA

// References: 
//		Robert Laganiere, “OpenCV 2 Computer Vision: Application Programming Cookbook”, Chapter 5, 10
//		Gary Bradski and Adrian Kaehler, “Learning OpenCV”, Chapter 9
//		http://docs.opencv.org | http://opencv.willowgarage.com/documentation
//		http://en.wikipedia.org/wiki/Connected-component_labeling
//		http://www.aishack.in/2010/03/connected-component-labelling/
//=================================================

// Includes
#include <opencv2/video/background_segm.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "VideoProcessor.h"
#include "Segmentor.h"

// Namespaces
using namespace cv;
using namespace std;

// Main Function
int main(int argc, char* argv[]) {

	// Create video procesor instance
	VideoProcessor processor;

	// Create background/foreground segmentor 
	Segmentor segmentor;
	segmentor.setThreshold(25);

	// Open video file
	processor.setInput("carpark_f6_compressed.avi");

	// set frame processor
	processor.setFrameProcessor(&segmentor);

	// Play the video at the original frame rate
	processor.setDelay(1000./processor.getFrameRate());

	// Start the process
	processor.run();

	cv::waitKey();
}
