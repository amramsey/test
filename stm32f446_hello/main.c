#include "stm32f4xx.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_tim.h"
#include <string.h>
#include <stdlib.h>

void SystemClock_Config(void);
void UART2_Init(void);
void LED_Init(void);
void Servo_Init(void);
void Servo_SetPulse(uint32_t us);
void UART_SendString(const char *s);
void Handle_UART(void);

static char rxbuf[32];
static uint32_t rxindex = 0;

int main(void)
{
    SystemClock_Config();
    LED_Init();
    UART2_Init();
    Servo_Init();

    UART_SendString("Hello World\r\n");

    while (1) {
        Handle_UART();
    }
}

void Handle_UART(void)
{
    if (LL_USART_IsActiveFlag_RXNE(USART2)) {
        char c = LL_USART_ReceiveData8(USART2);
        if (c == '\r' || c == '\n') {
            rxbuf[rxindex] = '\0';
            rxindex = 0;
            if (strlen(rxbuf) > 0) {
                if (strncmp(rxbuf, "SERVO ", 6) == 0) {
                    int val = atoi(rxbuf + 6);
                    Servo_SetPulse(val);
                    UART_SendString("OK\r\n");
                } else if (strcmp(rxbuf, "LED ON") == 0) {
                    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
                    UART_SendString("OK\r\n");
                } else if (strcmp(rxbuf, "LED OFF") == 0) {
                    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);
                    UART_SendString("OK\r\n");
                } else {
                    UART_SendString("ERR\r\n");
                }
            }
        } else if (rxindex < sizeof(rxbuf) - 1) {
            rxbuf[rxindex++] = c;
        }
    }
}

void UART_SendString(const char *s)
{
    while (*s) {
        LL_USART_TransmitData8(USART2, *s++);
        while (!LL_USART_IsActiveFlag_TXE(USART2));
    }
}

void LED_Init(void)
{
    /* Enable GPIOA clock */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    /* Configure PA5 as output */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_5, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_5, LL_GPIO_SPEED_FREQ_LOW);
}

void Servo_Init(void)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_6, LL_GPIO_AF_2);

    LL_TIM_SetPrescaler(TIM4, 84 - 1); /* 1 MHz */
    LL_TIM_SetAutoReload(TIM4, 20000 - 1); /* 20 ms period */
    LL_TIM_OC_SetMode(TIM4, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_EnablePreload(TIM4, LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableARRPreload(TIM4);
    Servo_SetPulse(1500);

    LL_TIM_EnableCounter(TIM4);
    LL_TIM_CC_EnableChannel(TIM4, LL_TIM_CHANNEL_CH1);
}

void Servo_SetPulse(uint32_t us)
{
    if (us < 500)
        us = 500;
    if (us > 2500)
        us = 2500;
    LL_TIM_OC_SetCompareCH1(TIM4, us);
}

void UART2_Init(void)
{
    /* Enable GPIOA and USART2 clocks */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

    /* Configure PA2 (TX) and PA3 (RX) */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_2, LL_GPIO_AF_7);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_3, LL_GPIO_AF_7);

    /* USART2 configuration */
    LL_USART_SetTransferDirection(USART2, LL_USART_DIRECTION_TX_RX);
    LL_USART_SetParity(USART2, LL_USART_PARITY_NONE);
    LL_USART_SetDataWidth(USART2, LL_USART_DATAWIDTH_8B);
    LL_USART_SetStopBitsLength(USART2, LL_USART_STOPBITS_1);
    LL_USART_SetBaudRate(USART2, SystemCoreClock/2, LL_USART_OVERSAMPLING_16, 115200);

    LL_USART_Enable(USART2);
}

void SystemClock_Config(void)
{
    /* Set flash latency */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);

    /* Enable HSE and wait */
    LL_RCC_HSE_Enable();
    while (LL_RCC_HSE_IsReady() != 1)
        ;

    /* Configure PLL */
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, 8, 336, 4);
    LL_RCC_PLL_Enable();
    while (LL_RCC_PLL_IsReady() != 1)
        ;

    /* Set system clock source to PLL */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
        ;

    /* Set prescalers */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    /* Update SystemCoreClock */
    LL_SetSystemCoreClock(84000000);

    /* Configure systick to 1 ms */
    LL_Init1msTick(SystemCoreClock);
}

