//=====================================================
// Final Project: VEHICLE REGISTRATION PLATE DETECTION
// Coded by: Sagun Man SINGH SHRESTHA
//=====================================================

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <iostream>
#include <conio.h>
#include <time.h>
#include <vector>

using namespace std;
using namespace cv;
				
#define MAX_HEIGHT 140
#define MIN_HEIGHT 30
#define MAX_WIDTH 340
#define MIN_WIDTH 80
#define HEIGHT_BEGIN 140
#define HEIGHT_END 100
#define WIDTH_BEGIN 115
#define WIDTH_END 150
#define SUBSAMPLING_FACTOR 2

void mexicanHatOperation(IplImage*, IplImage*);
void thresholdOperation(IplImage*, IplImage*);
void morphoOpenOperation(IplImage*, IplImage*);
CvRect regionGrowingAndLocalization(IplImage*, IplImage*);
void finalLPLocalization(CvRect, IplImage*, IplImage*);
void displayMultipleImages(char*, int, ...);

// ===================
// -- MAIN FUNCTION --
// ===================
int main(int argc, char* argv[]) { 
	
	// Vector of image filename to be used as input
	// After compiling, please run the executable file directly from the executable file
	char *imgList[] = {
		"IMG_2415.jpg",
		"IMG_2412.jpg", 
		"IMG_2413.jpg",
		"IMG_2418.jpg",
		"IMG_2419.jpg",
		"IMG_2421.jpg",
		"IMG_2422.jpg",
		"IMG_2417.jpg",
		"IMG_2420.jpg",
		"IMG_2423.jpg",
		"IMG_2416.jpg",
		"IMG_2386.jpg"
	};

	IplImage *imgOriginal;
	vector<string> images(imgList, end(imgList));
	int imgListSize = (int)images.size(), imgIndex = 0;
	char* currentImageFile;

	// -- ITERATE THROUGH ALL TEST IMAGES IN IMGLIST OR ARGV --
	while(((argc < 2) && (imgIndex < imgListSize)) || ((argc >= 2) && (imgIndex < argc-1))) {
		
		//If input filenames are specified as arguments
		if(((argc >= 2) && (imgIndex < argc-1))) {
				currentImageFile = argv[imgIndex + 1];
				imgOriginal = cvLoadImage(currentImageFile);
		}
		//If input filenames are not specified as arguments
		else {
			currentImageFile = imgList[imgIndex];
			imgOriginal = cvLoadImage(currentImageFile);
			if(!imgOriginal) {
				cout << endl << "Error: Could not open or find the image " << currentImageFile
					 << ". " << endl
					 << "Please copy all the test images in the same path as the .exe file or," << endl
					 << "Enter image path explicitly in the command line as second argument. " << endl
					 << endl << "Press any key to exit..." << endl;
				_getch();	//Wait for user key press
				return -1;
			}
		}

		cout << endl << "Processing image file: " << currentImageFile << "..." << endl;
		imgIndex++;
		double totalTime = 0, t, ts = GetTickCount(); // Start timer

		// -- GRAYSCALING OPERATION --
		cout << endl << "Grayscaling Operation... "; 
		IplImage *imgGray = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
		CvSize imgSize = cvGetSize(imgGray);
		cvCvtColor(imgOriginal, imgGray, CV_RGB2GRAY); 
		t = (double)GetTickCount(); cout << "(" << t - ts << " ms)" << endl; totalTime += t - ts; ts = t; 	//Time keeping

		//_getch();
		//CvScalar s = cvGet2D(imgGray, 200, 1);
		//cout << "value = " << (int)((uchar*)(imgGray->imageData + imgGray->widthStep*200))[1] << endl << s.val[0]; //imgGray->imageData[imgGray->widthStep * 10 + 10]; //CV_IMAGE_ELEM(*imgGray, uchar, 10, 10) << endl; //((short*)(imgGray->imageData) + 10*imgGray->widthStep)[10] << endl;
		//_getch();


		// -- MEXICAN HAT OPERATION --
		cout << endl << "Mexican Hat Operation... ";
		IplImage *imgMexicanHat = cvCreateImage(imgSize, IPL_DEPTH_8U, 1); 
		mexicanHatOperation(imgGray, imgMexicanHat);
		t = (double)GetTickCount(); cout << "(" << t - ts << " ms)" << endl; totalTime += t - ts; ts = t; 	//Time keeping

		// -- THRESHOLDING OPERATION --
		cout << endl << "Thresholding Operation... ";
		IplImage *imgThreshold = cvCreateImage(imgSize, IPL_DEPTH_8U, 1); 
		thresholdOperation(imgMexicanHat, imgThreshold); 
		t = (double)GetTickCount(); cout << "(" << t - ts << " ms)" << endl; totalTime += t - ts; ts = t; 	//Time keeping
	 
		// -- MORPHOLOGICAL OPENING --
		cout << endl << "Morphological Opening... ";
		IplImage *imgMorphoOpen = cvCloneImage(imgGray); 
		morphoOpenOperation(imgThreshold, imgMorphoOpen);
		t = (double)GetTickCount(); cout << "(" << t - ts << " ms)" << endl; totalTime += t - ts; ts = t; 	//Time keeping
	
		// -- REGION GROWING --
		cout << endl << "Region Growing... " << endl;
		CvRect finalRegion = regionGrowingAndLocalization(imgGray, imgMorphoOpen);
		t = (double)GetTickCount(); cout << "(" << t - ts << " ms)" << endl; totalTime += t - ts; ts = t; 	//Time keeping

		// -- FINAL LICENSE PLATE LOCALIZATION --
		cout << endl << "Final License Plate Localization... ";
		IplImage* imgFinalLPLocalization = cvCloneImage(imgOriginal);
		finalLPLocalization(finalRegion, imgOriginal, imgFinalLPLocalization);
		t = (double)GetTickCount(); cout << "(" << t - ts << " ms)" << endl; totalTime += t - ts; ts = t; 	//Time keeping
	
		// -- DISPLAY ALL IMAGES OF ALL INTERMEDIATE STAGES --
		
		// Initialize BGR images for respective Gray images
		IplImage* imgGray_BGR = cvCreateImage(imgSize, IPL_DEPTH_8U, 3);
		IplImage* imgMexicanHat_BGR = cvCreateImage(imgSize, IPL_DEPTH_8U, 3);
		IplImage* imgThreshold_BGR = cvCreateImage(imgSize, IPL_DEPTH_8U, 3);
		IplImage* imgMorphoOpen_BGR = cvCreateImage(imgSize, IPL_DEPTH_8U, 3);

		// Convert Grayscale to BGR for compatibility with Multi-images Window
		cvCvtColor(imgGray, imgGray_BGR, CV_GRAY2BGR);
		cvCvtColor(imgMexicanHat, imgMexicanHat_BGR, CV_GRAY2BGR);
		cvCvtColor(imgThreshold, imgThreshold_BGR, CV_GRAY2BGR);	
		cvCvtColor(imgMorphoOpen, imgMorphoOpen_BGR, CV_GRAY2BGR);

		//Display
		cout << endl << "(Total Time: " << totalTime << "ms) " << endl << endl << "========================" << endl << endl;
		displayMultipleImages(currentImageFile, 6, imgOriginal, imgGray_BGR, imgMexicanHat_BGR, imgThreshold_BGR, imgMorphoOpen_BGR, imgFinalLPLocalization);

		// Clean up
		cvReleaseImage(&imgGray); 
		cvReleaseImage(&imgMexicanHat);
		cvReleaseImage(&imgThreshold);
		cvReleaseImage(&imgMorphoOpen);
		cvReleaseImage(&imgFinalLPLocalization);
	}

	cvReleaseImage(&imgOriginal); 
	return 0;
}

