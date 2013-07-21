//=================================================
// Project: siftImages
// Coded by: Sagun Man SINGH SHRESTHA
//=================================================

// Includes
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/nonfree/nonfree.hpp>
#include<iostream>

// Namespaces
using namespace cv;
using namespace std;

// Initializing variables
Mat convert_bayerGRBG_to_RGB(Mat imgInput);
Mat colorCorrection(Mat imgInput);
Mat gammaCorrection(Mat imgInput);
string asciiSymbol = "A";

// --- Miscellaneous Helper Functions ---
void resizeImagePreservingAspectRatio(Mat&, Mat&, Size);
void displayTwoImages(Mat&, Mat&, bool);
void drawCustomKeyPoints(vector<KeyPoint> &, Mat&, Scalar&, unsigned int);
void extractLuminance(Mat&, Mat&, Mat&);

// -- SIFT and BOW global functions and variables --
void implementSIFT_Single(Mat&, Mat&, Mat&, bool);
void implementSIFT_Multi(vector<Mat>&, int);

void extractVocabulary(vector<Mat>&, vector<Mat>&, int&);
void extractBOWDescriptor(vector<Mat>&, Mat&);

// OpenCV BOW implemention
Ptr<FeatureDetector> detector = FeatureDetector::create("SIFT");			//Task 1: SIFT FeatureDectector to identify the key points
Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create("SIFT");	//Task 2: SIFT DescriptorExtractor to extract features from the key-points found by the FeatureExtractor
Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");	//Task 3: FlannBased DescriptorMatcher to match those features to the features in the dictionary to construct the BOW representation of the image. The “FlannBased” matcher uses the flann (fast library for approximate nearest neighbors) library under the hood to perform faster but approximate matching.

// Variables for BOW implemention
int dictionarySize;								//value of K for K-means clustering
TermCriteria tc(CV_TERMCRIT_ITER, 10, 0.001);	//Termination Criteria for K-Means clustering algorithm
int retries = 1;
int flags = KMEANS_PP_CENTERS;					//K-means++ algorithm uses a heuristic for choosing good initial cluster centers which makes it a better choice than the standard k-means algorithm.

BOWImgDescriptorExtractor bowDE(extractor, matcher);	//BOWImgDescriptorExtractor class ties DescriptorExtractor and DescriptorMatcher together for BOW implementation


