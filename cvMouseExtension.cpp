#include "pch.h"
#include "cvMouseExtension.h"

// 마우스 이벤트 (확대/축소: 휠, 이동: 마우스 좌클릭 - 드래그, 우클릭: 줌 상태의 이미지 저장)
void cvMouseCallBack (int event, int x, int y, int flag, void* pUserData)
{
	cvMouseExtension* pParent = (cvMouseExtension*)pUserData;
	
	if (event == EVENT_MOUSEWHEEL) 
	{
		if (getMouseWheelDelta(flag) > 0 && pParent->zoomCount != pParent->maxZoomCount)
			pParent->zoomCount++;
		else if (getMouseWheelDelta(flag) < 0 && pParent->zoomCount != pParent->minZoomCount)
			pParent->zoomCount--;

		if (pParent->zoomCount == 0)
			pParent->offsetX = pParent->offsetY = 0;

		x = pParent->ptLButtonDown.x;
		y = pParent->ptLButtonDown.y;

		// double형으로 캐스팅하여 오버플로 방지
		double horzSliderBarPos = static_cast<double>(pParent->horizontalSliderPos);
		double vertSliderBarPos = static_cast<double>(pParent->verticalSliderPos);

		double dPixelX = (horzSliderBarPos + x + pParent->offsetX) / pParent->newScale;
		double dPixelY = (vertSliderBarPos + y + pParent->offsetY) / pParent->newScale;

		pParent->newScale = pParent->InitScale * pow(pParent->scaleRatio, pParent->zoomCount);

		int iW = pParent->originalWidth;
		int iH = pParent->originalHeight;
		pParent->horizontalSliderRangeMax = int(pParent->newScale * iW - pParent->InitScale * iW);
		pParent->verticalSliderRangeMax = int(pParent->newScale * iH - pParent->InitScale * iH);

		// double형으로 캐스팅하여 오버플로 방지
		int iBarPosX = static_cast<int>(dPixelX * pParent->newScale - x + 0.5);
		int iBarPosY = static_cast<int>(dPixelY * pParent->newScale - y + 0.5);

		pParent->SetHorzSliderCtrlPos(iBarPosX);
		pParent->SetVertSliderCtrlPos(iBarPosY);

		// double형으로 캐스팅하여 오버플로 방지
		pParent->offsetX = -iBarPosX + (dPixelX * pParent->newScale - x);
		pParent->offsetY = -iBarPosY + (dPixelY * pParent->newScale - y);

		pParent->InvalidateWindows();
	}
	else if (event == EVENT_LBUTTONDOWN)
	{
		pParent->ptLButtonDown.x = x;
		pParent->ptLButtonDown.y = y;
	}
	else if (flag == EVENT_FLAG_LBUTTON)
	{
		int iRButtonOffsetX = x - pParent->ptLButtonDown.x;
		int iRButtonOffsetY = y - pParent->ptLButtonDown.y;


		int iBarPosX = pParent->horizontalSliderPosCopy - iRButtonOffsetX;
		pParent->SetHorzSliderCtrlPos (iBarPosX);
		

		int iBarPosY = pParent->verticalSliderPosCopy - iRButtonOffsetY;
		pParent->SetVertSliderCtrlPos (iBarPosY);
		pParent->InvalidateWindows();
	}

	else if (event == EVENT_RBUTTONDOWN)
	{
		pParent->OnRButtonSaveImage();
	}
	else if (event == EVENT_MOUSEMOVE)
	{
		pParent->ptLButtonDown.x = x;
		pParent->ptLButtonDown.y = y;
		pParent->horizontalSliderPosCopy = pParent->horizontalSliderPos;
		pParent->verticalSliderPosCopy = pParent->verticalSliderPos;

	}
}

// 초기화
cvMouseExtension::cvMouseExtension(String strWindowName, int iFlag)
{
	namedWindow (strWindowName, iFlag);
	resizeWindow(strWindowName, cv::Size(800, 800)); // 초기 윈 사이즈 설정
	setMouseCallback (strWindowName, cvMouseCallBack, this);
	m_strWindowName = strWindowName;


	//Initial values
	zoomCount = 0;
	maxZoomCount = 10; // 줌 카운트 (x10)
	minZoomCount = 0;

	m_Resize.resize (maxZoomCount + 1);

	offsetX = 0;
	offsetY = 0;
	InitScale = 1;
	newScale = 1;
	scaleRatio = 1.25; // 줌 스케일

	originalWidth = 0;
	originalHeight = 0;

	horizontalSliderPos = 0;
	verticalSliderPos = 0;

	horizontalSliderRangeMin = 0;
	horizontalSliderRangeMax = 1;
	verticalSliderRangeMin = 0;
	verticalSliderRangeMax = 1;

}
cvMouseExtension::~cvMouseExtension()
{
	if (!m_strWindowName.empty() && windowExists(m_strWindowName)) 
	{
		setMouseCallback(m_strWindowName, nullptr);
	}


	if (!Image.empty())
		Image.release();
	for (auto& img : m_Resize) 
	{
		if (!img.empty())
			img.release();
	}
	m_Resize.clear();

	cv::destroyAllWindows();
}
bool cvMouseExtension::windowExists(const string& windowName)
{
	Mat testImage; // 임시 이미지 생성
	try 
	{
		imshow(windowName, testImage); // 윈도우를 표시해보고
		if (getWindowProperty(windowName, WND_PROP_AUTOSIZE) != -1) 
		{
			cv::destroyAllWindows(); // 윈도우가 존재하면 제거
			return true;
		}
	}
	catch (const cv::Exception&) {}
	return false; // 윈도우가 존재하지 않거나 예외가 발생한 경우
}