// ===========================
// -- MEXICAN HAT OPERATION --
// ===========================
void mexicanHatOperation(IplImage* imgGray, IplImage* imgMexicanHat) {

	// -- MEXICAN HAT OPERATION --
	// ---------------------------

	//Mexican Filter Operator
	double mexicanHatKernel[] = {
		0, 0, 0, -1, -1, -1, 0, 0, 0, 
		0, -1, -1, -3, -3, -3, -1, -1, 0, 
		0, -1, -3, -3, -1, -3, -3, -1, 0, 
		-1, -3, -3, 6, 13, 6, -3, -3, -1, 
		-1, -3, -1, 13, 24, 13, -1, -3, -1, 
		-1, -3, -3, 6, 13, 6, -3, -3, -1, 
		0, -1, -3, -3, -1, -3, -3, -1, 0, 
		0, -1, -1, -3, -3, -3, -1, -1, 0, 
		0, 0, 0, -1, -1, -1, 0, 0, 0,
	};

	CvScalar s, sVal; 
	CvSize imgSize= cvGetSize(imgGray); 
	// Creating image matrix for Mexican Hat Operator
	for(int i=4; i<imgSize.height-4; i++ ) { 
		for(int j=4; j<imgSize.width-4; j++) {

			double sValue = 0;
			for(int k=-4; k<5; k++) { 
				for(int l=-4; l<5; l++) {
					int n = 9;
					sValue += mexicanHatKernel[(k+4)*n + (l+4)] * ((uchar*)(imgGray->imageData + imgGray->widthStep*(i+k)))[j+l]; 
				}
			}

			sVal.val[0] = sValue;
			cvSet2D(imgMexicanHat, i, j, sVal);
		}
	} 
}

