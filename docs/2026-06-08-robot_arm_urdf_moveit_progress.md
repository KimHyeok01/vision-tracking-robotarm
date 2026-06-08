# Robot Arm URDF / RViz2 / MoveIt2 작업 정리

## 1. 작업 목표

이번 작업의 목표는 Onshape 기반 로봇팔 모델을 ROS2에서 사용할 수 있도록 구성하고, RViz2와 MoveIt2에서 확인하는 것이다.

전체 흐름은 다음과 같다.

```text
Onshape 모델
→ onshape-to-robot으로 URDF/STL 추출
→ ROS2 description 패키지 생성
→ RViz2에서 로봇 시각화
→ MoveIt2 설정 패키지 생성
→ Motion Planning 테스트
→ URDF joint 위치와 회전축 보정
```

현재는 MoveIt2 실행과 Planning Group 구성까지 완료했고, 다음 단계는 `robot_moveit.urdf`의 joint `origin`과 `axis`를 실제 로봇팔 구조에 맞게 보정하는 것이다.

---

## 2. 작업 환경

작업은 Docker Desktop 컨테이너 안에서 진행했다.

주요 경로는 다음과 같다.

```bash
/root/ros2_ws/vision-tracking-robotarm
```

ROS2 패키지 경로:

```bash
/root/ros2_ws/vision-tracking-robotarm/src/robot_arm_description
```

MoveIt2 설정 패키지 경로:

```bash
/root/ros2_ws/vision-tracking-robotarm/src/robot_arm_moveit_config
```

---

## 3. GitHub 저장소 사용

저장소 clone:

```bash
git clone https://github.com/KimHyeok01/vision-tracking-robotarm.git
cd vision-tracking-robotarm
```

작업 후 GitHub에 올리는 기본 흐름:

```bash
git status
git add .
git commit -m "Add robot arm URDF and MoveIt2 configuration"
git push origin main
```

만약 push 권한이 없으면 GitHub 저장소 collaborator 권한이 필요하다.

---

## 4. Onshape API Key 등록

`onshape-to-robot`을 사용하기 위해 Onshape API Key를 `.bashrc`에 등록했다.

```bash
nano ~/.bashrc
```

아래 내용을 추가했다.

```bash
export ONSHAPE_ACCESS_KEY="ACCESS_KEY"
export ONSHAPE_SECRET_KEY="SECRET_KEY"
```

적용:

```bash
source ~/.bashrc
```

확인:

```bash
echo $ONSHAPE_ACCESS_KEY
echo $ONSHAPE_SECRET_KEY
```

주의: API Key는 GitHub에 올리면 안 된다.

---

## 5. onshape-to-robot 설치 문제 해결

설치 중 다음 오류가 발생했다.

```text
RuntimeError: uv-build was not properly installed
metadata-generation-failed
```

이를 해결하기 위해 pip 관련 패키지를 업데이트했다.

```bash
python3 -m pip install --upgrade pip setuptools wheel
python3 -m pip install --upgrade uv uv-build
```

또한 NumPy 2.x 호환 문제를 피하기 위해 NumPy를 1.x로 고정했다.

```bash
pip install numpy==1.23.5 --force-reinstall
```

그 후 `onshape-to-robot`을 설치했다.

```bash
pip install onshape-to-robot
```

---

## 6. ROS2 description 패키지 생성

로봇팔 URDF와 STL 파일을 관리하기 위해 `robot_arm_description` 패키지를 생성했다.

```bash
cd /root/ros2_ws/vision-tracking-robotarm/src
ros2 pkg create robot_arm_description --build-type ament_cmake
```

필요한 폴더 생성:

```bash
cd /root/ros2_ws/vision-tracking-robotarm/src/robot_arm_description
mkdir -p urdf meshes launch rviz
```

---

## 7. package.xml 수정

`package.xml`에서 license를 `MIT`로 수정했다.

```xml
<license>MIT</license>
```

필요한 실행 의존성도 추가했다.

```xml
<exec_depend>robot_state_publisher</exec_depend>
<exec_depend>joint_state_publisher_gui</exec_depend>
<exec_depend>rviz2</exec_depend>
```

---

## 8. CMakeLists.txt 수정

