#include "utils.h"
#include <ctype.h>

char debug[15];

int strlen(char* string){
  unsigned char len = 0;
  while(*(string++)!=0){
    len++;
    if(len>250)
      break;
  }
  return len;
}


char* strncpy(char* dest, char* src, int num){
  int i;
  for(i=0;i<num;i++){
    dest[i] = src[i];
  }
  dest[num] = 0;
  return dest;
}

// Find position of char chr in string str before n characters if fails return n
int strchrn(char* str,char chr, int n)
{
  char* ptr = str;
  if(strlen(str)<n)
    n=strlen(str);

  for(ptr=str; ptr<str+n ;ptr++){
    if(*ptr==chr)
      return (int)(ptr-str);
  }
  return n;
  
}

void intToStr2(int nb, char* str){
  str[0] = '0'+nb/10;
  str[1] = '0'+nb%10;
  str[2] = 0;
}


void intToStr(int nb, char* str)
{
	int i=0;
	int cont=1;
	int neg=0;
	if(nb<0)
	{
		neg=1;
		nb=-nb;
	}
	while(cont)
	{
		str[i++] = '0'+nb%10;
		nb = nb/10;
		if(nb==0) cont=0;
	}
	if(neg)
		str[i++]='-';
	str[i] = 0;
	unsigned char temp;
	unsigned char k;
	for(k=0;k<i/2;k++)
	{
		temp = str[k];
		str[k] = str[i-k-1];
		str[i-k-1]=temp;
	}
}

void doubleToStr(double nb, char* str)
{
	nb = (int)(nb*1000);
	intToStr(nb,str);
	int i=strlen(str);
	int j;
	for(j=0;j<4;j++)
		str[i-j] = str[i-j-1];
	str[i+1] = '\0';
	str[i-3]='.';
}

void delai(int i)
{
	volatile int l;
	for(l=0;l<i;l++)
	{}
}

int atoi(char* s)
{
 int a=0;
 unsigned char c;
 while ((c = *s++) != '\0' && isdigit(c)) 
 {
   a = a*10 + (c - '0');
  }
  return a;
}

// convertit une chaine de caractères en double
double atof(char* s)
{
 double a=0.0;
 int e=0;
 unsigned char c;
 while ((c = *s++) != '\0' && isdigit(c)) 
 {
   a = a*10.0 + (c - '0');
  }
  if (c == '.') 
  {
   while ((c = *s++) != '\0' && isdigit(c)) 
   {
     a = a*10.0 + (c - '0');
      e = e-1;
    }
  }
  while(e<0)
  {
   a*=0.1;
   e++;
  }
  return a;
}
