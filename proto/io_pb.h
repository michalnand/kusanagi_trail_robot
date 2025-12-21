#ifndef PB_IO_PB_H_INCLUDED
#define PB_IO_PB_H_INCLUDED

/* Enum definitions */
typedef enum _MessageType {
    MessageType_MSG_NONE = 0,
    MessageType_MSG_PING_REQUEST = 1,
    MessageType_MSG_PING_RESPONSE = 2,
    MessageType_MSG_BMS_REQUEST = 3,
    MessageType_MSG_BMS_RESPONSE = 4,
    MessageType_MSG_IMU_REQUEST = 5,
    MessageType_MSG_IMU_RESPONSE = 6,
    MessageType_MSG_MOTOR_REQUEST = 7,
    MessageType_MSG_MOTOR_RESPONSE = 8
} MessageType;

/* Struct definitions */
/* ping test */
typedef struct _PingRequest {
    uint32_t type;
    int32_t data_0;
    int32_t data_1;
    int32_t data_2;
    int32_t data_3;
} PingRequest;  

typedef struct _PingResponse {
    uint32_t type;
    int32_t data_0;
    int32_t data_1;
    int32_t data_2;
    int32_t data_3;
} PingResponse;

/* baterry power management */
typedef struct _BMSRequest {
    uint32_t type;
    bool main_bus_a_enable;
    bool main_bus_b_enable;
} BMSRequest;

typedef struct _BMSResponse {
    uint32_t type;
    int32_t cell_voltage_1;
    int32_t cell_voltage_2;
    int32_t cell_voltage_3;
    int32_t main_bus_a_voltage;
    int32_t main_bus_a_current;
    int32_t main_bus_b_voltage;
    int32_t main_bus_b_current;
    bool main_bus_a_enabled;
    bool main_bus_b_enabled;
} BMSResponse;

/* IMU + GPS unit */
typedef struct _IMURequest {
    uint32_t type;
} IMURequest;

typedef struct _IMUResponse {
    uint32_t type;
    int32_t roll;
    int32_t pitch;
    int32_t yaw;
    int32_t gps_x;
    int32_t gps_y;
    int32_t gps_h;
} IMUResponse;

/* motor controll unit */
typedef struct _MotorRequest {
    uint32_t type;
    int32_t required_velocity;
    int32_t maximum_velocity;
    int32_t maximum_acceleration;
} MotorRequest;

typedef struct _MotorResponse {
    uint32_t type;
    int32_t current_velocity;
    int32_t current_encoder;
} MotorResponse;


#endif
