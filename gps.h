#ifndef __gps_h__
#define __gps_h__
#include "defines.h"


extern __IO int newGPSData;



extern double longitude;
extern double latitude;
extern int fix;
extern double hdop;
extern int heures;
extern int minutes;
extern int secondes;
extern int nbSats;

void gpsSetEnabled(int enable);

void gpsGetMessage(char *source, char *buffer, int bufferlen,char delimiter,int instance);

void gpsAnalyse(char* buffer);
void gpsInit();
void gpsDataReceived(char data);
#endif
