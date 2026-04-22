# EV3 Xbox Controller via ESP32 + Bluepad32 + Pybricks

LEGO EV3를 Xbox 컨트롤러로 무선 제어하는 프로젝트입니다.  
ESP32에서 **Bluepad32**를 사용해 Bluetooth 게임패드 입력을 받고, 이를 UART로 EV3에 전달하는 구조입니다.  
전체 동작은 `ESP32 (Bluepad32) ↔ UART ↔ EV3 (Pybricks Beta)` 구성입니다.

---

## 시스템 구성

```
Xbox Controller ──BT──► ESP32 (Bluepad32) ──UART──► EV3 (Pybricks)
EV3 5V ──────────────────────────────────► ESP32 VIN (전원 공급)
```

---

## 필요 환경

### 하드웨어
- LEGO Mindstorms EV3
- NodeMCU ESP32S (또는 ESP32 Dev Module)
- Xbox Wireless Controller (Bluetooth 지원 모델)
- EV3 RJ12 케이블 (커팅 가공)

### 소프트웨어

| 항목 | 버전 | 링크 |
|------|------|------|
| Arduino IDE | 2.x 이상 권장 | https://www.arduino.cc/en/software |
| ESP32 + Bluepad32 보드 | 최신 | 아래 설치 방법 참조 |
| Pybricks EV3 펌웨어 | **v4.0.0bX (Beta)** | https://labs.pybricks.com/ |
| Pybricks Beta IDE | 웹 기반 | https://labs.pybricks.com/ |

> ℹ️ ESP32 쪽은 일반 Bluetooth 시리얼 예제가 아니라 **Bluepad32 라이브러리**를 사용해 Xbox 컨트롤러 입력을 직접 읽습니다.
> 이 프로젝트의 Arduino 스케치는 Bluepad32 기반 게임패드 입력 처리 예제를 참고해 EV3 UART 전송용으로 단순화한 구성입니다.

> ⚠️ EV3는 **정식 버전(v2.0)이 아닌 Beta 버전**을 사용해야 합니다.  
> 이 프로젝트의 `ev3_control.py`는 **Pybricks Beta 기준**으로 작성되었으며, 정식 버전과 API 및 설치 방식이 다를 수 있습니다.  
> 특히 Beta 펌웨어 설치는 **microSD 카드 방식이 아니라** `https://labs.pybricks.com/`에서 진행하는 방식 기준입니다.

---

## 배선

### EV3 RJ12 케이블 커팅 핀아웃

RJ12 커넥터를 커팅하면 아래 색상으로 선이 나옵니다.

```
RJ12 핀 번호 (탭이 아래를 향할 때 왼쪽부터)

 ┌─────────────────────┐
 │  1   2   3   4   5   6  │  ← 핀 번호
 └─────────────────────┘
      탭 (아래)

핀 1: 흰색  - 미사용
핀 2: 검정  - 미사용
핀 3: 빨강  - GND
핀 4: 초록  - +5V
핀 5: 노랑  - Tx (EV3 → ESP32)
핀 6: 파랑  - Rx (ESP32 → EV3)
```

### ESP32 연결표

| EV3 케이블 | 색상 | ESP32 핀 | 역할 |
|-----------|------|----------|------|
| 핀 3 | 빨강 | GND | GND 공통 |
| 핀 4 | 초록 | VIN | ESP32 전원 (+5V) |
| 핀 5 | 노랑 | GPIO16 (RX2) | EV3 Tx → ESP32 Rx |
| 핀 6 | 파랑 | GPIO17 (TX2) | ESP32 Tx → EV3 Rx |

```
EV3 RJ12 케이블          NodeMCU ESP32S
────────────────────────────────────────
빨강  (핀3, GND)  ──►  GND
초록  (핀4, +5V)  ──►  VIN        ← ESP32 전원
노랑  (핀5, Tx)   ──►  GPIO16 (RX2)
파랑  (핀6, Rx)   ◄──  GPIO17 (TX2)
```

> ⚠️ EV3 5V로 ESP32에 전원을 공급합니다. EV3 전원 ON 시 ESP32도 자동 부팅됩니다.

---

## Arduino IDE 설정

### 1. 보드 매니저 URL 추가

```
Arduino IDE → 파일 → 기본 설정 → 추가 보드 관리자 URLs
```

