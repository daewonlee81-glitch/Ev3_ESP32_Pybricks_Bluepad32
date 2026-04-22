/**
 * ESP32 Xbox Controller -> EV3 UART control
 *
 * Library:   ESP32 + Bluepad32
 * Board:     ESP32 Dev Module (Bluepad32)
 * Baudrate:  9600 (EV3 Pybricks communication)
 * Drive:     Mecanum 4-wheel
 *
 * Wiring:
 *   GPIO16 (RX2) <- EV3 cable yellow (pin 5, EV3 Tx)
 *   GPIO17 (TX2) -> EV3 cable blue   (pin 6, EV3 Rx)
 *   GND          -- EV3 cable red    (pin 3, GND)
 *   VIN          <- EV3 cable green  (pin 4, +5V) <- ESP32 power
 */

#include <Bluepad32.h>

#define RXD2 16
#define TXD2 17
#define DEADZONE 20

GamepadPtr myGamepad = nullptr;

void sendStopCommand() {
  Serial2.println("X:0,Y:0,R:0");
}

void onConnectedGamepad(GamepadPtr gp) {
  myGamepad = gp;
  Serial.println("Xbox Controller Connected!");
  sendStopCommand();
}

void onDisconnectedGamepad(GamepadPtr gp) {
  myGamepad = nullptr;
  sendStopCommand();
  Serial.println("Xbox Disconnected! Waiting for reconnect...");
}

int applyDeadzone(int value) {
  return (abs(value) < DEADZONE) ? 0 : value;
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);

  // Keep pairing info for auto reconnect after reboot.
  // Uncomment only once if you need to clear saved controller pairing.
  // BP32.forgetBluetoothKeys();

  BP32.enableVirtualDevice(false);

  Serial.println("Power on - waiting for Xbox controller...");
}

void loop() {
  BP32.update();

  if (myGamepad && myGamepad->isConnected()) {
    // Requested mapping:
    // Left stick Y  -> forward / backward
    // Left stick X  -> strafe left / right
    // Right stick X -> rotation
    int lx = applyDeadzone(myGamepad->axisX());
    int ly = applyDeadzone(myGamepad->axisY());
    int rx = applyDeadzone(myGamepad->axisRX());

    int X = map(lx, -512, 512, -100, 100);
    int Y = map(ly, -512, 512, -100, 100);
    int R = map(-rx, -512, 512, -100, 100);

    String cmd = "X:" + String(X) + ",Y:" + String(Y) + ",R:" + String(R);
    Serial2.println(cmd);
    Serial.println(cmd);
  }

  delay(20);
}
