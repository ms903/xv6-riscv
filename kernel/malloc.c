#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "malloc.h"

struct PageInfo *head_page;
// 队头

struct BuddyNode *root;

// alloc a new BuddyNode in specific page
struct BuddyNode *
new_node(void)
{
    struct PageInfo *cpage = head_page;

    while (cpage->cnt >= MAX_NODE && cpage->nextPage != 0)
    {
        // 从链表最开始，寻找第一个有空余空间的页面
        cpage = cpage->nextPage;
    }

    if (cpage->cnt >= MAX_NODE && cpage->nextPage == 0)
    {
        printf("~~~~~~~~page %x is full, allocating new page~~~~~~~~~~\n", (uint64)cpage);
        // 当前页面已满，且没有新的页面，申请新的页面
        struct PageInfo *newpage = (struct PageInfo *)kalloc();

        if (head_page == 0)
        {
            panic("new node");
            return 0;
        }

        // 配置新页面
        cpage->nextPage = newpage;
        newpage->prePage = cpage;
        newpage->nextPage = 0;
        newpage->cnt = 0;
        for (uint8 i = 0; i < 6; i++)
            newpage->bitalloc[i] = (uint32)(0);

        newpage->nextFree = (struct BuddyNode *)((uint64)newpage + (uint64)sizeof(struct PageInfo));

        // 更新cpage
        cpage = newpage;
        printf("=========== new page %x is allocated ==========\n", (uint64)cpage);
    }

    // 在找到的页面中申请新的BuddyNode，并更新页面信息
    struct BuddyNode *rst = cpage->nextFree;
    memset(rst, 0, sizeof(struct BuddyNode));
    cpage->cnt += 1;
    uint64 pos = ((uint64)rst - (uint64)cpage - sizeof(struct PageInfo)) / sizeof(struct BuddyNode);
    printf("pos: %d \nalloc0: %x alloc1: %x alloc2: %x alloc3: %x alloc4: %x alloc5: %x \n", pos, cpage->bitalloc[0], cpage->bitalloc[1], cpage->bitalloc[2], cpage->bitalloc[3], cpage->bitalloc[4], cpage->bitalloc[5]);
    uint8 ind = pos / 32;
    cpage->bitalloc[ind] = cpage->bitalloc[ind] | (1 << (pos - 32 * ind));

    // 寻找当前页面内下一个freeNode
    if (cpage->cnt == MAX_NODE)
    {
        cpage->nextFree = 0;
    }
    else
    {
        uint64 p;

        for (p = 0; p < MAX_NODE; p++)
        {
            uint8 ind = p / 32;
            if ((cpage->bitalloc[ind] & (1 << (p - 32 * ind))) == 0)
            {
                break;
            }
        }

        // printf("next free pos is: %d \n", p);
        cpage->nextFree = (struct BuddyNode *)((uint64)cpage + sizeof(struct PageInfo) + p * sizeof(struct BuddyNode));
    }

    return rst;
}

void free_node(struct BuddyNode *na)
{
}

uint64 get_log(uint64 memsize)
{
    uint8 rst = 1;
    uint64 tmp = 2;
    while (memsize > tmp)
    {
        tmp <<= 1;
        rst++;
    }
    return rst;
}

void mallocinit(void)
{

    head_page = (struct PageInfo *)kalloc();
    if (head_page == 0)
    {
        panic("new node");
        return;
    }
    memset(head_page, 0, PGSIZE);

    head_page->cnt = 1;
    head_page->bitalloc[0] = (uint32)(1);
    for (uint8 i = 1; i < 6; i++)
        head_page->bitalloc[i] = (uint32)(0);

    // 这里是下一个空闲可以放BuddyNode的地址
    head_page->nextFree = (struct BuddyNode *)((uint64)head_page + (uint64)(sizeof(struct PageInfo)));
    printf("%x %x\n", head_page, head_page->nextFree);
    // 指向下一个存储BuddyNode块的控制块
    head_page->prePage = 0;
    // 指向上一个存储BuddyNode块的控制块
    head_page->nextPage = 0;

    // 为树根申请空间
    root = head_page->nextFree;

    head_page->nextFree = root + 1;

    // 初始化树根
    root->lc = 0;
    root->rc = 0;
    root->pa = 0;

    for (uint64 i = 1; i <= 170; i++)
    {
        // printf("0x%x\n", kalloc());
        printf("Allocating node %d. \n", i);
        printf("Allocated at addr: 0x%x\n\n", new_node());
    }

    printf("size of PageInfo:0x%x , size of BuddySize:0x%x \n", sizeof(struct PageInfo), sizeof(struct BuddyNode));
    printf("malloc init finished!\n");
}

void *
malloc(uint64 memsize)
{

    return 0;
}

void free(void *pt)
{
}
