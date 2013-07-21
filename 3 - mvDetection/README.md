#Objective#
Design a C++ program that extracts moving objects from video using OpenCV 2.4.2

#Methodology#
Extraction of moving objects from a sequence of images or video is often used in many video analysis tasks such as intelligent video surveillance systems. This program aims to detect, separate and label all moving objects from a given sequence of images or video which captured by a stationary camera.

Three key steps involved in the extraction of moving objects from video captured by a stationary camera:

1. Detecting moving pixels using background modelling and subtraction,
2. Removing noisy detection using morphological operators or majority voting and
3. Labelling separate moving objects using connected component analysis.

OpenCV 2.4.2 provides various algorithms for each of the steps 1 & 2. However, a custom connected component analysis algorithm was implemented. 

The program displays the original video frames, detected moving pixels after the background modeling and subtraction, the moving pixels after morphological operations or majority voting and the detected moving objects in a single window as illustrated below. The detected object is color-coded, i.e. objects (i.e. connected components) are displayed in different colors.

```
    	   Origin Video Frame | Detected Moving Pixels
------------------------------|-------------------------------
Moving Pixels after Filtering | Detected Objects (color-coded)
```

#Discussion#
In order to process a given video sequence (.avi file), the program mvDetection has to be able to first read each of its frames by the help of Opens framework to perform the frame extraction of the given video. Cv::VideoCapture class has been used to read the frames of the video sequence. For a more object oriented approach to make the program extensible, the approach described in the book “OpenCV2 Computer Vision Application Programming Cookbook” has been applied. Here, the Opens video capture framework is encapsulated in VideoProcessor class and the Segmentor class is used which holds a processing function called processFrame (a callback function so that will be called at each frame of a video sequence. This function holds two optional methods for Background Modeling and Subtraction i.e. Moving Average as well as Mixture of Gaussian Method. Moving Average method of Background Subtraction and Modeling has been used in the code by default.

In Line 475, The Filtering of Detected Moving Pixels using Morphological Operators  using cv::dilate and cv::erode methods are applied where they are applied for 2 and 7 iterations respectively just due to better formation of blobs.

After this, the Connected Component Labeling algorithm is called via cTracker::extractBlobs() method. 4-neighbourhood method for connected component labeling has been used with the help of UnionFind algorithm based on the description in Wikipedia and other sources.

In the end, the display of the Video Image Processes are shown via the function displayFourProcesses.

#Software Used#
* Visual Studio 2010 x86
* OpenCV 2.4.2
* Language: C++