`urdf`, `meshes`, `launch`, `rviz` 폴더가 install 공간에 복사되도록 설정했다.

```cmake
cmake_minimum_required(VERSION 3.8)
project(robot_arm_description)

find_package(ament_cmake REQUIRED)

install(
  DIRECTORY urdf meshes launch rviz
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
```

---

## 9. Onshape에서 URDF 추출

`robot_arm_description` 패키지 안에 `config.json`을 만들었다.

```bash
nano config.json
```

내용:

```json
{
  "documentId": "a60c130b936cc49845e64be8",
  "workspaceId": "5dd6517471cd705938c34336",
  "elementId": "e5d6cfc85c2bedf0dabfbb5a",
  "robotName": "robot_arm_description",
  "outputFormat": "urdf"
}
```

URDF 추출:

```bash
onshape-to-robot .
```

생성된 파일:

```text
robot.urdf
assets/
```

패키지 구조에 맞게 파일 이동:

```bash
mv robot.urdf urdf/
mv assets/* meshes/
rmdir assets
```

mesh 경로 수정:

```bash
sed -i 's|assets/|package://robot_arm_description/meshes/|g' urdf/robot.urdf
```

잘못된 중복 경로가 생겼을 때 수정:

```bash
sed -i 's|package://package://robot_arm_description/meshes/|package://robot_arm_description/meshes/|g' urdf/robot.urdf
```

---

## 10. Onshape 추출 경고

`onshape-to-robot` 실행 중 다음 경고가 발생했다.

```text
WARNING: Parts with same name "link2", incrementing STL name to "link2__2"
WARNING: Multiple base links detected, which is not supported by URDF.
Only the first base link will be considered.
```

의미:

* `link2`라는 이름의 부품이 여러 개 있어 `link2__2.stl`이 생성됨
* Onshape Assembly의 Mate 연결이 제대로 되어 있지 않아 여러 base link처럼 감지됨
* URDF는 하나의 root link만 지원하므로 첫 번째 base link만 사용됨

결과적으로 자동 생성된 URDF에는 `base`와 `servo1` 정도만 포함되었고, 나머지 부품은 직접 URDF로 구성해야 했다.

---

## 11. 보유한 STL 파일

`meshes` 폴더에는 다음 파일들이 있었다.

```text
base.part
joint.part
link1.part
link2__2.part
link2.part
rotate_base.part
servo1.part

base.stl
joint.stl
link1.stl
link2__2.stl
link2.stl
rotate_base.stl
servo1.stl
```

MoveIt2용으로 사용한 주요 STL 파일은 다음과 같다.

```text
base.stl
servo1.stl
rotate_base.stl
joint.stl
link1.stl
link2.stl
link2__2.stl
```

---

## 12. RViz2 launch 파일

`robot_description`을 문자열로 넘기기 위해 `ParameterValue(..., value_type=str)`를 사용했다.

```python
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import Command
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import PathJoinSubstitution
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description():
    urdf_file = PathJoinSubstitution([
        FindPackageShare('robot_arm_description'),
        'urdf',
        'robot_moveit.urdf'
    ])

    robot_description = ParameterValue(
        Command(['cat ', urdf_file]),
        value_type=str
    )

    return LaunchDescription([
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            parameters=[{
                'robot_description': robot_description
            }],
            output='screen'
        ),

        Node(
            package='joint_state_publisher_gui',
            executable='joint_state_publisher_gui',
            output='screen'
        ),

        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            output='screen'
        )
    ])
```

---

## 13. Docker Desktop에서 RViz2 실행

Docker Desktop 안에서 RViz2를 실행하기 위해 GUI 전달 설정을 했다.

Windows Docker Desktop 기준:

```bash
export DISPLAY=host.docker.internal:0.0
export LIBGL_ALWAYS_SOFTWARE=1
```

Mac Docker Desktop 기준:

```bash
export DISPLAY=host.docker.internal:0
export LIBGL_ALWAYS_SOFTWARE=1
```

실행:

```bash
ros2 launch robot_arm_description display.launch.py
```

---

## 14. robot_flat.urdf

처음에는 모든 STL을 하나의 `base` 링크 안에 넣은 `robot_flat.urdf`를 만들었다.

