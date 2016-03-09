#迷你型类Linux操作系统



##<a name = "index"/>目录
* [操作系统介绍](#操作系统介绍)
* [ELF文件格式介绍](#ELF文件格式介绍)
* [Grub加载器介绍](#加载器介绍) 
* [Buddy系统介绍](#语法要素)
* [slub内存分配系统介绍](#语法要素)

<a name = "操作系统介绍"/>
#操作系统介绍
此项目的灵感来源于 MIT 6.828 的课程
由于本人水平有限,又由于只想写一个玩具操作系统，所以部分代码可能不是很规范，注释也是很少，请大家谅解
此操作系统在重定向和管道功能上有点小bug，由于时间原因我就不debug了


开发环境:

  Ubuntu14.04+
  GCC version 5.2.1

测试环境:

  QEMU emulator version 2.3.0
  ThinkPad E431

具体实现与特点:

  1、可在真机上运行

  2、实现buddy分页系统，slub内存分配系统

  3、实现VFS(虚拟文件系统)

  4、实现各种库函数(eg.  malloc)

  5、支持SMP，也就是多处理器


使用方法:

  下载源码后,在终端输入make qemu,会自动完成内核的编译，然后内核就会运行在qemu虚拟机里

制作u盘启动:
一会再写。。

<a name = "Grub加载器介绍"/>
#加载器介绍
此操作系统并没有实现加载器，而是使用的现成的Grub加载器。为了使内核可以被Grub加载到内存，内核的前8192个字节内必须包含多重引导头部

其实很简单，只要在内核入口文件entry.S前加上几行代码就可以了,这些字段和多重引导头部的详细定义请参考[Multiboot规范](http://www.red-bean.com/doc/multiboot/html/multiboot.html "悬停显示")  
```
	.text
	.globl  start, _start
start: 
_start:
	jmp 	multiboot_entry

	.balign	MULTIBOOT_HEADER_ALIGN
_header_start:
	.long	MULTIBOOT2_HEADER_MAGIC
	.long	MULTIBOOT_ARCHITECTURE_I386
	.long	_header_end - _header_start
	.long	CHECKSUM
	.balign MULTIBOOT_TAG_ALIGN
	.word MULTIBOOT_HEADER_TAG_END
	.word 0	
	.long 8
_header_end:
```
有了这个头部，Grub就会把内核加载到制定的物理内存中，然后Grub就会把控制权交给内核的入口点，也就是start

<a name = "ELF文件格式介绍"/>
#ELF文件格式介绍
ELF全称为Executable and Linking Format，在《深入理解计算机系统》的第七章里面说的很明白了，说白了就是一种目标文件格式。
目标文件有三种格式：
1) 可重定位目标文件
这是由汇编器汇编生成的 .o 文件。链接器拿一个或一些可重定位目标文件（Relocatable object files） 作为输入，经链接处理后，生成一个可执行的目标文件 (Executable file) 或者一个可被共享的对象文件(Shared object file)。
2) 可执行目标文件(Executable file)
包含二进制代码和数据，可以直接加载进内存并执行的一种文件。文本编辑器vi、调式用的工具gdb、播放mp3歌曲的软件mplayer等等都是Executable object file。
3) 可被共享的对象文件(Shared object file)
一种特殊类型的可重定位目标文件，可以在加载或者运行时被动态地加载到内存中，这些就是所谓的动态库文件，也即 .so 
A mini operating system.

You need bochs or qemu to boot this os.

type make on the terminal to compile the kernel
and type make boch or make qemu to start kernel up.

You need sudo apt-get install xorriso.
