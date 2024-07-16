#include "pch.h"
#include "cvMouseExtension.h"

/*
	마우스 이벤트
	휠:				확대/축소
	마우스 이동:			확대/축소 시 커서 위치로부터 확대/축소
	좌 클릭 후 드래그:		확대 시 화면 이동
	우 클릭:			확대/축소 상태의 이미지 저장
*/
void cvMouseCallBack(int event, int x, int y, int flag, void* pUserData)
{
	cvMouseExtension* pParent = (cvMouseExtension*)pUserData;

	//bool IsCtrl = (flag & EVENT_FLAG_CTRLKEY) != 0;
	if (event == EVENT_MOUSEWHEEL)
	{
		if (getMouseWheelDelta(flag) > 0 && pParent->zoomCount != pParent->maxZoomCount)
		{
			pParent->zoomCount++;
			pParent->ImageAutoRelease(); // 무한대로 상승하는 메모리 관리
		}
			  
		else if (getMouseWheelDelta(flag) < 0 && pParent->zoomCount != pParent->minZoomCount)
		{
			pParent->zoomCount--;
			pParent->ImageAutoRelease(); // 무한대로 상승하는 메모리 관리
		}
			

		if (pParent->zoomCount == 0)
			pParent->offsetX = pParent->offsetY = 0;

		x = pParent->ptRButtonDown.x;
		y = pParent->ptRButtonDown.y;

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
		pParent->SetHorzSliderCtrlPos(iBarPosX);


		int iBarPosY = pParent->verticalSliderPosCopy - iRButtonOffsetY;
		pParent->SetVertSliderCtrlPos(iBarPosY);
		pParent->InvalidateWindows();
	}
	else if (event == EVENT_RBUTTONDOWN)
		// 더블클릭 동시 입력 시 오류
	{
		pParent->OnRButtonSaveImage();
	}
	else if (event == EVENT_MOUSEMOVE)
	{
		pParent->ptRButtonDown.x = x;
		pParent->ptRButtonDown.y = y;
		pParent->horizontalSliderPosCopy = pParent->horizontalSliderPos;
		pParent->verticalSliderPosCopy = pParent->verticalSliderPos;

	}
}

