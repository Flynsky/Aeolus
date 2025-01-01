import serial
import serial.tools.list_ports
import time
import threading
import struct
import json
import csv
import os

from colored_terminal import *
from config import *

class Serial_Handler:
    """Read/Writes Serial data from/to COM port.
    This data has to be in a fixed format"""

    def __init__(
        self,
        port="automatic",
        baud_rate=STANDART_BAUD,
        timeout=1,
        format_package="I7f",
        buffer_max_size=10000,
    ):

        # port wich is to be connected
        self.port = port  # here goes the port y wanna connect. Set it to "automatic" to connect to the next "non standart" COM device
        self.baud_rate = baud_rate
        self.timeout = timeout

        # connected port
        self.port_active = None  # port wich is currently connected

        # package
        self.format = format_package
        self.sizeof_package = struct.calcsize(format_package)

        # formated packages storage
        self.buffer_max_size = buffer_max_size
        self.BUFFER_RAW = bytearray()
        self.BUFFER_TIMESTAMP = []  # timestamp in ns when a package is recieved
        self.BUFFER_FROMATED = [
            [] for _ in range(self.sizeof_package)
        ]  # This is a nested list where each value form new recieved packages gets in a list

        # buffer for messages to be uplinked
        self.BUFFER_UPLINK = []

        # starting superloop in extra thread
        self.IsRunning = True
        self.thread = threading.Thread(target=self.run_serial)
        self.thread.start()

        print_cyan(f"Started  Serial Handler\n")
        print_cyan(f"COM:{port}|Baud:{baud_rate}|timeout:{timeout}\n")
        print_cyan(f"Package format:{format_package}|size:{self.sizeof_package}\n")

    def list_serial_ports(self, starred_port=None):
        ports = serial.tools.list_ports.comports()
        if not ports:
            print_cyan("No serial ports found.\n")
            return

        print_cyan("COM port list:\n")
        for port in ports:
            if port.device != "":
                if port.device == starred_port:
                    print_cyan(f"*{port.device} - {port.description}\n")
                else:
                    print_cyan(
                        f"{port.device} - {port.description} - {port.product} - {port.manufacturer} - {port.name}\n"
                    )

    def run_serial(self):
        """superloop to read / write data from the serial port"""

        def connect_serial_port(self, __cycle=[0]):
            # automatic
            if self.port == "automatic":

                port_autoselected = None

                # loads PortInfo for all connected ports
                ports = serial.tools.list_ports.comports()

                if not ports:
                    print("No serial ports found.")
                    return

                # searches for specific PortInfo
                for port in ports:
                    if port.device != "":
                        if port.manufacturer != "(Standardanschlusstypen)":
                            port_autoselected = port
                            break

                # selects to PortInfo the fitting right port
                if port_autoselected is not None:
                    return serial.Serial(
                        port_autoselected.device,
                        baudrate=STANDART_BAUD,
                        timeout=1,
                    )
                else:
                    # pretty loading animation
                    print(" " * 20, end="\r")  # Overwrite with spaces

                    if __cycle[0] < 3:
                        print_yellow(
                            "\rwaiting for device"
                            + __cycle[0] * "."
                            + (3 - __cycle[0]) * " "
                        )
                        __cycle[0] += 1
                        time.sleep(DELAY_ERROR)

                    if __cycle[0] == 3:
                        print_yellow("\rwaiting for device...")
                        __cycle[0] = 0
                        time.sleep(DELAY_ERROR)

                    return None

            # manual
            else:
                return serial.Serial(self.port, self.baud_rate, timeout=self.timeout)

        def format_buffer(self, format, buffer_raw):
            try:
                # checks if package has right length
                if len(buffer_raw) != self.sizeof_package:
                    return
                    print(
                        f"package corrupted. only {len(new_data_formated)} of {self.sizeof_package} bytes \n"
                    )
                
                #formats data
                new_data_formated = struct.unpack(format, buffer_raw)
                
                if PRINT_RECIEVED_PACKAGES:
                    print_cyan("\n")
                    for nr,type in enumerate(PACKET_NAMES):
                        print_cyan(f'|{type}: {new_data_formated[nr]}|\n')
                
                if SAVE_IN_BUFFER_FORMATED == True:
                    # checks weather serial buffer is over the its size
                    n_data = len(self.BUFFER_TIMESTAMP)
                    if (n_data) <= self.buffer_max_size:
                        # buffer not full -> adds new values to the buffer
                        self.BUFFER_TIMESTAMP.append(time.time_ns())
                        for nr, el in enumerate(new_data_formated):
                            self.BUFFER_FROMATED[nr].append(float(el))
                    else:
                        # buffer full -> insets new value at index 0 and deletes last value
                        self.BUFFER_TIMESTAMP.pop(0)
                        self.BUFFER_TIMESTAMP.append(time.time_ns())
                        for nr, el in enumerate(new_data_formated):
                            self.BUFFER_FROMATED[nr].pop(0)
                            self.BUFFER_FROMATED[nr].append(float(el))

                # Save the formated packet as a JSON file
                if SAVE_AS_JSON:
                    print_cyan("saved as json")
                    # Mapping the data to a dictionary with appropriate field names
                    packet_data = dict(zip(PACKET_NAMES, new_data_formated))

                    #saving
                    with open(FILEPATH_JSON, 'a') as jsonl_file:  # Open in append mode
                        jsonl_file.write(json.dumps(packet_data) + '\n')

                # Save the formated packet as a CSV
                if SAVE_AS_CSV:
                    print_cyan("saved as csv")
                    
                    file_exists = os.path.isfile(FILEPATH_CSV)
                    
                    # Open the file
                    with open(FILEPATH_CSV, 'a', newline='') as csv_file:
                        writer = csv.writer(csv_file, delimiter=',')
                        #if new -> header
                        if not file_exists:
                            writer.writerow(list(PACKET_NAMES))

                        writer.writerow(new_data_formated)

            except struct.error as e:
                print(f"Unpacking error: {e}")
                self.BUFFER_RAW.clear()

            except KeyboardInterrupt:
                print("Exiting...")
                self.IsRunning = False

        while self.IsRunning:
            try:
                ##connects to a Serial port
                if self.port_active is None:
                    self.port_active = connect_serial_port(self)
                    if self.port_active is not None:
                        self.list_serial_ports(starred_port=self.port_active.name)

                ##reads -if avaliable- data from this port
                if self.port_active is not None and self.port_active.in_waiting > 0:
                    self.BUFFER_RAW += self.port_active.read_until(
                        END_OF_TRAMISSION_MARKER
                    )

                    ##checks if its a package
                    if (
                        self.BUFFER_RAW[0] == PACKAGE_SPECIFIER
                        and self.BUFFER_RAW[1] == PACKAGE_SPECIFIER
                        and self.BUFFER_RAW[2] == PACKAGE_SPECIFIER
                    ):
                        # if (len(self.BUFFER_RAW)) % struct.calcsize(self.format) == 0:
                        # reads Serial data formated in other buffer
                        print_cyan("recieved package ")
                        # print(self.BUFFER_RAW)
                        self.BUFFER_RAW = self.BUFFER_RAW[
                            3:-1
                        ]  # removes first 3 elements as they are PACKAGE_SPECIFIER and last terminating char
                        format_buffer(self, self.format, self.BUFFER_RAW)
                        print_cyan("\n")
                    else:
                        ##if not a full package has been recieved, this is a debug message -> prints it
                        try:
                            # print_magenta("recieved debug: ")
                            # print(self.BUFFER_RAW[:-1])
                            print_magenta(f"{self.BUFFER_RAW.decode(errors='ignore')[:-1]}") #-1 because of the END_OF_TRAMISSION_MARKER
                        except Exception as e:
                            print_red(f"Serial debugg message error: {e}\n")

                    self.BUFFER_RAW.clear()

                ##sends -if avaliable- command up
                if self.port_active is not None:
                    if len(self.BUFFER_UPLINK):
                        try:
                            # print_cyan(
                            #     f"sending: (hex)|{'|'.join(f"{byte:02X}" for byte in self.BUFFER_UPLINK[0])} ({len(self.BUFFER_UPLINK[0])} bytes)\n"
                            # )
                            self.port_active.writelines(self.BUFFER_UPLINK)
                            self.BUFFER_UPLINK.clear()
                        except Exception as e:
                            print_red(
                                f"Serial send error: {e} with message {self.BUFFER_UPLINK}\n"
                            )
                            self.BUFFER_UPLINK.clear()

            except KeyboardInterrupt:
                print("Exiting...")
                self.IsRunning = False

            except serial.SerialException as e:
                self.BUFFER_RAW.clear()
                self.BUFFER_UPLINK.clear()
                if str(e)[0:19] == "could not open port":
                    print_red("SerialException: No suitable Serial device found\n")
                    time.sleep(TIMEOUT_RECONNECT)
                elif str(e)[0:21] == "ClearCommError failed":
                    print_red("Serial disconnected\n")
                else:
                    print_red(f"Serial error: {e}\n")
                if self.port_active is not None:
                    self.port_active.close()
                    self.port_active = None
                time.sleep(TIMEOUT_RECONNECT)

            except Exception as e:
                print_red(f"Serial Handler error: {e}\n")

        if self.port_active is not None:
            self.port_active.close()


if __name__ == "__main__":
    # ser = Serial_Handler()
    ser = Serial_Handler(port="automatic")
