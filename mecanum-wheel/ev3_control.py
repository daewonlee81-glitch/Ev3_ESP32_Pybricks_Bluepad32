"""
EV3 Xbox Controller 수신 제어
Pybricks MicroPython

배선:
  Port S2 (RJ12) ← ESP32 UART
    핀3 빨강: GND
    핀4 초록: +5V  (ESP32 전원 공급)
    핀5 노랑: Tx   → ESP32 GPIO16 (RX2)
    핀6 파랑: Rx   ← ESP32 GPIO17 (TX2)

모터 배치 (로봇 진행 방향이 위쪽일 때):
  Port D: 앞왼쪽(FL)
  Port B: 앞오른쪽(FR)
  Port C: 뒤왼쪽(RL)
  Port A: 뒤오른쪽(RR)
"""

from pybricks.hubs import EV3Brick
from pybricks.ev3devices import Motor
from pybricks.parameters import Port, Direction
from pybricks.iodevices import UARTDevice
from pybricks.tools import wait


def clamp(value, low, high):
    return max(low, min(high, value))


def normalize(values, limit=100):
    peak = max(limit, max(abs(v) for v in values))
    return [int(v * limit / peak) for v in values]


def parse_cmd(line):
    """
    'X:20,Y:80,R:-10' 형식의 bytes 버퍼를 파싱
    반환: (x, y, r) 각각 -100 ~ 100
    """
    try:
        text = "".join(chr(b) for b in line)
        parts = text.split(",")
        values = {}
        for part in parts:
            key, value = part.split(":")
            values[key.strip()] = int(value.strip())
        return (
            clamp(values.get("X", 0), -100, 100),
            clamp(values.get("Y", 0), -100, 100),
            clamp(values.get("R", 0), -100, 100),
        )
    except Exception:
        return 0, 0, 0


ev3 = EV3Brick()

# 현재 포트 배치에서는 오른쪽 두 모터(A, B)를 반전해야
# Y=전후, X=평행이동, R=회전이 의도대로 동작합니다.
motor_a = Motor(Port.A, Direction.COUNTERCLOCKWISE)
motor_b = Motor(Port.B, Direction.COUNTERCLOCKWISE)
motor_c = Motor(Port.C)
motor_d = Motor(Port.D)

uart = UARTDevice(Port.S2, baudrate=9600)

ev3.speaker.beep()
print("EV3 Mecanum Control Ready")

buf = b""

while True:
    if uart.waiting():
        buf += uart.read(uart.waiting())

        while b"\n" in buf:
            line, buf = buf.split(b"\n", 1)
            line = line.replace(b"\r", b"")

            if line:
                x, y, r = parse_cmd(line)

                # D=앞왼쪽, B=앞오른쪽, C=뒤왼쪽, A=뒤오른쪽
                fl = y + x + r
                fr = y - x - r
                rl = y - x + r
                rr = y + x - r

                fl, fr, rl, rr = normalize([fl, fr, rl, rr])

                print("X:", x, "Y:", y, "R:", r)
                print("A:", rr, "B:", fr, "C:", rl, "D:", fl)

                # 100% -> 약 900 deg/s
                motor_a.run(rr * 9)
                motor_b.run(fr * 9)
                motor_c.run(rl * 9)
                motor_d.run(fl * 9)

    wait(10)
