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

#ifndef __K_LIST_H
#define __K_LIST_H

#include <fadefs.h>
#include <fatypes.h>

typedef struct tagLIST_HEAD LIST_HEAD;
typedef struct tagLIST_HEAD * PLIST_HEAD;
typedef struct tagLIST_HEAD FAR * LPLIST_HEAD;

struct tagLIST_HEAD{
    LPLIST_HEAD     lpPrev;
    LPLIST_HEAD     lpNext;
};

#define DEFINE_LIST_HEAD(HeadName)                                                      \
        STATIC LIST_HEAD HeadName = { &HeadName, &HeadName };

#define LIST_HEAD_INIT(lpHead)                                                          \
        do{(lpHead)->lpPrev = (lpHead); (lpHead)->lpNext = lpHead; }while(0)

#define LIST_EMPTY(lpHead)                  ((lpHead)->lpNext == lpHead)

#define LIST_ENTRY(lpNode, Type, Member)                                                \
        CONTAINER_OF(lpNode, Type, Member)
#define LIST_FOR_EACH(lpList, lpHead)                                                   \
        for((lpList)=(lpHead)->lpNext; lpList != (lpHead); (lpList)=(lpList)->lpNext)
#define LIST_FOR_EACH_SAFE(lpList, lpNode, lpHead)                                      \
        for(lpList=(lpHead)->lpNext, lpNode = lpList->lpNext; lpList != (lpHead);       \
            lpList = lpNode, lpNode = lpList->lpNext)

STATIC INLINE VOID LIST_INSERT_NEXT(LPLIST_HEAD lpThis, LPLIST_HEAD lpNode)
{
    LPLIST_HEAD lpNext = lpThis->lpNext;

    lpNode->lpNext = lpNext;
    lpNode->lpPrev = lpThis;
    lpThis->lpNext = lpNode;
    lpNext->lpPrev = lpNode;
}

#define LIST_INSERT_TAIL(lpHead, lpNode)    LIST_INSERT_PREV(lpHead, lpNode)

STATIC INLINE VOID LIST_INSERT_PREV(LPLIST_HEAD lpNext, LPLIST_HEAD lpNode)
{
    LPLIST_HEAD lpPrev = lpNext->lpPrev;

    lpNode->lpNext = lpNext;
    lpNode->lpPrev = lpPrev;
    lpPrev->lpNext = lpNode;
    lpNext->lpPrev = lpNode;
}

STATIC INLINE VOID LIST_REMOVE_NODE(LPLIST_HEAD lpNode)
{
    LPLIST_HEAD lpNext = lpNode->lpNext;
    LPLIST_HEAD lpPrev = lpNode->lpPrev;
    
    lpPrev->lpNext = lpNext;
    lpNext->lpPrev = lpPrev;
    lpNode->lpPrev = lpNode;
    lpNode->lpNext = lpNode;
}

#define     LIST_FIRST_NODE(lpNode)     LIST_NEXT_NODE(lpNode)

STATIC INLINE LPVOID LIST_NEXT_NODE(LPLIST_HEAD lpNode)
{
    return (LPVOID)(lpNode->lpNext == lpNode ? NULL : lpNode->lpNext);
}

STATIC INLINE LPVOID LIST_PREV_NODE(LPLIST_HEAD lpNode)
{
    return (LPVOID)(lpNode->lpPrev == lpNode ? NULL : lpNode->lpPrev);
}

#endif


