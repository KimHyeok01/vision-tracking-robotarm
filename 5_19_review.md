python3 -m pip install --upgrade pip setuptools wheel
python3 -m pip install --upgrade uv uv-build
python3 -m pip install onshape-to-robot
#순서대로 컨테이너에서 실행, pip 관련 유틸 업그레이드 하는 명령어임

python3 -m pip install git+https://github.com/Rhoban/onshape-to-robot.git
#onshape-to-robot 패키지 설치 명령어임

onshape-to-robot --help
#정상적으로 설치됐으면 옵션 주르륵 뜰거임

nano ~/.bashrc
#bash.rc 연 후, 가장 마지막 줄에

export ONSHAPE_API=https://cad.onshape.com
export ONSHAPE_ACCESS_KEY=발급받은_Access_Key
export ONSHAPE_SECRET_KEY=발급받은_Secret_Key

source ~/.bashrc
#.bashrc 재시작 하기

echo $ONSHAPE_API
echo $ONSHAPE_ACCESS_KEY
#환경변수 업데이트됐는지 확인하기

my-robot/config.json
#my-robot은 그냥 디렉토리명이니 알아서 쓰고, config.json을 제작해야함 이제

{
  "documentId": "a60c130b936cc49845e64be8",
  "workspaceId": "5dd6517471cd705938c34336",
  "elementId": "e5d6cfc85c2bedf0dabfbb5a",
  "outputFormat": "urdf",
  "robotName": "vision_tracking_robotarm"
}
#이건 config.json 에시파일 , 아까 햇던거보다 설정한게 많아졌는데, 아까 너무 조금설정한게 문제였는지는 아직 모르겠음
#이후 작업 디렉토리(eg my-robot)의 바로 상위 디렉토리에서 onshape-to-robot my-robot 명령어를 하면 됨
#아까 여기까지 하고 실패함.