이 구조는 RViz에서 전체 모양을 보기 위한 용도이다.

```text
base
 ├── base.stl
 ├── servo1.stl
 ├── rotate_base.stl
 ├── joint.stl
 ├── link1.stl
 ├── link2.stl
 └── link2__2.stl
```

하지만 이 방식은 joint가 없기 때문에 MoveIt2에는 적합하지 않다.

또한 TF 문제를 해결하기 위해 `world -> base` fixed joint를 추가했다.

```xml
<link name="world"/>

<joint name="world_to_base" type="fixed">
  <parent link="world"/>
  <child link="base"/>
  <origin xyz="0 0 0" rpy="0 0 0"/>
</joint>
```

RViz Fixed Frame은 `world`로 설정했다.

---

## 15. robot_moveit.urdf

MoveIt2에서 사용하기 위해 link와 joint가 분리된 `robot_moveit.urdf`를 만들었다.

구조:

```text
world
 └── base
      └── servo1
           └── rotate_base
                └── joint
                     └── link1
                          └── link2
                               └── link2_2
```

움직이는 joint:

```text
servo1_to_rotate_base
rotate_base_to_joint
joint_to_link1
link1_to_link2
```

예시 joint:

```xml
<joint name="servo1_to_rotate_base" type="revolute">
  <parent link="servo1"/>
  <child link="rotate_base"/>
  <origin xyz="0 0 0" rpy="0 0 0"/>
  <axis xyz="0 0 1"/>
  <limit lower="-3.14" upper="3.14" effort="10" velocity="1.0"/>
</joint>
```

바닥 회전 관절은 일반적으로 Z축 회전으로 설정했다.

```xml
<axis xyz="0 0 1"/>
```

어깨/팔꿈치 관절은 우선 Y축 회전으로 설정했다.

```xml
<axis xyz="0 1 0"/>
```

이 값들은 실제 로봇 구조에 맞게 추가 조정이 필요하다.

---

## 16. MoveIt Setup Assistant

MoveIt Setup Assistant 실행:

```bash
ros2 launch moveit_setup_assistant setup_assistant.launch.py
```

진행 순서:

```text
Create New MoveIt Configuration Package
→ robot_moveit.urdf 로드
→ Self-Collisions에서 Generate Collision Matrix
→ Planning Groups에서 arm 그룹 생성
→ Add Joints로 revolute joint 선택
→ Robot Poses에서 home pose 생성
→ Author Information 입력
→ Configuration Files에서 robot_arm_moveit_config 생성
```

URDF 경로:

```text
/root/ros2_ws/vision-tracking-robotarm/src/robot_arm_description/urdf/robot_moveit.urdf
```

Planning Group 이름:

```text
arm
```

선택한 joint:

```text
servo1_to_rotate_base
rotate_base_to_joint
joint_to_link1
link1_to_link2
```

생성된 MoveIt config 패키지:

```text
/root/ros2_ws/vision-tracking-robotarm/src/robot_arm_moveit_config
```

주요 launch 파일:

```text
demo.launch.py
moveit_rviz.launch.py
move_group.launch.py
rsp.launch.py
spawn_controllers.launch.py
static_virtual_joint_tfs.launch.py
warehouse_db.launch.py
```

---

## 17. MoveIt2 실행 문제 해결

MoveIt2 실행:

```bash
ros2 launch robot_arm_moveit_config demo.launch.py
```

처음에는 다음 문제가 발생했다.

```text
PlanningScene
No Planning Scene Loaded
```

원인은 `/move_group` 노드가 죽었기 때문이다.

로그 핵심:

```text
parameter 'robot_description_planning.joint_limits.servo1_to_rotate_base.max_velocity'
has invalid type: expected [double] got [integer]
```

해결을 위해 `joint_limits.yaml`에서 정수 값을 실수로 수정했다.

```bash
cd /root/ros2_ws/vision-tracking-robotarm

sed -i 's/max_velocity: 1$/max_velocity: 1.0/g' src/robot_arm_moveit_config/config/joint_limits.yaml
sed -i 's/max_acceleration: 0$/max_acceleration: 0.0/g' src/robot_arm_moveit_config/config/joint_limits.yaml
```

