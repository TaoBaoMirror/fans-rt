#ifndef __COMPILER_H
#define __COMPILER_H

#define     __CC_MAJOR_VERSION             (__ARMCC_VERSION / 1000000)
#define     __CC_MINOR_VERSION             ((__ARMCC_VERSION / 10000) - (__CC_MAJOR_VERSION * 100))
#define     __CC_BUILD_VERSION             (__ARMCC_VERSION - (__CC_MAJOR_VERSION * 1000000) - (__CC_MINOR_VERSION * 10000))
#define     __CC_COMPILER_NAME              "ARMCC"

#endif

