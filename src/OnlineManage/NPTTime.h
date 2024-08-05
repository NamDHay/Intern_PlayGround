#ifndef __NPTTIME_H__
#define __NPTTIME_H__
#ifdef CONFIG_USE_NPT_TIME
#include "time.h"
class NPT
{
public:
    void setup(void);
    void printLocalTime(void);
};
extern NPT npt;
#endif //CONFIG_USE_NPT_TIME
#endif //__NPTTIME_H__