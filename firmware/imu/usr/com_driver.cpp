#include "com_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

COMDriver *com_driver;

void USART2_IRQHandler(void)
{
    if (LL_USART_IsActiveFlag_RXNE(USART2))
    {
        uint8_t b = LL_USART_ReceiveData8(USART2);
        com_driver->rx_callback(b);
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




void COMDriver::init()
{
    this->init_rx();

    this->uart_init();

    com_driver = this;
}


void COMDriver::init_rx()
{
    this->state     = 0;
    this->crc_curr  = 0;
    this->data_ptr  = 0;

    this->rx_done   = false;
    
    for (unsigned int i = 0; i < DATA_SIZE; i++)
    {
        rx_buffer[i] = 0;
    }
}




void COMDriver::rx_callback(uint8_t c)
{
    // wait for first sync byte
    if (this->state == 0)
    {
        this->state    = 0;
        this->crc_curr = 0;
        this->data_ptr = 0;

        if (c == P_SYNC_0)
        {
            this->state = 1;
        }
    }

    // wait for second sync byte
    else if (this->state == 1)
    {
        if (c == P_SYNC_1)
        {
            this->state = 2;
        }
        else
        {
            this->state = 0;
        }
    }

    // wait for third sync byte
    else if (this->state == 2)
    {
        if (c == P_SYNC_2)
        {
            this->state = 3;
        }
        else
        {
            this->state = 0;
        }
    }

    // receive data
    else if (this->state == 3)
    {
        this->crc_curr = update_crc(this->crc_curr, c);

        this->rx_buffer[this->data_ptr] = c;
        this->data_ptr+= 1;

        if (this->data_ptr >= DATA_SIZE)
        {
            this->state = 4;
        }
    }

    // receive CRC and validate
    else if (this->state == 4)
    {
        // ground truth CRC
        uint8_t crc_gt = c;

        // compare CRC
        if ((this->crc_curr&0xff) == crc_gt)
        {
            // update valid data
            for (unsigned int i = 0; i < DATA_SIZE; i++)
            {
                this->rx_data[i] = this->rx_buffer[i];
            }
            
            // set flag
            this->rx_done = true;

            // waiting state
            this->state = 5;
        }

        // not valid data, next receiving cycle
        else
        {
            this->init_rx();
        }
    }
}


void COMDriver::tx_data(uint8_t *data)
{
    // send sync preambule
    for (unsigned int i = 0; i < PREAMBULE_BYTES_COUNT; i++)
    {
        uart_tx(P_BYTE);
    }

    // send sync
    uart_tx(P_SYNC_0);
    uart_tx(P_SYNC_1);
    uart_tx(P_SYNC_2);



    // send data and compute CRC
    uint32_t crc_curr = 0;

    for (unsigned int i = 0; i < DATA_SIZE; i++)
    {
        uint8_t b = data[i];
        crc_curr = update_crc(crc_curr, b);

        uart_tx(b);
    }

    // send CRC
    uart_tx(crc_curr);
}


void COMDriver::uart_init()
{
    /* 1. Enable GPIOA and USART2 clocks */
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

    /* 2. Configure PA2 (TX) and PA3 (RX) */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_2, LL_GPIO_AF_1);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_2, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_2, LL_GPIO_PULL_UP);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_3, LL_GPIO_AF_1);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_3, LL_GPIO_PULL_UP);

    /* 3. Configure USART2 */
    LL_USART_SetTransferDirection(USART2, LL_USART_DIRECTION_TX_RX);
    LL_USART_SetParity(USART2, LL_USART_PARITY_NONE);
    LL_USART_SetDataWidth(USART2, LL_USART_DATAWIDTH_8B);
    LL_USART_SetStopBitsLength(USART2, LL_USART_STOPBITS_1);

    LL_USART_SetBaudRate(USART2,
        SystemCoreClock,
        LL_USART_PRESCALER_DIV1,
        LL_USART_OVERSAMPLING_16,
        115200);

    /* 4. Enable RX interrupt */
    LL_USART_EnableIT_RXNE(USART2);
    LL_USART_EnableIT_ERROR(USART2);   // Optional but recommended

    /* 5. Enable USART */
    LL_USART_Enable(USART2);

    while (!LL_USART_IsActiveFlag_TEACK(USART2) ||
           !LL_USART_IsActiveFlag_REACK(USART2))
    {
        __asm("nop");
    }

    /* 6. Enable NVIC */
    NVIC_SetPriority(USART2_IRQn, 1);
    NVIC_EnableIRQ(USART2_IRQn);
}






uint32_t COMDriver::update_crc(uint32_t crc_curr, uint8_t byte)
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



void COMDriver::uart_tx(uint8_t b)
{
    // Wait until TXE (transmit buffer empty)
    while (!LL_USART_IsActiveFlag_TXE(USART2))
    {
        __asm("nop");
    }
    
    LL_USART_TransmitData8(USART2, b);
}
