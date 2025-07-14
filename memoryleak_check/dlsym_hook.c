#define _GNU_SOURCE
#include <dlfcn.h>
#include <link.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <mcheck.h>
#include <malloc.h>


typedef void *(*malloc_t)(size_t size);
typedef void (*free_t)(void *ptr);

malloc_t malloc_f = NULL;
free_t free_f = NULL;

int enable_malloc_hook = 1;
int enable_free_hook = 1;

void *ConvertToELF(void *addr)
{

    Dl_info info;
    struct link_map *link;

    dladdr1(addr, &info, (void **)&link, RTLD_DL_LINKMAP);

    return (void *)((size_t)addr - link->l_addr);
}

// main --> func --> f();
static void init_hook(void)
{

    if (malloc_f == NULL)
    {
        malloc_f = (malloc_t)dlsym(RTLD_NEXT, "malloc");
    }

    if (free_f == NULL)
    {
        free_f = (free_t)dlsym(RTLD_NEXT, "free");
    }
}

// __attribute__((constructor))
// void init(void)
// {
//     init_hook();
// }

void *malloc(size_t size)
{

    void *p = NULL;

    if (enable_malloc_hook)
    {
        enable_malloc_hook = 0;

        // printf("size: %ld\n", size);
        p = malloc_f(size);

        void *caller = __builtin_return_address(0);

        char buff[128] = {0};
        sprintf(buff, "./mem/%p.mem", p);

        FILE *fp = fopen(buff, "w");
        fprintf(fp, "[+]%p, addr: %p, size: %ld\n",
                ConvertToELF(caller), p, size);

        fflush(fp);

        enable_malloc_hook = 1;
    }
    else
    {

        p = malloc_f(size);
    }

    return p;
}

void free(void *ptr)
{

    if (enable_free_hook)
    {
        enable_free_hook = 0;

        char buff[128] = {0};
        sprintf(buff, "./mem/%p.mem", ptr);

        if (unlink(buff) < 0)
        {
            printf("double free: %p\n", ptr);
            return;
        }
        free_f(ptr);

        enable_free_hook = 1;
    }
    else
    {
        free_f(ptr);
    }
}

int main()
{

    init_hook();
    void *p1 = malloc(10);
    void *p2 = malloc(15);
    void *p3 = malloc(20);

    free(p2);
    free(p3);
}
