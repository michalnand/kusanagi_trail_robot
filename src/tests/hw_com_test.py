import robot_hw_endpoint
import random

if __name__ == "__main__":

    #port       =  "/dev/tty.usbserial-0001"
    port        =  "/dev/tty.usbmodem1203"
    baud_rate   =  115200


    endpoint = robot_hw_endpoint.SerialProtocolEndpoint(port, baud_rate)
    
    #scan_bus(endpoint)


    my_address          = 7
    device_address      = 100

    # ping test 
    request        = robot_hw_endpoint.ll_pb2.LLPingRequest()

    request.type   = robot_hw_endpoint.ll_pb2.MSG_PING_REQUEST
    request.data_0 = random.randint(0, 2**31)
    request.data_1 = random.randint(0, 2**31)
    request.data_2 = random.randint(0, 2**31)
    request.data_3 = random.randint(0, 2**31)

    print(request)

    rx_src_address, rx_dest_address, rx_crc_status, response, status = endpoint.send(my_address, device_address, request)

    print("ping test ", rx_crc_status, status)
    print(response, "\n\n")





    my_address          = 7
    device_address      = 100

    # ping test 
    request        = robot_hw_endpoint.ll_pb2.LLStatusRequest()

    request.type   = robot_hw_endpoint.ll_pb2.MSG_STATUS_REQUEST
    
    print(request)

    rx_src_address, rx_dest_address, rx_crc_status, response, status = endpoint.send(my_address, device_address, request)

    print("status test ", rx_crc_status, status)
    print(response, "\n\n")
    

    
    # BMS test
    request        = robot_hw_endpoint.ll_pb2.LLBMSRequest()

    request.type              = robot_hw_endpoint.ll_pb2.MSG_BMS_REQUEST
    request.main_bus_a_enable = True
    request.main_bus_b_enable = True
    
    rx_src_address, rx_dest_address, rx_crc_status, response, status = endpoint.send(my_address, device_address, request)

    print("bms test ", rx_crc_status, status)
    print(response, "\n\n")

    # IMU test
    request        = robot_hw_endpoint.ll_pb2.LLIMURequest()

    request.type    = robot_hw_endpoint.ll_pb2.MSG_IMU_REQUEST
    
    rx_src_address, rx_dest_address, rx_crc_status, response, status = endpoint.send(my_address, device_address, request)

    print("IMU test ", rx_crc_status, status)
    print(response, "\n\n")



    # MOTOR test    
    request        = robot_hw_endpoint.ll_pb2.LLMotorRequest()

    request.type   = robot_hw_endpoint.ll_pb2.MSG_MOTOR_REQUEST
    
    rx_src_address, rx_dest_address, rx_crc_status, response, status = endpoint.send(my_address, device_address, request)

    print("Motor test ", rx_crc_status, status)
    print(response, "\n\n")
    