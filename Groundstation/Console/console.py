import struct
import time

from serial_handler import Serial_Handler
from colored_terminal import *
from config import *


class INTERFACE:
    """for a terminal based controll over the server"""

    def __init__(self):
        self.isRunning = True
       # self.Serial = Serial_Handler(
        #    port=COM_PORT,
       #     baud_rate=STANDART_BAUD,
      #      timeout=1,
     #       format_package=PACKET_FORMAT,
    #        buffer_max_size=10000,
   #     )
        
        self.loop()

    def loop(self):
        print_yellow("\n<<Interface FexrayHV source>>\n/? for help\n")
        while self.isRunning:
            time.sleep(DELAY_CONSOLE_LOOP)  # delay to conserve performance
            try:
                self.Command(input())
            except KeyboardInterrupt:
                print_yellow(
                    "blocked keyboard interupt. Use /q to savely shutdown server\n"
                )

            except Exception as e:
                print(e)
                break

    def Command(self, command: str):
        if len(command) > 1 and command[0] == "/":
            match command[1:2]:

                case "?":
                    print_yellow("<Help>")
                    print_yellow(
                        """
/l List serial ports
/s select serial port
/c [command] [parameter] to uplink a command. 
   to see avaliable commands use /c [?]
/t [string] to uplink a raw string
/q | quit terminal
"""
                    )

                case "l":
                    self.Serial.list_serial_ports(starred_port=self.Serial.port)

                case "select"|"s":
                    self.Serial.isRunning = 0
                    self.Serial = Serial_Handler(
                         port=COM_PORT,
                         baud_rate=STANDART_BAUD,
                         timeout=1,
                         format_package=PACKET_FORMAT,
                         buffer_max_size=10000,
                    )
    
                case "c":
                    if self.Serial.port_active is None:
                        print_red("No device connected\n")
                    else:
                        try:
                            command_buf = (command[2:] + " 0 " * 6).split()[
                                :2
                            ]  # filling missed spaces with 0
                            print_yellow(f"Sending: {command_buf[0:2]} | ")

                            comm = command_buf[0]
                            param1 = float(command_buf[1])

                            uploadbuffer = comm.encode("utf-8") * 4 + struct.pack(
                                "fff", param1, param1, param1
                            )  # com x4 because of struct patting

                            # print_yellow(f'sending command: {command_buf}|{param1}|\nin bytes: {uploadbuffer}\n',indent=1)
                            self.Serial.BUFFER_UPLINK.append(uploadbuffer)
                        except Exception as e:
                            print_red(f"Error Command Console:{e}", indent=1)
                            
                case "t":
                    if self.Serial.port_active is None:
                        print_red("No device connected\n")
                    else:
                        try:
                            print_yellow(f"Sending: {command_buf}")
                            uploadbuffer = command_buf.encode("utf-8")
                            # print_yellow(f'sending command: {command_buf}|{param1}|\nin bytes: {uploadbuffer}\n',indent=1)
                            self.Serial.BUFFER_UPLINK.append(uploadbuffer)
                        except Exception as e:
                            print_red(f"Error Command Console:{e}", indent=1)

                case "quit" | "q":
                    print_yellow("<shuting down Interface>\n")
                    self.Serial.IsRunning = 0
                    self.isRunning = 0
                    print_yellow("<all down>\n", indent=1)
                case _:
                    print_red(f"Unknown Command {command[1:]}\n")
        else:
            print_yellow("use /? for help\n")


if __name__ == "__main__":
    interface = INTERFACE()
