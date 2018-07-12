#ifndef __utils_h__
#define __utils_h__
//#include <stdarg.h>

extern char debug[15];

void delai(int i);
void intToStr(int nb, char* str);
void doubleToStr(double nb, char* str);
int atoi(char* s);
double atof(char* s);
int strlen(char* string);
char* strncpy(char* dest, char* src, int num);
int strchrn(char* str,char chr, int n);
int strcpy(char* dest, char* src);

#endif
