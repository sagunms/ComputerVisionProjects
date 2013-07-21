#Automatic License Plate Localization (ANPR) System Using Mexican Hat Operators and Euler Number Criteria#

This program uses a robust algorithm for automatic license plate localization given input image with complex scenes. The algorithm consists of five stages: 

1. Grayscaling
2. Edge Detection with smoothing
3. Thresholding
4. Image Segmentation
5. Morphological Operations

The algorithm uses Laplacian of Gaussian (Mexican hat wavelet) operator for edge detection, 4-neighbourhood region growing for image segmentation and Euler number of a binary image as the criteria for identifying the final license plate region. The algorithm was tested against a small database of 12 images containing the back end of cars taken by an industrial surveillance camera. The complete system was coded and compiled using Microsoft Visual C++ 2010 using OpenCV 2.4.2 library. The average processing time was found to be 2.3385 seconds for each test image and the system showed an accuracy of 11 correct localization out of 12 test images (91.667%) for the license plate localization. Some practical considerations like existence of another text block in an image, presence of dirt or shadow on or near license plate region, license plate with rows of characters and sensitivity to license plate dimensions have been addressed.

#Methodology#

##Grayscaling##
The original image that is represented in 8-bit 3-channel RGB has to be converted into 1-channel Grayscale. OpenCV library provides cvtColor function which converts image from one color space to another. The function converts the input image from one color space to another. In the case of transformation to-from RGB color space the ordering of the channels should be specified explicitly (RGB or BGR).

##Edge Detection using Laplacian of Gaussian (Mexican Hat Wavelet)##
Since the Laplacian of Gaussian Operation is the combination of Gaussian smoothing with Laplacian second order derivative to find the edges, the benefits of both are included by Mexican Hat Operation. 

The equation of Laplacian of the Gaussian which is also called the hat or Mexican-hat operator is:

```
∇^2 G(x,y)= -[((x^2+y^2 )-σ^2)/σ^4 ] e^(-[((x^2+y^2))/(2σ)^2 ] )
```

where, ```∇^2 G ``` is a circularly symmetric operator and σ is the standard deviation.

The following 9x9 discrete approximations to LoG function is used in the implementation:

```
0    0	0	-1	-1	-1	0	0	0
0	-1	-1	-3	-3	-3	-1	-1	0
0	-1	-3	-3	-1	-3	-3	-1	0
-1	-3	-3	6	13	6	-3	-3	-1
-1	-3	-1	13	24	13	-1	-3	-1
-1	-3	-3	6	13	6	-3	-3	-1
0	-1	-3	-3	-1	-3	-3	-1	0
0	-1	-1	-3	-3	-3	-1	-1	0
0	0	0	-1	-1	-1	0	0	0
```

##Thresholding##
For reducing complexity in the algorithm and filtering out unwanted noise in the image, thresholding is used for binarization of the image.

##Morphological Operations##
Morphological opening is the dilation of the erosion of an image. The opening filter eliminates several of the small blobs introduced by image noise. In a morphological operation, the value of each pixel in the output image is based on a comparison of the corresponding pixel in the input image with its neighbors. By choosing the size and shape of the neighborhood, you can construct a morphological operation that is sensitive to specific shapes in the input image. A simple 3x3 square structuring kernel was used for both erosion and dilation morphological operations.

If the license plate image is blurred, edge detection output can have discontinuity which can result in failure cases or incorrect detection output. To avoid this and make these edges thick and continuous, dilation operation is performed. 

##Region Growing and License Plate Localization##
Grouping is needed to create blocks that could serve to create a way to identify the ROI from the rest of the image. In the algorithm, the entire image is split into different regions using region growing algorithm. Also only those regions are finally kept which have at least 'X' number of pixels. This helps to filter out unwanted regions and speed up the process of finding license plate.

Region growing is a region-based image segmentation method. It is also classified as a pixel-based image segmentation method since it involves the selection of initial seed points. This approach to segmentation examines neighboring pixels of initial “seed points” and determines whether the pixel neighbors should be added to the region. The process is iterated on, in the same manner as general data clustering algorithms. 

