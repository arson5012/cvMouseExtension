#include "pch.h"
#include "cvMouseExtension.h"

/*
	���콺 �̺�Ʈ
	��:						Ȯ��/���
	���콺 �̵�:			Ȯ��/��� �� Ŀ�� ��ġ�κ��� Ȯ��/���
	�� Ŭ�� �� �巡��:		Ȯ�� �� ȭ�� �̵�
	�� Ŭ��:				Ȯ��/��� ������ �̹��� ����
*/
void cvMouseCallBack (int event, int x, int y, int flag, void* pUserData)
{
	cvMouseExtension* pParent = (cvMouseExtension*)pUserData;

	//bool IsCtrl = (flag & EVENT_FLAG_CTRLKEY) != 0;
	if (event == EVENT_MOUSEWHEEL)
	{
		if (getMouseWheelDelta(flag) > 0 && pParent->zoomCount != pParent->maxZoomCount)
			pParent->zoomCount++;
		else if (getMouseWheelDelta(flag) < 0 && pParent->zoomCount != pParent->minZoomCount)
			pParent->zoomCount--;

		if (pParent->zoomCount == 0)
			pParent->offsetX = pParent->offsetY = 0;

		x = pParent->ptRButtonDown.x;
		y = pParent->ptRButtonDown.y;

		// double������ ĳ�����Ͽ� �����÷� ����
		double horzSliderBarPos = static_cast<double>(pParent->horizontalSliderPos);
		double vertSliderBarPos = static_cast<double>(pParent->verticalSliderPos);

		double dPixelX = (horzSliderBarPos + x + pParent->offsetX) / pParent->newScale;
		double dPixelY = (vertSliderBarPos + y + pParent->offsetY) / pParent->newScale;

		pParent->newScale = pParent->InitScale * pow(pParent->scaleRatio, pParent->zoomCount);

		int iW = pParent->originalWidth;
		int iH = pParent->originalHeight;
		pParent->horizontalSliderRangeMax = int(pParent->newScale * iW - pParent->InitScale * iW);
		pParent->verticalSliderRangeMax = int(pParent->newScale * iH - pParent->InitScale * iH);

		// double������ ĳ�����Ͽ� �����÷� ����
		int iBarPosX = static_cast<int>(dPixelX * pParent->newScale - x + 0.5);
		int iBarPosY = static_cast<int>(dPixelY * pParent->newScale - y + 0.5);

		pParent->SetHorzSliderCtrlPos(iBarPosX);
		pParent->SetVertSliderCtrlPos(iBarPosY);

		// double������ ĳ�����Ͽ� �����÷� ����
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
		// ����Ŭ�� ���� �Է� �� ����
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
	cvMouseExtension:		�ʱ�ȭ
	~cvMouseExtension:		����
	windowExists:			OpenCV â�� ���� ���� �˻�
*/
cvMouseExtension::cvMouseExtension(String strWindowName, int iFlag)
{
	namedWindow (strWindowName, iFlag);
	resizeWindow(strWindowName, cv::Size(800, 800)); // �ʱ� �� ������ ����
	setMouseCallback (strWindowName, cvMouseCallBack, this);
	m_strWindowName = strWindowName;


	//Initial values
	zoomCount = 0;
	maxZoomCount = 10; // �� ī��Ʈ (x10)
	minZoomCount = 0;

	m_Resize.resize (maxZoomCount + 1);

	offsetX = 0;
	offsetY = 0;
	InitScale = 1;
	newScale = 1;
	scaleRatio = 1.25; // �� ������

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
	if (prop >= 0) // �����ִٸ�
	{
		return true;
	}
	else 
	{
		return false;
	}
}

/*
	LoadPath:					��η� �̹��� �ҷ�����
	LoadImg:					cv::Mat���� �̹��� �ҷ�����

	/����� ����/
	ImRead:						��η� �̹��� �ҷ�����
	ImShow:						cv::Mat���� �̹��� �ҷ�����
*/
bool cvMouseExtension::LoadPath(String winname, double InitScale)
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
bool cvMouseExtension::LoadImg(Mat& mat, double InitScale)
{
	SetInitailScale(InitScale);
	
	Image = mat.clone();

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

bool cvMouseExtension::ImRead(String winname, double InitScale)
{
	SetInitailScale(InitScale);

	Image = imread(winname);

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

	Image = mat.clone();

	if (Image.empty())
		return false;
	originalWidth = Image.cols;
	originalHeight = Image.rows;

	Size size(Image.cols * InitScale, Image.rows * InitScale);
	resize(Image, m_Resize[0], size, 0, 0, resizeFlag);
	if (!m_Resize[0].empty())
		imshow(m_strWindowName, m_Resize[0]);

	return !m_Resize[0].empty();
}

/*
	ShowMessageBox:					�޽��� �ڽ� ǥ��
	SetMessageBox:					�޽��� �ڽ� ��ġ ����
	CreateMessageBox:				�޽��� �ڽ� ����
	cvGetWindowRect:				OpenCV â ����Ʈ ���
*/
bool cvMouseExtension::ShowMessageBox(String text, Size winsize)
{
	if (SetMessageBox(CreateMessageBox(text, winsize)))
	{
		return true;
	}
}
bool cvMouseExtension::SetMessageBox(Mat& curImage)
{	
	// �ణ�� Ʈ�� 
	imshow("Message", curImage);
	Rect windowRect = cvGetWindowRect(m_strWindowName);
	Point bl = windowRect.tl() + Point(0, windowRect.height);
	Point tl = windowRect.tl();
	//if (windowRect.width < curImage.cols || 
	//	windowRect.height < curImage.rows) // â�� �۾��� ��
	//{
	//	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	//	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	//	int width = curImage.cols;
	//	int height = curImage.rows;
	//
	//	int cx = (screenWidth - width) / 2;
	//	int cy = (screenHeight - height) / 2;
	//
	//	// â ��ġ ȭ�� ���߾����� ����
	//	moveWindow("Message", cx, cy);
	//}
	//int x = windowRect.tl().x + (windowRect.width / 5);
	//int y = windowRect.tl().y + (windowRect.height / 3);

	//moveWindow("Message", bl.x, bl.y - 110);
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

	// ������ Rect�� ����
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

/*
	SetInitailScale:		������ ���� 
	InvalidateWindows:		Ȯ��/ ��� �̹��� ����
	SetHorzSliderCtrlPos:	Ȯ�� �� �巡�� �̵� ���� �Լ�
	SetVertSliderCtrlPos:	Ȯ�� �� �巡�� �̵� ���� �Լ�
	OnRButtonSaveImage:		�� Ŭ�� �̹��� ���� 
*/
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
	if ((_waccess(_T(".\\cvExtension"), 0)) == -1)
		CreateDirectory(_T(".\\cvExtension"), NULL);

	time_t timer;
	timer = time(NULL);
	struct tm t;
	localtime_s(&t, &timer);
	String folderName = "cvExtension\\";
	String temp = format("%s%04d%02d%02d%02d%02d%d.bmp", folderName.c_str(), t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

	if (Image.empty())
		return;
	if (m_Resize[zoomCount].empty())
	{
		Size size(int(newScale * Image.cols), int(newScale * Image.rows));
		resize(Image, m_Resize[zoomCount], size, 0, 0, resizeFlag);
	}
	int iW = m_Resize[0].cols - 1, iH = m_Resize[0].rows - 1;//-1: for bar size
	Rect rectShow(Point(horizontalSliderPos, verticalSliderPos), Size(iW, iH));
	int CurZoomCnt = zoomCount;
	if (windowExists("Message")) // �޽��� �ڽ��� ���ٸ�
	{
		if (!m_Resize[CurZoomCnt].empty() &&
			ShowMessageBox("Image Save successful !", Size(500, 70)))
			// ������ __acrt_first_block == header ����
		{
			imwrite(temp, m_Resize[CurZoomCnt](rectShow));
		}
	}
}


/*
	void cvExtensionWindow(String winname, Mat& image, int Wait = 0);
	
	void cvExtensionWindow(String winname, Mat& image, int Wait)
	{
		cvMouseExtension window(winname);
		window.ImShow(image); // cv::Mat���� �ҷ�����
		cv::waitKey(Wait);
	}

	Ctrl + Click
	https://github.com/arson5012/cvMouseExtension
*/