//==================================================================
// --- MAIN FUNCTION ---
//==================================================================
int main(int argc, char* argv[]) {

	// --- INITIALIZATION ---
	Mat imgOriginal, imgResized, imgAllY, imgY, imgSiftKeypoints;

	// --- SINGLE INPUT IMAGE ---
	if(argc == 2) {

		imgOriginal = imread(argv[1], CV_LOAD_IMAGE_UNCHANGED);	//Read input image file as is
		if(!imgOriginal.data) {
			cout << "Could not open or find the image" << endl;
			return -1;
		}
		// --- RESIZE IMAGE PRESERVING ASPECT RATIO ---
		resizeImagePreservingAspectRatio(imgOriginal, imgResized, Size(600, 480));
		cout << "Original Image = " << imgOriginal.rows << "x" << imgOriginal.cols << "," << imgOriginal.channels() << "|" << imgOriginal.type();
		cout << " ---> ";
		cout << "Resized Image = " << imgResized.rows << "x" << imgResized.cols << "," << imgResized.channels() << "|" << imgResized.type() << endl;

		// --- RESIZE IMAGE PRESERVING ASPECT RATIO ---
		extractLuminance(imgResized, imgAllY, imgY);

		// --- IMPLEMENT SIFT ALGORITHM FOR SINGLE INPUT IMAGE ---
		implementSIFT_Single(imgResized, imgY, imgAllY, false);

	}

	// --- MULTIPLE INPUT IMAGES ---
	else if(argc > 2) {

		vector<Mat> imgOriginalMulti(argc), imgResizedMulti(argc), imgAllYMulti(argc), imgYMulti(argc), imgSiftKeypointsMulti(argc);

		for(int i = 1; i < argc; i++) {	//Iterate over all input images

			imgOriginalMulti[i] = imread(argv[i], CV_LOAD_IMAGE_UNCHANGED);	//Read input image file as is
			if(!imgOriginalMulti[i].data) {
				cout << "Could not open or find the image" << endl;
				return -1;
			}

			// --- RESIZE IMAGE PRESERVING ASPECT RATIO ---
			resizeImagePreservingAspectRatio(imgOriginalMulti[i], imgResizedMulti[i], Size(600, 480));
			cout << "Original Image = " << imgOriginalMulti[i].rows << "x" << imgOriginalMulti[i].cols << "," << imgOriginalMulti[i].channels() << "|" << imgOriginalMulti[i].type();
			cout << " ---> ";
			cout << "Resized Image = " << imgResizedMulti[i].rows << "x" << imgResizedMulti[i].cols << "," << imgResizedMulti[i].channels() << "|" << imgResizedMulti[i].type() << endl;

			// --- RESIZE IMAGE PRESERVING ASPECT RATIO ---
			extractLuminance(imgResizedMulti[i], imgAllYMulti[i], imgYMulti[i]);
		}

		// --- IMPLEMENT SIFT, BOW AND CHI-SQUARE ALGORITHMS FOR MULTIPLE INPUT IMAGES ---
		implementSIFT_Multi(imgYMulti, 5);	//5% of Total number of Keypoints
		implementSIFT_Multi(imgYMulti, 10);	//10% of Total number of Keypoints
		implementSIFT_Multi(imgYMulti, 20);	//20% of Total number of Keypoints

	}
	else {
		cout << "Error! Usage: siftImages [InputImage1] [InputImage2] ... [InputImageN]" << endl;
		return -1;
	}
	
	waitKey(0);
	return 0;
}


//=====================================================================
// --- SIFT, BAG OF VISUAL WORDS and CHI-SQUARED DISSIMILARITY TEST ---
//=====================================================================

// --- IMPLEMENTING SIFT ---
void implementSIFT_Single(Mat& imgResized, Mat& imgY, Mat& imgSiftKeypoints, bool isMultiInputImages = false) {

	SIFT sift;
    vector<KeyPoint> key_points;

	Mat descriptors;
    sift(imgY, Mat(), key_points, descriptors);
	
	drawCustomKeyPoints(key_points, imgSiftKeypoints, Scalar(0, 255, 255), 3);
	cout << "No. of detected keypoints = " << key_points.size() << endl;
	displayTwoImages(imgResized, imgSiftKeypoints, isMultiInputImages);
}

