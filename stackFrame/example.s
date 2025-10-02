
example.o：     文件格式 elf64-x86-64


Disassembly of section .text:

0000000000000000 <Add>:
#include <stdio.h>

int Add(int x, int y)
{
   0:	f3 0f 1e fa          	endbr64 
   4:	55                   	push   %rbp        
   5:	48 89 e5             	mov    %rsp,%rbp
   8:	89 7d ec             	mov    %edi,-0x14(%rbp)
   b:	89 75 e8             	mov    %esi,-0x18(%rbp)
	int z = 0;
   e:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%rbp)
	z = x + y;
  15:	8b 55 ec             	mov    -0x14(%rbp),%edx
  18:	8b 45 e8             	mov    -0x18(%rbp),%eax
  1b:	01 d0                	add    %edx,%eax
  1d:	89 45 fc             	mov    %eax,-0x4(%rbp)
	return z;
  20:	8b 45 fc             	mov    -0x4(%rbp),%eax
}
  23:	5d                   	pop    %rbp
  24:	c3                   	ret    

0000000000000025 <main>:


int main()
{
  25:	f3 0f 1e fa          	endbr64 
  29:	55                   	push   %rbp             # 保存调用者栈基指针
  2a:	48 89 e5             	mov    %rsp,%rbp        # 建立当前栈帧基地址
  2d:	48 83 ec 10          	sub    $0x10,%rsp       # 预留16字节局部变量空间。
	int a = 10;
  31:	c7 45 f4 0a 00 00 00 	movl   $0xa,-0xc(%rbp)  # 赋值a,低地址（栈地址向下增长）
	int b = 20;
  38:	c7 45 f8 14 00 00 00 	movl   $0x14,-0x8(%rbp) # 赋值b
	int c = 0;
  3f:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%rbp)  # 赋值c
	c = Add(a, b);
  46:	8b 55 f8             	mov    -0x8(%rbp),%edx   # 将变量 b 加载到寄存器 %edx。
  49:	8b 45 f4             	mov    -0xc(%rbp),%eax   # 将变量 a 加载到寄存器 %eax。
  4c:	89 d6                	mov    %edx,%esi         # 参数传递 ，从右向左，即第二个参数
  4e:	89 c7                	mov    %eax,%edi         # 将 %eax 的值传递给 %edi，即第一个参数
  50:	e8 00 00 00 00       	call   55 <main+0x30>    # 调用add()，将下一条指令地址（返回地址）压栈，跳转到被调用函数入口
  55:	89 45 fc             	mov    %eax,-0x4(%rbp)   # 将返回值存储到变量 c 中
	printf("%d\n", c);
  58:	8b 45 fc             	mov    -0x4(%rbp),%eax   # 将变量 c 的值加载到寄存器 %eax，准备打印。
  5b:	89 c6                	mov    %eax,%esi         # 将 %eax 的值传递给 %esi，作为 printf 的参数。
  5d:	48 8d 05 00 00 00 00 	lea    0x0(%rip),%rax    # 计算格式化字符串的地址。
  64:	48 89 c7             	mov    %rax,%rdi         # 将格式化字符串地址传递给 %rdi，作为 printf 的参数。
  67:	b8 00 00 00 00       	mov    $0x0,%eax
  6c:	e8 00 00 00 00       	call   71 <main+0x4c>    # 调用printf()
	return 0;
  71:	b8 00 00 00 00       	mov    $0x0,%eax         # 作用是将main返回值0放到寄存器eax，等main返回后调用main可拿到这个值。
}
  76:	c9                   	leave                    # 等价于：
                                                     mov %rbp, %rsp   # 恢复栈指针到进入函数时的位置
                                                     pop %rbp         # 恢复旧的基址指针。
  77:	c3                   	ret                      # 等价于：
                                                     pop %rip         # 将返回地址弹出到指令指针寄存器
