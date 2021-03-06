/*
 *    Copyright(C) 2013-2015, Fans-rt development team.
 *
 *    All rights reserved.
 *
 *    This is open source software.
 *    Learning and research can be unrestricted to  modification, use and dissemination.
 *    If you need for commercial purposes, you should get the author's permission.
 *
 *    date           author          notes
 *    2014-09-07     JiangYong       new file
 */
#ifndef __STDLIB_H
#define __STDLIB_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
#define abs(x)          ((x) >= 0 ? (x) : (-(x)))
    extern void * malloc(size_t length);
#ifdef __cplusplus
}
#endif 
#endif

