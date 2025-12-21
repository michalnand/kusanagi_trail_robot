#include "drivers.h"
#include "common.h"
#include "tmath.h"


#define PWM_FREQEUNCY       ((uint32_t)(20000))
#define PWM_VALUE_MAX       (((uint32_t)64000000)/PWM_FREQEUNCY)


static void GPIO_Init_TIM1(void)
{
    /* Enable GPIO clocks */
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    LL_GPIO_InitTypeDef gpio = {0};

    /* PA8  - TIM1_CH1 */
    /* PA9  - TIM1_CH2 */
    /* PA10 - TIM1_CH3 */
    gpio.Pin        = LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_10;
    gpio.Mode       = LL_GPIO_MODE_ALTERNATE;
    gpio.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Pull       = LL_GPIO_PULL_NO;
    gpio.Alternate  = LL_GPIO_AF_2;
    LL_GPIO_Init(GPIOA, &gpio);

    /* PA7 - TIM1_CH1N */
    gpio.Pin = LL_GPIO_PIN_7;
    LL_GPIO_Init(GPIOA, &gpio);

    /* PB0 - TIM1_CH2N */
    /* PB1 - TIM1_CH3N */
    gpio.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1;
    LL_GPIO_Init(GPIOB, &gpio);
}


static void TIM1_Init_PWM(void)
{
    /* Enable TIM1 clock */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

    /* Basic timer configuration */
    LL_TIM_SetPrescaler(TIM1, 0);
    LL_TIM_SetAutoReload(TIM1, PWM_VALUE_MAX - 1);
    LL_TIM_SetCounterMode(TIM1, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockDivision(TIM1, LL_TIM_CLOCKDIVISION_DIV1);

    /* PWM Mode 1 on all channels */
    LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_PWM1);

    /* Enable preload */
    LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH1);
    LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH2);
    LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH3);
    LL_TIM_EnableARRPreload(TIM1);

    /* Polarity */
    LL_TIM_OC_SetPolarity(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_SetPolarity(TIM1, LL_TIM_CHANNEL_CH2, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_SetPolarity(TIM1, LL_TIM_CHANNEL_CH3, LL_TIM_OCPOLARITY_HIGH);

    LL_TIM_OC_SetPolarity(TIM1, LL_TIM_CHANNEL_CH1N, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_SetPolarity(TIM1, LL_TIM_CHANNEL_CH2N, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_SetPolarity(TIM1, LL_TIM_CHANNEL_CH3N, LL_TIM_OCPOLARITY_HIGH);

    /* Initial duty = 0 */
    LL_TIM_OC_SetCompareCH1(TIM1, 0);
    LL_TIM_OC_SetCompareCH2(TIM1, 0);
    LL_TIM_OC_SetCompareCH3(TIM1, 0);

    /* Dead-time + MOE */
    LL_TIM_BDTR_InitTypeDef bdtr = {0};
    bdtr.OSSRState       = LL_TIM_OSSR_ENABLE;
    bdtr.OSSIState       = LL_TIM_OSSI_ENABLE;
    bdtr.LockLevel       = LL_TIM_LOCKLEVEL_OFF;
    bdtr.DeadTime        = 20;  // ~312ns wait time
    bdtr.BreakState      = LL_TIM_BREAK_DISABLE;
    bdtr.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_ENABLE;
    LL_TIM_BDTR_Init(TIM1, &bdtr);

    /* Enable outputs */
    LL_TIM_CC_EnableChannel(TIM1,
        LL_TIM_CHANNEL_CH1  | LL_TIM_CHANNEL_CH1N |
        LL_TIM_CHANNEL_CH2  | LL_TIM_CHANNEL_CH2N |
        LL_TIM_CHANNEL_CH3  | LL_TIM_CHANNEL_CH3N);
        
    /* Enable counter */
    LL_TIM_EnableCounter(TIM1);

    /* Main output enable */
    LL_TIM_EnableAllOutputs(TIM1);
}




void BLDC_SetPhasePWM(uint16_t pwmA, uint16_t pwmB, uint16_t pwmC)
{
    LL_TIM_OC_SetCompareCH1(TIM1, pwmA);
    LL_TIM_OC_SetCompareCH2(TIM1, pwmB);
    LL_TIM_OC_SetCompareCH3(TIM1, pwmC);
}

int main() 
{       

    drivers_init();   
    
    Timer timer;
    
    Gpio<'A', 5, GPIO_MODE_OUT> led_1;
    Gpio<'A', 6, GPIO_MODE_OUT> led_2;
    
    
    led_1 = 1;  
    led_2 = 1;

    timer.init();
    
    GPIO_Init_TIM1();
    TIM1_Init_PWM();

    uint32_t cnt   = 0;
    uint32_t speed = 0;
    uint32_t phase = 0;

    uint32_t torque = 50;

    while (1)
    {
        uint32_t a = sine_table[(phase + (SINE_TABLE_SIZE*0)/3)%SINE_TABLE_SIZE];
        uint32_t b = sine_table[(phase + (SINE_TABLE_SIZE*1)/3)%SINE_TABLE_SIZE];
        uint32_t c = sine_table[(phase + (SINE_TABLE_SIZE*2)/3)%SINE_TABLE_SIZE];

        
        a = (torque*a*PWM_VALUE_MAX)/(100*SINE_VALUE_MAX);
        b = (torque*b*PWM_VALUE_MAX)/(100*SINE_VALUE_MAX);
        c = (torque*c*PWM_VALUE_MAX)/(100*SINE_VALUE_MAX);


        BLDC_SetPhasePWM(a, b, c);

        if ((phase%200) > 100)
        {
            led_2 = 1;
        }
        else
        {
            led_2 = 0;
        }

        phase+= speed;

        if ((cnt%100) == 0)
        {
            if (speed < 10)
            {
                speed++;
            }
        }

        cnt++;
        timer.delay_ms(1);
    }

    

    return 0;
}
