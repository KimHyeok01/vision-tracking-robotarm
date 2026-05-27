## Project Directory Structure (프로젝트 폴더 구조)

본 프로젝트는 복합 시스템(기구, 펌웨어, 소프트웨어) 레이어로 구성되어 있으며, 개발 효율성을 위해 각 파트별 작업 환경을 분리하여 관리합니다.

```text
vision-tracking-robotarm/
├──  ros2_team_env/      # ROS2 개발 및 팀 공통 도커 환경 설정 (Ubuntu 22.04 기반)
├──  opencv/             # OpenCV 기반 영상 처리 및 타겟 추적 알고리즘 소스
├──  stm32_firmware/     # STM32CubeIDE 기반 로봇암 제어 펌웨어 (C/C++)
└──  mechanical_design/  # 로봇암 기구 설계 데이터 (STEP, STL 등 CAD 파일)

##  Phase 1: 시스템 아키텍처 및 환경 구축
본격적인 개발에 앞서 소프트웨어 및 통신 환경을 표준화합니다.
 * **OS & 미들웨어:** Ubuntu 22.04 기반 ROS 2 Humble 설치 (PC 우선 개발 후 RPi 이식).
 * **통신 프로토콜 정의:** 상위-하위 간 데이터 규격(Header, ID, Length, Payload, Checksum) 정의.
 * **Git 레포지토리 구성:** firmware/, ros2_ws/, hardware/, docs/ 구조의 모노레포 생성.
## 🏗️ Phase 2: 기구 설계 및 디지털 트윈 (Onshape)
로봇의 물리적 한계를 결정하고 시뮬레이션 환경을 준비합니다.
 1. **3D 모델링:** Onshape를 활용하여 4축(Base, Shoulder, Elbow, Wrist) 구조 설계.
   * 스마트폰(약 200g) 부하를 고려한 관절 강성 확보.
   * 배선 관리를 위한 프레임 내 통로(Cable Guide) 설계.
 2. **Mates 설정:** 각 관절의 회전축과 가동 범위(Limit)를 Onshape 내에서 정의.
 3. **URDF 추출:** onshape-to-robot을 사용하여 ROS 2에서 사용 가능한 로봇 모델링 파일 생성.
## 👁️ Phase 3: 비전 인식 및 경로 계획 (High-Level)
로봇의 '눈'과 '뇌'를 구현하는 단계입니다.
 * **얼굴 인식:** OpenCV 및 MediaPipe를 연동하여 실시간 얼굴 중심점 (x, y) 추출.
 * **좌표 변환:** 영상 픽셀 좌표를 로봇 좌표계 상의 목표 지점 $(X, Y, Z)$으로 매핑.
 * **역운동학(IK) 구현:**
   * **방법 1:** 해석적 방법(Analytical IK) 수식을 파이썬으로 직접 구현 (리소스 최소화).
   * **방법 2:** MoveIt 2를 활용한 경로 계획 및 충돌 회피 적용 (전문성 강화).
## ⚡ Phase 4: 임베디드 펌웨어 개발 (Low-Level)
실시간성을 보장하며 모터를 구동하는 하드웨어 제어 로직입니다.
 * **환경:** STM32CubeMX + Makefile (또는 CLion) 환경 설정.
 * **실시간 제어:** FreeRTOS를 탑재하여 통신 태스크와 제어 태스크 분리.
 * **모터 드라이버:**
   * **LX-15D:** Half-duplex UART 통신을 통한 패킷 제어 및 상태 피드백 처리.
   * **MG996R:** Timer PWM 및 DMA를 활용한 정밀 펄스 제어.
 * **안전 로직:** 통신 두절 시 모터 즉시 정지 및 관절 가동 범위 제한(Software Limit).
## 🚀 Phase 5: 시스템 통합 및 최적화
모든 모듈을 결합하여 성능을 극대화합니다.
 1. **시리얼 루프 테스트:** PC에서 보낸 각도 값이 STM32에서 지연 없이 실행되는지 확인.
 2. **비전 트래킹 튜닝:** 얼굴의 움직임 속도에 따른 로봇암의 추종 감도(Smoothing) 및 가감속 제어.
 3. **RPi 이식:** PC에서 검증된 ROS 2 노드들을 라즈베리 파이 CLI 환경으로 옮겨 Standalone 구동 확인.
