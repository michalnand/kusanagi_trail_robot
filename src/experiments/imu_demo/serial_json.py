import serial
import json

import time

class SerialJson:

    def __init__(self, port_name = "/dev/tty.usbserial-10", speed = 115200, max_length = 10000):
        self.sp     = serial.Serial(port_name, speed)
        self.state  = 0
        self.buffer = ""

        self.max_length = max_length

        self.time_now  = time.time() 
        self.time_prev = self.time_now

    def get_json(self):
        parsed  = {} 
        success = False

        try:
            c = self.sp.read()
            c = str(c.decode("utf-8"))
        except:
            return parsed, success, 1


      
        # wait for start char >
        if self.state == 0:
            self.buffer = ""
            if c == '>':
                self.state = 1
            
        elif self.state == 1:
            # terminating char, parse buffer
            if c == '<':
                parsed, success = self._parse_json(self.buffer)
                self.state = 0
            else:
                self.buffer+= str(c)

            # too long buffer
            if len(self.buffer) > self.max_length:
                self.state = 0 

        if success:
            self.time_prev = self.time_now 
            self.time_now  = time.time()

        dt = self.time_now - self.time_prev

        return parsed, success, dt

    def _parse_json(self, s):
        parsed  = {}
        success = False

        try:
            parsed = json.loads(s)
            success = True
        except:
            parsed  = {}
            success = False

        return parsed, success
