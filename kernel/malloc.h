#include "types.h"

#define MAX_ORDER 14
#define MAX_NODE ((PGSIZE - sizeof(struct PageInfo))/sizeof(struct BuddyNode))

struct PageInfo
{
    uint32 bitalloc[6];
    uint32 cnt;
    struct BuddyNode* nextFree;
    struct PageInfo *prePage, *nextPage;
};


// 双向链表，
struct BuddyNode
{
    void *pa, *lc, *rc;
};

