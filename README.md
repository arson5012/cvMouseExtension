# cvMouseExtension
### WARNING : Opencv를 사용하는 프로젝트만 사용 가능 !
### WARNING : This project is only compatible with projects using OpenCV !  
---
* #### 💡 개요
  * 기존 ```cv::namedWindow();``` 사용 시 이미지 사이즈가 
테스크톱 해상도보다 크면  
최대화를 해도 잘 보이지 않는 문제점을 보완한 확장 코드
기존 Reference 코드에서 기능 수정 및 버그 수정   
개인 저장용 repositories      
   
* #### 🌍 환경
  * Language : C++
  * Opencv version : 4.6.0 ~

---
<!-------------------------------------------------------------Part 1------------------------------------------------------------------------------------------>

 ## 1. 기능 설명


 1. __마우스 휠 (Mouse Wheel Up / Down)__  
    마우스 커서 위치로 확대(Zoom in), 축소(Zoom out) (이)가 가능하다. 

       
 2. __마우스 드래그 (Mouse LButton Down & Drag)__      
    마우스 휠로 확대를 한 경우 마우스 왼쪽을 누른 상태로 드래그하여 화면을 이동할 수 있다.
     

 <!-------------------------------------------------------------Part 2------------------------------------------------------------------------------------------>
 
## 2. 사용 방법
   * __설치(Install)__   
      A. 파일 디렉터리에 ```cvMouseExtension.cpp```와 ```cvMouseExtension.h``` 추가  
      B. 솔루션 탐색기⇾ 프로젝트 우클릭⇾ 추가⇾ 기존 항목⇾ ```cvMouseExtension.cpp```와 ```cvMouseExtension.h``` 추가   
      C. 헤더 파일 또는 .cpp에  ```#include cvMouseExtension.h``` 추가  

   * __사용 예시(Example)__
		>	```cpp
		>	void 클래스::함수명(String winname, Mat& image)
		>	{
		>		cvMouseExtension cvWin(winname);
		>		cvWin.ImShow(image); // cv::Mat으로 불러오기
		>		cv::waitKey(0);
		>	}
		>	```
		>	또는,
		>	```cpp
		>	void 클래스::함수명(String Path, String winname)
		>	{
		>		cvMouseExtension cvWin(winname);
		>		cvWin.ImRead(Path); // 경로로 불러오기
		>		cv::waitKey(0);
		>	}
		>	```
		>	
		> 트랙바 사용 예시

		>	```cpp
		>	cvMouseExtension cvWin("이름", WINDOW_NORMAL); // 플래그는 초기값 1
		>	cvWin.SetInitailScale(1); // 초기 배율 설정 1 = 배율 없음
		>	cvWin.ImRead(path); // 파일 경로 string 또는 cv::String 형식
		>	
		>	/*
		>	트랙바 생성 또는 
		>	Window 관련 코드 추가
		>	*/
		>	
		>	waitKey(0); // 입력 대기
		>	```
## 3. Reference 
 [Reference : Github Repositories](https://github.com/DennisLiu1993/Zoom-In-Out-with-OpenCV)



