#pragma once
#include "opencv2/opencv.hpp"

#include <string>
#include <io.h>
#include <tchar.h>
#include <direct.h>
#include <windows.h> 
//MFC 가 아닌 상황에서도

using namespace std;
using namespace cv;

class cvMouseExtension
{
public:
#pragma region SYSTEM
	cvMouseExtension(String strWindowName, int iFlag = WINDOW_NORMAL);
	/*
	   WINDOW_NORMAL     = 0x00000000, //!< the user can resize the window (no constraint) / also use to switch a fullscreen window to a normal size.
	   WINDOW_AUTOSIZE   = 0x00000001, //!< the user cannot resize the window, the size is constrainted by the image displayed.
	   WINDOW_OPENGL     = 0x00001000, //!< window with opengl support.

	   WINDOW_FULLSCREEN = 1,          //!< change the window to fullscreen.
	   WINDOW_FREERATIO  = 0x00000100, //!< the image expends as much as it can (no ratio constraint).
	   WINDOW_KEEPRATIO  = 0x00000000, //!< the ratio of the image is respected.
	   WINDOW_GUI_EXPANDED=0x00000000, //!< status bar and tool bar
	   WINDOW_GUI_NORMAL = 0x00000010, //!< old fashious way
	*/
	~cvMouseExtension();

	bool windowExists(const string& windowName);
	Rect cvGetWindowRect(const string& windowName);
#pragma endregion

#pragma region cvMessageBox
	bool SetMessageBox(Mat curImage);
	Mat	 CreateMessageBox(String text, Size winsize);
	bool ShowMessageBox(String text, Size winsize = Size(500, 70));
	Size calculateAspectRatio(const Mat& image);
	int _gcd(int a, int b);
#pragma endregion

#pragma region INIT
	bool LoadImg(Mat& mat, double InitScale = 1.0f); // default = 1
	bool LoadPath(String winname, double InitScale = 1.0f); // default = 1

	bool ImShow(Mat& mat, double InitScale = 1.0f); // default = 1
	bool ImRead(String winname, double InitScale = 1.0f); // default = 1

	void SetInitailScale(double dScale);
#pragma endregion

#pragma region FUNCTION
	void InvalidateWindows();
	void ImageAutoRelease();
	void SetHorzSliderCtrlPos(int iPos);
	void SetVertSliderCtrlPos(int iPos);
	void OnRButtonSaveImage();
#pragma endregion

public:
	Mat		Image;
	vector<Mat> m_Resize;
	
	String	m_strWindowName;
	Point	ptLButtonDown;
	Point	ptRButtonDown;

#pragma region variable
	int resizeFlag = 0;		
	/*
	INTER_NEAREST        = 0, // Default
	INTER_LINEAR         = 1, 
	INTER_CUBIC          = 2,
	INTER_AREA           = 3,
	INTER_LANCZOS4       = 4,
	INTER_LINEAR_EXACT   = 5,
	INTER_NEAREST_EXACT  = 6,
	INTER_MAX            = 7,
	WARP_FILL_OUTLIERS   = 8,
	WARP_INVERSE_MAP     = 16
	*/

	int zoomCount;
	int maxZoomCount;
	int minZoomCount;

	double offsetX;
	double offsetY;
	double InitScale;
	double newScale;
	double scaleRatio;

	int originalWidth;
	int originalHeight;

	int horizontalSliderPos;
	int verticalSliderPos;

	int horizontalSliderPosCopy;
	int verticalSliderPosCopy;

	int horizontalSliderRangeMin;
	int horizontalSliderRangeMax;
	int verticalSliderRangeMin;
	int verticalSliderRangeMax;


#pragma endregion	
};

