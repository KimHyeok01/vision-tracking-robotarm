# ROS2 Humble Docker Environment

Ubuntu 22.04 기반 ROS2 Humble + MoveIt2 + Gazebo 개발환경입니다.

## Requirements

* Docker Desktop
* Git

ros_team_env 폴더를 홈디렉토리(유저이름) 폴더에 놓고, 도커 데스크탑 실행 후 아래대로 실행?

---

# Clone

```bash
git clone https://github.com/KimHyeok01/vision-tracking-robotarm.git
cd vision-tracking-robotarm
```

---

# Build Docker Image

```bash
docker compose build
```

---

# Run Container
1회 테스트용
```bash
docker compose run --rm ros2

```
## 
컨테이너 백그라운드 설정
```bash
# 처음 한 번 세팅
docker compose up -d  

# 작업마다 실행
docker compose exec ros2 bash
```
정상적으로 실행되면 아래와 비슷한 프롬프트가 나타납니다.

```bash
root@xxxxxxxx:/root/ros2_ws#
```

---

# Environment Check

컨테이너 안에서:

```bash
echo $ROS_DISTRO
echo $RMW_IMPLEMENTATION
uname -m
gazebo --version
which rviz2
```

정상 출력 예시:

```bash
humble
rmw_fastrtps_cpp
x86_64
```

---

# ROS2 Workspace

ROS2 패키지는 아래 경로에 추가합니다.

```bash
workspace/src/
```

---

# Build ROS2 Packages

컨테이너 안에서:

```bash
cd /root/ros2_ws
colcon build
source install/setup.bash
```

---

# Notes

* 기본 DDS는 FastDDS(`rmw_fastrtps_cpp`)를 사용합니다.
* Mac Apple Silicon에서는 RViz/Gazebo GUI가 정상 동작하지 않을 수 있습니다.
* Windows에서는 WSL2 기반 Docker 사용을 권장합니다.
* Windows CMD 환경에서 RViz GUI를 사용하려면 VcXsrv 등의 X11 서버가 필요할 수 있습니다.



