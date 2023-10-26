import serial
import time

# first attempt
# https://projecthub.arduino.cc/ansh2919/serial-communication-between-python-and-arduino-663756

port = 'COM7'
baudrate = 115200
arduino = serial.Serial(port, baudrate, timeout=.1)
time.sleep(1)
num = [[],[]]

def send(input1,input2):
    arduino.write(bytes(input1, 'utf-8'))
    time.sleep(0.05)
    arduino.write(bytes(input2, 'utf-8'))
    time.sleep(0.05)
    

while True:
    drive = input("Enter first number: ") # Taking input from user 
    steer = input("Enter second number: ")
    send(drive, steer) 

