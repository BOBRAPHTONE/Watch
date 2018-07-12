// -*- coding: iso-latin-9-dos -*-

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_dma.h"


#include "defines.h" 
#include "main.h" 
#include "lcd.h"
#include "utils.h"
#include "gps.h"

struct {
  int VBat;
  int TBat;
} ADCValues;

void powerOff(void)
{
  GPIO_WriteBit(PON_PORT,PON_PIN,Bit_RESET);
}

void powerOn(void)
{
  GPIO_WriteBit(PON_PORT,PON_PIN,Bit_SET);
}

void gotoBootloader(){
  USART_Cmd(GPS_USART,DISABLE);
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);
  //Init BootLoader
  //Write 0x6675 ("fu") to BKP_DR10 and reset to BL
  /*Enable Backup Access */
  PWR_BackupAccessCmd(ENABLE);
  BKP_WriteBackupRegister(BKP_DR10,0x6675);
  int *ptr;
  ptr=(int*) 0x8000004;
  void (*fct)() = (void*)(*ptr);
  (*fct)();
}

void switchLed(int on)
{
  if(on)
    LED_PORT->BSRR = LED_PIN;
  else
    LED_PORT->BRR = LED_PIN;
}



void dispInfos(){
  char tempStr[30];
  char numStr[10];
      
  LCDClearBuffer();
  LCDSetFontSize(SMALL);
  strcpy(tempStr,"Satellites : ");
  intToStr(nbSats,tempStr+strlen(tempStr));
  //LCDClearLine(0);
  LCDPrintStringXY(0,8,tempStr);
      
  strcpy(tempStr,"Long : ");
  doubleToStr(longitude,tempStr+strlen(tempStr));
  //LCDClearLine(15);
  LCDPrintStringXY(0,16,tempStr);

  strcpy(tempStr,"Lat : ");
  doubleToStr(latitude,tempStr+strlen(tempStr));
  //LCDClearLine(30);
  LCDPrintStringXY(0,24,tempStr);

  strcpy(tempStr,"Heure : ");
  intToStr(heures,tempStr+strlen(tempStr));
  strcpy(tempStr+strlen(tempStr),"h");
  intToStr(minutes,tempStr+strlen(tempStr));
  strcpy(tempStr+strlen(tempStr),"m");
  intToStr(secondes,tempStr+strlen(tempStr));
  strcpy(tempStr+strlen(tempStr),"s");
  //LCDClearLine(45);
  LCDPrintStringXY(0,32,tempStr);
      
  strcpy(tempStr,"VBAT : ");
  intToStr(ADCValues.VBat,tempStr+strlen(tempStr));
  //LCDClearLine(60);
  LCDPrintStringXY(0,40,tempStr);
      
  strcpy(tempStr,"TEMP : ");
  intToStr(ADCValues.TBat,tempStr+strlen(tempStr));
  //LCDClearLine(75);
  LCDPrintStringXY(0,48,tempStr);
      
  LCDDrawBuffer();
  //LCDPrintStringXY(0,60,debug);
}

void dispImage(){
  LCDDrawImageTest();
}

void dispTime(){
  char tempStr[30];
  char numStr[10];
  LCDClearBuffer();

  LCDSetFontSize(LARGE);
  intToStr2(heures,tempStr);
  strcpy(tempStr+strlen(tempStr),":");
  intToStr2(minutes,tempStr+strlen(tempStr));
  LCDPrintStringXY(15,60,tempStr);
  LCDDrawBuffer();
}

void dispPosition(){
  char tempStr[30];
  char numStr[10];

  LCDClearBuffer();
  LCDSetFontSize(NORMAL);
  
  LCDPrintStringXY(20,15,"Position");
  
  strcpy(tempStr,"Long : ");
  doubleToStr(longitude,tempStr+strlen(tempStr));
  LCDPrintStringXY(0,30,tempStr);

  strcpy(tempStr,"Lat : ");
  doubleToStr(latitude,tempStr+strlen(tempStr));
  LCDPrintStringXY(0,45,tempStr);
  LCDDrawBuffer();
}



void dispUpdate(){
  switch(screen){
  case TIME:
    dispTime();
    break;
  case POSITION:
    dispPosition();
    break;
  case INFOS :
    dispInfos();
    break;
  case IMAGE :
    dispImage();
    break;
  default:
    break;
  }
}




