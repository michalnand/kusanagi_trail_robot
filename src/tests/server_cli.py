import robot_hw_endpoint
import random

if __name__ == "__main__":

    server_config = {}
    server_config["serial_port"]        = "/dev/tty.usbmodem1203"
    server_config["serial_baud_rate"]   = 115200
    server_config["serial_master_address"] = 1
    server_config["port"]               = 50051
    server_config["max_workers"]        = 8


    robot_hw_endpoint.hw_serve(server_config)
    