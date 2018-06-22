#include<time.h>
struct tm* getTimeStruct();
unsigned short getDate(struct tm* t);
unsigned short getTime(struct tm* t);
unsigned short getHour(unsigned short date,unsigned short time);
unsigned short getMinute(unsigned short time);
unsigned short getSecond(unsigned short time);
unsigned short getYear(unsigned short time);
unsigned short getMonth(unsigned short time);
unsigned short getDay(unsigned short time);
void showCurrentTime();