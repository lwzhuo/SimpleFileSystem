#include"time.h"
#include<stdio.h>
struct tm* getTimeStruct(){
    time_t t = time(NULL);
    return localtime(&t);
}
unsigned short getDate(struct tm* t){
    int year = t->tm_year-70;//以1970为基准 最大存放88年
    int mon = t->tm_mon;
    int day = t->tm_mday;
    unsigned short result=0;
    if(t->tm_hour>12)//上下午标志存放在这里 short变量最高位
        result = 1<<15;
    result += year*12*31+mon*31+day;
    return result;
}

unsigned short getTime(struct tm* t){
    //为了精确表示 只存放12h的时间 标志上下午的标志位放在date里
    unsigned short hour = t->tm_hour;
    unsigned short min = t->tm_min;
    unsigned short sec = t->tm_sec;
    if(hour>12)
        hour-=12;
    return hour*3600+min*60+sec;
}

unsigned short getHour(unsigned short date,unsigned short time){
    if(date>>15==1)
        return 12 + time/3600;
    else
        return time/3600;
}

unsigned short getMinute(unsigned short time){
    return (time%3600)/60;
}

unsigned short getSecond(unsigned short time){
    return (time%3600)%60;
}

unsigned short getYear(unsigned short time){
    time = time & 32767;
    return 1970+time/372;//372=12*31
}

unsigned short getMonth(unsigned short time){
    time = time & 32767;
    return (time%372)/31+1;//372=12*31
}

unsigned short getDay(unsigned short time){
    time = time & 32767;
    return (time%372)%31;//372=12*31
}