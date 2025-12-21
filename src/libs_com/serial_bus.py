import serial
import threading

import time
import numpy

from frame_spec import *

class SerialBus:
    def __init__(self, port, baud_rate = 115200):
        self.port       = port
        self.baud_rate  = baud_rate

        self.mutex = threading.Lock()
        self.init_receiving_data()


        self.ser        = serial.Serial(self.port, self.baud_rate, timeout=1)

        self.rx_thread  = threading.Thread(target=self.receiver, daemon=True)
        self.rx_thread.start()



    def send_data(self, src_adr, dest_adr, data_buffer : list):

        frame = bytearray()

        frame += PREAMBULE_BYTE * PREAMBULE_SIZE
        frame += SYNC_BYTE_1
        frame += SYNC_BYTE_2
        frame += bytes([src_adr, dest_adr])

        crc = 0
        crc = self._update_crc(crc, src_adr)
        crc = self._update_crc(crc, dest_adr)

        for b in data_buffer:
            crc = self._update_crc(crc, b)
            frame.append(b & 0xFF)

            #print(int(b), end = " ")

        # padding
        while len(frame) < DATA_SIZE + PREAMBULE_SIZE + 2 + 2:
            frame.append(0)
            crc = self._update_crc(crc, 0)

        frame.append(crc & 0xFF)

        self.ser.write(frame)
        self.ser.flush()





    def _send_byte(self, value):
        # value: int from 0 to 255
        self.ser.write(bytes([value&0xFF]))



    def _update_crc(self, crc_curr, byte):
        crc = crc_curr & 0xFF
        crc ^= byte & 0xFF

        for _ in range(8):
            if crc & 0x80:
                crc = ((crc << 1) ^ 0x31) & 0xFF
            else:
                crc = (crc << 1) & 0xFF

        return crc
    


    def receiver(self):

        self.init_receiving_data()

        state = 0   

        while True:
            data = self.ser.read()   # read 1 byte (binary-safe)

            if not data:
                continue

            c  = data[0]&0xff
            
            if state == 0:
                rx_crc    = 0
                rx_ptr    = 0
        
                rx_src_address  = 0
                rx_dest_address = 0

                rx_buffer = DATA_SIZE*[0]

                if c == SYNC_BYTE_1[0]:
                    state = 1

            elif state == 1:
                if c == SYNC_BYTE_2[0]:
                    state = 2
                else:
                    state = 0
            
            # receive source address
            elif state == 2:
                rx_src_address = c
                rx_crc = self._update_crc(rx_crc, c)
                state = 3

            # receive desination address
            elif state == 3:
                rx_dest_address = c
                rx_crc = self._update_crc(rx_crc, c)
                state = 4

            elif state == 4:
                
                if rx_ptr < DATA_SIZE:
                    rx_buffer[rx_ptr] = c
                    rx_crc = self._update_crc(rx_crc, c)
                    rx_ptr+= 1

                if rx_ptr >= DATA_SIZE:
                    state = 5

            elif state == 5:
                crc_ref = c

                if crc_ref == rx_crc:
                    crc_valid = True
                else:
                    crc_valid = False

                self._update_receiving_data(rx_src_address, rx_dest_address, crc_valid, rx_buffer)

                state = 0


    def is_new_data(self):
        with self.mutex:
            return self.rx_done
        
    def get_received_data(self):
        with self.mutex:
            return self.rx_done, self.rx_src_address, self.rx_dest_address, self.rx_crc_status, self.rx_data
        
    def init_receiving_data(self):
        with self.mutex:
            self.rx_done            = False
            self.rx_src_address     = 0
            self.rx_dest_address    = 0 
            self.rx_crc_status      = False 
            self.rx_data            = None

    def _update_receiving_data(self, rx_src_address, rx_dest_address, crc_valid, rx_buffer):
        
        with self.mutex:
            self.rx_done            = True
            self.rx_src_address     = rx_src_address
            self.rx_dest_address    = rx_dest_address 
            self.rx_crc_status      = crc_valid 
            self.rx_data            = bytes(rx_buffer)



if __name__ == "__main__":
    #port = "/dev/tty.usbserial-0001"
    port = "/dev/tty.usbmodem1203"

    sl = SerialBus(port)

    num_packets = 100
    dt_wait = 0.001

    t_start = time.time()

    for n in range(num_packets):
        data = []

        for i in range(DATA_SIZE):
            data.append(0)

        data[0] = 1
        data[1] = 0
        data[2] = 0
        data[3] = 0

        for i in range(16):
            data[i+4] = 0# numpy.random.randint(0, 255)

        src_adr  = 7
        dest_adr = 100

        sl.send_data(src_adr, dest_adr, data)

        while sl.rx_done != True:
            time.sleep(0.001)

        sl.rx_done = False

        time.sleep(dt_wait)

    t_stop = time.time()

    duration = (t_stop - t_start) - num_packets*dt_wait

    print("tp = ", num_packets/duration )
