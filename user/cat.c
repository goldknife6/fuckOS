#include <lib.h>
#include <stdio.h>
#include <fcntl.h>
char buf[8192];

void
cat(int f, char *s)
{
	long n;
	int r;

	while ((n = read(f, buf, (long)sizeof(buf))) > 0)
		if ((r = write(1, buf, n)) != n) {
			printf("write error copying %s: %e", s, r);
			exit();
		}
	if (n < 0) {
		printf("error reading %s: %e", s, n);
		exit();	
	}
}

int
main(int argc, char **argv)
{
	int f, i;

	if (argc == 1)
		cat(0, "<stdin>");
	else
		for (i = 1; i < argc; i++) {
			f = open(argv[i], O_RDONLY,0);
			if (f < 0)
				printf("can't open %s: %e\n", argv[i], f);
			else {
				cat(f, argv[i]);
				close(f);
			}
		}
	return 0;
}
