#ifndef _GETTIME_
#define _GETTIME_


#include <time.h>
#include <sys/time.h>  



long getCurrentTime_us()    
{    
   struct timeval tv;    
   gettimeofday(&tv,NULL);    
   return tv.tv_usec;
}  

#endif