// =========================
// -- THRESHOLD OPERATION --
// =========================
void thresholdOperation(IplImage* imgInput, IplImage* imgOutput) {
// -- THRESHOLDING OPERATION --
	// ----------------------------
	//Remove the unwanted edges by using a certain thresholdAmount
	double maxAmount, minAmount, thresholdAmount; 
	cvMinMaxLoc(imgInput, &minAmount, &maxAmount); 
	thresholdAmount = (maxAmount + minAmount) / 3; 
	cvThreshold(imgInput, imgOutput, thresholdAmount, maxAmount, CV_THRESH_BINARY); 
}

// =============================
// -- MORPHOLOGICAL OPERATION --
// =============================
void morphoOpenOperation(IplImage* imgInput, IplImage* imgOutput) {
	
	//Dilation operation/Image with Dilation Performed
	int anchorAmount = 1;
	IplConvKernel* element = cvCreateStructuringElementEx(anchorAmount*2+1, anchorAmount*2+1, anchorAmount, anchorAmount, CV_SHAPE_RECT);
	// Morphological Openinig = Erode + Dilate
	cvErode(imgInput, imgOutput, element, 1);
	cvDilate(imgOutput, imgOutput, element, 1);
}

// =====================================
// -- REGION GROWING AND LOCALIZATION --
// =====================================
CvRect regionGrowingAndLocalization(IplImage* imgGray, IplImage* imgInput) {
	
	IplImage *imgRG1 = cvCloneImage(imgGray); 
	IplImage *imgRG2 = cvCloneImage(imgGray);
	
	CvScalar pixInit;
	pixInit.val[0] = 0; 
	CvSize imgSize= cvGetSize(imgGray); 
	//Initialize images for Region Growing
	for(int i=0; i<imgSize.height; i++) {
		for(int j=0; j<imgSize.width; j++) {
			cvSet2D(imgRG1, i, j, pixInit);
			cvSet2D(imgRG2, i, j, pixInit); 
		}
	}
	
	imgRG2 = imgInput;

	int lbl = 1, n = 0, x = 0, bestEuler = 0, candidateId = 0;
	CvRect candidateRegion, finalRegion; 
	CvSize finalRectSize;
	
	//Subsampling: Start from the region around the center
	for(int i=HEIGHT_BEGIN; i<imgSize.height-HEIGHT_END; i+=SUBSAMPLING_FACTOR) { 
		for(int j=WIDTH_BEGIN; j<imgSize.width-WIDTH_END; j+=SUBSAMPLING_FACTOR) {

			//Initializing a matrix for segmented image
			vector<int> rowPoints(1), colPoints(1);
			rowPoints[0] = i; colPoints[0] = j;
			CvScalar rgPixVal1, rgPixVal2;

			rgPixVal1 = cvGet2D(imgRG1, i, j);
			
			if(rgPixVal1.val[0] == 0) {

				while(true) {
					// Region Growing: Using 4 Neighbour Connectivity, group pixels into small segments of varying sizes between predefined height and widths
					// So that small block of binary pixels can be judged as candidate license plate ROI later on the basis of size and Euler Number Criteria
					rgPixVal1 = cvGet2D(imgRG1, rowPoints[n], colPoints[n]);
					rgPixVal2 = cvGet2D(imgRG2, rowPoints[n], colPoints[n]); 
					
					if(rgPixVal2.val[0] == 255.0 && rgPixVal1.val[0] == 0) {

						rgPixVal1.val[0] = lbl;
						cvSet2D(imgRG1, rowPoints[n], colPoints[n], rgPixVal1);
						
						if(rowPoints[n]+1 < imgSize.height) {	// Bottom Neighbouring Pixel
							rgPixVal2 = cvGet2D(imgRG2, rowPoints[n]+1, colPoints[n]);
							if(rgPixVal2.val[0] == 255.0) {
								rowPoints.push_back(rowPoints[n] + 1);
								colPoints.push_back(colPoints[n]);
								x++;
							}
						}
						
						if(colPoints[n]-1 >= 0) {				// Left Neighbouring Pixel
							rgPixVal2 = cvGet2D(imgRG2, rowPoints[n], colPoints[n]-1); 
							if(rgPixVal2.val[0] == 255.0) {
								rowPoints.push_back(rowPoints[n]); 
								colPoints.push_back(colPoints[n] - 1);
								x++; 
							}
						}
						
						if(colPoints[n]+1 < imgSize.width) {	// Right Neighbouring Pixel
							rgPixVal2 = cvGet2D(imgRG2, rowPoints[n], colPoints[n]+1); 
							if(rgPixVal2.val[0] == 255.0) { 
								rowPoints.push_back(rowPoints[n]);
								colPoints.push_back(colPoints[n] + 1);
								x++;
							}
						}
						
						if(rowPoints[n]-1 >= 0) {				// Top Neighbouring Pixel
							rgPixVal2 = cvGet2D(imgRG2, rowPoints[n]-1, colPoints[n]);
							if(rgPixVal2.val[0] == 255.0) { 
								rowPoints.push_back(rowPoints[n] - 1); 
								colPoints.push_back(colPoints[n]); 
								x++;
							}
						}
					};
					
					if(n >= x) break;
					n++;
				};
				
				const int rowHigh = *max_element(rowPoints.begin(), rowPoints.end()); 
				const int rowLow = *min_element(rowPoints.begin(), rowPoints.end()); 
				const int colHigh = *max_element(colPoints.begin(), colPoints.end()); 
				const int colLow = *min_element(colPoints.begin(), colPoints.end());

				//If candidate license plate is within a predefined size range
				if((rowHigh-rowLow)<MAX_HEIGHT && (rowHigh-rowLow)>MIN_HEIGHT && (colHigh-colLow)<MAX_WIDTH && (colHigh-colLow)>MIN_WIDTH) {

					candidateRegion.height = rowHigh - rowLow;
					candidateRegion.width = colHigh - colLow;
					candidateRegion.x = colLow;
					candidateRegion.y = rowLow;

					finalRectSize.height = rowHigh - rowLow;
					finalRectSize.width = candidateRegion.width;
					
					IplImage *imgCandidateRegion = cvCreateImage(finalRectSize, IPL_DEPTH_8U, 1);
					cvSetImageROI(imgRG2, cvRect(colLow, rowLow, colHigh-colLow, rowHigh-rowLow));
					cvCopy(imgRG2, imgCandidateRegion, NULL);
					cvResetImageROI(imgRG2);

					candidateId++;
					cvNamedWindow("LP" + candidateId, CV_WINDOW_AUTOSIZE);
					cvShowImage("LP" + candidateId, imgCandidateRegion);

					// Calculation of Euler Number for individual candidate regions
					int euler1, euler2 = 0, newEuler;
					CvMemStorage* storage = cvCreateMemStorage(); 
					CvSeq* contours = NULL; 

					// Use Connected Component Analysis to find the number of Objects and Holes
					euler1 = cvFindContours(imgCandidateRegion, storage, &contours, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_CODE);

					if(contours != NULL) {
						CvSeq *aux = contours;
						if(aux->v_next) {
							euler2++;
							aux = aux->v_next;
						}
						while(aux->h_next) {
							aux = aux->h_next;
							euler2++;
						}
					}

					newEuler = euler2 - euler1;

					//Find the candidate region parameters with the minimum euler number
					if(newEuler < bestEuler) {
						bestEuler = newEuler;
						finalRegion.height = rowHigh - rowLow; 
						finalRegion.width = colHigh - colLow; 
						finalRegion.x = colLow; 
						finalRegion.y = rowLow;
					};

					cout << endl
						<< "candidateRegion.height = " << candidateRegion.height << endl
						<< "candidateRegion.width = " << candidateRegion.width << endl
						<< "candidateRegion.x = " << candidateRegion.x << endl
						<< "candidateRegion.y = " << candidateRegion.x << endl
						<< "euler1 = " << euler1 << endl
						<< "euler2 = " << euler2 << endl
						<< "newEuler = " << newEuler << endl
						<< endl << "------------------------" << endl;

					cvReleaseImage(&imgCandidateRegion);
				};
			};
			
			x=0; n=0; lbl++; 
		};
	};

	//cout << lbl;
	return finalRegion;
}