int main(void)
{
  /* System Clocks Configuration */
  RCC_Configuration();
  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* Configure USART */
  USART_Configuration();
  /*Interruptions*/
  NVIC_Configuration();
  /*ADC Configuration*/
  ADC_Configuration();
  /*DMA Configuration */
  DMA_Configuration();

  // Active l'alimentation
  powerOn();  

  LCDInit();
  
  //LCDClearScreen();
  //LCDPrintf("Enable GPS...");
  USART_Cmd(GPS_USART,ENABLE);
  //Active le GPS
  gpsSetEnabled(TRUE);
  //LCDPrintf("OK\n");
  delai(1000);
  //LCDPrintf("Init GPS...");
  gpsInit();
  //LCDPrintf("OK\n");
 
  //int updateData=TRUE;
  screen = TIME;
  int i=0;
  while((BTN_GAUCHE_PORT->IDR & BTN_GAUCHE_PIN) != 0) 
  {

    if(newGPSData){
      dispUpdate();
      newGPSData=FALSE;
    }

    if((BTN_BACK_PORT->IDR & BTN_BACK_PIN) == 0){
      switchLed(OFF);
      gotoBootloader();
    }

    if((BTN_DROITE_PORT->IDR & BTN_DROITE_PIN) == 0){
      
    }
     
    if((BTN_BAS_PORT->IDR & BTN_BAS_PIN) == 0){
      screen = screen+1;
      screen = screen%4;
      dispUpdate();
      for(i=0;i<100;i++)
	delai(10000);
      //switchLed(ON);
      
    }  

    if((BTN_HAUT_PORT->IDR & BTN_HAUT_PIN) == 0){
      screen = screen+3;
      screen = screen%4;
      dispUpdate();
      for(i=0;i<100;i++)
	delai(10000); 
    }
  }  
  powerOff();
}


/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configures the USART
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART_Configuration(void) {
  USART_InitTypeDef   USART_InitStructure;

  // GPS USART 1
  USART_InitStructure.USART_BaudRate = 38400;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl =   USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(GPS_USART, &USART_InitStructure);

  // Active les interruptions sur USART1
  USART_ITConfig(GPS_USART,USART_IT_RXNE,ENABLE);
}


/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void) 
{
  /* Setup the microcontroller system. Initialize the Embedded Flash Interface,
     initialize the PLL and update the SystemFrequency variable. */
  SystemInit();
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD |RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE);
  
  /* Access to backup register */
  RCC_APB2PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, ENABLE );
  
  /* USART1 Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  
  
  /*DMA Clock*/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  /* Timer3 Clock */
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  
}