void implementSIFT_Multi(vector<Mat>& images, int percentOfKeypoints) {

	vector<Mat> matFeatures(images.size());
	int totalKeypoints = 0;

	cout << "Creating dictionary..." << endl;
	extractVocabulary(images, matFeatures, totalKeypoints);

	dictionarySize = (int)(percentOfKeypoints * totalKeypoints / 100);	//Re-adjust dictionary size
	BOWKMeansTrainer bowTrainer(dictionarySize, tc, retries, flags);

	for(int i = 1; i < matFeatures.size(); i++) {
		bowTrainer.add(matFeatures[i]);
	}

	vector<Mat> descriptors = bowTrainer.getDescriptors();
	int count = 0;
	for(vector<Mat>::iterator iter = descriptors.begin(); iter != descriptors.end(); iter++) {
		count += iter->rows;
	}
	
	cout << endl << "Clustering " << count << " features..." << endl;

	Mat dictionary = bowTrainer.cluster();	//Returns trained vocabulary (i.e. cluster centers)
	bowDE.setVocabulary(dictionary);		//Each row of the vocabulary i.e. "dictionary" is a visual word (cluster center)

	cout << "Rows and cols of setVocabulary: " << dictionary.rows << ", " << dictionary.cols << endl << endl; //dictionary.rows contains the visual words (cluster centers)
	cout << "Processing training data..." << endl;

	Mat descriptorsHistogram(0, dictionarySize, CV_32FC1);	//Create Histogram of occurance of Visual Words
	extractBOWDescriptor(images, descriptorsHistogram);		//SEE FUNCTION

	cout << "descriptorsHistogram rows (images):" << descriptorsHistogram.rows << ", descriptorsHistogram cols (words): " << descriptorsHistogram.cols << endl;
	
	// -- PRINT THE DISSIMILARITY MATRICES --
	cout << endl;
	cout << "K = " << percentOfKeypoints << "% * " << totalKeypoints << " = " << dictionarySize << endl;
	cout << "Dissimilarity Matrix" << endl;
	cout << "==============================================" << endl;

	for(char i = 0; i < descriptorsHistogram.rows; i++) {
		asciiSymbol[0] = 65 + i; //"A" = 65
		cout << "\t" << asciiSymbol;
	}

	double compareVal = 0;
	for(int i = 0; i < descriptorsHistogram.rows; i++) {

		asciiSymbol[0] = 65 + i; 
		cout << endl << asciiSymbol << "\t";

		for(int j = 0; j < descriptorsHistogram.rows; j++) {
			compareVal = compareHist(Mat(descriptorsHistogram.row(i)), Mat(descriptorsHistogram.row(j)), CV_COMP_CHISQR); //OpenCV Chi-Square alorithm for comparing two histograms of word occurances
			cout.precision(3);
			cout << fixed << compareVal << "\t";
		}
	}
	cout << endl << "==============================================" << endl << endl;
}

//Extracts features from the images and adds them to the bowTrainer.
void extractVocabulary(vector<Mat>& images, vector<Mat>& matFeatures, int& totalKeypoints)	{

	totalKeypoints = 0;

	for(int i = 1; i < images.size(); i++) {

		asciiSymbol[0] = 65 + i - 1; //"A" = 65
		cout << "Processing image " << asciiSymbol << endl;

		if (!images[i].empty()) {

			vector<KeyPoint> keypoints;
			detector->detect(images[i], keypoints);

			if (keypoints.empty()) {
				cerr << "Warning: Could not find key points in image: " << asciiSymbol << endl;
			} 
			else {
				Mat features;
				extractor->compute(images[i], keypoints, features);
				
				totalKeypoints += keypoints.size();
				matFeatures[i] = features;
				
				cout << "Image " << asciiSymbol << ": No. of Keypoints = " << keypoints.size() << ", No. of Features = " << features.rows << endl;
			}
		}
		else {
			cerr << "Warning: Could not read image: " << asciiSymbol << endl;
		}
	}
}

//Creates a BoW descriptor for each image encountered.
void extractBOWDescriptor(vector<Mat>& images, Mat& descriptors) {	

	for(int i = 1; i < images.size(); i++) {

		asciiSymbol[0] = 65 + i - 1; //"A" = 65
		cout << "Processing image " << asciiSymbol << endl;
		if (!images[i].empty()) {
			vector<KeyPoint> keypoints;
			detector->detect(images[i], keypoints);
			if (keypoints.empty()) {
				cerr << "Warning: Could not find key points in image: " << asciiSymbol << endl;
			} 
			else {
				Mat bowDescriptor;
				bowDE.compute(images[i], keypoints, bowDescriptor);	//Computes an image descriptor "bowDescriptor" using the set visual vocabulary.
				descriptors.push_back(bowDescriptor);	//Descriptors is the normalized histogram of occurances of each Word.
			}
		} 
		else {
			cerr << "Warning: Could not read image: " << asciiSymbol << endl;
		}
	}
}


//==================================================================
// --- MISCILLANEOUS HELPER FUNCTIONS ---
//==================================================================