// ======================================
// -- FINAL LICENSE PLATE LOCALIZATION --
// ======================================
void finalLPLocalization(CvRect finalRegion, IplImage* imgInput, IplImage* imgOutput) {

	//Segmented License Plate
	IplImage* imgFinalRegion = cvCreateImage(cvSize(finalRegion.height, finalRegion.width), IPL_DEPTH_8U, 3);
	cvSetImageROI(imgInput, finalRegion);
	imgFinalRegion = cvCloneImage(imgInput);
	cvResetImageROI(imgInput);

	cvNamedWindow("FinalRegion", CV_WINDOW_AUTOSIZE);
	cvShowImage("FinalRegion", imgFinalRegion);

	cvRectangle (
		imgOutput,
		cvPoint(finalRegion.x, finalRegion.y),
		cvPoint(finalRegion.x + finalRegion.width, finalRegion.y + finalRegion.height),
		cvScalar(0xff, 0x00, 0x00),	//Blue
		3,
		CV_AA
	);
}

// ===================================
// -- DISPLAY MULTIPLE IMAGE WINDOW --
// ===================================
void displayMultipleImages(char* title, int nArgs, ...) {

    IplImage *img;		 //for getting the arguments 
    IplImage *dispImage; //image in which input images are to be copied
    int size, i;
    int m, n;
    int x, y;
    int w, h;			 // w - Maximum number of images in a row, h - Maximum number of images in a column 
    float scale;		 //How much we have to resize the image
    int max;

    // If the number of arguments is lesser than 0 or greater than 12, return without displaying 
    if(nArgs <= 0) {
        cout << "Number of arguments too small....\n";
        return;
    }
    else if(nArgs > 12) {
        cout << "Number of arguments too large....\n";
        return;
    }
    // Determine the size of the image, and the number of rows/cols from number of arguments 
    else if (nArgs == 1) {
        w = h = 1;
        size = 300;
    }
    else if (nArgs == 2) {
        w = 2; h = 1;
        size = 300;
    }
    else if (nArgs == 3 || nArgs == 4) {
        w = 2; h = 2;
        size = 300;
    }
    else if (nArgs == 5 || nArgs == 6) {
        w = 3; h = 2;
        size = 200;
    }
    else if (nArgs == 7 || nArgs == 8) {
        w = 4; h = 2;
        size = 200;
    }
    else {
        w = 4; h = 3;
        size = 150;
    }

	size *= 2;	//double the size

    // Create a new 3 channel image
    dispImage = cvCreateImage(cvSize(60 + size*w, 45 + size*h*.75), IPL_DEPTH_8U, 3);	//Old values = (100 + size*w, 40 + size*h*.8)
	cvSetZero(dispImage);	//Initialize it to Black

    // Get the arguments passed
    va_list args;
    va_start(args, nArgs);

    // Loop for nArgs number of arguments
    for (i = 0, m = 15, n = 15; i < nArgs; i++, m += (15 + size)) {
        // Get the Pointer to the IplImage
        img = va_arg(args, IplImage*);
        // Check whether it is NULL or not. If it is NULL, release the image, and return
        if(img == 0) {
            cout << "Invalid arguments";
            cvReleaseImage(&dispImage);
            return;
        }

        // Find the width and height of the image
        x = img->width;
        y = img->height;

        // Find whether height or width is greater in order to resize the image
        max = (x > y)? x: y;

        // Find the scaling factor to resize the image
        scale = (float) ( (float) max / size );

        // Used to Align the images
        if( i % w == 0 && m!= 15) {
            m = 15;
            n+= 15 + size*.75;	//n+= 20 + size;
        }

        // Set the image ROI to display the current image
        cvSetImageROI(dispImage, cvRect(m, n, (int)( x/scale ), (int)( y/scale )));
        // Resize the input image and copy the it to the Single Big Image
        cvResize(img, dispImage);
        // Reset the ROI in order to display the next image
        cvResetImageROI(dispImage);
    }

    // Create a new window, and show the Single Big Image
    cvNamedWindow(title, 1);
    cvShowImage(title, dispImage);
    cvWaitKey();
    cvDestroyWindow(title);
    // End the number of arguments
    va_end(args);
    // Release the Image Memory
    cvReleaseImage(&dispImage);
}
