#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
	int fd;
	struct flock my_lock;

	my_lock.l_type = F_WRLCK;
	my_lock.l_whence = SEEK_SET;
	my_lock.l_start = 0;
	my_lock.l_len = 10;

	fd = open("locktest", O_RDWR);

	if(fcntl(fd, F_SETLKW, &my_lock) == -1)
	{
		perror("parent: locking");
		exit(1);
	}

	printf("parent: locked record\n");

	switch(fork())
	{
		case -1:
			perror("fork");
			exit(1);
		case 0:
			my_lock.l_len = 5;
			if(fcntl(fd, F_SETLKW, &my_lock) == -1)
			{
				perror("child: locking");
				exit(1);
			}
			printf("child: locked\n");
			printf("child: exiting\n");
			exit(0);
	}

	printf("parent: unlocking\n");
	my_lock.l_type = F_UNLCK;
	if(fcntl(fd, F_SETLKW, &my_lock) == -1)
	{
		perror("parent: unlocking");
		exit(1);
	}

	wait(NULL);

	printf("parent: exiting\n");
	exit(0);
	return 0;
}
