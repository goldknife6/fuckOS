// Concurrent version of prime sieve of Eratosthenes.
// Invented by Doug McIlroy, inventor of Unix pipes.
// See http://swtch.com/~rsc/thread/.
// The picture halfway down the page and the text surrounding it
// explain what's going on here.
//
// Since NENVS is 1024, we can print 1022 primes before running out.
// The remaining two environments are the integer generator at the bottom
// of main and user/idle.

#include <lib.h>
#include <stdio.h>
#include <fcntl.h>
unsigned
primeproc(int fd)
{
	int i, id, p, pfd[2], wfd, r;

	// fetch a prime from our left neighbor
top:
	if ((r = readn(fd, &p, 4)) != 4) { 
		printf("primeproc could not read initial prime: %d, %e", r, r >= 0 ? 0 : r);
		exit();
	}
	printf("%d\n", p);
	// fork a right neighbor to continue the chain
	if ((i=pipe(pfd,0)) < 0) {
		printf("pipe: %e", i);
		exit();	
	}
	if ((id = fork()) < 0) {
		printf("fork: %e", id);
		exit();	
	}
	if (id == 0) {
		close(fd);
		close(pfd[1]);
		fd = pfd[0];
		goto top;
	}

	close(pfd[0]);
	wfd = pfd[1];

	// filter out multiples of our prime
	for (;;) {
		if ((r=readn(fd, &i, 4)) != 4) {
			printf("primeproc %d readn %d %d %e", p, fd, r, r >= 0 ? 0 : r);
			exit();		
		}
		if (i%p)
			if ((r=write(wfd, (char*)&i, 4)) != 4) {
				printf("primeproc %d write: %d %e", p, r, r >= 0 ? 0 : r);
				exit();
			}
	}
}

int
main(int argc, char **argv)
{
	int i, id, p[2], r,fd;
	fd = open("/dev/tty",O_RDWR,0);
	//binaryname = "primespipe";

	if ((i=pipe(p,0)) < 0) {
		printf("pipe: %e", i);
		exit();
	}

	// fork the first prime process in the chain
	if ((id=fork()) < 0) {
		printf("fork: %e", id);
		exit();
	}
	if (id == 0) {
		close(p[1]);
		primeproc(p[0]);
	}

	close(p[0]);

	// feed all the integers through
	for (i=2;; i++)
		if ((r=write(p[1], (char*)&i, 4)) != 4) {
			printf("generator write: %d, %e", r, r >= 0 ? 0 : r);
			exit();
		}

	return 0;
}

