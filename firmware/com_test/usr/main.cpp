#include "drivers.h"
#include "common.h"
#include "tmath.h"


Timer   timer;

                
#include "com_driver.h"

#include <io_pb.h>

// my device address
#define MY_DEVICE_ADR   ((uint8_t)100)

COMDriver com_driver;


// ping handler
void process_ping(uint8_t dest_adr, PingRequest &received)
{
    PingResponse response;

    response.type   = MessageType::MessageType_MSG_PING_RESPONSE;
    response.data_0 = received.data_0;
    response.data_1 = received.data_1;
    response.data_2 = received.data_2;  
    response.data_3 = received.data_3;

    send_data<PingResponse>(com_driver, dest_adr, response);
}


// bms dummy handler
void process_bms(uint8_t dest_adr, BMSRequest &received)
{
    BMSResponse response;

    response.type   = MessageType::MessageType_MSG_BMS_RESPONSE;
    response.cell_voltage_1 = 10;
    response.cell_voltage_2 = 15;
    response.cell_voltage_3 = 17;

    response.main_bus_a_voltage = 1234;
    response.main_bus_a_current = 1500;
    response.main_bus_b_voltage = 4321;
    response.main_bus_b_current = 2100;

    response.main_bus_a_enabled = true;
    response.main_bus_b_enabled = true;
    
    send_data<BMSResponse>(com_driver, dest_adr, response);
}

// imu dummy handler
void process_imu(uint8_t dest_adr, IMURequest &received)
{
    IMUResponse response;

    response.type   = MessageType::MessageType_MSG_IMU_RESPONSE;
    response.roll   = 1000;
    response.pitch  = 2000;
    response.yaw    = 3000;

    response.gps_x = 1234;
    response.gps_y = 2345;
    response.gps_h = 3456;
    
    send_data<IMUResponse>(com_driver, dest_adr, response);
}



// motor dummy handler
void process_motor(uint8_t dest_adr, MotorRequest &received)
{
    MotorResponse response;

    response.type   = MessageType::MessageType_MSG_MOTOR_RESPONSE;

    response.current_velocity = 1000000;
    response.current_encoder  = 1234567;
    
    send_data<MotorResponse>(com_driver, dest_adr, response);
}


int main() 
{       
    drivers_init();  
    
    timer.init();

    // debug leds
    Gpio<'C', 6, GPIO_MODE_OUT> led_0;
    Gpio<'A', 5, GPIO_MODE_OUT> led_1;
    Gpio<'A', 6, GPIO_MODE_OUT> led_2;
    
    led_0 = 0;
    led_1 = 1;  
    led_2 = 1;


    com_driver.init(MY_DEVICE_ADR);


    while (1)
    {
        if (com_driver.rx_valid == true)
        {
            // here process data
            led_0 = 1;
            
            
            // process ping
            if (com_driver.rx_data_type() == MessageType::MessageType_MSG_PING_REQUEST)
            {
                uint8_t     dest_adr = com_driver.get_src_adr();
                PingRequest received = process_received_data<PingRequest>(com_driver.received_data);

                process_ping(dest_adr, received);
            }

            // process dummy BMS
            else if (com_driver.rx_data_type() == MessageType::MessageType_MSG_BMS_REQUEST)
            {
                uint8_t     dest_adr = com_driver.get_src_adr();
                BMSRequest received = process_received_data<BMSRequest>(com_driver.received_data);

                process_bms(dest_adr, received);
            }

            // process dummy IMU
            else if (com_driver.rx_data_type() == MessageType::MessageType_MSG_IMU_REQUEST)
            {
                uint8_t     dest_adr = com_driver.get_src_adr();
                IMURequest received = process_received_data<IMURequest>(com_driver.received_data);

                process_imu(dest_adr, received);
            }

            // process dummy Motor
            else if (com_driver.rx_data_type() == MessageType::MessageType_MSG_MOTOR_REQUEST)
            {
                uint8_t      dest_adr = com_driver.get_src_adr();
                MotorRequest received = process_received_data<MotorRequest>(com_driver.received_data);

                process_motor(dest_adr, received);
            }
            else
            {
                // unknow type
            }
            
            // ready for next packet
            com_driver.rx_enable();

            led_0 = 0;  
        }
        else
        {
            timer.delay_ms(2);
        }
    }

    return 0;
}
