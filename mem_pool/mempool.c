
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/*
实现：内存池是按照大小块进行存储的（设置了个阈值判定，比如4k)。内存池会预申请一块
4k的内存块，当用户向内存池申请小于4k时，内存池会从这内存块的空间中划出size空间，并且进行引
用计数加1。再有申请，依旧划分并且计数加1。如果这个内存块剩余空间不足以分配size大小时，此时内
存池会再次申请一块内存块(4k)，再从新的内存块划分size空间给用户。每次释放小内存时，都会在内存块中
引用计数减1，只有当引用计数为0的时候。才会回收内存块使他重新成为空闲的空间，以便重复利用空
间。这样，内存池避免频繁向内核申请/释放内存，从而提高了系统性能。对于大块内存的话，内存池不
会预先申请内存，用户申请的时候，内存池再申请内存，然后返回给用户。并且对于大块，回收就直接

引用计数：通过指针移动管理内存状态，而非显式引用计数。
具体的：用 last 指针标记当前分配位置，通过剩余空间判断是否需新内存块
*/

#define MP_ALIGNMENT 32
#define MP_PAGE_SIZE 4096 // 内存块大小
#define MP_MAX_ALLOC_FROM_POOL (MP_PAGE_SIZE - 1)

// 内存对齐
// 将整数 n 向上对齐到 alignment 的倍数。
#define mp_align(n, alignment) (((n) + (alignment - 1)) & ~(alignment - 1))
// 将指针 p 向上对齐到 alignment 的边界，返回值类型是 void *
#define mp_align_ptr(p, alignment) (void *)((((size_t)p) + (alignment - 1)) & ~(alignment - 1))

// 大块内存的链表节点
struct mp_large_s
{
    struct mp_large_s *next; // 指向下一个大内存块的指针
    void *alloc; // 指向实际分配的内存块的指针
};

// 小内存块的状态
struct mp_node_s
{

    unsigned char *last; // 当前未分配区域的起始位置
    unsigned char *end;  // 当前内存块的结束位置
    struct mp_node_s *next; // 下一内存块
    size_t failed;          // 记录分配失败次数
};

struct mp_pool_s
{

    size_t max; // 小块内存的最大分配大小

    struct mp_node_s *current; // 当前可用的小内存块链表

    struct mp_large_s *large;  // 大内存块链表

    struct mp_node_s head[0]; // 柔性数组（Flexible Array Member, FAM）。主要作用是让结构体末尾可以动态扩展，从而在运行时分配额外的内存空间。
};

struct mp_pool_s *mp_create_pool(size_t size);
void mp_destory_pool(struct mp_pool_s *pool);
void *mp_alloc(struct mp_pool_s *pool, size_t size);
void *mp_nalloc(struct mp_pool_s *pool, size_t size);
void *mp_calloc(struct mp_pool_s *pool, size_t size);
void mp_free(struct mp_pool_s *pool, void *p);

// 创建内存池
struct mp_pool_s *mp_create_pool(size_t size)
{

    struct mp_pool_s *p;
    // 分配一块 对齐到 32 字节 的内存
    int ret = posix_memalign((void **)&p, MP_ALIGNMENT, size + sizeof(struct mp_pool_s) + sizeof(struct mp_node_s));
    if (ret)
    {
        return NULL;
    }

    p->max = (size < MP_MAX_ALLOC_FROM_POOL) ? size : MP_MAX_ALLOC_FROM_POOL;
    // 控制结构体 mp_pool_s
    // 第一个小块管理节点 mp_node_s（作为 head）
    // 用户实际可用的数据区 size 字节

    p->current = p->head;
    p->large = NULL;

    // 把第一个内存块的 last 设置为可用区域起始位置
    p->head->last = (unsigned char *)p + sizeof(struct mp_pool_s) + sizeof(struct mp_node_s);
    p->head->end = p->head->last + size; // 第一个内存块的结束位置

    p->head->failed = 0;

    return p;
}

