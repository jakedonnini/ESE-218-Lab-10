from email.utils import parsedate_to_datetime
from pickletools import string1
from turtle import delay
import serial
import time

with open('C:/Users/jaked/Desktop/215 Lab 10/test.txt', 'r') as f:
    points = []
    lines = f.readlines()
    for point in lines:
        try:
            numbers = point.split()
            numbers = [int(x) for x in numbers]
            lineNum = str(numbers[0]) + " " + str(numbers[1])
            points.append(lineNum)
        except ValueError:
            print("The string could not be converted to an integer.")
    
    print(points)

arduino = serial.Serial(port='COM9', baudrate=9600, timeout=.1)
def write(x):
    arduino.write(bytes(x, 'utf-8'))
    time.sleep(0.05)
def read():
    senData = arduino.readline() # read line form serial
    data = senData.decode() # convert the byte string to a unicode string
    parsedData = data.split('\r')
    if parsedData[0] != '':
       return parsedData[0]
    else:
        return 0

for x in points:
    while(True):
        word = read()
        if word == "ready":
            print("GOOD!")
            break
        #print(word)
        #print("waiting")
    print(x)
    write(x)
    time.sleep(1)
    #print(read())
    
