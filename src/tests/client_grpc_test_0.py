import robot_hw_endpoint
import time
import numpy

if __name__ == "__main__":

    client = robot_hw_endpoint.HWClient()

    device_address = 100
    
    response = client.ping(device_address)
    print("ping test ", response.receive_status, response.crc_status, response.ping_status)


    request = robot_hw_endpoint.hardware_endpoint_pb2.BMSRequest()
    request.main_bus_a_enable = True
    request.main_bus_b_enable = True

    response = client.BMSControl(device_address, request)
    print("BMS test ", response.receive_status, response.crc_status)
    print(response)
