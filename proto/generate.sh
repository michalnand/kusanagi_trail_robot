nanopb_generator ll.proto -D ../firmware/proto/


OUTPUT_PATH="../src/robot-hw-endpoint/src/robot_hw_endpoint/ll/"

python3 -m grpc_tools.protoc --proto_path=. --python_out=$OUTPUT_PATH ll.proto

OUTPUT_PATH="../src/robot-hw-endpoint/src/robot_hw_endpoint/grpc/"
python3 -m grpc_tools.protoc --proto_path=. --python_out=$OUTPUT_PATH ll.proto
python3 -m grpc_tools.protoc --proto_path=. --python_out=$OUTPUT_PATH --grpc_python_out=$OUTPUT_PATH hardware_endpoint.proto


sed -i '' 's/^import ll_pb2 as ll__pb2$/from . import ll_pb2 as ll__pb2/' $OUTPUT_PATH/hardware_endpoint_pb2.py
sed -i '' 's/^import hardware_endpoint_pb2 as hardware__endpoint__pb2$/from . import hardware_endpoint_pb2 as hardware__endpoint__pb2/' $OUTPUT_PATH/hardware_endpoint_pb2_grpc.py