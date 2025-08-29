#ifndef DEADLOCK_H
#define DEADLOCK_H

// 初始化 hook 函数
int init_hook(void);

// 启动死锁检测线程
void start_check(void);

#endif // DEADLOCK_H
