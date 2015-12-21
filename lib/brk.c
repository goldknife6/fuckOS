#include <syscall.h>
#include <lib.h>

viraddr_t _startbrk = 0;
viraddr_t _curbrk = 0;

viraddr_t brk(viraddr_t end_data_segment)
{
	viraddr_t va = 0;
	if (! _startbrk) {
		_startbrk = sys_brk(0);
		_curbrk = _startbrk;
	}

	va = sys_brk(end_data_segment);

	if (va == end_data_segment)
		_curbrk = va;

	return va;
}

viraddr_t sbrk(uint32_t inc)
{
	viraddr_t va = 0;
	viraddr_t tmp;
	if (! _curbrk) {
		brk(0);
	}
	tmp = _curbrk;
	va = brk(_curbrk + inc);
	if (va) {
		return tmp;
	}
	return 0;
}
