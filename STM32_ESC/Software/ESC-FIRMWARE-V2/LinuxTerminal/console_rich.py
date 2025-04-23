import struct
from threading import Thread
import time
import serial
import serial.tools.list_ports
from colored_terminal import *

DELAY_CONSOLE_LOOP = 0.001


class INTERFACE:
    """for a terminal based controll over the server"""

    def __init__(self):
        self.isRunning = True
        self.Port = None
        self.Serial = None

        self.start()

    def start(self):
        print_yellow("\n<<Compartion>>\n/? for help\n")
        self.autoconnect()

        # Start input thread
        Thread(target=self.command_loop, daemon=True).start()

        while self.isRunning:
            time.sleep(DELAY_CONSOLE_LOOP)  # delay to conserve performance
            try:
                #self.input_command(input("~")) #needs an extra thread
                if self.Serial is not None:
                
                    self.receive_data()

                    if self.Serial.is_open == 0:
                        print_red("COM disconnected\n")
                        self.Serial = None
                        self.Port = None
                
            except KeyboardInterrupt:
                print_yellow(
                    "blocked keyboard interupt. Use /q to savely shutdown server\n"
                )

            except Exception as e:
                print(e)
                break

    def command_loop(self):
        """This handles user input. It's in an extra Thread to not block incomming data"""
        while self.isRunning:
            try:
                cmd = input("~")
                #print("\033[2K", end="\r") #resets cursor
                self.input_command(cmd)
            except EOFError:
                break
            except Exception as e:
                print_red(f"Command error: {e}\n")

    def receive_data(self):
        if self.Serial != None:
            if self.Serial.in_waiting > 0:
                try:
                    received = self.Serial.readline().decode('utf-8')
                except Exception as e:
                    print_red(f"Error Serial Recive:{e}\n", indent=1) 
                    
                if received:
                    print_magenta(f"{received}")

    def autoconnect(self):
        ports = serial.tools.list_ports.comports()
        # test for stm32
        for port in ports:
            if "STM" in str(port.manufacturer) or "Arduino" in str(port.manufacturer):
                self.Port = port
                self.Serial = serial.Serial(
                self.Port.device, baudrate=9600, timeout=1
                )
                break

        
        if self.Port is None:
            print_red("Autoconnect failed\n")
        else:
            print_green("Autoconnect")
            print_white(
                            f":{self.Port.device}|{self.Port.manufacturer}|9600\n"
                        )


    def input_command(self, command: str):
        if len(command) > 1 and command[0] == "/":
            match command[1:2]:

                case "?":
                    print_yellow("<Help>")
                    print_yellow(
                        """
/l List serial ports
/s [number] [baudrate=9600] select serial port
/t [string] to uplink a raw string
/c [command] [parameter] to uplink a command. 
   to see avaliable commands use /c [?]
/q | quit terminal
"""
                    )

                case "l":
                    ports = serial.tools.list_ports.comports()
                    if not ports:
                        print_yellow("No serial ports found.\n")
                        return

                    print_yellow("COM ports:\n")
                    print("nr|name|manufacturer|description")
                    a = 0
                    for port in ports:
                        if port.device != "":
                            if port == self.Port:  # replaxe none with stared port
                                print_green(
                                    f">>{a}|{port.name}|{port.manufacturer}|{port.description}\n"
                                )
                            else:
                                print(
                                    f"{a}|{port.name}|{port.manufacturer}|{port.description}"
                                )  # - {port.description}- {port.product} -- {port.name}

                            a += 1
                            

                case "select" | "s":
                    # select com port
                    comand_buf = command[3:].split()
                    ports = serial.tools.list_ports.comports()
                    number = int(comand_buf[0])
                    
                    if(number>len(ports)):
                        print_red("values to high. try /l\n")
                        return

                    #select baundrate
                    baudrate = 9600
                    if len(comand_buf) > 1:
                        baudrate = int(comand_buf[1])                       
                        
                    # select com port
                    if len(ports) >= number:
                        self.Port = ports[number]
                        self.Serial = serial.Serial(
                            self.Port.device, baudrate, timeout=1
                        )

                    print_yellow(
                        f"Selected:{self.Port.device}-{self.Port.manufacturer}\n"
                    )

                    pass
                
                # sends string
                case "t":
                    if self.Serial is None:
                        print_red("No device connected\n")
                    else:
                        try:
                            #print_cyan(f"{command[2:]}\n")
                            self.Serial.write((command[2:] +'\n').encode())
                        except Exception as e:
                            print_red(f"Error Command Console:{e}", indent=1)
                    pass
                
                # sends a command
                case "c":
                    # if self.Serial.port_active is None:
                    #     print_red("No device connected\n")
                    # else:
                    #     try:
                    #         command_buf = (command[2:] + " 0 " * 6).split()[
                    #             :2
                    #         ]  # filling missed spaces with 0
                    #         print_yellow(f"Sending: {command_buf[0:2]} | ")

                    #         comm = command_buf[0]
                    #         param1 = float(command_buf[1])

                    #         uploadbuffer = comm.encode("utf-8") * 4 + struct.pack(
                    #             "fff", param1, param1, param1
                    #         )  # com x4 because of struct patting

                    #         # print_yellow(f'sending command: {command_buf}|{param1}|\nin bytes: {uploadbuffer}\n',indent=1)
                    #         self.Serial.BUFFER_UPLINK.append(uploadbuffer)
                    #     except Exception as e:
                    #         print_red(f"Error Command Console:{e}", indent=1)
                    pass

                case "quit" | "q":
                    print_yellow("<shuting down Interface>\n")
                    # self.Serial.IsRunning = 0
                    self.isRunning = 0
                    print_yellow("<all down>\n", indent=1)
                case _:
                    print_red(f"Unknown Command {command[1:]}\n")
        else:
            print_yellow("use /? for help\n")


if __name__ == "__main__":
    interface = INTERFACE()
