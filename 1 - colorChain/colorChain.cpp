#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>

using namespace cv;
using namespace std;

// Initializing functions
Mat convert_bayerGRBG_to_RGB(Mat imgInput);
Mat colorCorrection(Mat imgInput);
Mat gammaCorrection(Mat imgInput);

// Main function
int main(int argc, char* argv[]) {

	//=====================================
	// --- STAGE 1: ORIGINAL IMAGE PART ---
	//=====================================

	if(argc != 2) {
		cout << "Usage: colorChain InputImage" << endl;
		return -1;
	}

	Mat imgOriginal;
	imgOriginal = imread(argv[1], CV_LOAD_IMAGE_UNCHANGED);	//Read input image file as is (including alpha channel if present)

	if(!imgOriginal.data) {
		cout << "Could not open or find the image" << endl;
		return -1;
	}

	//==================================================================
	// --- STAGE 2: BILINEAR COLOR INTERPOLATION (BAYER-GRBG TO RGB) ---
	//==================================================================

	Mat imgSrc;
	imgSrc = imgOriginal;

	Mat imgCFAInterpol;	//(imgSrc.size(), CV_32F, Scalar(0, 0, 0));

	Mat imgDst(imgSrc.size(), CV_8UC3);
	imgDst = Scalar(0, 0, 0); //BGR order. R = 255;

	uchar m, n;
	Mat inR(imgSrc.size(), CV_8UC1), inG(imgSrc.size(), CV_8UC1), inB(imgSrc.size(), CV_8UC1);
	Mat tempOutR(imgSrc.size(), CV_8UC1), outR(imgSrc.size(), CV_8UC1), outG(imgSrc.size(), CV_8UC1), outB(imgSrc.size(), CV_8UC1);
	m = imgSrc.rows;
	n = imgSrc.cols;
	
	inR = imgSrc;
	inG = imgSrc;
	inB = imgSrc;

	// R channel
	for(int j = 1; j <= m-1; j += 2) {
		for(int i = 3; i <= n-1; i += 2) {
			outR.at<uchar>(j, i) = .5 * (inR.at<uchar>(j, i-1) + inR.at<uchar>(j, i+1));
		}
	}
	
	for(int j = 2; j <= m-2; j += 2) {
		for(int i = 2; i <= n; i += 2) {
			outR.at<uchar>(j, i) = .5 * (inR.at<uchar>(j-1, i) + inR.at<uchar>(j+1, i));
		}
		for(int i = 4; i <= n; i += 2) {
			outR.at<uchar>(j, i-1) = .25 * (inR.at<uchar>(j-1, i-2) + inR.at<uchar>(j-1, i) + inR.at<uchar>(j+1, i-2) + inR.at<uchar>(j+1, i));
		}
	}

	// B channel
	for(int j = 2; j <= m; j += 2) {
		for(int i = 2; i <= n-2; i += 2) {
			outB.at<uchar>(j, i) = .5 * (inR.at<uchar>(j, i-1) + inR.at<uchar>(j, i+1));
		}
	}
		
	for(int j = 3; j <= m-1; j += 2) {
		for(int i = 1; i <= n-1; i += 2) {
			outB.at<uchar>(j, i) = .5 * (inR.at<uchar>(j-1, i) + inR.at<uchar>(j+1, i));
			
		}
		for(int i = 3; i <= n-1; i += 2) {
			outB.at<uchar>(j, i-1) = .25 * (inR.at<uchar>(j-1, i-2) + inR.at<uchar>(j-1, i) + inR.at<uchar>(j+1, i-2) + inR.at<uchar>(j+1, i));
		}
	}

	// G channel
	for(int j = 2; j <= m-2; j += 2) {
		for(int i = 3; i <= n-1; i += 2) {
			outG.at<uchar>(j, i) = .25 * (inR.at<uchar>(j, i-1) + inR.at<uchar>(j, i+1) + inR.at<uchar>(j-1, i) + inR.at<uchar>(j+1, i));
		}
	}

	for(int j = 3; j <= m-1; j += 2) {
		for(int i = 2; i <= n-2; i += 2) {
			outG.at<uchar>(j, i) = .25 * (inR.at<uchar>(j, i-1) + inR.at<uchar>(j, i+1) + inR.at<uchar>(j-1, i) + inR.at<uchar>(j+1, i+1));
		}
	}

	//
	for(int i = 2; i <= n-2; i += 2) {
		outG.at<uchar>(1, i) = (1/3) * (inR.at<uchar>(1, i-1) + inR.at<uchar>(3, i+1) + inR.at<uchar>(2, i));
	}
	outG.at<uchar>(1, n) = .5 * (inR.at<uchar>(1, n-1) + inR.at<uchar>(2, n));
	for(int j = 3; j <= m-1; j += 2) {
		outG.at<uchar>(j, n) = (1/3) * (inR.at<uchar>(j-1, n) + inR.at<uchar>(j+1, n) + inR.at<uchar>(j, n-1));
	}
	for(int j = 2; j <= m-2; j += 2) {
		outG.at<uchar>(j, 1) = (1/3) * (inR.at<uchar>(j-1, 1) + inR.at<uchar>(j+1, 1) + inR.at<uchar>(j, 2));
	}
	outG.at<uchar>(m, 1) = .5 * (inR.at<uchar>(m-1, 1) + inR.at<uchar>(m, 2));
	for(int i = 3; i <= n-1; i += 2) {
		outG.at<uchar>(m, i) = (1/3) * (inR.at<uchar>(m, i-1) + inR.at<uchar>(m, i+1) + inR.at<uchar>(m-1, i));
	}

	for(int i = 0; i < m; ++i) {
		for(int j = 0; j < n; ++j) {
			imgDst.at<Vec3b>(i, j)[0] = outB.at<uchar>(i, j);	//B
			imgDst.at<Vec3b>(i, j)[1] = outG.at<uchar>(i, j);	//G
			imgDst.at<Vec3b>(i, j)[2] = outR.at<uchar>(i, j);	//R
		}
	}
	
	imgCFAInterpol = imgDst;

	//========================================
	//// --- STAGE 3: COLOR CORRECTION---
	//========================================

	Mat imgColorCorrected(imgSrc.size(), CV_8UC3);
	//Mat correctionKernel = (Mat_<float>(3,3) << 1.18, -0.05, -0.13, -0.24, 1.29, -0.05, -0.18, -0.44, 1.62);
	Mat correctionKernel = (Mat_<float>(3,3) << -0.18, -0.44, 1.62, -0.24, 1.29, -0.05, 1.18, -0.05, -0.13);

	Mat tempImgCFAInterpol;
	for(int i = 0; i < m; ++i) {
		for(int j = 0; j < n; ++j) {
			Mat(imgCFAInterpol.at<Vec3b>(i, j)).convertTo(tempImgCFAInterpol, CV_32FC3);
			imgColorCorrected.at<Vec3b>(i, j) = Mat(correctionKernel * tempImgCFAInterpol);
		}
	}
	
	//==================================================================
	// --- SHOW ALL IMAGES ---
	//==================================================================

	namedWindow("Original", CV_WINDOW_AUTOSIZE);
	imshow("Original", imgOriginal);
	cout << imgOriginal.rows << "x" << imgOriginal.cols << "," << imgOriginal.channels() << "|" << imgOriginal.type() << endl;

	namedWindow("outR", CV_WINDOW_AUTOSIZE);
	imshow("outR", outR);
	
	namedWindow("outG", CV_WINDOW_AUTOSIZE);
	imshow("outG", outG);
	
	namedWindow("outB", CV_WINDOW_AUTOSIZE);
	imshow("outB", outB);
	
	namedWindow("Bayer2RGBConversion", CV_WINDOW_AUTOSIZE);
	imshow("Bayer2RGBConversion", imgDst);

	namedWindow("ColorCorrected", CV_WINDOW_AUTOSIZE);
	imshow("ColorCorrected", imgColorCorrected);

	waitKey(0);
	return 0;
}