In this algorithm, 4-connected neighborhood method is used to grow from the seed points. Here, we keep examining the adjacent pixels of seed points. If they have the same intensity value with the seed points, we classify them into the seed points. It is an iterated process until there are no changes in two successive iterative stages. 

For a binary image, Euler Number is defined as the total number of objects in the image minus the number of holes in those objects. The Euler Number is calculated as: ```E = C – H```

After segmenting the entire image in smaller regions, this criteria can be used to identify license plate region from it. The combined criteria i.e. Euler number and license plate dimensions is used to overcome false positives. Here, we specify a range of acceptable minimum and maximum license plate sizes to reduce the sensitivity due to license plate dimensions.

The algorithm scans the binary image of each pixel, when it finds a pixel that is not labeled, push the pixel into the stack and label its neighborhood, until the stack is empty. The steps are:

* Choose the seed pixel
* Check the neighboring pixels (four direct neighbours) and add them to the region if they are similar to the seed; 
* Repeat step 2 for each of the newly added pixels; stop if no more pixels can be added.

Outcomes of Region-Based Segmentation are as follows:

* the segmentation must be complete; that is, every pixel must be in a region.
* that points in a region must be connected in some predefined sense.
* the regions must be disjoint.
* deals with the properties that must be satisfied by the pixels in a segmented region. For example P(Ri) = True if all pixels in Ri have the same gray level.
* indicates that region Ri and Rj are different in the sense of predicate.

##Increase speed of algorithm execution for real-time applications##
In order to increase the algorithm speed, sub-sampling was used, where algorithm operation was performed for every two pixels, instead of performing for each and every pixel in the image.

The upper and lower limits of ROI width and heights were also predetermined in the algorithm code and predefined constraints were included in the algorithm to avoid processing in the high resolution image.

#Processing Time#

```
Grayscaling	(10.33ms)
Mexican Hat Wavelet	(614.917ms)
Thresholding (7.75ms)
Morphological Dilation of Erosion (6.417ms)
4-Neighbourhood Region Growing and Euler Number Criteria (1686.25ms)
Final License Plat Localization	(12.833ms)
----------------------
Total Time	(2338.5ms)
```

The above observation gives the tentative processing times of all the different stages of the localization algorithm. It can be seen that the most computationally intensive stage is the 4-neighbourhood region growing algorithm which is followed by the Mexican Hat wavelet operation. This software was compiled and executed in an Intel Core2Duo 2 GHz CPU and the total execution time was 2.3385 seconds for localization of each input image. More code optimization could be done to improve the speed by using pointer arithmetic instead of high level array read-write procedures. However, considering the accuracy of the system, this system looks quite promising even with the speed tradeoff. 

#Software Used#
* Visual Studio 2010 x86
* OpenCV 2.4.2
* Language: C++

#References#

1.	Dhawal Wazalwar, Erdal Oruklu, Jafar Saniie. A Design Flow for Robust License Plate Localization and Recognition in Complex Scenes, Department of Electrical and Computer Engineering, Illinois Institute of Technology, Chicago, USA 2011
2.	Edge Detection, 3D Computer Vision and Computing Zhigang Zhi, City College of New York, USA.
3.	2004 Robert Fisher, Simon Perkins, Ashley Walker, Erik Wolfart, Department of Informatics, The University of Edinburgh (http://homepages.inf.ed.ac.uk/rbf/HIPR2/log.htm).
4.	OpenCV Computer Vision Application Programming Cookbook
5.	Xiaozhu lin, Junwei Ji, Yingying Gu, The Euler Number Study of Image and Its Application, School of Information Engineering, Beining Institute of Petrochemical Technology, Beijing 102617, China
6.	Computer Vision" by Linda Shapiro, George Stockman
7.	Feng Zhang and Shunyong Zhou, Wenling Xie, Sichuan University of Science & Engineering, Zigong, Sichuan Province, China, 2010 Second Pacific-Asia Conference on Circuits, Communications and System (PACCS)
[8]	A Line Labeling and region Growing Based Algorithm for Binary Image Connected Component Labeling