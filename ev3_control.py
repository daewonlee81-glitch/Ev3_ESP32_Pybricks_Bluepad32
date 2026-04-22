"""
EV3 Xbox Controller 수신 제어
Pybricks MicroPython

배선:
  Port S2 (RJ12) ← ESP32 UART
    핀3 빨강: GND
    핀4 초록: +5V  (ESP32 전원 공급)
    핀5 노랑: Tx   → ESP32 GPIO16 (RX2)
    핀6 파랑: Rx   ← ESP32 GPIO17 (TX2)

모터:
  Port B: 왼쪽 모터
  Port C: 오른쪽 모터
"""

from pybricks.hubs import EV3Brick
from pybricks.ev3devices import Motor
from pybricks.parameters import Port, Direction
from pybricks.iodevices import UARTDevice
from pybricks.tools import wait

# ─── 초기화 ────────────────────────────────────────

ev3 = EV3Brick()

left_motor  = Motor(Port.B, Direction.COUNTERCLOCKWISE)
right_motor = Motor(Port.C)

# ESP32 연결 포트 및 통신 속도 (ESP32와 동일하게 설정)
uart = UARTDevice(Port.S2, baudrate=9600)

ev3.speaker.beep()
print("EV3 Xbox Control Ready")

# ─── 명령 파싱 ─────────────────────────────────────

def parse_cmd(line):
    """
    'L:50,R:-30' 형식의 bytes 버퍼를 파싱
    반환: (L, R) 각각 -100 ~ 100
    """
    try:
        text = ''.join(chr(b) for b in line)
        parts = text.split(',')
        values = {}
        for p in parts:
            k, v = p.split(':')
            values[k.strip()] = int(v.strip())
        return values.get('L', 0), values.get('R', 0)
    except:
        return 0, 0

# ─── 메인 루프 ─────────────────────────────────────

buf = b''  # bytes 버퍼로 누적 (분할 수신 대응)

while True:
    if uart.waiting():
        buf += uart.read(uart.waiting())

        # 개행 기준으로 완성된 라인 처리
        while b'\n' in buf:
            line, buf = buf.split(b'\n', 1)
            line = line.replace(b'\r', b'')  # \r 제거

            if line:
                L, R = parse_cmd(line)
                print("L:", L, "R:", R)

                # 속도 적용 (100% → 900 deg/s)
                left_motor.run(L * 9)
                right_motor.run(R * 9)

    wait(10)
