import serial
from pyfiglet import Figlet
import serial.tools.list_ports

SERIAL_PORT = None
BAUD_RATE = 460800


def configure_port():
    global SERIAL_PORT
    ports = serial.tools.list_ports.comports()
    print("[*] Available ports:\n")
    for i, port in enumerate(ports):
        print(f"[{i + 1}] {port.name}")
    selected_port = int(input("\n[*] Enter port number to connect: ")) - 1
    SERIAL_PORT = "/dev/{}".format(ports[selected_port].name)


def intro_screen():
    fig = Figlet(font='slant')
    print(fig.renderText('AirCmd'))
    print("\t   By MalwareMystic\n")
    print("[*] Type your commands below and hit enter to send them.")
    print("[*] Type 'exit' to quit the program. CTRL + C also works.\n")
    print("-" * 57, "\n")
    configure_port()


def air_shell():
    if not SERIAL_PORT or not BAUD_RATE:
        print("[ERROR] Please set baud rate and serial port before running the shell!")
        return
    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
            print(f"\n[INFO] Connected to {SERIAL_PORT}\n")
            print("-" * 57, "\n")
            while True:
                user_input = input("$ ")
                if user_input.lower().strip() == 'exit':
                    print("[INFO] Ending session...")
                    break
                user_input = f"[PLD] {user_input}"
                ser.write(user_input.encode('utf-8'))
                ser.write(b'\n')
                while True:
                    line = ser.readline().decode('utf-8').strip()
                    if '[OUT]' in line:
                        line = line.replace('[OUT] ', '')
                        print(line.replace('?', '\n'))
                        break
                    elif line:
                        print(line)
                        break
    except serial.serialutil.SerialException:
        print("\n[ERROR] Could not connect to serial port!")


if __name__ == "__main__":
    try:
        intro_screen()
        air_shell()
    except KeyboardInterrupt:
        print("\n[INFO] Ending session...")
