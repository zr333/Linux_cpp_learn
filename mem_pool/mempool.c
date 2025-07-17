#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/*
实现：内存池是按照大小块进行存储的（设置了个阈值判定，比如4k)。内存池会预申请一块
4k的内存块，当用户向内存池申请小于4k时，内存池会从这内存块的空间中划出size空间，并且进行引
用计数加1。再有申请，依日划分并且计数加1。如果这个内存块剩余空间不足以分配size大小时，此时内
存池会再次申请一块内存块(4k)，再从新的内存块划分size空间给用户。每次释放小内存时，都会在内存块中
引用计数减1，只有当引用计数为0的时候。才会回收内存块使他重新成为空闲的空间，以便重复利用空
间。这样，内存池避免频繁向内核申请/释放内存，从而提高了系统性能。对于大块内存的话，内存池不
会预先申请内存，用户申请的时候，内存池再申请内存，然后返回给用户。并且对于大块，回收就直接
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
    struct mp_large_s *next;
    void *alloc;
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

    struct mp_node_s *current; // 当前使用中的小内存块
    struct mp_large_s *large;  // 大内存块链表

    struct mp_node_s head[0]; 
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
    int ret = posix_memalign((void **)&p, MP_ALIGNMENT, size + sizeof(struct mp_pool_s) + 
    sizeof(struct mp_node_s));
    if (ret)
    {
        return NULL;
    }

    p->max = (size < MP_MAX_ALLOC_FROM_POOL) ? size : MP_MAX_ALLOC_FROM_POOL;
    // 控制结构体 mp_pool_s
    //第一个小块管理节点 mp_node_s（作为 head）
    //用户实际可用的数据区 size 字节

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

    struct mp_node_s *h, *n;
    struct mp_large_s *l;

    for (l = pool->large; l; l = l->next)
    {
        if (l->alloc)
        {
            free(l->alloc);
        }
    }

    h = pool->head->next;

    while (h)
    {
        n = h->next;
        free(h);
        h = n;
    }

    free(pool);
}

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

    for (h = pool->head; h; h = h->next)
    {
        h->last = (unsigned char *)h + sizeof(struct mp_node_s);
    }
}

static void *mp_alloc_block(struct mp_pool_s *pool, size_t size)
{

    unsigned char *m;
    struct mp_node_s *h = pool->head;
    size_t psize = (size_t)(h->end - (unsigned char *)h);

    int ret = posix_memalign((void **)&m, MP_ALIGNMENT, psize);
    if (ret)
        return NULL;

    struct mp_node_s *p, *new_node, *current;
    new_node = (struct mp_node_s *)m;

    new_node->end = m + psize;
    new_node->next = NULL;
    new_node->failed = 0;

    m += sizeof(struct mp_node_s);
    m = mp_align_ptr(m, MP_ALIGNMENT);
    new_node->last = m + size;

    current = pool->current;

    for (p = current; p->next; p = p->next)
    {
        if (p->failed++ > 4)
        { //
            current = p->next;
        }
    }
    p->next = new_node;

    pool->current = current ? current : new_node;

    return m;
}

static void *mp_alloc_large(struct mp_pool_s *pool, size_t size)
{

    void *p = malloc(size);
    if (p == NULL)
        return NULL;

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

    large = mp_alloc(pool, sizeof(struct mp_large_s));
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

void *mp_memalign(struct mp_pool_s *pool, size_t size, size_t alignment)
{

    void *p;

    int ret = posix_memalign(&p, alignment, size);
    if (ret)
    {
        return NULL;
    }

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

void *mp_alloc(struct mp_pool_s *pool, size_t size)
{

    unsigned char *m;
    struct mp_node_s *p;

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

    return mp_alloc_large(pool, size);
}

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

void *mp_calloc(struct mp_pool_s *pool, size_t size)
{

    void *p = mp_alloc(pool, size);
    if (p)
    {
        memset(p, 0, size);
    }

    return p;
}

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

    // printf("mp_create_pool: %ld\n", p->max);
    printf("mp_align(123, 32): %d, mp_align(17, 32): %d\n", mp_align(24, 32), mp_align(17, 32));
    // printf("mp_align_ptr(p->current, 32): %lx, p->current: %lx, mp_align(p->large, 32): %lx, p->large: %lx\n", mp_align_ptr(p->current, 32), p->current, mp_align_ptr(p->large, 32), p->large);

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

    for (i = 0; i < 5; i++)
    {
        void *l = mp_alloc(p, 8192);
        mp_free(p, l);
    }

    mp_reset_pool(p);

    // printf("mp_destory_pool\n");
    for (i = 0; i < 58; i++)
    {
        mp_alloc(p, 256);
    }

    mp_destory_pool(p);

    return 0;
}
