#ifndef TRACKER_H
#define TRACKER_H

#include <opencv2/opencv.hpp>

#include "UnionFind.h"
#include <vector>

class cTracker {

  private:
	node **labels; 
	unsigned int width, height;
	cUnionFind ds;		//Disjoint Sets for Connected Component Labelling

  public:
	cTracker();
	~cTracker();

	cv::Mat extractBlobs(cv::Mat &mat);
};

#endif