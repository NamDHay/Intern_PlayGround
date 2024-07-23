#ifndef __NPTTIME_H__
#define __NPTTIME_H__

#include "time.h"

class NPT
{
public:
    void setup(void);
    void printLocalTime(void);
};

extern NPT npt;

#endif //__NPTTIME_H__