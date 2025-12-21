

## LED connection

| led           | gpio    | color  |
| ------------- | ------- | ------ |
| led1          | PA5     | red    |
| led2          | PA6     | blue   |


## RS485 connection

| function      | gpio    | uart function  |
| ------------- | ------- | -------------  |
| USART_TX      | PA2     | USART2_TX, AF1 |
| USART_RX      | PA3     | USART2_RX, AF1 |
| USART_DIR     | PA4     | gpio           |



## address connection

| function      | gpio    |
| ------------- | ------- |
| a0            | PB3     |
| a1            | PB4     |
| a2            | PB5     |




## PWM connection

| phase           | gpio    | timer function |
| --------------- | ------- | ---------------|
| phase A high    | PA8     | TIM1_CH1, AF2  |   
| phase A low     | PA7     | TIM1_CH1N, AF2 |
| phase B high    | PA9     | TIM1_CH2, AF2  |
| phase B low     | PB0     | TIM1_CH2N, AF2 |
| phase C high    | PA10    | TIM1_CH3, AF2  |
| phase C low     | PB1     | TIM1_CH3N, AF2 |


## HALL connection

| hall sensor    | gpio   |
| ------------- | ------- |
| H1            | PB8     |
| H2            | PB7     |
| H3            | PB6     |
