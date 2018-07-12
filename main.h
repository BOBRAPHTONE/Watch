#ifndef __main_h__
#define __main_h__

typedef enum {TIME,POSITION,INFOS,IMAGE} screen_t;
screen_t screen;

void powerOff(void);
void powerOn(void);
void gotoBootloader();
void switchLed(int on);



void RCC_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
void ADC_Configuration(void);
void DMA_Configuration(void);
void NVIC_Configuration(void); 
void TIM3_Configuration(void); 
#endif