void mp_destory_pool(struct mp_pool_s *pool)
{
    // 定义局部变量，用于遍历内存池中的节点和大块内存
    struct mp_node_s *h, *n;
    struct mp_large_s *l;

    // 遍历内存池中的大块内存链表，并释放每个大块内存的分配空间
    for (l = pool->large; l; l = l->next)
    {
        if (l->alloc) // 如果大块内存已分配
        {
            free(l->alloc); // 释放该内存
        }
    }

    // 从内存池的头节点的下一个节点开始遍历（跳过头节点）
    h = pool->head->next;
    // 遍历内存池中的普通节点链表，并逐个释放
    while (h)
    {
        n = h->next; // 保存下一个节点指针
        free(h);     // 释放当前节点
        h = n;       // 移动到下一个节点
    }

    // 最后释放内存池结构体本身
    free(pool);
}

// 重置内存池 (mp_pool_s 结构体) 的状态，包括释放大块内存和重置小块内存节点的指针。
void mp_reset_pool(struct mp_pool_s *pool)
{

    struct mp_node_s *h;
    struct mp_large_s *l;

    for (l = pool->large; l; l = l->next)
    {
        if (l->alloc)
        {
            free(l->alloc);
        }
    }
    pool->large = NULL;

    // 遍历 pool->head 链表（存储小块内存节点）。
    // 将每个节点的 last 指针重置为该节点的起始地址加上 mp_node_s 结构体的大小，表示该节点可重新用于分配小块内存。 
    for (h = pool->head; h; h = h->next)
    {
        h->last = (unsigned char *)h + sizeof(struct mp_node_s);
    }
}


// 分配新的内存块
static void *mp_alloc_block(struct mp_pool_s *pool, size_t size)
{
    // 计算内存池头部节点（pool->head）所代表的内存块的总大小（4k）
    unsigned char *m; 
    struct mp_node_s *h = pool->head; 
    size_t psize = (size_t)(h->end - (unsigned char *)h);
    // 分配一块大小为 4k 的内存。
    int ret = posix_memalign((void **)&m, MP_ALIGNMENT, psize);
    if (ret)
        return NULL;

    struct mp_node_s *p, *new_node, *current;
    new_node = (struct mp_node_s *)m; // 将新分配的内存块转换为节点结构

    new_node->end = m + psize;
    new_node->next = NULL;
    new_node->failed = 0;

    m += sizeof(struct mp_node_s); // 将 m 指针向后移动，跳过节点结构体大小
    m = mp_align_ptr(m, MP_ALIGNMENT); // 将 m 指针对齐到 MP_ALIGNMENT
    new_node->last = m + size;

    // 遍历内存池中的节点链表，找到最后一个节点，将新节点加入链表
    current = pool->current;
    for (p = current; p->next; p = p->next)
    {
        if (p->failed++ > 4)
        { 
            current = p->next;
        }
    }
    p->next = new_node;

    pool->current = current ? current : new_node; //如果current不为NULL，则使用current作为新的当前节点
                                                // 如果current为NULL(可能所有节点都失败超过4次)，则使用新添加的节点作为当前节点
    return m;
}

// 从内存池中分配一块大块内存，大小为 size 字节
static void *mp_alloc_large(struct mp_pool_s *pool, size_t size)
{
    // 
    void *p = malloc(size);
    if (p == NULL)
        return NULL;

    /*
    首先遍历内存池中已有的 large 链表（存储大块内存的链表），检查是否有未使用的槽（large->alloc==NULL）。
    如果找到空闲槽，将新分配的 p 赋值给 large->alloc，并返回 p。
    限制遍历深度（最多检查 4 个节点，n > 3 时退出循环），避免性能问题。
    */
    size_t n = 0;
    struct mp_large_s *large;
    for (large = pool->large; large; large = large->next)
    {
        if (large->alloc == NULL)
        {
            large->alloc = p;
            return p;
        }
        if (n++ > 3)
            break;
    }

    // 如果没有找到空闲槽，调用 mp_alloc 从内存池中分配一个新的 mp_large_s 结构（用于管理大块内存）。
    large = mp_alloc(pool, sizeof(struct mp_large_s));
    if (large == NULL)
    {
        free(p);
        return NULL;
    }

    // 将新分配的内存挂载到链表头部
    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}
