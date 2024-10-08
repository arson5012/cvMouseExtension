# cvMouseExtension

* #### 💡 개요
  * 기존 ```cv::namedWindow();``` 에 이미지 확대/축소, 저장 기능을 추가한 확장 코드</br>
  (이미지 사이즈가 테스크톱 해상도보다 크면 보기 불편한 문제점을 보완)
   
* #### 🌍 환경 & 호환성  
  * OS:	MS Windows 10 ~
  * Language : C++ 11 ~
  * Opencv version : 4.6.0 ~
  * MFC 아닌 환경에서도 사용가능 (수정완료)

---
<!-------------------------------------------------------------Part 1------------------------------------------------------------------------------------------>

 ## 1. 기능 설명

![Demo](./src/Demo.gif)


 1. __마우스 휠 (Mouse Wheel Up / Down)__  
    마우스 커서 위치로 확대(Zoom in), 축소(Zoom out) (이)가 가능하다. 

       
 2. __마우스 드래그 (Mouse LButton Down & Drag)__      
    마우스 휠로 확대를 한 경우 마우스 왼쪽을 누른 상태로 드래그하여 화면을 이동할 수 있다.
      
 
 3. __마우스 우클릭 (Mouse RButton Down)__      
    마우스 우클릭 시 현재(확대 / 축소) 이미지를 현재시간 이름으로 디렉터리에</br> "cvExtension" 폴더를 생성 후 폴더에 이미지를 저장하고 저장 완료 메시지 박스를 표시한다.  
    
 <!-------------------------------------------------------------Part 2------------------------------------------------------------------------------------------>
 
## 2. 사용 방법
   * __설치(Install)__   
      A. 파일 디렉터리에 ```cvMouseExtension.cpp```와 ```cvMouseExtension.h``` 추가  
      B. 솔루션 탐색기⇾ 프로젝트 우클릭⇾ 추가⇾ 기존 항목⇾ ```cvMouseExtension.cpp```와 ```cvMouseExtension.h``` 추가   
      C. 헤더 파일 또는 .cpp에  ```#include cvMouseExtension.h``` 추가  

   * __사용 예시(Example)__
		>	```cpp
		>	void 클래스::함수명(String winname, Mat& image, int ms)
		>	{
		>		cvMouseExtension window(winname);
		>		window.ImShow(image); // cv::Mat으로 불러오기
		>		waitKey(ms);
		>	}
		>	```
		>	또는,
		>	```cpp
		>	void 클래스::함수명(String Path, String winname, int ms)
		>	{
		>		cvMouseExtension window(winname);
		>		window.ImRead(Path); // 경로로 불러오기
		>		waitKey(ms);
		>	}
		>	```
		>	
		> 트랙바 사용 예시

		>	```cpp
		>	cvMouseExtension window("이름", WINDOW_NORMAL); // 플래그는 초기값 1
		>	window.SetInitailScale(1); // 초기 배율 설정 1 = 배율 없음
		>	window.ImRead(path); // 파일 경로 string 또는 cv::String 형식
		>	
		>	/*
		>	트랙바 생성 또는 
		>	Window 관련 코드 추가
		>	*/
		>	
		>	waitKey(0);
		>	```
## 3. Reference 
 [Reference : Github Repositories](https://github.com/DennisLiu1993/Zoom-In-Out-with-OpenCV)


---

+PS : 저장 기능은 Imshow 에서 Ctrl + S 를 누르면 동일하게 나온다 (단, 확대 축소가 불가능 하므로 확대된 상태에서의 이미지는 아님)
