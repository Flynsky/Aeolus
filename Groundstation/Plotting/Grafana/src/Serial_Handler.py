import serial
import serial.tools.list_ports
import time
import threading
import struct

TIMEOUT_RECONNECT = 0.1  # in s


class Serial_Handler:
    """Reads out Serial data in a buffer
    This data has to be in a fixed format"""

    def __init__(
        self,
        port="automatic",
        baud_rate=9600,
        timeout=1,
        format_package="f f f",
        buffer_max_size=10000,
    ):
        self.port = port
        self.baud_rate = baud_rate
        self.timeout = timeout
        
        self.format = format_package
        self.sizeof_package = len(format_package.split())

        self.buffer_max_size = buffer_max_size
        self.BUFFER_RAW = bytearray()
        self.BUFFER_TIMESTAMP = []
        self.BUFFER_FROMATED = [[] for _ in range(self.sizeof_package)]

        self.IsRunning = True

        self.thread = threading.Thread(target=self.run_serial)
        self.thread.start()
        
        print(f"Started  Serial Handler")
        print(f"COM:{port}|Baud:{baud_rate}|timeout:{timeout}")
        print(f"Package format:{format_package}|size:{self.sizeof_package}")

    def list_serial_ports(self, starred_port=None):
        ports = serial.tools.list_ports.comports()
        if not ports:
            print("No serial ports found.")
            return

        print("COM port list:")
        for port in ports:
            if port.device != "":
                if port.device == starred_port:
                    print(f"*{port.device} - {port.description}")
                else:
                    print(f"{port.device} - {port.description}")

    def run_serial(self):
        """Read data from the serial port"""

        def format_buffer(self):
            try:
                new_data_formated = struct.iter_unpack(self.format, self.BUFFER_RAW)
                for package in new_data_formated:
                    #checks if part of package got lost
                    if len(package) == self.sizeof_package: 
                        #checks weather serial buffer is over the its size
                        n_data = len(self.BUFFER_TIMESTAMP)
                        if(n_data)<=self.buffer_max_size:
                            #buffer not full -> adds new values to the buffer
                            self.BUFFER_TIMESTAMP.append(time.time_ns())
                            for nr, el in enumerate(package):
                                self.BUFFER_FROMATED[nr].append(float(el))
                        else:
                             #buffer full -> insets new value at index 0 and deletes last value
                            self.BUFFER_TIMESTAMP.pop(0)                         
                            self.BUFFER_TIMESTAMP.append(time.time_ns())
                            for nr, el in enumerate(package):
                                self.BUFFER_FROMATED[nr].pop(0)
                                self.BUFFER_FROMATED[nr].append(float(el))
                            
                    else:
                        print(f"package {new_data_formated} is corrupted")
                # for el in self.BUFFER_FROMATED:
                #     print(F'{el[-1]} ',end='')
                # print("")
                # print(self.BUFFER_FROMATED[0][-1])

                self.BUFFER_RAW.clear()

            except struct.error as e:
                print(f"Unpacking error: {e}")
                self.BUFFER_RAW.clear()

            except KeyboardInterrupt:
                print("Exiting...")
                self.IsRunning = False

        def select_port_automatic(self):
            ports = serial.tools.list_ports.comports()
            if not ports:
                print("No serial ports found.")
                return

            for port in ports:
                if port.device != "":
                    if port.manufacturer != "(Standardanschlusstypen)":
                        return port
            return None

        selected_port = None
        while self.IsRunning:
            try:
                ##connects to a Serial port
                if selected_port is None:
                    if self.port == "automatic":
                        port_autoselected = select_port_automatic(self)
                        if port_autoselected is not None:
                            selected_port = serial.Serial(
                                port_autoselected.device, baudrate=9600, timeout=1
                            )
                            self.list_serial_ports(starred_port=selected_port.name)
                    else:
                        selected_port = serial.Serial(
                            self.port, self.baud_rate, timeout=self.timeout
                        )
                        self.list_serial_ports(starred_port=selected_port.name)

                ##reads -if avaliable- data from this port
                if selected_port is not None and selected_port.in_waiting > 0:
                    self.BUFFER_RAW += selected_port.read(selected_port.in_waiting)

                    ##if a full package has been recieved, formats this package
                    if (len(self.BUFFER_RAW)) % struct.calcsize(self.format) == 0:
                        # reads Serial data formated in other buffer
                        format_buffer(self)

            except KeyboardInterrupt:
                print("Exiting...")
                self.IsRunning = False

            except serial.SerialException as e:
                if str(e)[0:19] == "could not open port":
                    print("No Serial device connected")
                elif str(e)[0:21] == "ClearCommError failed":
                    print("Serial disconnected")
                else:
                    print(f"Serial error: {e}")
                self.BUFFER_RAW.clear()
                if selected_port is not None:
                    selected_port.close()
                    selected_port = None
                time.sleep(TIMEOUT_RECONNECT)

            except Exception as e:
                print(f"Read error: {e}")

        if selected_port is not None:
            selected_port.close()


if __name__ == "__main__":
    # ser = Serial_Handler()
    ser = Serial_Handler(port="automatic")