/*
	cvMouseExtension:		초기화
	~cvMouseExtension:		종료
	windowExists:			OpenCV 창이 존재 유무 검사
*/
cvMouseExtension::cvMouseExtension(String strWindowName, int iFlag)
{
	namedWindow(strWindowName, iFlag);
	setMouseCallback(strWindowName, cvMouseCallBack, this);
	m_strWindowName = strWindowName;


	//Initial values
	zoomCount = 0;
	maxZoomCount = 8; // 줌 카운트 (x10)

	// 기존 x10 배율 경우
	// 5120x5120 (25MB) 이미지 경우 최대 메모리 19.7 GB
	// 축소 시 메모리 해제

	minZoomCount = 0;

	m_Resize.resize(maxZoomCount + 1);

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
	if (!m_strWindowName.empty() && !windowExists(m_strWindowName))
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
	int prop = cv::getWindowProperty(windowName, WND_PROP_VISIBLE);
	if (prop >= 0) // 열려있다면
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*
	LoadPath:					경로로 이미지 불러오기
	LoadImg:					cv::Mat으로 이미지 불러오기

	/사용자 정의/
	ImRead:						경로로 이미지 불러오기
	ImShow:						cv::Mat으로 이미지 불러오기

*/
bool cvMouseExtension::LoadPath(String winname, double InitScale)
{
	SetInitailScale(InitScale);

	Image = imread(winname);

	if (Image.empty())
		return false;
	originalWidth = Image.cols;
	originalHeight = Image.rows;

	Size size = calculateAspectRatio(Image);
	resizeWindow(m_strWindowName, size); // 초기 윈 사이즈 설정

	Size sizeInitial(Image.cols * InitScale, Image.rows * InitScale);
	resize(Image, m_Resize[0], sizeInitial, 0, 0, resizeFlag);
	if (!m_Resize[0].empty())
		imshow(m_strWindowName, m_Resize[0]);

	return !m_Resize[0].empty();
}
bool cvMouseExtension::LoadImg(Mat& mat, double InitScale)
{
	SetInitailScale(InitScale);

	Image = mat.clone();

	if (Image.empty())
		return false;
	originalWidth = Image.cols;
	originalHeight = Image.rows;

	Size size(Image.cols * InitScale, Image.rows * InitScale);

	Size winsize = calculateAspectRatio(Image);
	resizeWindow(m_strWindowName, winsize); // 초기 윈 사이즈 설정

	resize(Image, m_Resize[0], size, 0, 0, resizeFlag);
	if (!m_Resize[0].empty())
		imshow(m_strWindowName, m_Resize[0]);

	return !m_Resize[0].empty();
}

bool cvMouseExtension::ImRead(String winname, double InitScale)
{
	return LoadPath(winname, InitScale);

}
bool cvMouseExtension::ImShow(Mat& mat, double InitScale)
{
	return LoadImg(mat, InitScale);

}

/*
	ShowMessageBox:					메시지 박스 표시
	SetMessageBox:					메시지 박스 위치 설정
	CreateMessageBox:				메시지 박스 생성
	cvGetWindowRect:				OpenCV 창 포인트 얻기
	calculateAspectRatio				화면 비율에 따라 WIN 사이즈 결정
	_gcd						최대공약수 계산
*/
bool cvMouseExtension::ShowMessageBox(String text, Size winsize)
{
	if (SetMessageBox(CreateMessageBox(text, winsize)))
	{
		return true;
	}
}
bool cvMouseExtension::SetMessageBox(Mat curImage)
{
	// 약간의 트릭 
	imshow("Message", curImage);
	Rect windowRect = cvGetWindowRect(m_strWindowName);
	Point bl = windowRect.tl() + Point(0, windowRect.height);
	Point tl = windowRect.tl();

	moveWindow("Message", tl.x, tl.y);
	if (waitKey(420)) // 0.42 sec
	{
		cv::destroyWindow("Message");
		curImage.release();
		return true;
	}
	return false;
}
Mat cvMouseExtension::CreateMessageBox(String text, Size winsize)
{
	int baseline = 0;
	Mat empty = Mat::zeros(winsize, CV_8UC3);
	empty.setTo(Scalar(249, 243, 241));

	Size textSize = getTextSize(text, FONT_HERSHEY_SIMPLEX, 1, 1, &baseline);
	Point textOrg((empty.cols - textSize.width) / 2, (empty.rows + textSize.height) / 2);

	//int padding = 30;
	//Point rectTopLeft(textOrg.x - padding, textOrg.y - textSize.height - padding);
	//Point rectBottomRight(textOrg.x + textSize.width + padding, textOrg.y + baseline + padding);
	//rectangle(empty, rectTopLeft, rectBottomRight, Scalar(0, 255, 255), 2);

	putText(empty, text, textOrg, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);

	return empty;
}
Rect cvMouseExtension::cvGetWindowRect(const string& windowName)
{
	wstring windowNameW(windowName.begin(), windowName.end());

	HWND hwnd = FindWindow(NULL, windowNameW.c_str());
	if (hwnd == NULL)
	{
		return Rect();
	}

	// 윈도우 Rect를 얻음
	RECT rect;
	if (GetWindowRect(hwnd, &rect))
	{
		return Rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	}
	else
	{
		return Rect();
	}
}
Size cvMouseExtension::calculateAspectRatio(const Mat& image)
{
	if (image.empty()) 
	{
		return Size(800, 800); 
		// 비어있으면 기존 800x800으로 결정
	}

	// 바탕화면 사이즈 계산
	RECT desktop;
	GetWindowRect(GetDesktopWindow(), &desktop);
	Size desktopSize(desktop.right, desktop.bottom);

	// 이미지의 가로, 세로 크기
	int width = image.cols;
	int height = image.rows;

	// 이미지 사이즈가 작을 경우 비율은 유지하고 늘림
	width += (width < 300) ? 500 : 0;
	height += (height < 300) ? 500 : 0;

	// 최대 공약수(GCD) 계산
    int gcd = _gcd(width, height);

	// 비율을 계산
	int nWid려
	if (targetWidth > desktopSize.width - _max)
	{
		targetWidth = desktopSize.width - _max;
		targetHeight = static_cast<int>((targetWidth * nHeight) / nWidth);
	}

	if (targetHeight > desktopSize.height - _max)
	{
		targetHeight = desktopSize.height - _max;
		targetWidth = static_cast<int>((targetHeight * nWidth) / nHeight);
	}
	return Size(targetWidth, targetHeight);

}
int cvMouseExtension::_gcd(int a, int b)
{
	while (b != 0) {
		int t = b;
		b = a % b;
		a = t;
	}
	return a;
}
/*
	SetInitailScale:		스케일 설정
	InvalidateWindows:		확대 / 축소 이미지 갱신
	ImageAutoRelease:		확대 /축소에 따른 메모리 해제
	SetHorzSliderCtrlPos:		확대 및 드래그 이동 관련 함수
	SetVertSliderCtrlPos:		확대 및 드래그 이동 관련 함수
	OnRButtonSaveImage:		우 클릭 이미지 저장
*/
void cvMouseExtension::SetInitailScale(double dScale)
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
		Size size(int(newScale * Image.cols), int(newScale * Image.rows));
		//resize (Image, m_Resize[zoomCount], size);
		resize(Image, m_Resize[zoomCount], size, 0, 0, resizeFlag);
	}
	int iW = m_Resize[0].cols - 1, iH = m_Resize[0].rows - 1;//-1: for bar size


	Rect rectShow(Point(horizontalSliderPos, verticalSliderPos), Size(iW, iH));

	imshow(m_strWindowName, m_Resize[zoomCount](rectShow));
}
void cvMouseExtension::ImageAutoRelease()
{
	for (int i = zoomCount + 1; i <= maxZoomCount; ++i)
	{
		if (!m_Resize[i].empty())
		{
			m_Resize[i].release(); // 다음 이미지를 해제
			m_Resize[i] = Mat();   // Mat 객체 초기화
		}
	}
}
void cvMouseExtension::SetHorzSliderCtrlPos(int iPos)
{
	if (iPos > horizontalSliderRangeMax)
		horizontalSliderPos = horizontalSliderRangeMax;
	else if (iPos < 0)
		horizontalSliderPos = horizontalSliderRangeMin;
	else
		horizontalSliderPos = iPos;

}
void cvMouseExtension::SetVertSliderCtrlPos(int iPos)
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
	if ((_waccess(_T(".\\ExtensionSaveImage"), 0)) == -1)
		CreateDirectory(_T(".\\ExtensionSaveImage"), NULL);

	time_t timer;
	timer = time(NULL);
	struct tm t;
	localtime_s(&t, &timer);
	String folderName = "ExtensionSaveImage\\";
	String temp = format("%s%04d%02d%02d%02d%02d%d.bmp", folderName.c_str(), t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

	if (Image.empty())
		return;
	if (m_Resize[zoomCount].empty())
	{
		Size size(int(newScale * Image.cols), int(newScale * Image.rows));
		resize(Image, m_Resize[zoomCount], size, 0, 0, resizeFlag);
	}
	int iW = m_Resize[0].cols, iH = m_Resize[0].rows;
	Rect rectShow(Point(horizontalSliderPos, verticalSliderPos), Size(iW, iH));
	int CurZoomCnt = zoomCount;
	if (windowExists("Message")) // 메시지 박스가 없다면
	{
		if (!m_Resize[CurZoomCnt].empty() &&
			ShowMessageBox("Image Save successful !", Size(500, 70)))
			// 없으면 __acrt_first_block == header 오류
		{
			imwrite(temp, m_Resize[CurZoomCnt](rectShow));
		}
	}
}

/*
	void cvExtensionWindow(String winname, Mat& image, int ms = 0);

	void cvExtensionWindow(String winname, Mat& image, int ms)
	{
		cvMouseExtension window(winname);
		window.ImShow(image); // cv::Mat으로 불러오기
		window.Wait(ms);
	}

	Ctrl + Click
	https://github.com/arson5012/cvMouseExtension
*/
