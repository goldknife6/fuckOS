#include <lib.h>
#include <errno.h>
#include <stdio.h>
int
getchar()
{
	int c;
	int r;
	while ((r = read(0, &c, 1)) == 0)
		;
	if (r < 0)
		return r;
	else
		return c;
}

int
putchar(char c)
{
	return write(0, &c, 1);
}

#define BUFLEN 1024
static char buf[BUFLEN];

char *
readline(const char *prompt)
{
	int i, c, echoing;
/*
#if JOS_KERNEL
	if (prompt != NULL)
		printf("%s", prompt);
#else
	if (prompt != NULL)
		fprintf(1, "%s", prompt);
#endif
*/
	i = 0;
	//echoing = iscons(0);
	echoing = 1;
	while (1) {
		c = getchar();
		if (c < 0) {
			if (c != -EEOF)
				printf("read error: %e\n", c);
			return NULL;
		} else if ((c == '\b' || c == '\x7f') && i > 0) {
			if (echoing)
				putchar('\b');
			i--;
		} else if (c >= ' ' && i < BUFLEN-1) {
			if (echoing)
				putchar(c);
			buf[i++] = c;
		} else if (c == '\n' || c == '\r') {
			if (echoing)
				printf("1\n");
			buf[i] = 0;
			return buf;
		}
	}
}