확인:

```bash
grep -n "max_" src/robot_arm_moveit_config/config/joint_limits.yaml
```

빌드:

```bash
rm -rf build/robot_arm_moveit_config install/robot_arm_moveit_config log
colcon build
source install/setup.bash
```

다시 실행:

```bash
ros2 launch robot_arm_moveit_config demo.launch.py
```

노드 확인:

```bash
ros2 node list
```

`/move_group`이 보이면 정상이다.

---

## 18. ros2_control 관련 문제

MoveIt demo 실행 중 다음 오류도 발생했다.

```text
no ros2_control tag
```

이는 URDF에 `<ros2_control>` 태그가 없는데 `ros2_control_node`가 실행되어 생기는 문제다.

현재 목표가 MoveIt2에서 Plan 테스트라면 당장 해결하지 않아도 된다.

나중에 Execute까지 하려면 다음 작업이 필요하다.

```text
URDF에 ros2_control 태그 추가
ros2_controllers.yaml 작성
joint_state_broadcaster 설정
joint_trajectory_controller 설정
```

---

## 19. 현재 완료된 작업

현재까지 완료한 작업은 다음과 같다.

```text
GitHub 저장소 clone
Onshape API Key 설정
onshape-to-robot 설치 및 실행
robot_arm_description 패키지 생성
URDF/STL 파일 구조 정리
RViz2 launch 파일 작성
Docker Desktop에서 RViz2 실행
robot_flat.urdf 작성
robot_moveit.urdf 작성
MoveIt Setup Assistant 실행
robot_arm_moveit_config 생성
joint_limits.yaml 타입 문제 수정
MoveIt2 demo 실행
Planning Group arm 구성
```

---

## 20. 다음 작업: joint 위치와 회전축 조정

다음 단계는 `robot_moveit.urdf`의 joint 위치와 회전축을 맞추는 것이다.

수정할 파일:

```bash
cd /root/ros2_ws/vision-tracking-robotarm/src/robot_arm_description
nano urdf/robot_moveit.urdf
```

주요 수정 대상:

```xml
<origin xyz="..." rpy="..."/>
<axis xyz="..."/>
```

의미:

```text
origin xyz = 부모 링크 기준 관절 위치
origin rpy = 자식 링크의 초기 회전 방향
axis       = revolute joint의 회전축
```

확인용 실행:

```bash
cd /root/ros2_ws/vision-tracking-robotarm

rm -rf build/robot_arm_description install/robot_arm_description log
colcon build --packages-select robot_arm_description
source install/setup.bash

ros2 launch robot_arm_description display.launch.py
```

RViz에서 확인:

```text
Fixed Frame = world
joint_state_publisher_gui에서 슬라이더 조작
```

확인 순서:

```text
1. servo1_to_rotate_base
   → 바닥 회전처럼 도는지 확인

2. rotate_base_to_joint
   → 어깨 관절처럼 위아래로 접히는지 확인

3. joint_to_link1
   → 다음 링크가 자연스럽게 따라오는지 확인

4. link1_to_link2
   → 팔꿈치처럼 접히는지 확인
```

판단 기준:

```text
부품이 떨어져 있다
→ origin xyz 문제

부품 방향이 90도 틀어져 있다
→ origin rpy 문제

슬라이더를 움직였을 때 엉뚱한 방향으로 돈다
→ axis 문제
```

각도 참고:

```text
1.5708  = 90도
3.14159 = 180도
```

---

## 21. 최종 정리

자동 추출된 URDF는 Onshape Assembly Mate 문제로 인해 base 중심의 불완전한 구조만 생성되었다. CAD 파일을 직접 수정할 수 없었기 때문에 STL 파일을 기반으로 `robot_flat.urdf`와 `robot_moveit.urdf`를 수동으로 구성했다.

`robot_flat.urdf`는 RViz 시각화용이고, `robot_moveit.urdf`는 MoveIt2용이다.

현재 MoveIt2 실행까지 성공했으며, 앞으로는 joint `origin`과 `axis`를 조정하여 로봇팔 움직임을 실제 구조에 가깝게 만드는 작업이 필요하다.
