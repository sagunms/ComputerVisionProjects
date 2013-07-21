#include "Tracker.h"
#include <iostream>

cTracker::cTracker() : labels(NULL) { }

cTracker::~cTracker() {
	if (labels) delete [] labels;
}

cv::Mat cTracker::extractBlobs(cv::Mat &mat) {

	// mat.cols, mat.rows -- allocate vectors
	if (mat.cols != width || mat.rows != height) {

		width = mat.cols; height = mat.rows;

		if (labels) 
			delete [] labels;

		labels = new node*[width*height];
	}

	// reset our data structure for reuse
	ds.Reset();
	int index;

	// generate equivalence sets -- connected component labeling (4-connected)
	labels[0] = ds.MakeSet(0);

	for (int j = 1; j < mat.cols; j++)
		labels[j] = mat.data[j] != mat.data[j-1] ? ds.MakeSet(0) : labels[j-1];

	for (int j = mat.cols; j < mat.rows*mat.cols; j++) {

		if (mat.data[j] == mat.data[j-1]) {
			labels[j] = labels[j-1];

			if (mat.data[j-1] == mat.data[j-mat.cols]) 
				ds.Union(labels[j-1], labels[j-mat.cols]);
		}

		else if (mat.data[j] == mat.data[j-mat.cols]) 
			labels[j] = labels[j-mat.cols];

		else labels[j] = ds.MakeSet(0);
	}

	cv::Mat imgOut(mat.size(), CV_8UC3);

	int numBlobs = ds.Reduce();

	std::vector<cv::Vec3b> colors;
	colors.push_back(cv::Vec3b(0, 0, 0));	//background

	for(int lbl = 1; lbl < numBlobs; ++lbl )
		colors.push_back( cv::Vec3b( (rand()&255), (rand()&255), (rand()&255) ) );

	for (int j = 0; j < mat.rows; j++) {
		for (int i = 0; i < mat.cols; i++) {
			index = ds.Find(labels[j*mat.cols+i])->i;

			cv::Vec3b &pix = imgOut.at<cv::Vec3b>(j, i);
			pix = colors[index];
		}
	}

	return imgOut;
}