#迷你型类Linux操作系统

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


1、PC Bootstrap（PC自举）
	一般一个PC的物理地址空间被硬连线成以下结构

+------------------+  <- 0xFFFFFFFF (4GB)
|      32-bit      |
|  memory mapped   |
|     devices      |
|                  |
/\/\/\/\/\/\/\/\/\/\

/\/\/\/\/\/\/\/\/\/\
|                  |
|      Unused      |
|                  |
+------------------+  <- depends on amount of RAM
|                  |
|                  |
| Extended Memory  |
|                  |
|                  |
+------------------+  <- 0x00100000 (1MB)
|     BIOS ROM     |
+------------------+  <- 0x000F0000 (960KB)
|  16-bit devices, |
|  expansion ROMs  |
+------------------+  <- 0x000C0000 (768KB)
|   VGA 显存        |
+------------------+  <- 0x000A0000 (640KB)
|                  |
|    低端内存       |
|                  |
+------------------+  <- 0x00000000

	早期的PC是基于16-bit Intel 8088处理器，只能寻址1MB的物理地址，像VGA显存，BIOS ROM都安排的0 ～ 1MB内的物理空间中，随着科技的发展，现在的内存大多是4GB，甚至更多。
为了为已有的软件提供向后兼容性，因此现在的PC一般在 0x000A0000 ～ 0x00100000留有一个空洞。

	PC加电后，首先执行BIOS的代码（就是这么设计的），初始化多种设备（比如VGA显示器），配置中断向量表，最后就开始搜索启动设备（硬盘，软盘，光盘），找到时候就会把加载器加载到内存0x7c00 ～ 0x7dff的地方，然后就会把控制转移到0x7c00这个地方，之后加载器就会加载内核。



















A mini operating system.

You need bochs or qemu to boot this os.

type make on the terminal to compile the kernel
and type make boch or make qemu to start kernel up.

You need sudo apt-get install xorriso.
