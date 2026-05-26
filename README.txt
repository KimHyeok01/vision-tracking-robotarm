OpenCV 4.5.4 for Visual Studio

파일명:
opencv-4.5.4-vs2022-x64.zip

환경:
- Windows
- Visual Studio 2022
- x64
- OpenCV 4.5.4

설치 위치:
1. zip 파일을 압축 해제한다.
2. 압축 해제한 opencv-4.5.4 폴더를 C드라이브 바로 아래로 옮긴다.
3. 최종 경로는 아래와 같아야 한다.

C:\opencv-4.5.4

Visual Studio 설정 방법:

1. 프로젝트 플랫폼 설정
- Visual Studio 상단에서 플랫폼을 x64로 설정한다.
- Win32 또는 x86으로 설정하면 안 된다.

2. include 경로 설정
프로젝트 우클릭
→ 속성
→ C/C++
→ 일반
→ 추가 포함 디렉터리

아래 경로 추가:

C:\opencv-4.5.4\build\include

3. lib 경로 설정
프로젝트 우클릭
→ 속성
→ 링커
→ 일반
→ 추가 라이브러리 디렉터리

아래 경로 추가:

C:\opencv-4.5.4\build\x64\vc15\lib

4. 추가 종속성 설정
프로젝트 우클릭
→ 속성
→ 링커
→ 입력
→ 추가 종속성

아래 항목 추가:

opencv_world454.lib

5. dll 설정
프로그램 실행 시 아래 경로에 있는 dll 파일이 필요하다.

C:\opencv-4.5.4\build\x64\vc15\bin

방법 1:
아래 파일을 exe 파일이 생성되는 폴더에 복사한다.

opencv_world454.dll

예시 exe 위치:
프로젝트폴더\x64\Release\

방법 2:
Windows 환경 변수 Path에 아래 경로를 추가한다.

C:\opencv-4.5.4\build\x64\vc15\bin

주의사항:
- 이 파일은 x64 기준이다.
- Visual Studio 프로젝트도 반드시 x64로 설정해야 한다.
- Release 설정에서는 opencv_world454.lib를 사용한다.
- Debug 설정에서는 opencv_world454d.lib가 필요할 수 있다.
- Debug용 lib/dll이 없는 경우 Visual Studio 빌드 구성을 Release로 맞춰서 사용한다.

확인해야 할 최종 경로:

include:
C:\opencv-4.5.4\build\include

lib:
C:\opencv-4.5.4\build\x64\vc15\lib

bin:
C:\opencv-4.5.4\build\x64\vc15\bin

추가 종속성:
opencv_world454.lib

필요 dll:
opencv_world454.dll