// 直接分配大块内存，支持指定的对齐格式
void *mp_memalign(struct mp_pool_s *pool, size_t size, size_t alignment)
{

    void *p;

    int ret = posix_memalign(&p, alignment, size);
    if (ret)
    {
        return NULL;
    }

    // 将新分配的大内存插入到内存池的大内存链表头部
    struct mp_large_s *large = mp_alloc(pool, sizeof(struct mp_large_s));
    if (large == NULL)
    {
        free(p);
        return NULL;
    }
    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}


// 分配大小为size的内存（对齐）
void *mp_alloc(struct mp_pool_s *pool, size_t size)
{

    unsigned char *m;
    struct mp_node_s *p;

    // 如果请求分配的内存大小小于等于内存池的最大块大小（max），则从内存池中分配内存。
    if (size <= pool->max)
    {

        p = pool->current;

        do
        {
            m = mp_align_ptr(p->last, MP_ALIGNMENT);
            if ((size_t)(p->end - m) >= size)
            {
                p->last = m + size;
                return m;
            }
            p = p->next;
        } while (p);

        return mp_alloc_block(pool, size);
    }

    // 如果请求分配的内存大小大于内存池的最大块大小，则直接调用 mp_alloc_large 分配大块内存。
    return mp_alloc_large(pool, size);
}
// 分配大小为size的内存（非对齐）
void *mp_nalloc(struct mp_pool_s *pool, size_t size)
{

    unsigned char *m;
    struct mp_node_s *p;

    if (size <= pool->max)
    {
        p = pool->current;

        do
        {
            m = p->last;
            if ((size_t)(p->end - m) >= size)
            {
                p->last = m + size;
                return m;
            }
            p = p->next;
        } while (p);

        return mp_alloc_block(pool, size);
    }

    return mp_alloc_large(pool, size);
}
// 分配大小为size的内存并初始化为0
void *mp_calloc(struct mp_pool_s *pool, size_t size)
{

    void *p = mp_alloc(pool, size);
    if (p)
    {
        memset(p, 0, size);
    }

    return p;
}

// 释放指定大块内存
void mp_free(struct mp_pool_s *pool, void *p)
{

    struct mp_large_s *l;
    for (l = pool->large; l; l = l->next)
    {
        if (p == l->alloc)
        {
            free(l->alloc);
            l->alloc = NULL;

            return;
        }
    }
}

/**
 * @brief 内存池测试主函数
 *
 * 该函数演示了内存池的基本操作流程，包括：
 * 1. 创建指定大小的内存池
 * 2. 进行多次内存分配和释放操作
 * 3. 测试内存对齐功能
 * 4. 测试calloc清零功能
 * 5. 测试大块内存分配
 * 6. 重置内存池
 * 7. 销毁内存池
 *
 * @return int 程序执行状态码，0表示成功
 */
int main()
{

    int size = 1 << 12; // 4096

    struct mp_pool_s *p = mp_create_pool(size);

    int i = 0;
    for (i = 0; i < 10; i++)
    {

        void *mp = mp_alloc(p, 512);
        //		mp_free(mp);
    }

    printf("mp_create_pool: %ld\n", p->max);
    printf("mp_align(123, 32): %d, mp_align(17, 32): %d\n", mp_align(24, 32), mp_align(17, 32));
    printf("mp_align_ptr(p->current, 32): %lx, p->current: %lx, mp_align(p->large, 32): %lx, p->large: %lx\n", 
    mp_align_ptr(p->current, 32), p->current, 
    mp_align_ptr(p->large, 32), p->large);


    // 测试calloc
    int j = 0;
    for (i = 0; i < 5; i++)
    {

        char *pp = mp_calloc(p, 32);
        for (j = 0; j < 32; j++)
        {
            if (pp[j])
            {
                printf("calloc wrong\n");
            }
            printf("calloc success\n");
        }
    }

    // printf("mp_reset_pool\n");

    // 测试大块内存分配
    for (i = 0; i < 5; i++)
    {
        void *l = mp_alloc(p, 8192);
        mp_free(p, l);
    }

    mp_reset_pool(p);

    // printf("mp_destory_pool\n");
    //  密集分配测试
    for (i = 0; i < 58; i++)
    {
        mp_alloc(p, 256);
    }

    mp_destory_pool(p);

    return 0;
}
