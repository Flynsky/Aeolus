import serial
import serial.tools.list_ports
import time

# Configure the serial port settings
port = 'COM7'  # Change this to your COM port
baud_rate = 9600  # Change this to your desired baud rate
timeout = 1  # 1 second read timeout
 
class Serial():
    def __init__(self):
        self.list_serial_ports()
        self.SerialPort = self.open_port(['COM7',9600,1])
        time.sleep(2)
        self.read(self.SerialPort)

    def list_serial_ports(self):
            ports = serial.tools.list_ports.comports()
            if not ports:
                print("No serial ports found.")
                return

            print("Available serial ports:")
            for port in ports:
                print(f"{port.device} - {port.description}")

    def open_port(self,port:list):
            # Open the serial port
        try:
            ser = serial.Serial(port[0], port[1], timeout=port[2])
            print(f"Opened {port} at {baud_rate} baud rate.")
            return ser
        except serial.SerialException as e:
            print(f"Error opening port: {e}")

    def read(self, port=None):
        if port is None:
            port = self.SerialPort
        # Read data from the serial port
        try:
            if port.in_waiting > 0:
                received_data = port.read(port.in_waiting).decode('utf-8')
                # print(f"Received: {received_data}")
                print(received_data,end='')
                return received_data

        except KeyboardInterrupt:
            print("Exiting...")
        # finally:
            # Close the serial port
            # port.close()
            # print("Serial port closed.")
    
    def send_port(self,string:str,port = None):
        if port is None:
            port = self.SerialPort
            # Send the text to the COM port
            port.write(string.encode('utf-8'))


if __name__ == "__main__":
    ser = Serial()
    while True:
        ser.read()
        # time.sleep(1)
    # main()
