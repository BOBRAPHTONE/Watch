#include "lcd.h"
#include "utils.h"
#include "image_data.c"
#include <stdarg.h>
 
char buffer[3200];

 
u8g_t u8g;
u8g_dev_t u8g_dev;



void LCDInit_u8g(){
  u8g_dev.dev_fn = &u8g_dev_kalenji700gps_fn;
  u8g_dev.dev_mem = buffer;
  u8g_dev.com_fn= NULL;

  u8g.width = 128;
  u8g.height = 100;
  u8g.dev = &u8g_dev;
  //u8g_SetFont(&u8g,u8g_font_6x10r);
  //u8g_SetFont(&u8g,u8g_font_9x15r);
  u8g_SetFont(&u8g,u8g_font_5x7r);
  u8g.current_page.x0=0;
  u8g.current_page.x1=128;
  u8g.current_page.y0=0; 
  u8g.current_page.y1=100;
}


void LCDSetFontSize(fontSize_t size){
  switch(size){
  case SMALL:
    u8g_SetFont(&u8g,u8g_font_5x7r);
    break;
  case NORMAL:
    u8g_SetFont(&u8g,u8g_font_6x10r);
    break;
  case LARGE:
    u8g_SetFont(&u8g,u8g_font_gdr30n);
    break;
  default:
    break;
  }
   
}


void LCDPulseClock()
{
	GPIO_WriteBit(LCD_CLK_PORT,LCD_CLK_PIN,Bit_RESET);
	GPIO_WriteBit(LCD_CLK_PORT,LCD_CLK_PIN,Bit_SET);
}

//Send Command byte to LCD
void LCDSendCommand(unsigned char data)
{
  //Select command mode
  GPIO_WriteBit(LCD_CS_PORT,LCD_CS_PIN,Bit_RESET);

  int k;
  unsigned char mask = 0x80;
  for(k=0;k<8;k++)
    {
      if(data & mask)
	GPIO_WriteBit(LCD_SDA_PORT,LCD_SDA_PIN,Bit_SET);
      else
	GPIO_WriteBit(LCD_SDA_PORT,LCD_SDA_PIN,Bit_RESET);
      LCDPulseClock();
      mask>>=1;
    }

  //Deactivate LCD
  GPIO_WriteBit(LCD_CS_PORT,LCD_CS_PIN,Bit_SET);
}

unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   return b;
}

//Send Data byte to LCD
void LCDSendData(unsigned char data)
{
  //Select Data mode
  GPIO_WriteBit(LCD_CS_PORT,LCD_CS_PIN,Bit_SET);

  // A0=1 pour écrire des données
  //GPIO_WriteBit(LCD_SDA_PORT,LCD_SDA_PIN,Bit_SET);
  //LCDPulseClock();
  data = reverse(data); //Inverse pairs order
  int k;
  unsigned char mask = 0x80;
  for(k=0;k<8;k++)
    {
      if(data & mask)
	GPIO_WriteBit(LCD_SDA_PORT,LCD_SDA_PIN,Bit_SET);
      else
	GPIO_WriteBit(LCD_SDA_PORT,LCD_SDA_PIN,Bit_RESET);
      LCDPulseClock();
      mask>>=1;
    }
  //Deselect Data Mode
  GPIO_WriteBit(LCD_CS_PORT,LCD_CS_PIN,Bit_RESET);
}

//Initialize LCD display
void LCDInit(){
  //Reset
  LCDSendCommand(0xE2);
  //select regul register
  LCDSendCommand(0x24);
  //SHL Select ?
  LCDSendCommand(0xC2);
  //ADC normal direction
  LCDSendCommand(0xA0);
  //??
  LCDSendCommand(0xD2);
  //??
  LCDSendCommand(0xEB);

  LCDSendCommand(0x81);
  LCDSendCommand(0x1E);

  LCDSendCommand(0xF1); //2 bytes instr
  LCDSendCommand(0x63);

  LCDSendCommand(0xF2);
  LCDSendCommand(0x00);
  
  LCDSendCommand(0xF3); //2 bytes instr
  LCDSendCommand(0x63);

  // Display On
  LCDSendCommand(0xAF);

  posX = posY = 0;
  LCDInit_u8g();
 
}

