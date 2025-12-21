#include "com_driver.h"

#include "drivers.h"


#ifdef __cplusplus
extern "C" {
#endif

COMDriver *g_com_driver;

void USART2_IRQHandler(void)
{
    if (LL_USART_IsActiveFlag_RXNE(USART2))
    {
        uint8_t c = LL_USART_ReceiveData8(USART2);
        g_com_driver->_rx_callback(c);
    }

    if (LL_USART_IsActiveFlag_ORE(USART2))
    {
        LL_USART_ClearFlag_ORE(USART2);
    }
    if (LL_USART_IsActiveFlag_FE(USART2))
    {
        LL_USART_ClearFlag_FE(USART2);
    }
    if (LL_USART_IsActiveFlag_NE(USART2))
    {
        LL_USART_ClearFlag_NE(USART2);
    }
}


#ifdef __cplusplus
}
#endif



void COMDriver::init(uint8_t device_address)
{
    g_com_driver = this;

    // self address
    this->device_address = device_address;

    this->rx_crc    = 0;
    this->rx_ptr    = 0;
    this->rx_state  = 0;
         
    this->rx_src_address    = 0;
    this->rx_dest_address   = 0;

    this->rx_valid = false;

    // clear transmit buffer
    for (unsigned int i = 0; i < DATA_SIZE; i++)
    {
        tx_buffer[i] = 0;   
    }

    // clear receive buffer
    for (unsigned int i = 0; i < DATA_SIZE; i++)
    {
        rx_buffer[i] = 0;   
    }

    for (unsigned int i = 0; i < DATA_SIZE; i++)
    {
        this->received_data[i] = 0;
    }

    // transmitter disable, enable receiver
    this->dir = 0;

    // init uart incl interrupt
    this->_uart_init();
}

void COMDriver::send_data(uint8_t dest_adr, uint8_t *data, uint8_t data_size)
{
    // transmitter enable
    this->dir = 1;

    // init buffer
    for (unsigned int i = 0; i < DATA_SIZE; i++)
    {
        this->tx_buffer[i] = 0;
    }

    // copy data
    for (unsigned int i = 0; i < data_size; i++)
    {
        this->tx_buffer[i] = data[i];
    }

    // send sync preambule
    for (unsigned int i = 0; i < P_SIZE; i++)
    {
        _uart_putc(P_BYTE);
    }

    // send sync bytes
    _uart_putc(P_SYNC_0);
    _uart_putc(P_SYNC_1);

    // send source address
    _uart_putc(this->device_address);

    // send destination address
    _uart_putc(dest_adr);

    // update CRC   
    uint32_t crc = 0;
    crc = _update_crc(crc, this->device_address);
    crc = _update_crc(crc, dest_adr);
    

    // send data
    for (unsigned int i = 0; i < DATA_SIZE; i++)
    {
        uint8_t c = this->tx_buffer[i];
        _uart_putc(c);
        

        crc = _update_crc(crc, c);
    }

    // send CRC
    _uart_putc(crc&0xFF);

    // transmitter disable, enable receiver
    this->dir = 0;
}

void COMDriver::rx_enable()
{
    this->dir      = 0;
    this->rx_state = 0;
    this->rx_valid = false;
}

uint8_t COMDriver::get_src_adr()
{
    return this->rx_src_address;  
}

uint8_t COMDriver::rx_data_type()
{
    return this->received_data[0];  
}