// --- EXTRACT LUMINANCE (Y) COMPONENT FROM THE RGB IMAGES ---
void extractLuminance(Mat& imgResized, Mat& imgAllY, Mat& imgY) {

	Mat imgYUV;	
	cvtColor(imgResized, imgYUV, CV_BGR2YCrCb);	//Convert RGB to YUV color space

	vector<Mat> imgYUVPlanes;	//Use the STL's vector structure to store multiple Mat objects
	split(imgYUV, imgYUVPlanes);	//split the image into separate color planes (Y U V)
	
	imgY = imgYUVPlanes[0];	//Extract the Y component from the Y U V planes

	// Convert Y component to Mono color RGB (for compatibility with function displayTwoImages()) and for highlighting Sift Keypoints
	imgYUVPlanes[1] = imgY;	
	imgYUVPlanes[2] = imgY;	

	merge(imgYUVPlanes, imgAllY);
}

// --- RESIZE IMAGES, PRESERVING THE ASPECT RATIO ---
void resizeImagePreservingAspectRatio(Mat& src, Mat& dest, Size newSize) {	//Mat& dest, Size newSize) {

    float aspectRatios[2] = {(float)newSize.width / src.cols, (float)newSize.height / src.rows}; 

    Size newSizeCorrected(src.cols, src.rows);

    if(aspectRatios[0] < aspectRatios[1]) { 
            newSizeCorrected.width = (int)(newSizeCorrected.width * aspectRatios[0] + 0.5); 
            newSizeCorrected.height = (int)(newSizeCorrected.height * aspectRatios[0] + 0.5); 
    } 
    else { 
            newSizeCorrected.width = (int)(newSizeCorrected.width * aspectRatios[1] + 0.5); 
            newSizeCorrected.height = (int)(newSizeCorrected.height * aspectRatios[1] + 0.5); 
    }

	resize(src, dest, newSizeCorrected, 0, 0, CV_INTER_AREA); //CV_INTER_AREA preferred for image decimation for Moire-free result
}

// --- DISPLAY TWO IMAGES SIDE BY SIDE ---
void displayTwoImages(Mat& leftImage, Mat& rightImage, bool isMultiInputImages) {

	Mat doubleImage(leftImage.rows, leftImage.cols*2, leftImage.type());

	leftImage.copyTo(doubleImage(Rect(0, 0, leftImage.cols, leftImage.rows)));
	rightImage.copyTo(doubleImage(Rect(leftImage.cols, 0, leftImage.cols, leftImage.rows)));

	if(!isMultiInputImages) {
		namedWindow("Display Images", CV_WINDOW_AUTOSIZE);
		imshow("Display Images", doubleImage);
	}
}

// --- DRAW KEY POINTS ON TOP OF THE LUMINANCE COMPONENT OF THE YUV IMAGE ---
void drawCustomKeyPoints(vector<KeyPoint> &key_points, Mat& img, Scalar &ptColor, unsigned int size) {

	Point2f endPt;

	for(unsigned int i=0; i < key_points.size(); ++i) {

		// Draw Cross at the location of the keypoint
		endPt.x = key_points[i].pt.x + size;
		endPt.y = key_points[i].pt.y;
		line(img, key_points[i].pt, endPt, ptColor, 1, CV_AA); //8

		endPt.x = key_points[i].pt.x - size;
		endPt.y = key_points[i].pt.y;
		line(img, key_points[i].pt, endPt, ptColor, 1, CV_AA);

		endPt.x = key_points[i].pt.x;
		endPt.y = key_points[i].pt.y + size;
		line(img, key_points[i].pt, endPt, ptColor, 1, CV_AA);

		endPt.x = key_points[i].pt.x;
		endPt.y = key_points[i].pt.y - size;
		line(img, key_points[i].pt, endPt, ptColor, 1, CV_AA);

		// Draw Circles around keypoint whose radius is proportional to scale of the keypoint
		circle(img, key_points[i].pt, key_points[i].size * .5, ptColor, 1, CV_AA);
	}
}
