原理：
使用dlsym来实现hook(具体过程)，使用_builtin_returnaddress函数来返回调用所在函数的代码段地址，进行定位内存泄露的位置。

方法：
malloc的时候创建一个文件，文件名使用申请内存的地址，文件内容里记录申请该内存的代码段地址；
free的时候删除对应的文件，最终看是否有文件的存在即可判断是否存在内存泄露。

注意：
在执行 fopen()、fprintf()、sprintf() 等函数时，C 库内部也会调用 malloc() 来分配缓冲区内存。
如果没有关闭 hook（enable_malloc_hook = 0），
当 malloc() 被调用时，它又会再次进入你自己定义的 malloc() 函数，造成无限递归调用 → 栈溢出 → 崩溃