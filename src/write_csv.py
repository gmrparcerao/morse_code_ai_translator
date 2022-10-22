"""
/*---------------Header----------------------------
    SENAI Technology College "Mariano Ferraz"
    Sao Paulo, 27/08/2022
    Graduate - Internet of Things

    Names of postgraduate students:
      Claudinei, Guilherme, Renan
    Lecturer:
      André and Daniel

    Goals:
      Software to train an AI to understand morse code

    Hardware:

    Libraries:

    Reviews:
      R000 - begin

Libraries:
- matplotlib
- numpy
- pyserial
"""

# --- Import libraries --- #
import sys
import serial
import time
import os

# --- Global variable --- #
mode = 0
port = 'COM3'
baud = 9600
dataRead = 0
i = 0

# --- Auxiliary functions --- #


# --- Main program ---

letter = input("Type the letter: ")
print(f"Letter typed: {letter}")
file = open(f"{letter}.csv", "a")   # mode a - Open and update file {letter}

try:
    serialConnection = serial.Serial(port, baud, timeout=400)
    print('Connected to ' + str(port) + ' at ' + str(baud) + ' bps.')

except:
    print("Failed to connect with " + str(port) + ' at ' + str(baud) + ' bps.')
    sys.exit("Process Failed")
mode = int(input("Select an option: 0 - Waiting mode, 1 - Read to record, 3 - Play last data, 4 - record: "))

match mode:
    case 0:
        serialConnection.write(b'0')
    case 1:
        serialConnection.write(b'1')
    case 3:
        serialConnection.write(b'3')
    case 4:
        serialConnection.write(b'4')

while True:
    time.sleep(1)

    # Receive datas from esp32 and save in a file
    if serialConnection.inWaiting() > 0:
        data = str(serialConnection.readline(), 'UTF-8') # Transform bytes in string
        data = data.replace('\n', '')
        file.write(data)
        file.flush()
        serialConnection.write(b'1')
        print(i)
        i += 1

print("End of program")
file.close()
serialConnection.close()
