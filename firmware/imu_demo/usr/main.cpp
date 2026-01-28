#include "drivers.h"
#include "common.h"
#include "tmath.h"

#include "imu_ll.h"

Terminal terminal;
Timer timer;
TI2C<'B', 7, 8, 5> i2c;

                

// my device address
#define DEVICE_ADR   ((uint8_t)100)

int main() 
{       
    drivers_init();   
    uart_init(); 

        
    Gpio<'A', 5> led_1;
    Gpio<'A', 6> led_2;


    //Terminal terminal;
    terminal << "\n\nuart init done\n";

    //timer.init();
    i2c.init();

    IMULL imu_ll;
    
    // turn on led if reading done
    if (imu_ll.init(i2c) == 0)
    {
        led_2 = 1;
    }

    while (1)
    {
        led_2 = 0;
        led_1 = 1;

        int status = imu_ll.read_status();
        imu_ll.read();  

        terminal << "\n\n>";
        terminal << "{\n";
        terminal << "\"status\"  : " << status << ",\n";
        terminal << "\"acc\"  : [" << imu_ll.ax << ", " << imu_ll.ay << ", " << imu_ll.az << "],\n";
        terminal << "\"gyro\" : [" << imu_ll.gx << ", " << imu_ll.gy << ", " << imu_ll.gz << "]\n";
        terminal << "}\n";
        terminal << "<";

       


        delay_loops(10000);
        
        if (status == 1)
        {
            led_2 = 1;
        }

        led_1 = 0;
        delay_loops(100000);
    }



   
    return 0;
}
