import grpc

from robot_hw_endpoint.grpc import ll_pb2, hardware_endpoint_pb2, hardware_endpoint_pb2_grpc


class HWClient:
    def __init__(self, server_address = "localhost:50051"):
        self.server_address = server_address


    def ping(self, dest_adr):
        channel = grpc.insecure_channel(self.server_address)
        stub = hardware_endpoint_pb2_grpc.HardwareEndpointStub(channel)

        request = hardware_endpoint_pb2.PingRequest(dest_adr=dest_adr)

        resp = stub.Ping(request)
        
        return resp
    

    def BMSControl(self, dest_adr, request):
        channel = grpc.insecure_channel(self.server_address)
        stub = hardware_endpoint_pb2_grpc.HardwareEndpointStub(channel)

        resp = stub.BMSControl(request)
        
        return resp
    