void COMDriver::_rx_callback(uint8_t c)
{
    // transmitter disable, enable receiver
    this->dir = 0;

    // wait for first sync
    if (this->rx_state == 0)
    {
        this->rx_crc    = 0;
        this->rx_ptr    = 0;

        this->rx_src_address  = 0;
        this->rx_dest_address = 0;

        this->rx_valid        = false;

        if (c == P_SYNC_0)
        {
            this->rx_state = 1;
        }
    }

    // wait for second sync
    else if (this->rx_state == 1)
    {
        if (c == P_SYNC_1)  
        {
            this->rx_state = 2;
        }
        else
        {
            this->rx_state = 0;
        }
    }


    // wait for source address
    else if (this->rx_state == 2)
    {
        this->rx_src_address    = c;
        this->rx_crc            = this->_update_crc(this->rx_crc, c);
        this->rx_state          = 3;
    }

    // wait for destination address
    else if (this->rx_state == 3)
    {
        
        this->rx_dest_address   = c;
        this->rx_crc            = this->_update_crc(this->rx_crc, c);
        this->rx_state          = 4;
    }

    // receive data
    else if (this->rx_state == 4)
    {
        if (this->rx_ptr < DATA_SIZE)
        {
            this->rx_buffer[this->rx_ptr]   = c;
            this->rx_crc                    = this->_update_crc(this->rx_crc, c);
            this->rx_ptr++; 
        }
        
        if (this->rx_ptr >= DATA_SIZE)
        {
            this->rx_state = 5;
        }
    }

    // receive CRC, last byte
    else if (this->rx_state == 5)
    {
        uint8_t crc_ref = c;
        
        // check address match and CRC  
        if ( (this->rx_dest_address == this->device_address) && (crc_ref == (this->rx_crc&0xff)) )
        {
            // set valid data flag
            this->rx_valid = true;
            
            // copy data
            for (unsigned int i = 0; i < DATA_SIZE; i++)
            {
                this->received_data[i] = this->rx_buffer[i];
            }

            // go to wait state
            this->rx_state = 6;
        }
        else
        {
            // receiving failed or data not for us, wait for next data
            this->rx_valid = false;
            this->rx_state = 0;
        }
    }

    else if (this->rx_state == 6)
    {
        // wait state after success receiving 
    }
}


uint32_t COMDriver::_update_crc(uint32_t crc_curr, uint8_t byte)
{
    uint8_t crc = (uint8_t)crc_curr;  // use only low 8 bits

    crc ^= byte;
    for (uint8_t i = 0; i < 8; i++)
    {
        if (crc & 0x80)
            crc = (crc << 1) ^ 0x31;  // CRC-8 Dallas/Maxim polynomial
        else
            crc <<= 1;
    }

    return (uint32_t)crc;
}








 
void COMDriver::_uart_init()
{
    // 1. Enable GPIOA and USART2 clocks
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

    // 2. Configure PA2 (TX) and PA3 (RX)
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_2, LL_GPIO_AF_1);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_2, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_2, LL_GPIO_PULL_UP);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_3, LL_GPIO_AF_1);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_3, LL_GPIO_PULL_UP);

    // 3. Configure USART2
    LL_USART_SetTransferDirection(USART2, LL_USART_DIRECTION_TX_RX);
    LL_USART_SetParity(USART2, LL_USART_PARITY_NONE);
    LL_USART_SetDataWidth(USART2, LL_USART_DATAWIDTH_8B);
    LL_USART_SetStopBitsLength(USART2, LL_USART_STOPBITS_1);

    LL_USART_SetBaudRate(USART2,
        SystemCoreClock,
        LL_USART_PRESCALER_DIV1,
        LL_USART_OVERSAMPLING_16,
        115200);

    // 4. Enable RX interrupt   
    LL_USART_EnableIT_RXNE(USART2);
    LL_USART_EnableIT_ERROR(USART2);   // Optional but recommended

    // 5. Enable USART
    LL_USART_Enable(USART2);

    while (!LL_USART_IsActiveFlag_TEACK(USART2) ||
           !LL_USART_IsActiveFlag_REACK(USART2))
    {
        __asm("nop");
    }

    // 6. Enable NVIC
    NVIC_SetPriority(USART2_IRQn, 1);
    NVIC_EnableIRQ(USART2_IRQn);
}

    
void COMDriver::_uart_putc(char c)
{
    // Wait until TXE (transmit buffer empty)
    while (!LL_USART_IsActiveFlag_TXE(USART2))
    {
        __asm("nop");
    }
    
    LL_USART_TransmitData8(USART2, c);
}
