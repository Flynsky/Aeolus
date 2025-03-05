import serial
import time

# Serial port configuration
port = 'COM6'  # Replace with your serial port
baudrate = 115200  # Replace with the correct baud rate
timeout = 1  # Timeout for reading (seconds)

# Open the serial port
ser = serial.Serial(port, baudrate, timeout=timeout)

# Open a file to log the data
with open('serial_log.txt', 'a') as log_file:
    try:
        print(f"Logging data from {port}...")
        while True:
            # Read data from the serial port
            if ser.in_waiting > 0:  # Check if there is data available to read
                data = ser.readline().decode('utf-8').strip()  # Read line and decode
                if data:
                    print(f"Received: {data}")  # Print to console
                    log_file.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')} - {data}\n")  # Write to log file
                    log_file.flush()  # Ensure data is written immediately
            # time.sleep(0.1)  # Small delay to avoid high CPU usage
    except KeyboardInterrupt:
        print("\nLogging stopped by user.")
    finally:
        ser.close()  # Close the serial port when done
