import os
import serial
import time
import re
from os import path

path_to_logs = path.expandvars(r'%APPDATA%\Microsoft\Teams\logs.txt')
print(path_to_logs)
arduino = serial.Serial(port = 'COM3', baudrate=115200, timeout=.1)
last_running = -1
def find_processes():
    global last_running
    processes = os.popen('tasklist /fo table /v /fi "imagename eq CptHost.exe"').read().splitlines()
    is_running = 1
    for item in processes:
        if item.find('CptHost.exe') > -1:
            is_running = 0
    
    if (is_running != last_running):
        arduino.flush()
        arduino.write(bytes(str(is_running), 'utf-8'))
        last_running = is_running
    
def return_teams_status():
    global last_running
    pattern = re.compile("-- info -- StatusIndicatorStateService: Added ")
    for line in open(path_to_logs):
        for match in re.finditer(pattern, line):
            status = line.split().pop(15)
    print(status)
    is_avail = 1
    if (status == "Available"):
        is_avail = 1
    else:
        is_avail = 0
        
    if (is_avail != last_running):
        arduino.flush()
        arduino.write(bytes(str(is_avail), 'utf-8'))
        print("changing last_running")
        last_running = is_avail


while True:
    #find_processes()
    return_teams_status()
    time.sleep(1)

