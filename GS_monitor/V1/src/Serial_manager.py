import serial
import serial.tools.list_ports
import time
import threading
import struct

class Serial():
    """Reads out Serial data in a buffer
    This data has to be in a fixed format"""
    def __init__(self, port='COM7', baud_rate=9600, timeout=1,format="L"):
        self.port = port
        self.baud_rate = baud_rate
        self.timeout = timeout
        self.format = format

        self.SERIAL_READ_BUFFER = bytearray()
        self.SERIAL_BUFFER_FORMATED = []
        self.lock = threading.Lock()
        
        self.list_serial_ports()
        self.SerialPort = self.open_port(self.port, self.baud_rate, self.timeout)
        
        self.thread = threading.Thread(target=self.read_serial, daemon=True)
        self.thread.start()

    def list_serial_ports(self):
        ports = serial.tools.list_ports.comports()
        if not ports:
            print("No serial ports found.")
            return

        print("Available serial ports:")
        for port in ports:
            print(f"{port.device} - {port.description}")

    def open_port(self, port_nr, baud_rate, timeout):
        # Open the serial port
        try:
            ser = serial.Serial(port_nr, baud_rate, timeout=timeout)
            print(f"Opened {port_nr} at {baud_rate} baud rate.")
            return ser
        except serial.SerialException as e:
            print(f"Error opening port: {e}")

    def read_serial(self):
        # Read data from the serial port
        port = self.SerialPort
        while True:
            try:
                if port is not None and port.in_waiting > 0:
                    with self.lock:
                        self.SERIAL_READ_BUFFER += port.read(port.in_waiting)
                    if (len(self.SERIAL_READ_BUFFER)) % len(format) == 0:
                        #reads Serial data formated in other buffer
                        pass

            except KeyboardInterrupt:
                print("Exiting...")
                port.close()
                break  # Exit the loop

            except Exception as e:
                print(f"Read error: {e}")
                break

    def format_buffer(self):
        with self.lock:
            data = self.SERIAL_READ_BUFFER.copy()


            try:
                unpacked_data = struct.iter_unpack(self.format, data)
                return unpacked_data
            except struct.error as e:
                pass
                # print(f"Unpacking error: {e}")

if __name__ == "__main__":
    ser = Serial(port='COM7', baud_rate=9600)
    try:
        while True:
            data = ser.SERIAL_READ_BUFFER
            print(data)
            unpacked_data = struct.iter_unpack("L", data)
            print(list(unpacked_data))
            time.sleep(3)
            print("\n\n")
    except KeyboardInterrupt:
        print("Program terminated.")
