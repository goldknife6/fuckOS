#include <lib.h>
#include <stdio.h>

int main()
{
	int i;
	for (i = 0;i < 1220; i++) {
		pid_t pid = fork();
		if (pid == 0)
			exit();
	}
	return 0;
}
