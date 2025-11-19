#include "logStream.h"
#include "timestamp.h"
#include "logging.h"

int main()
{
    CTimestamp timestam = CTimestamp::now();
    printf("now is %s\n", timestam.toFormatedString().c_str());

    CTimestamp nowTime(time(NULL));
    printf("now is %s\n", nowTime.toFormatedString().c_str());

    
    int array[] = {0};
    LOG_INFO << array[1] <<"\n";
    return 0;
}