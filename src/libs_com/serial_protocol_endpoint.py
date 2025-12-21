import io_pb2   
import random
import time


from serial_bus import *
from io_data    import *



class SerialProtocolEndpoint(SerialBus, IOData):
    def __init__(self, port, baud_rate = 115200):
        super().__init__(port, baud_rate )

    '''
        send data_msg as specified in proto file
        then waits for max time_limit
        
        if success received, returns    : rx_src_address, rx_dest_address, rx_crc_status, rx_data, True
        if time out received, returns   : rx_src_address, rx_dest_address, rx_crc_status, rx_data, False

        note : if time out, 
        rx_src_address is set to 0
        rx_dest_address is set to 0
        rx_crc_status is False
        rx_data is None
    '''
    def send(self, src_adr, dest_adr, request, time_limit = 0.025):
        # prepare for new receiving 
        self.init_receiving_data()

        # serialise protobuffer data
        transmit_buffer = self._parse_transmit_data(request)

        self.send_data(src_adr, dest_adr, transmit_buffer)

        
        time_start = time.time()
        time_stop  = time_start + time_limit

        while True:
            rx_done, rx_src_address, rx_dest_address, rx_crc_status, rx_buffer = self.get_received_data()

            # receiving finished
            if rx_done:
                # prepare for new receiving
                self.init_receiving_data()

                # deserialise data
                response = self._parse_received_data(rx_buffer)

                return  rx_src_address, rx_dest_address, rx_crc_status, response, True

            elif time.time() > time_stop: 
                # prepare for new receiving
                self.init_receiving_data()

                # deserialise data
                response = self._parse_received_data(rx_buffer)

                return  rx_src_address, rx_dest_address, rx_crc_status, response, False

            else:
                time.sleep(0.001)
        

    def _parse_received_data(self, buffer):
        if buffer is None:
            return io_pb2.EmptyResponse()
        
        data_type = buffer[0]

        if data_type == io_pb2.MSG_PING_RESPONSE:
            return self._ping_response_from_bytes(buffer)
        elif data_type == io_pb2.MSG_BMS_RESPONSE:
            return self._bms_response_from_bytes(buffer)
        elif data_type == io_pb2.MSG_IMU_RESPONSE:
            return self._imu_response_from_bytes(buffer)
        elif data_type == io_pb2.MSG_MOTOR_RESPONSE:
            return self._motor_response_from_bytes(buffer)
        
        return io_pb2.EmptyResponse()

    def _parse_transmit_data(self, data):
        data_type = data.type

        if data_type == io_pb2.MSG_PING_REQUEST:
            return self._ping_request_to_bytes(data)
        elif data_type == io_pb2.MSG_BMS_REQUEST:
            return self._bms_request_to_bytes(data)
        elif data_type == io_pb2.MSG_IMU_REQUEST:
            return self._imu_request_to_bytes(data)
        elif data_type == io_pb2.MSG_MOTOR_REQUEST:
            return self._motor_request_to_bytes(data)

        return None

def scan_bus(com_buffer):
    my_address          = 1

    for device_address in range(255):
        print("scanning ", device_address)
        # ping test
        request        = io_pb2.PingRequest()

        request.type   = io_pb2.MSG_PING_REQUEST
        request.data_0 = random.randint(0, 2**31)
        request.data_1 = random.randint(0, 2**31)
        request.data_2 = random.randint(0, 2**31)
        request.data_3 = random.randint(0, 2**31)

        rx_src_address, rx_dest_address, rx_crc_status, response, status = com_buffer.send(my_address, device_address, request)

        if status and rx_crc_status:
            if response.data_0 == request.data_0 and response.data_1 == request.data_1 and response.data_2 == request.data_2 and response.data_3 == request.data_3:
                print("device response on address ", device_address)

import json

if __name__ == "__main__":

    #port       =  "/dev/tty.usbserial-0001"
    port        =  "/dev/tty.usbmodem1203"
    baud_rate   =  115200


    endpoint = SerialProtocolEndpoint(port, baud_rate)
    
    #scan_bus(endpoint)

    
    my_address          = 7
    device_address      = 100

    # ping test
    request        = io_pb2.PingRequest()

    request.type   = io_pb2.MSG_PING_REQUEST
    request.data_0 = random.randint(0, 2**31)
    request.data_1 = random.randint(0, 2**31)
    request.data_2 = random.randint(0, 2**31)
    request.data_3 = random.randint(0, 2**31)

    rx_src_address, rx_dest_address, rx_crc_status, response, status = endpoint.send(my_address, device_address, request)

    print("ping test ", rx_crc_status, status)
    print(response, "\n\n")
    

    
    # BMS test
    request        = io_pb2.BMSRequest()

    request.type              = io_pb2.MSG_BMS_REQUEST
    request.main_bus_a_enable = True
    request.main_bus_b_enable = True
    
    rx_src_address, rx_dest_address, rx_crc_status, response, status = endpoint.send(my_address, device_address, request)

    print("bms test ", rx_crc_status, status)
    print(response, "\n\n")

    # IMU test
    request        = io_pb2.IMURequest()

    request.type              = io_pb2.MSG_IMU_REQUEST
    
    rx_src_address, rx_dest_address, rx_crc_status, response, status = endpoint.send(my_address, device_address, request)

    print("IMU test ", rx_crc_status, status)
    print(response, "\n\n")



    # MOTOR test
    request        = io_pb2.MotorRequest()

    request.type   = io_pb2.MSG_MOTOR_REQUEST
    
    rx_src_address, rx_dest_address, rx_crc_status, response, status = endpoint.send(my_address, device_address, request)

    print("Motor test ", rx_crc_status, status)
    print(response, "\n\n")


    print("done")
    