
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
  29:	55                   	push   %rbp
  2a:	48 89 e5             	mov    %rsp,%rbp
  2d:	48 83 ec 10          	sub    $0x10,%rsp
	int a = 10;
  31:	c7 45 f4 0a 00 00 00 	movl   $0xa,-0xc(%rbp)
	int b = 20;
  38:	c7 45 f8 14 00 00 00 	movl   $0x14,-0x8(%rbp)
	int c = 0;
  3f:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%rbp)
	c = Add(a, b);
  46:	8b 55 f8             	mov    -0x8(%rbp),%edx
  49:	8b 45 f4             	mov    -0xc(%rbp),%eax
  4c:	89 d6                	mov    %edx,%esi
  4e:	89 c7                	mov    %eax,%edi
  50:	e8 00 00 00 00       	call   55 <main+0x30>
  55:	89 45 fc             	mov    %eax,-0x4(%rbp)
	printf("%d\n", c);
  58:	8b 45 fc             	mov    -0x4(%rbp),%eax
  5b:	89 c6                	mov    %eax,%esi
  5d:	48 8d 05 00 00 00 00 	lea    0x0(%rip),%rax        # 64 <main+0x3f>
  64:	48 89 c7             	mov    %rax,%rdi
  67:	b8 00 00 00 00       	mov    $0x0,%eax
  6c:	e8 00 00 00 00       	call   71 <main+0x4c>
	return 0;
  71:	b8 00 00 00 00       	mov    $0x0,%eax
}
  76:	c9                   	leave  
  77:	c3                   	ret    
