import serial
import time
import threading

# Set up the serial connection
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)  # Adjust the baud rate if needed

# Function to receive data
def receive_data():
    while True:
        if ser.in_waiting > 0:
            received = ser.readline().decode('utf-8').strip()
            if received:
                print(f"{received}")

# Function to send data
def send_data():
    while True:
        line = input("")  # Wait for Enter to send the line
        ser.write((line +'\n').encode())  # Send data with newline

# Start a thread to continuously read data from the serial port
thread = threading.Thread(target=receive_data)
thread.daemon = True
thread.start()

# Send data from the main thread
send_data()
