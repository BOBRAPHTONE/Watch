/*
// global nmea sentences for ues in gps reset, etc.
unsigned char DISABLE_GPGGA[26]="$PSRF103,00,00,00,01*24\r\n";
unsigned char DISABLE_GPGLL[26]="$PSRF103,01,00,00,01*25\r\n";
unsigned char DISABLE_GPGSA[26]="$PSRF103,02,00,00,01*26\r\n";
unsigned char DISABLE_GPGSV[26]="$PSRF103,03,00,00,01*27\r\n";
unsigned char DISABLE_GPRMC[26]="$PSRF103,04,00,00,01*20\r\n";
unsigned char DISABLE_GPVTG[26]="$PSRF103,05,00,00,01*21\r\n";
unsigned char ENABLE_GPGGA[26]="$PSRF103,00,00,01,01*25\r\n";
unsigned char ENABLE_GPGLL[26]="$PSRF103,01,00,01,01*24\r\n";
unsigned char ENABLE_GPGSA[26]="$PSRF103,02,00,01,01*27\r\n";
unsigned char ENABLE_GPGSV[26]="$PSRF103,03,00,01,01*26\r\n";
unsigned char ENABLE_GPRMC[26]="$PSRF103,04,00,01,01*21\r\n";
unsigned char ENABLE_GPVTG[26]="$PSRF103,05,00,01,01*20\r\n";
*/
#include "stm32f10x.h"
#include "gps.h" 
#include "lcd.h"
#include "utils.h"
#include "defines.h"
#include "main.h"

char recvBuff[82];	// buffer de reception		
char recvTemp[82];	// buffer temporaire de reception
__IO int recvIndex=0;           // index courant de reception

__IO int newGPSData=FALSE;



double longitude;
double latitude;
int fix;
double hdop;
int heures;
int minutes;
int secondes;
int nbSats;



void gpsSetEnabled(int enable){
  if(enable==TRUE){
    GPIO_WriteBit(GPS_POWER_PORT,GPS_POWER_PIN,Bit_SET);
  }
  else{
    GPIO_WriteBit(GPS_POWER_PORT,GPS_POWER_PIN,Bit_RESET);  
  }
}
 

void gpsInit(){
  int i;
  const char initSeq1[]="$PSRF151,00*0E\r\n";
  const char initSeq2[]="$PSRF103,03,00,05,01*22\r\n";
  heures=minutes=secondes=0;
  for(i=0;initSeq1[i]!=0;i++)
  {
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET){};
    USART_SendData(USART1, initSeq1[i]);
  }
  
  for(i=0;initSeq2[i]!=0;i++)
  {
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET){};
    USART_SendData(USART1, initSeq1[i]);
  }
}


// New gps data has arrived
void gpsDataReceived(char data){
  if(data == '$'){  // previous NMEA frame finished
    int j;
    for(j=0;j<recvIndex;j++)
      recvBuff[j] = recvTemp[j];
    recvBuff[j] = '\0';
    recvIndex = 0;
    gpsAnalyse(recvBuff);
    newGPSData=TRUE;
  }
  recvTemp[recvIndex++] = data;
}

/**
 * takes a source string, a target buffer, the length of the target buffer,
 * a delimiter character, and the count of the delimiters (before getting a value)
 * for a string (like an NMEA message that delimits fields with a comma) where
 * you want the third field, you would use something like so:
 *  gps_get_message(source,buffer,80,',',3);
 */ 
void gpsGetMessage(char *source, char *buffer, int bufferlen,char delimiter,int instance)
{
 int i = 0;
 int j = 0;
 int k = 0;
 while (source[i] != '\0' && k <= instance)
 {
  if (k == instance) {
   if (source[i] != delimiter) {
    buffer[j] = source[i];
    j++;
   }
   else
   {
    buffer[j] = '\0';
    k++;
   }
  }
  else if(source[i] == delimiter)
  {
   k++;
  }
  i++;
 }
 if(source[i] == '\0')
  buffer[j] = '\0';
}


void gpsAnalyse(char* buffer)
{
  char result[20];
  if (buffer[0] == '$' && buffer[3] == 'G' && buffer[4] == 'S' && buffer[5] == 'V'){
    gpsGetMessage(buffer,result,12,',',3);
    nbSats = atoi(result);
  }
 
  if (buffer[0] == '$' && buffer[3] == 'G' && buffer[4] == 'G' && buffer[5] == 'A'){
    //trouve l'heure
    gpsGetMessage(buffer,result,20,',',1);
    if(strlen(result)>5){
      heures = 10*(result[0]-'0') + (result[1]-'0');
      minutes = 10*(result[2]-'0') + (result[3]-'0');
      secondes = 10*(result[4]-'0') + (result[5]-'0');
    }
  
    double degs, mins;
    //trouve la Latitude
    gpsGetMessage(buffer,result,20,',',2);
    //strcpy(debug,result);
    if(strlen(result)>3){
      degs = 10*(result[0]-'0') + (result[1]-'0');
      mins = atof(result+2);
      latitude = degs+mins/60;
    }  
    else
      latitude = -1;
    //Nord ou sud ?
    gpsGetMessage(buffer,result,20,',',3);
    if(result[0] == 'S')
      latitude = -latitude;
   
    //Trouve la Longitude
    gpsGetMessage(buffer,result,20,',',4);
    if(strlen(result)>3){
      degs = 100*(result[0]-'0') + 10*(result[1]-'0') + (result[2]-'0');
      mins = atof(result+3);
      longitude = degs+mins/60;
    }
    else
      longitude = -1;
  
    //est ou west ?
    gpsGetMessage(buffer,result,20,',',5);
    if(result[0] == 'W')
      longitude = -longitude;
    
    //fix
    gpsGetMessage(buffer,result,20,',',6);
    fix = result[0]-'0';
    
    //hdop
    gpsGetMessage(buffer,result,20,',',8);
    hdop = atof(result);
  }
}

/*
int calcDistObjectif()
{
 	double R=6371000.0;
	double pi = 3.141592;
	double delta = (longitude-longitudeObjectif)*pi/180.0;
	double sdlong = sin(delta);
	double cdlong = cos(delta);
	double slat1 = sin(latitude*pi/180);
	double clat1 = cos(latitude*pi/180);
	double slat2 = sin(latitudeObjectif*pi/180);
	double clat2 = cos(latitudeObjectif*pi/180);
	delta = (clat1 * slat2) - (slat1 * clat2 * cdlong); 
	delta = delta*delta; 
	delta += clat2 * sdlong * clat2 * sdlong; 
	delta = sqrt(delta); 
	double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong); 
	delta = atan2(delta, denom); 
	return (int)(delta * 6372795.0);
}

*/
