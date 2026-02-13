## coredump

1. 什么是coredump文件？
CoreDump：Core的意思是内存，Dump的意思是扔出来，堆出来（段错误）。开发和使用Unix程序时，有时程序莫名其妙的down了，却没有任何的提示(有时候会提示core dumped)，这时候可以查看一下有没有形如`core.进程号`的文件生成，这个文件便是操作系统把程序down掉时的内存内容扔出来生成的, 它可以做为调试程序的参考，能够很大程序帮助我们定位问题。
2. 生成coredump文件的方法
首先需要确认当前会话的`ulimit –c`，若为0，则不会产生对应的coredump，需要进行修改和设置。
设置core大小为无限: `ulimit -c unlimited`
3. 更改core dump生成路径
因为core dump默认会生成在程序的工作目录，但是有些程序存在切换目录的情况，导致core dump生成的路径没有规律，
所以最好是自己建立一个文件夹，存放生成的core文件。
建立一个 /data/coredump 文件夹，在根目录data里的coredump文件夹
临时：
    调用 `echo /home/zr333/data/coredump/core.%e.%p > /proc/sys/kernel/core_pattern` 更改core文件生成路径，自动放在这个/data/coredump文件夹里 (%e表示程序名， %p表示进程id)
永久生效：
    修改sudo vim /etc/sysctl.conf文件，在文件末尾加上`kernel.core_pattern=/data/coredump/core.%e.%p` `kernel.core_uses_pid=1`,最后加上`sudo sysctl -p` 使配置立即生效
推荐方式（开发环境）：
`sudo sysctl -w kernel.core_pattern="core.%e.%p"`
4.如何调试
    1. gdb ./可执行程序 --> run
    2. gdb ./可执行程序 ./段错误文件 -->bt (建议用方法二)