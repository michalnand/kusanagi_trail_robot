#include "drivers.h"
#include "common.h"
#include "tmath.h"

#include "imu_ll.h"

Terminal terminal;
Timer timer;
TI2C<'A', 0, 1> i2c;

                
#include "com_driver.h"

// my device address
#define DEVICE_ADR   ((uint8_t)100)

int main() 
{       
    
    drivers_init();   
    uart_init(); 


    //Terminal terminal;
    terminal << "\n\nuart init done\n";

    
    timer.init();
    i2c.init();

    IMULL imu_ll;
    imu_ll.init(i2c);


    COMDriver com_driver;
    com_driver.init();

    while (1)
    {
        // wait for data receving done
        if (com_driver.rx_done) 
        {
            // validate if data are for us
            if (com_driver.rx_data[0] == DEVICE_ADR)
            {   
                // process data
                //com_driver.rx_data;

                // send response
                //com_driver.tx_data(tx_buffer);
            }

            // preprare for next data - data not for us
            else
            {
                com_driver.init_rx();
            }
        }
    }
    
    /*
    while (1)
    {
        imu_ll.read();

        terminal << "###";

        terminal << "{ ";

        terminal << "\"ax\" : " << imu_ll.ax << ", ";
        terminal << "\"ay\" : " << imu_ll.ay << ", ";
        terminal << "\"az\" : " << imu_ll.az << ", ";

        terminal << "\"gx\" : " << imu_ll.gx << ", ";
        terminal << "\"gy\" : " << imu_ll.gy << ", ";
        terminal << "\"gz\" : " << imu_ll.gz << " ";
        
        terminal << "}\n";
        

        timer.delay_ms(100);
    }
    */

   
    return 0;
}
