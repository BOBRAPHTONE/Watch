#ifndef __defines_h__
#define __defines_h__
#include "stm32f10x.h"

#define ADC1_DR_Address ((uint32_t)0x4001244C)

#define ON 1
#define OFF 0

#define LED_PORT GPIOB
#define LED_PIN	 GPIO_Pin_5

#define PON_PORT GPIOC
#define PON_PIN GPIO_Pin_8

#define BTN_GAUCHE_PORT GPIOA
#define BTN_GAUCHE_PIN GPIO_Pin_0

#define BTN_DROITE_PORT GPIOC
#define BTN_DROITE_PIN GPIO_Pin_3

#define BTN_HAUT_PORT GPIOC
#define BTN_HAUT_PIN GPIO_Pin_0

#define BTN_BAS_PORT GPIOC
#define BTN_BAS_PIN GPIO_Pin_4

#define BTN_BACK_PORT GPIOC
#define BTN_BACK_PIN GPIO_Pin_1

#define PON_PORT GPIOC
#define PON_PIN GPIO_Pin_8

#define LCD_WIDTH 128
#define LCD_HEIGHT 100

#define LCD_CS_PORT GPIOA
#define LCD_CS_PIN GPIO_Pin_4
#define LCD_SDA_PORT GPIOA
#define LCD_SDA_PIN GPIO_Pin_7
#define LCD_CLK_PORT GPIOA
#define LCD_CLK_PIN GPIO_Pin_5

#define GPS_USART USART1
#define GPS_USART_TX_PORT GPIOA
#define GPS_USART_TX_PIN GPIO_Pin_9
#define GPS_USART_RX_PORT GPIOA
#define GPS_USART_RX_PIN GPIO_Pin_10
#define GPS_POWER_PORT GPIOD
#define GPS_POWER_PIN GPIO_Pin_2

#define VBAT_PORT GPIOA
#define VBAT_PIN GPIO_Pin_1
#define TBAT_PORT GPIOB
#define TBAT_PIN GPIO_Pin_1



#endif 