/*******************************************************************************
* Function Name  : ADC_Configuration
* Description    : Configures Analog to digital converter
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_Configuration(){
  ADC_InitTypeDef ADC_InitStructure;
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 2;
  ADC_Init(ADC1,&ADC_InitStructure);

  ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_9,2,ADC_SampleTime_239Cycles5);
  
  ADC_DMACmd(ADC1, ENABLE);
  ADC_Cmd(ADC1,ENABLE);
  //Calibration
  ADC_ResetCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1)){} // Wait for calibration reset
  ADC_StartCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1)){} //Wait for calibration finished


  //Start Conversion
  ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}



/*******************************************************************************
* Function Name  : DMA_Configuration
* Description    : Configures DMA
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_Configuration(){

  DMA_DeInit(DMA1_Channel1);
  
  DMA_InitTypeDef DMA_InitStructure;
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(&ADCValues) ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC ;
  DMA_InitStructure.DMA_BufferSize = 2 ;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable  ;
  DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize =DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular ;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
 
  DMA_Init(DMA1_Channel1,&DMA_InitStructure);
  DMA_Cmd(DMA1_Channel1,ENABLE);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern uint32_t _isr_vectorsflash_offs;
void NVIC_Configuration(void)
{
  /* Set the Vector Table base location at 0x08000000+_isr_vectorsflash_offs */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, (uint32_t)&_isr_vectorsflash_offs);
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* USART1 Interrupts*/
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* TIM3 Interrupts */
  /*NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  */
  /* USB Interrupts */
  //NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  //NVIC_Init(&NVIC_InitStructure);
}
/*
void TIM3_Configuration(void)
{
  TIM3->PSC = 23999;
  TIM3->ARR = 1000;
  TIM3->DIER = TIM_DIER_UIE; // Enable update interrupt (timer level)
  TIM3->CR1 = TIM_CR1_CEN;   // Enable timer
}
*/
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
d
d* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  //Active l'alimentation
  GPIO_InitStructure.GPIO_Pin =  PON_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PON_PORT, &GPIO_InitStructure);
  
  // Configure PA.8 as output push-pull (LED)
  GPIO_InitStructure.GPIO_Pin =  LED_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED_PORT, &GPIO_InitStructure);

  // LCD CS Output 
  GPIO_InitStructure.GPIO_Pin =  LCD_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LCD_CS_PORT, &GPIO_InitStructure);
  GPIO_WriteBit(LCD_CS_PORT,LCD_CS_PIN,Bit_SET);
    
  // LCD SDA Output 
  GPIO_InitStructure.GPIO_Pin =  LCD_SDA_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LCD_SDA_PORT, &GPIO_InitStructure);

  // LCD CLK Output 
  GPIO_InitStructure.GPIO_Pin =  LCD_CLK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LCD_CLK_PORT, &GPIO_InitStructure);
  GPIO_WriteBit(LCD_CLK_PORT,LCD_CLK_PIN,Bit_SET);

  
  // USART TX : M->GPS
  GPIO_InitStructure.GPIO_Pin =  GPS_USART_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPS_USART_TX_PORT, &GPIO_InitStructure);
  
  // USART RX : GPS->M
  GPIO_InitStructure.GPIO_Pin =  GPS_USART_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPS_USART_RX_PORT, &GPIO_InitStructure);

  // GPS Power
  GPIO_InitStructure.GPIO_Pin =  GPS_POWER_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPS_POWER_PORT, &GPIO_InitStructure);
  
  // VBAT
  GPIO_InitStructure.GPIO_Pin =  VBAT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(VBAT_PORT, &GPIO_InitStructure);

  // TBAT
  GPIO_InitStructure.GPIO_Pin =  TBAT_PIN; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(TBAT_PORT, &GPIO_InitStructure);
  
  /*
  // Configure PC.7 as output push-pull (Motor)
  GPIO_WriteBit(MOT_PORT,MOT_PIN,Bit_RESET);
  GPIO_InitStructure.GPIO_Pin =  MOT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(MOT_PORT, &GPIO_InitStructure);


  
  // GPS Reset Pin
  GPIO_InitStructure.GPIO_Pin =  GPS_RESET_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPS_RESET_PORT, &GPIO_InitStructure);
  
  // LCD CS Output 
  GPIO_InitStructure.GPIO_Pin =  LCD_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LCD_CS_PORT, &GPIO_InitStructure);
  GPIO_WriteBit(LCD_CS_PORT,LCD_CS_PIN,Bit_SET);
    
  // LCD SDA Output 
  GPIO_InitStructure.GPIO_Pin =  LCD_SDA_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LCD_SDA_PORT, &GPIO_InitStructure);

  // LCD CLK Output 
  GPIO_InitStructure.GPIO_Pin =  LCD_CLK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LCD_CLK_PORT, &GPIO_InitStructure);
  GPIO_WriteBit(LCD_CLK_PORT,LCD_CLK_PIN,Bit_SET);
  
  //GPIO_WriteBit(GPS_RESET_PORT,GPS_RESET_PIN,Bit_SET);
  GPS_RESET_PORT->BRR = GPS_RESET_PIN;

  
  */
}

/*
// Interruption du timer
void TIM3_IRQHandler(void)
{
	//switchLed(0);
  if(TIM3->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM3->SR &=~TIM_SR_UIF; // clear UIF flag
    
    //LCDWriteCenterBigNum(1,curr_index);
    if(++digit_cnt==TIME_DIGIT) {
    	digit_cnt=0;
    	inhib=0;
    	curr_index=(curr_index+1)%10;
    	if(updateNum) 
    		aMAJ |= MAJ_DIGIT |  MAJ_NUMBERS;
    }
    majDisplay();
  }
}
*/