// ImRead: 파일 불러오기 / ImShow: Mat 불러오기
bool cvMouseExtension::ImRead(String winname, double InitScale)
{
	SetInitailScale(InitScale);

	Image = imread (winname);

	if (Image.empty())
		return false;
	originalWidth = Image.cols;
	originalHeight = Image.rows;

	Size sizeInitial(Image.cols * InitScale, Image.rows * InitScale);
	resize(Image, m_Resize[0], sizeInitial, 0, 0, resizeFlag);
	if (!m_Resize[0].empty())
		imshow(m_strWindowName, m_Resize[0]);

	return !m_Resize[0].empty();
}
bool cvMouseExtension::ImShow(Mat& mat, double InitScale)
{
	SetInitailScale(InitScale);
	if (mat.channels() == 1)
	{
		cvtColor(mat, Image, COLOR_GRAY2BGR);
	}
	else
	{
		Image = mat.clone();
	}
	

	if (Image.empty())
		return false;
	originalWidth = Image.cols;
	originalHeight = Image.rows;

	Size size (Image.cols * InitScale, Image.rows * InitScale);
	resize(Image, m_Resize[0], size, 0, 0, resizeFlag);
	if (!m_Resize[0].empty())
		imshow (m_strWindowName, m_Resize[0]);

	return !m_Resize[0].empty();
}
void cvMouseExtension::OnWait(Mat& curImage)
{	
	// 약간의 트릭 
	imshow("Message", curImage);

	Rect mainWindowRect;
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);
	int window_width = curImage.cols;
	int window_height = curImage.rows;

	int x = (screen_width - window_width) / 2;
	int y = (screen_height - window_height) / 2;

	// 창 위치 화면 정중앙으로 설정
	moveWindow("Message", x, y);
	if (waitKey(1500))
	{
		cv::destroyWindow("Message");
		curImage.release();
	}
	
}
Mat cvMouseExtension::OnDrawText(String text, Size winsize)
{
	int baseline = 0;
	Mat empty = Mat::zeros(winsize, CV_8UC3);
	empty.setTo(Scalar(255, 255, 255));

	Size textSize = getTextSize(text, FONT_HERSHEY_SIMPLEX, 1, 1, &baseline);
	Point textOrg((empty.cols - textSize.width) / 2, (empty.rows + textSize.height) / 2);

	//int padding = 30;
	//Point rectTopLeft(textOrg.x - padding, textOrg.y - textSize.height - padding);
	//Point rectBottomRight(textOrg.x + textSize.width + padding, textOrg.y + baseline + padding);
	//rectangle(empty, rectTopLeft, rectBottomRight, Scalar(0, 255, 255), 2);

	putText(empty, text, textOrg, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);

	return empty;
}

void cvMouseExtension::SetInitailScale (double dScale)
{
	if (dScale <= 0)
		return;

	InitScale = dScale;
	newScale = dScale;
}
void cvMouseExtension::InvalidateWindows()
{
	if (Image.empty())
		return;
	if (m_Resize[zoomCount].empty())
	{
		Size size (int (newScale * Image.cols), int (newScale * Image.rows));
		//resize (Image, m_Resize[zoomCount], size);
		resize(Image, m_Resize[zoomCount], size, 0, 0, resizeFlag);
	}
	int iW = m_Resize[0].cols - 1, iH = m_Resize[0].rows - 1;//-1: for bar size


	Rect rectShow (Point (horizontalSliderPos, verticalSliderPos), Size (iW, iH));
	
	imshow (m_strWindowName, m_Resize[zoomCount](rectShow));
}
void cvMouseExtension::SetHorzSliderCtrlPos (int iPos)
{
	if (iPos > horizontalSliderRangeMax)
		horizontalSliderPos = horizontalSliderRangeMax;
	else if (iPos < 0)
		horizontalSliderPos = horizontalSliderRangeMin;
	else
		horizontalSliderPos = iPos;

}
void cvMouseExtension::SetVertSliderCtrlPos (int iPos)
{
	if (iPos > verticalSliderRangeMax)
		verticalSliderPos = verticalSliderRangeMax;
	else if (iPos < 0)
		verticalSliderPos = verticalSliderRangeMin;
	else
		verticalSliderPos = iPos;
}
void cvMouseExtension::OnRButtonSaveImage()
{
	time_t timer;
	timer = time(NULL);
	struct tm t;
	localtime_s(&t, &timer);
	cv::String temp = cv::format("%04d%02d%02d%02d%02d%d.bmp", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	
	if (Image.empty())
		return;
	if (m_Resize[zoomCount].empty())
	{
		Size size(int(newScale * Image.cols), int(newScale * Image.rows));
		resize(Image, m_Resize[zoomCount], size, 0, 0, resizeFlag);
	}
	int iW = m_Resize[0].cols - 1, iH = m_Resize[0].rows - 1;//-1: for bar size
	Rect rectShow(Point(horizontalSliderPos, verticalSliderPos), Size(iW, iH));

	OnWait(OnDrawText("Image Save successful !", Size(500, 70))); // 없으면 __acrt_first_block == header 오류
	if(!m_Resize[zoomCount].empty())
		imwrite(temp, m_Resize[zoomCount](rectShow));
}
