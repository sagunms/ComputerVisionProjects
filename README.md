#Computer Vision Projects#

This is a collection of computer vision projects implemented using C++ and OpenCV 2.4.2

##Color Chain##

A C++ program that implements a color image processing chain converting raw image data captured by a CMOS image sensor into true color RGB images. The chain consists of 2 components: Color Interpolation and Color Correction.

##SIFT Images##

Detection of keypoints from an image and comparasion of two images based on SIFT (Scale Invariant Feature Transform) descriptors.

Recent research has revealed that keypoint-based descriptors are very effective to characterize both individual objects and entire images. Such descriptors are widely used in object detection and image comparison. This program detects and displays keypoints from an image and compares two images based on SIFT descriptors. The program takes one or multiple image files. 

##Real-time Moving Object Detection##

Extraction of moving objects from a sequence of images or video is often used in many video analysis tasks such as intelligent video surveillance systems. This program aims to detect, separate and label all moving objects from a given sequence of images or video which captured by a stationary camera.

Three key steps involved in the extraction of moving objects from video captured by a stationary camera:

1. Detecting moving pixels using background modelling and subtraction,
2. Removing noisy detection using morphological operators or majority voting and
3. Labelling separate moving objects using connected component analysis.

##Automatic License Plate Localization (ANPR) System##

This program uses Mexican Hat Operators and Euler Number Criteria for automatic license plate localization given input image with complex scenes. The algorithm consists of five stages: 

1. Grayscaling
2. Edge Detection with smoothing
3. Thresholding
4. Image Segmentation
5. Morphological Operations

#Software Used#
* Visual Studio 2010 x86
* OpenCV 2.4.2
* Language: C++
