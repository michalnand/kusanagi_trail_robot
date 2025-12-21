import struct
import io_pb2 

class IOData:

    # Empty
    EMPTY_FORMAT = "<i"
    EMPTY_SIZE   = struct.calcsize(EMPTY_FORMAT)

    # Ping
    PING_FORMAT = "<iiiii"
    PING_SIZE   = struct.calcsize(PING_FORMAT)

    # BMS
    BMS_REQ_FORMAT = "<i??"
    BMS_REQ_SIZE   = struct.calcsize(BMS_REQ_FORMAT)

    BMS_RESP_FORMAT = "<iiiiiiii??"
    BMS_RESP_SIZE   = struct.calcsize(BMS_RESP_FORMAT)

    # IMU
    IMU_REQ_FORMAT = "<i"
    IMU_REQ_SIZE   = struct.calcsize(IMU_REQ_FORMAT)

    IMU_RESP_FORMAT = "<iiiiiii"
    IMU_RESP_SIZE   = struct.calcsize(IMU_RESP_FORMAT)

    # Motor
    MOTOR_REQ_FORMAT = "<iiii"
    MOTOR_REQ_SIZE   = struct.calcsize(MOTOR_REQ_FORMAT)

    MOTOR_RESP_FORMAT = "<iii"
    MOTOR_RESP_SIZE   = struct.calcsize(MOTOR_RESP_FORMAT)



    def _ping_request_to_bytes(self, msg) -> bytes:
        return struct.pack(
            self.PING_FORMAT,
            msg.type,
            msg.data_0,
            msg.data_1,
            msg.data_2,
            msg.data_3,
        )
    
    def _ping_response_from_bytes(self, buffer: bytes) -> io_pb2.PingResponse:
        values = struct.unpack(self.PING_FORMAT, buffer[:self.PING_SIZE])

        msg = io_pb2.PingResponse()
        msg.type   = values[0]
        msg.data_0 = values[1]
        msg.data_1 = values[2]
        msg.data_2 = values[3]
        msg.data_3 = values[4]
        return msg



    def _bms_request_to_bytes(self, msg) -> bytes:
        return struct.pack(
            self.BMS_REQ_FORMAT,
            msg.type,
            msg.main_bus_a_enable,
            msg.main_bus_b_enable,
        )
    
    def _bms_response_from_bytes(self, buffer: bytes) -> io_pb2.BMSResponse:
        v = struct.unpack(self.BMS_RESP_FORMAT, buffer[:self.BMS_RESP_SIZE])

        msg = io_pb2.BMSResponse()

        msg.type                = v[0]
        msg.cell_voltage_1      = v[1]
        msg.cell_voltage_2      = v[2]
        msg.cell_voltage_3      = v[3]
        msg.main_bus_a_voltage  = v[4]
        msg.main_bus_a_current  = v[5]
        msg.main_bus_b_voltage  = v[6]
        msg.main_bus_b_current  = v[7]
        msg.main_bus_a_enabled  = v[8]
        msg.main_bus_b_enabled  = v[9]
        return msg
    


    def _imu_request_to_bytes(self, msg) -> bytes:
        return struct.pack(self.IMU_REQ_FORMAT, msg.type)
    
    def _imu_response_from_bytes(self, buffer: bytes) -> io_pb2.IMUResponse:
        v = struct.unpack(self.IMU_RESP_FORMAT, buffer[:self.IMU_RESP_SIZE])

        msg = io_pb2.IMUResponse()
        msg.type  = v[0]
        msg.roll  = v[1]
        msg.pitch = v[2]
        msg.yaw   = v[3]
        msg.gps_x = v[4]
        msg.gps_y = v[5]
        msg.gps_h = v[6]
        return msg

   
    
    def _motor_request_to_bytes(self, msg) -> bytes:
        return struct.pack(
            self.MOTOR_REQ_FORMAT,
            msg.type,
            msg.required_velocity,
            msg.maximum_velocity,
            msg.maximum_acceleration,
        )
    
    def _motor_response_from_bytes(self, buffer: bytes) -> io_pb2.MotorResponse:
        v = struct.unpack(self.MOTOR_RESP_FORMAT, buffer[:self.MOTOR_RESP_SIZE])

        msg = io_pb2.MotorResponse()
        msg.type              = v[0]
        msg.current_velocity  = v[1]
        msg.current_encoder   = v[2]
        return msg
    

    
    
    def _empty_response_from_bytes(self, buffer: bytes) -> io_pb2.EmptyResponse:
        (msg_type,) = struct.unpack(self.EMPTY_FORMAT, buffer[:self.EMPTY_SIZE])

        msg = io_pb2.EmptyResponse()
        msg.type = msg_type
        return msg

   