void LCDGotoXY(unsigned char x, unsigned char y){
  if( (x<128) && (y<100)){
    LCDGotoLine(y);
    LCDGotoColumn(x>>2);
  }
}


//Print test pattern
void LCDPrintTest(unsigned char x, unsigned char y){
  LCDGotoXY(x,y);
  LCDSendData(0xFF);
  LCDGotoXY(x,y+1);
  LCDSendData(0xC3);
  LCDGotoXY(x,y+2);
  LCDSendData(0xC3);
  LCDGotoXY(x,y+3);
  LCDSendData(0xFF);
}

/*
void LCDPrintf(char* string){
  int length = strlen(string);
  char* strend = string+length;
  char strTemp[LCD_WIDTH/(CHAR_WIDTH+1)+1];
  char* buff=strTemp;
  int maxChars = LCD_WIDTH/(CHAR_WIDTH+1);
  int char_left = (LCD_WIDTH-posX)/(CHAR_WIDTH+1);
  if(char_left==0){
    posX=0;
    posY+=CHAR_HEIGHT+1;
    char_left=maxChars;
  }
  while(string<strend){
    if(*string != '\n'){
      *(buff++)=*(string++);
      char_left--;
    }
    else{
      string++;
      *(buff)=0;
      LCDPrintStringXY(posX,posY,strTemp);
      buff=strTemp;
      posX=0;
      posY+=CHAR_HEIGHT+1;
      char_left=maxChars;
    }
    
    if(char_left==0){
      *(++buff)=0;
      LCDPrintStringXY(posX,posY,strTemp);
      buff=strTemp;
      posX=0;
      posY+=CHAR_HEIGHT+1;
      char_left=maxChars;
    }
  }
  if(buff>strTemp){
    *(++buff)=0;
    LCDPrintStringXY(posX,posY,strTemp);
    posX+=(CHAR_WIDTH+1)*strlen(strTemp);
  }
}
  
*/


/*
// print string on screen at x,y
void LCDPrintStringXY(unsigned char x, unsigned char y, char *string){
  // unsigned char total_width = (CHAR_WIDTH+1)*strlen(string);
  unsigned char line;
  for(line=0;line<CHAR_HEIGHT;line++){
    unsigned char current_bit=3;
    unsigned char current_byte=0;
    int char_i=0;
    LCDGotoXY(x,y+line);
    for(char_i=0; char_i<strlen(string);char_i++){
      unsigned char current_char=string[char_i];
      unsigned char current_chardata = charset[decodeChar(current_char)][line];
      int charbit;
      // 8 first bits
      for(charbit = 7; charbit>=2; charbit--){
	//if current bit is 1
	if((current_chardata>>charbit) & 0x01){  
	  current_byte|=(3<<(current_bit*2));  // shift 0b11 to left
	}
	current_bit--;  
	if(current_bit==0xFF){
	  LCDSendData(current_byte);
	  current_byte=0;
	  current_bit=3;
	}
      }

      current_bit--; //Space after char
      if(current_bit==0xFF){ 
	  LCDSendData(current_byte);
	  current_byte=0;
	  current_bit=3;
      }
    }
    // check if there is something left to send
    if(current_bit!=3){
      LCDSendData(current_byte); //send remaining data
    }
  }
}
*/

void LCDPrintStringXY(unsigned char x, unsigned char y, char *string)
{
  u8g_DrawStr(&u8g, x, y,string);
}


void LCDDrawImageTest(){
  LCDGotoXY(0,0);
  int i;
  for(i=0;i<3200;i++)
    LCDSendData(imagedata[i]);
}


// Draw screen Image
void LCDDrawImage(char* data){
  LCDGotoXY(0,0);
  int i;
  for(i=0;i<3200;i++)
    LCDSendData(data[i]);
}

void LCDDrawBuffer(){
  LCDDrawImage(buffer);
}

