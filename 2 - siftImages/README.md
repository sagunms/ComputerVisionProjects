#Objective#

Detection of keypoints from an image and comparasion of two images based on SIFT (Scale Invariant Feature Transform) descriptors

#Introduction#

Recent research has revealed that keypoint-based descriptors are very effective to characterize both individual objects and entire images. Such descriptors are widely used in object detection and image comparison. This program detects and displays keypoints from an image and compares two images based on SIFT descriptors. The program takes one or multiple image files. 

#Methodology#

##Single image file input##

When a single image file is supplied to the program:

* Rescale the image to a size comparable to VGA size (480(rows) x 600(columns)) to reduce the computation. Note that the aspect ratio of the image should be kept when the image is rescaled.
* Extract SIFT keypoints from the Y component of the image
* For each detected keypoint, draw a cross “+” at the location of the key point and a circle around the keypoint whose radius is proportional to the scale of the keypoint.
* Both the original image and the image with highlighted keypoints should be displayed in a window as follows:
    Original Image (rescaled) | Image with highlighed keypoints
* In the command window, the program outputs the number of detected keypoints.

##Multiple image files input##

When multiple image files, say A.jpg, B.jpg, C.jpg, D.jpg and E.jpg, are supplied to the program, the program will compare each pair of the images using a Bag-of-Words model constructed from SIFT descriptors. The program will:

* Rescale all images to sizes comparable to VGA size (480x600) to reduce the computation. Note that the aspect ratio of the images should be kept when the image is rescaled, but there is no need to rescale all images to the same size.
* Extract SIFT keypoints and descriptors from the Y-components of all scaled images. 
* Cluster the SIFT descriptors from ALL images into K-clusters using K-means algorithm. The values of K should be specified as a percentage of the total number of keppoints. Each cluster represents a visual word.
* For each image, construct a histogram of the occurrence of the visual words. This should be
done by classifying each SIFT descriptor of the image into one of the K-clusters (words) and
continuing how many time each word occurred in the image.
* For each pair of images, calculate the c2 distance between the histograms of the images. This c2 distance is a measurement of the dissimilarity of the pair of images.
* The program should output the following information
a. Number of keypoints for each image and the total number of keypoints of all images
b. Dissimilarity matrices for K=5%, 10% and 20% of the total number of keypoints from all images. Note: please arrange the dissimilarity matrices in a readable format.

#Discussion#

In the main function, when argc == 2, we know that there is only one addition argument given while executing siftImages.exe i.e. input image. Similarly, when argc > 2, we know that there is multiple images.

To rescale the image to size comparable to 480(rows)x600(cols), preserving aspect ratio. For this, resizeImagePreservingAspectRatio() method is available which takes source and destination images and the new desired size. The trick used is to take two ratios: ```(width of desired size) / (no. of columns of original image)``` and ```(height of desired size) / (no. of rows of original image)```.
If first ratio is smaller than second then (corrected width) = ((new width)*first ratio + 0.5 and similar for (corrected height). Else, multiply by second ratio.

To extract SIFT key points from Y component (luminance) of the image, extractLuminance() method was used. This uses cvtColor() with CV_BGR2YCrCb mode to convert RGB to YUV color space outputted to imgYUV matrix. The split() method was used to isolate the single Y component into imgY for SIFT processing.

For the single input image, SIFT class was used with sift() constructor where the luminance matrix imgY was inputted and the Scale Invariant Feature Transform (SIFT) keypoints were extracted and its descriptors were computed. The keypoints is a data structure for salient point detectors found in the image. OpenCV’s feature detection library consists of everything required to implement all the stages of SIFT algorithm i.e. construction of scale spaces (with 4 octaves and 5 blur levels by factor of sqrt(2) as per D. Lowe’s implementation); Difference of Gaussian images which approximate the scale-invariant Laplacian of Gaussian to find key points by locating maxima and minima of DoG images and subpixels. For each extracted keypoint, which is effectively scale invariant, rotation invariance is also applied to the keypoints which is possible by collecting gradient directions and magnitudes around all pixels of each keypoint. Doing so, a histogram of 36 bins (10 degrees of orientation each) is created and a feature vector of 128 numbers is outputted by the algorithm.

The method implementSIFT_Multi() takes multiple images and the percentOfKeypoints is used to vary the cluster size or the number of words of the dictionary.

For the Opens Bag of Visual Words implementation, the tasks can be divided into three steps: 

* SIFT FeatureDectector to identify the key points
* SIFT DescriptorExtractor to extract features from the key-points found by the FeatureExtractor
* FlannBased DescriptorMatcher to match those features to the features in the dictionary to construct the BOW representation of the image. The “FlannBased” matcher uses the flann (fast library for approximate nearest neighbors) library under the hood to perform faster but approximate matching.

K-means++ algorithm was used for clustering as it uses a heuristic for choosing good initial cluster centers which makes it a better choice than the standard k-means algorithm.

#Software Used#

* Visual Studio 2010 x86
* OpenCV 2.4.2
* Language: C++
