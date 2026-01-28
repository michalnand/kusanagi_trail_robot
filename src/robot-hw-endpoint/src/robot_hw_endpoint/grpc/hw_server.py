import numpy
import grpc
from concurrent import futures

import random

#import hardware_endpoint_pb2


from robot_hw_endpoint.ll.serial_protocol_endpoint import SerialProtocolEndpoint
from robot_hw_endpoint.grpc import ll_pb2, hardware_endpoint_pb2, hardware_endpoint_pb2_grpc

class HWServer:
    def __init__(self, config):
        self.hw_endpoint = SerialProtocolEndpoint(config["serial_port"], config["serial_baud_rate"])
        
        # TODO camera endpoint
        #self.camera_endpoint = CameraEndpoint(config["camera"])

        self.master_address = config["serial_master_address"]

        print("HWServer running")

    def Ping(self, request, context):

        device_address = request.dest_adr

        # create ping request
        ll_request        = ll_pb2.LLPingRequest()

        ll_request.type   = ll_pb2.MSG_PING_REQUEST
        ll_request.data_0 = random.randint(0, 2**31)
        ll_request.data_1 = random.randint(0, 2**31)
        ll_request.data_2 = random.randint(0, 2**31)
        ll_request.data_3 = random.randint(0, 2**31)

        # send to bus
        rx_src_address, rx_dest_address, rx_crc_status, ll_response, status = self.hw_endpoint.send(self.master_address, device_address, ll_request)

        # validate response

        if status == True and rx_crc_status == True:
            data_match = True

            if ll_request.data_0 != ll_response.data_0:
                data_match = False
            if ll_request.data_1 != ll_response.data_1:
                data_match = False
            if ll_request.data_2 != ll_response.data_2:
                data_match = False
            if ll_request.data_3 != ll_response.data_3:
                data_match = False
        else:
            data_match = False


        response = hardware_endpoint_pb2.PingResponse(
            receive_status  = status,
            crc_status      = rx_crc_status,
            ping_status     = data_match
        )

    
        return response
    


   
    def GetBMSStatus(self, request, context): 
        device_address = request.dest_adr

        # create status request
        ll_request          = ll_pb2.LLStatusRequest()
        ll_request.dest_adr = device_address

        # send to bus
        rx_src_address, rx_dest_address, rx_crc_status, ll_response, status = self.hw_endpoint.send(self.master_address, device_address, ll_request)

        # parse response
        response = ll_pb2.BMSResponse()
        response.receive_status = status
        response.crc_status     = rx_crc_status
        if status == True and rx_crc_status == True and ll_response is not None:
            response.response = ll_response



        return response



    def BMSControl(self, request, context):
        device_address = request.dest_adr


        # create ping request
        ll_request          = ll_pb2.LLBMSRequest()

        ll_request.type              = ll_pb2.MSG_BMS_REQUEST
        ll_request.main_bus_a_enable = request.main_bus_a_enable
        ll_request.main_bus_b_enable = request.main_bus_b_enable

        
        # send to bus
        rx_src_address, rx_dest_address, rx_crc_status, ll_response, status = self.hw_endpoint.send(self.master_address, device_address, ll_request)
        
        print("AAAA")
        print(ll_response)

        
        # parse response
        response = hardware_endpoint_pb2.BMSResponse()

        response.receive_status     = status
        response.crc_status         = rx_crc_status

        if status == True and rx_crc_status == True and ll_response is not None:
            if ll_response.type == ll_pb2.MSG_BMS_RESPONSE:
                response.cell_voltage_1        = ll_response.cell_voltage_1 
                response.cell_voltage_2        = ll_response.cell_voltage_2
                response.cell_voltage_3        = ll_response.cell_voltage_3  

                response.main_bus_a_voltage    = ll_response.main_bus_a_voltage 
                response.main_bus_a_current    = ll_response.main_bus_a_current

                response.main_bus_b_voltage    = ll_response.main_bus_b_voltage 
                response.main_bus_b_current    = ll_response.main_bus_b_current

                response.main_bus_a_enabled     = ll_response.main_bus_a_enabled
                response.main_bus_b_enabled     = ll_response.main_bus_b_enabled
            else:
                response.receive_status = False
        else:
            response.receive_status = False

        return response





    def GetIMUStatus(self, request, context):
        pass

    def IMUControl(self, request, context):
        pass

    def GetMotorStatus(self, request, context):
        pass

    def MotorControl(self, request, context):
        pass

    def Camera(self, request, context):
        '''
        img = numpy.random.randint(
            0, 256,
            size=(request.height, request.width, request.channels),
            dtype=numpy.uint8
        )

        return hw_pb2.CameraResponse(
            width=request.width,
            height=request.height,
            channels=request.channels,
            image=img.tobytes()
        )
        '''


def hw_serve(server_config):
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=server_config["max_workers"]))

    hardware_endpoint_pb2_grpc.add_HardwareEndpointServicer_to_server(HWServer(server_config), server)

    server.add_insecure_port("[::]:" + str(server_config["port"]))
    server.start()
    server.wait_for_termination()


if __name__ == "__main__":
    server_config = {}
    server_config["serial_port"]        = "/dev/tty.usbmodem1203"
    server_config["serial_baud_rate"]   = 115200
    server_config["serial_master_address"] = 1
    server_config["port"]               = 50051
    server_config["max_workers"]        = 8
    

    hw_serve(server_config)