void LCDClearBuffer(){
  int i;
  for(i=0;i<3200;i++)
    buffer[i] = 0;
}

void LCDClearScreen(){
  int line; 
  int column;
  LCDGotoXY(0,0);
  posX=0;
  posY=0;
  for(line=0;line<100;line++)
    for(column=0;column<32;column++)
      LCDSendData(0x00); 
}

/*
// Clear a line of chars
void LCDClearLine(int y){
  LCDGotoXY(0,y);
  int col;
  int line;
  for(line=0;line<CHAR_HEIGHT+1;line++)
    for(col=0;col<32;col++)
      LCDSendData(0x00); 
}
*/

void LCDGotoColumn(unsigned char column)
{
  if(column<32){
    LCDSendCommand(0x00 | (column & 0x1F));
  }
}

// Select page of LCD (line)
void LCDGotoLine(unsigned char line){
  if(line<100){ //only 16 pages
    //Pourquoi ?
    LCDSendCommand(0x00);
    
    //poids faible
    unsigned char command = 0x60 | (line&0xF);
    LCDSendCommand(command);
    
    //poids fort
    command = 0x70 | ((line>>4)&0xF);
    LCDSendCommand(command);
  }
}

void LCDSetPixel(u8g_dev_t *dev,u8g_dev_arg_pixel_t *arg_pixel){
  if((arg_pixel->x>128) || ((arg_pixel->y>100)))
    return;
  uint8_t bits = ( 0x3 << (2*(3-(arg_pixel->x % 4)))) & 0xFF;
  buffer[arg_pixel->x/4+arg_pixel->y*32] |= bits;

}


void LCDSet8Pixels(u8g_dev_t *dev,u8g_dev_arg_pixel_t *arg_pixel){
  register uint8_t pixel = arg_pixel->pixel;
  int i;
  for(i=0;i<8;i++){
    if ( pixel & 128 ){
      LCDSetPixel(dev,arg_pixel);
    }
    switch( arg_pixel->dir ){
    case 0: arg_pixel->x++; break;
    case 1: arg_pixel->y++; break;
    case 2: arg_pixel->x--; break;
    case 3: arg_pixel->y--; break;
    }
    pixel <<= 1;
  }
}

void LCDGetPageBox(u8g_box_t *box){
  box->x0 = 0;
  box->y0 = 0;
  box->x1 = 128;
  box->y1 = 100;
}


uint8_t u8g_dev_kalenji700gps_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg){
  switch(msg){
  case U8G_DEV_MSG_SET_8PIXEL:
    LCDSet8Pixels(dev,(u8g_dev_arg_pixel_t *)arg);
    break;
  case U8G_DEV_MSG_SET_PIXEL:
    LCDSetPixel(dev,(u8g_dev_arg_pixel_t *)arg);
    break;
  case U8G_DEV_MSG_INIT:
    LCDInit();
    break;
  case U8G_DEV_MSG_STOP:
      break;
  case U8G_DEV_MSG_PAGE_FIRST:
    LCDClearBuffer();
    break;
  case U8G_DEV_MSG_PAGE_NEXT:     
    LCDDrawBuffer();
    //return 0;
    break;
#ifdef U8G_DEV_MSG_IS_BBX_INTERSECTION
  case U8G_DEV_MSG_IS_BBX_INTERSECTION:
    //return u8g_pb_IsIntersection(pb, (u8g_dev_arg_bbx_t *)arg);
    return 0;
#endif
  case U8G_DEV_MSG_GET_PAGE_BOX:
    LCDGetPageBox((u8g_box_t *)arg);
    break;
  case U8G_DEV_MSG_GET_WIDTH:
    *((u8g_uint_t *)arg) = 128;
    break;
  case U8G_DEV_MSG_GET_HEIGHT:
    *((u8g_uint_t *)arg) = 100;
    break;
  case U8G_DEV_MSG_SET_COLOR_ENTRY:
    break;
  case U8G_DEV_MSG_SET_XY_CB:
    break;
  case U8G_DEV_MSG_GET_MODE:
    return U8G_MODE_GRAY2BIT;
  }
  return 0;
}
