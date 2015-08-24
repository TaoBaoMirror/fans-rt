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
#ifndef __FA_DEBUG_H
#define __FA_DEBUG_H

typedef enum{
    LOG_LEVEL_FATAL         =           0,
    LOG_LEVEL_ERROR         =           1,
    LOG_LEVEL_WARN          =           2,
    LOG_LEVEL_BUG           =           3,
    LOG_LEVEL_FIX           =           4,
    LOG_LEVEL_INFOR         =           5,
    LOG_LEVEL_TRACE         =           6,
    LOG_LEVEL_DEBUG         =           7,
    LOG_LEVEL_MAX
}E_LOG_LEVEL;

#define     LOG_BUFFER_SIZE                 128
#define     LOG_FATAL_MASK                  (1 << LOG_LEVEL_FATAL)
#define     LOG_ERROR_MASK                  (1 << LOG_LEVEL_ERROR)
#define     LOG_WARN_MASK                   (1 << LOG_LEVEL_WARN)
#define     LOG_BUG_MASK                    (1 << LOG_LEVEL_BUG)
#define     LOG_FIX_MASK                    (1 << LOG_LEVEL_FIX)
#define     LOG_INFOR_MASK                  (1 << LOG_LEVEL_INFOR)
#define     LOG_TRACE_MASK                  (1 << LOG_LEVEL_TRACE)
#define     LOG_DEBUG_MASK                  (1 << LOG_LEVEL_DEBUG)

#define     DEBUG_LOG_MASK                  (LOG_FATAL_MASK | LOG_ERROR_MASK | LOG_WARN_MASK | LOG_BUG_MASK |\
                                             LOG_FIX_MASK | LOG_INFOR_MASK | LOG_TRACE_MASK)

enum{
    LPC_SDM_WRITE_BYTE          =   0,
    LPC_SDM_WRITE_DATA          =   1,
    LPC_SDM_DEBUG_MASK          =   2,
    LPC_SDM_REQUEST_MAX,
};

#endif


