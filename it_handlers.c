#include "it_handlers.h"
#include "defines.h"
#include "main.h"
#include "lcd.h"
#include "gps.h"

// Interruption de reception de données série
void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    char data = USART_ReceiveData(USART1);
    gpsDataReceived(data);
  }
}

void HardFault_Handler(void){
  LCDPrintStringXY(0,0,"HardFault");
}

void MemManage_Handler(void){
  LCDPrintStringXY(0,0,"MemManager");
}

void BusFault_Handler(void){
  LCDPrintStringXY(0,0,"BusFault");
}

void UsageFault_Handler(void){
  LCDPrintStringXY(0,0,"Usage Fault");
}
