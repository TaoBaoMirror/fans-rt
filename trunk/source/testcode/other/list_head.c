#ifdef BUILD_TESTCODE_LIST_HEAD
#include <string.h>
#include <fadefs.h>
#include <faklist.h>
#include <kdebug.h>
#include <fapi.h>

STATIC LIST_HEAD ListHead = {&ListHead, &ListHead};

typedef struct tagTEST_OBJECT{
    LPSTR               lpName;
    LIST_HEAD           ListHead;
}TEST_OBJECT;

TEST_OBJECT Object1 = {"Object1", {NULL, NULL}};
TEST_OBJECT Object2 = {"Object2", {NULL, NULL}};
TEST_OBJECT Object3 = {"Object3", {NULL, NULL}};

VOID FansMain(VOID)
{
    LPLIST_HEAD lpList = NULL;
    LPLIST_HEAD lpNode = NULL;

    LIST_INSERT_TAIL(&ListHead, &Object1.ListHead);
    LIST_INSERT_TAIL(&ListHead, &Object2.ListHead);
    LIST_INSERT_TAIL(&ListHead, &Object3.ListHead);
    
    CORE_INFOR(TRUE, "ListHead = %p    %p    %p", &ListHead, ListHead.lpNext, ListHead.lpPrev);
    CORE_INFOR(TRUE, "%s  = %p    %p    %p", Object1.lpName, &Object1.ListHead, Object1.ListHead.lpNext, Object1.ListHead.lpPrev);
    CORE_INFOR(TRUE, "%s  = %p    %p    %p", Object2.lpName, &Object2.ListHead, Object2.ListHead.lpNext, Object2.ListHead.lpPrev);
    CORE_INFOR(TRUE, "%s  = %p    %p    %p", Object3.lpName, &Object3.ListHead, Object3.ListHead.lpNext, Object3.ListHead.lpPrev);

    LIST_FOR_EACH_SAFE(lpList, lpNode, &ListHead)
    {
        TEST_OBJECT * Object = CONTAINER_OF(lpList, TEST_OBJECT, ListHead);
        
        CORE_INFOR(TRUE, "%s  = %p    %p    %p", Object->lpName, &Object->ListHead,
                Object->ListHead.lpNext, Object->ListHead.lpPrev);
        
        LIST_REMOVE_NODE(&Object->ListHead);
    }
    CORE_INFOR(TRUE, "Test end!");
}
#endif

