#ifndef _COMDriver_
#define _COMDriver_

#include <stdint.h>

#include "com_def.h"

#include "drivers.h"

class COMDriver
{
    public:
        void init(uint8_t device_address);
        void send_data(uint8_t dest_adr, uint8_t *data, uint8_t data_size);
        void rx_enable();

        uint8_t get_src_adr();
        uint8_t rx_data_type();
        

        void _rx_callback(uint8_t c);



    private:
        uint32_t _update_crc(uint32_t crc_curr, uint8_t byte);

        void _uart_init();
        void _uart_putc(char c);

    // received data
    public:
        bool    rx_valid;
        uint8_t received_data[DATA_SIZE];

    private:
        uint8_t device_address; 

        uint32_t rx_crc;
        uint32_t rx_ptr;
        uint32_t rx_state;
         
        uint8_t rx_src_address;
        uint8_t rx_dest_address;

        uint8_t tx_buffer[DATA_SIZE];
        uint8_t rx_buffer[DATA_SIZE];

    private:
        Gpio<'A', 4, GPIO_MODE_OUT> dir;
        Gpio<'C', 6, GPIO_MODE_OUT> led;
};



template<class DataType> DataType process_received_data(uint8_t *buffer)
{
    DataType result;

    uint8_t *ptr;

    ptr = (uint8_t*)(&result); 

    for (unsigned int i = 0; i < sizeof(result); i++)
    {
        ptr[i] = buffer[i];
    }

    return result;
}

template<class DataType> void send_data(COMDriver &driver, uint8_t dest_adr, DataType &data)
{
    uint8_t *ptr;

    ptr = (uint8_t*)(&data);

    driver.send_data(dest_adr, ptr, sizeof(data));
}

#endif