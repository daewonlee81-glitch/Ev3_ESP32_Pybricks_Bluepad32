/**
 * ESP32 Xbox Controller → EV3 UART 제어
 * 
 * 라이브러리: ESP32 + Bluepad32
 * 보드:      ESP32 Dev Module (Bluepad32)
 * Baudrate:  9600 (EV3 Pybricks 통신)
 * 
 * 배선:
 *   GPIO16 (RX2) ← EV3 케이블 노랑 (핀5, EV3 Tx)
 *   GPIO17 (TX2) → EV3 케이블 파랑  (핀6, EV3 Rx)
 *   GND          ─  EV3 케이블 빨강  (핀3, GND)
 *   VIN          ←  EV3 케이블 초록  (핀4, +5V) ← ESP32 전원
 */

#include <Bluepad32.h>

#define RXD2     16
#define TXD2     17
#define DEADZONE 20

GamepadPtr myGamepad = nullptr;

// ─── 콜백 ───────────────────────────────────────────

void onConnectedGamepad(GamepadPtr gp) {
  myGamepad = gp;
  Serial.println("Xbox Controller Connected!");
  Serial2.println("L:0,R:0");  // 연결 시 정지 명령
}

void onDisconnectedGamepad(GamepadPtr gp) {
  myGamepad = nullptr;
  Serial2.println("L:0,R:0");  // 연결 끊길 시 정지
  Serial.println("Xbox Disconnected! 재연결 대기중...");
}

// ─── 데드존 ─────────────────────────────────────────

int applyDeadzone(int value) {
  return (abs(value) < DEADZONE) ? 0 : value;
}

// ─── Setup ──────────────────────────────────────────

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);

  // 페어링 정보 유지 (전원 ON 시 자동 재연결)
  // 최초 페어링 시에만 아래 줄 주석 해제 후 업로드, 이후 다시 주석
  // BP32.forgetBluetoothKeys();

  BP32.enableVirtualDevice(false);

  Serial.println("전원 ON - Xbox 컨트롤러 연결 대기중...");
}

// ─── Loop ───────────────────────────────────────────

void loop() {
  BP32.update();

  if (myGamepad && myGamepad->isConnected()) {

    // 스틱 값 읽기 (-512 ~ 512)
    int ly = applyDeadzone(myGamepad->axisY());   // 왼쪽 스틱 Y (전후)
    int rx = applyDeadzone(myGamepad->axisRX());  // 오른쪽 스틱 X (좌우)

    // 탱크 드라이브 믹싱
    int leftRaw  = constrain(-ly + rx, -512, 512);
    int rightRaw = constrain(-ly - rx, -512, 512);

    // -100 ~ 100 정규화
    int L = map(leftRaw,  -512, 512, -100, 100);
    int R = map(rightRaw, -512, 512, -100, 100);

    // EV3로 UART 전송
    String cmd = "L:" + String(L) + ",R:" + String(R);
    Serial2.println(cmd);
    Serial.println(cmd);  // 디버그용
  }

  delay(20);  // 50Hz
}
