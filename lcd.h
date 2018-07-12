#ifndef __lcd_h__
#define __lcd_h__
#include "defines.h"
#include "stm32f10x.h"
#include "u8g/u8g.h"

typedef enum{SMALL,NORMAL,LARGE} fontSize_t;


extern u8g_t u8g;
extern u8g_dev_t u8g_dev;

int posX,posY;

void LCDPulseClock();
void LCDSendData(unsigned char data);
void LCDSendCommand(unsigned char data);
void LCDInit();
void LCDGotoLine(unsigned char line);
void LCDGotoColumn(unsigned char column);
void LCDPrintTest(unsigned char x, unsigned char y);
void LCDGotoXY(unsigned char x, unsigned char y);
void LCDSetFontSize(fontSize_t size);
void LCDPrintStringXY(unsigned char x, unsigned char y, char *string);
//void LCDPrintf(char* string);
void LCDClearScreen();
//void LCDClearLine(int y);
void LCDDrawImageTest();

// u8g
void LCDInit_u8g();
void LCDGetPageBox(u8g_box_t *box);
void LCDSet8Pixels(u8g_dev_t *dev,u8g_dev_arg_pixel_t *arg_pixel);
void LCDSetPixel(u8g_dev_t *dev,u8g_dev_arg_pixel_t *arg_pixel);
uint8_t u8g_dev_kalenji700gps_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg);


#endif