아래 URL 입력:
```
https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json
```

### 2. 보드 설치

```
툴 → 보드 → 보드 매니저 → "bluepad32" 검색
→ ESP32 + Bluepad32  설치
```

### 3. 보드 선택

```
툴 → 보드 → ESP32 + Bluepad32 보드 → ESP32 Dev Module
```

### 4. 업로드 설정

| 항목 | 설정값 |
|------|--------|
| Upload Speed | 921600 |
| Flash Size | 4MB |
| Port | /dev/cu.usbserial-XXXX (Mac) 또는 COM# (Windows) |

> ℹ️ CH340 USB 칩 사용 시 드라이버 별도 설치 필요  
> Mac: https://github.com/WCHSoftGroup/ch34xser_macos

---

## Pybricks Beta 설정 (EV3 필수)

> ⚠️ EV3는 반드시 **Beta 버전**을 사용해야 합니다.  
> 이 프로젝트는 `UARTDevice` 사용을 포함해 **Pybricks Beta 환경**을 전제로 합니다.  
> 또한 아래 안내는 **microSD 카드 설치가 아닌 Beta 펌웨어 설치 절차** 기준입니다.

### 1. Beta 사이트 접속

```
https://labs.pybricks.com/
```

### 2. EV3 펌웨어 설치 (Beta 설치 방식)

1. `https://labs.pybricks.com/` 접속
2. **Install Firmware** 클릭
3. **EV3** 선택
4. 안내에 따라 EV3를 USB로 연결하고 Beta 펌웨어 설치 진행
5. 설치 완료 후 EV3 재부팅

> ℹ️ 이 단계는 **microSD 카드에 이미지를 굽는 방식이 아닙니다.**  
> README의 나머지 설명도 모두 **Pybricks Beta 설치 환경**을 기준으로 합니다.

### 3. 코드 업로드 및 실행

1. `https://labs.pybricks.com/` 접속
2. mini-USB 케이블로 EV3 연결
3. **Connect** 버튼 클릭 → EV3 선택
4. `ev3_control.py` 코드 붙여넣기
5. **Run** 버튼 클릭

---

## Xbox 컨트롤러 페어링

### 최초 페어링
1. EV3 전원 ON → ESP32 자동 부팅
2. Xbox 컨트롤러 전원 ON
3. 컨트롤러 상단 **페어링 버튼(작은 버튼)** 길게 누르기
4. 로고 버튼 빠르게 깜빡임 → 연결 완료 시 고정

### 이후 재연결
- 페어링 정보가 저장되므로 **페어링 버튼 짧게 한 번**만 누르면 자동 연결

---

## 조작 방법

| Xbox 입력 | 동작 |
|-----------|------|
| 왼쪽 스틱 ↑↓ | 전진 / 후진 |
| 오른쪽 스틱 ←→ | 좌회전 / 우회전 |
| 스틱 중앙 (Deadzone) | 정지 |

---

## 파일 구성

```
├── README.md              # 이 파일
├── esp32_bluepad32.ino    # ESP32 Arduino 코드
└── ev3_control.py         # EV3 Pybricks 코드
```

---

## 참고 자료

- Bluepad32 Arduino + Pybricks 예제 참고:
  [bp_led_display_pybricks.py](https://github.com/ste7anste7an/bluepad32_arduino/blob/main/Examples/Pybricks/python/bp_led_display_pybricks.py)
- 관련 저장소:
  [ste7anste7an/bluepad32_arduino](https://github.com/ste7anste7an/bluepad32_arduino)

이 README와 예제 구조 설명은 위 저장소의 Pybricks 예제를 참고해 현재 프로젝트의 `UART` 기반 EV3 제어 방식에 맞게 정리했습니다.

---

## 문제 해결

| 증상 | 원인 | 해결 |
|------|------|------|
| EV3 수신 없음 | Tx/Rx 반전 | 노랑/파랑 교체 |
| ESP32 부팅 안 됨 | 전원 부족 | EV3 5V VIN 연결 확인 |
| 컨트롤러 연결 안 됨 | 페어링 정보 없음 | 페어링 버튼 길게 누르기 |
| 모터 방향 반대 | Direction 설정 | `Direction.COUNTERCLOCKWISE` 변경 |
| 데이터 깨짐 | Baudrate 불일치 | 양쪽 모두 9600 확인 |
