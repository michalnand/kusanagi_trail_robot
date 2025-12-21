#ifndef _COM_DRIVER_H_
#define _COM_DRIVER_H_

#include "drivers.h"

// preambule bytes
#define P_BYTE      ((uint8_t)0xAA)

// sync bytes
#define P_SYNC_0    ((uint8_t)0xAB)
#define P_SYNC_1    ((uint8_t)0xCD)
#define P_SYNC_2    ((uint8_t)0xEF)


// packet size
#define DATA_SIZE   ((uint32_t)40)


#define PREAMBULE_BYTES_COUNT   ((uint32_t)8)


class COMDriver
{
    public:
        void init();
        void init_rx();

    public:
        void rx_callback(uint8_t b);

        void tx_data(uint8_t *data);
            
    private:
        void uart_init();
        uint32_t update_crc(uint32_t crc_curr, uint8_t byte);

        void uart_tx(uint8_t b);
    

    
    public:
        bool rx_done;
        uint8_t rx_data[DATA_SIZE];
        
    private:
        uint8_t     state;
        uint32_t    data_ptr;
        uint32_t    crc_curr;

        uint8_t     rx_buffer[DATA_SIZE];
};


#endif