#include"time.h"
struct tm* getTimeStruct(){
    time_t t = time(NULL);
    return localtime(&t);
}
short getdate(struct tm* t){
    int year = t->tm_year;
    int mon = t->tm_mon;
    int day = t->tm_mday;
    return (mon+1)*100+day;
}

short gettime(struct tm* t){
    int hour = t->tm_hour;
    int min = t->tm_min;
    return hour*100+min;
}