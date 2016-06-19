#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
	int fd;
	struct flock first_lock;
	struct flock second_lock;

	first_lock.l_type = F_WRLCK;
	first_lock.l_whence = SEEK_SET;
	first_lock.l_start = 0;
	first_lock.l_len = 10;

	second_lock.l_type = F_WRLCK;
	second_lock.l_whence = SEEK_SET;
	second_lock.l_start = 10;
	second_lock.l_len = 5;

	fd = open("locktest", O_RDWR);

	if(fcntl(fd, F_SETLKW, &first_lock) == -1)
	{
		perror("parent: first locking");
		exit(1);
	}
	printf("parent: first locked record\n");

	switch(fork())
	{
		case -1:
			perror("fork");
			exit(1);
		case 0:
			if(fcntl(fd, F_SETLKW, &second_lock) == -1)
			{
				perror("child: second locking");
				exit(1);
			}
			printf("child: second locked\n");

			if(fcntl(fd, F_SETLKW, &first_lock) == -1)
			{
				perror("child: first locking");
				exit(1);
			}
			printf("child: first locked\n");

			printf("child: exiting\n");
			exit(0);
		default:
			printf("parent sleeping\n");
			sleep(10);
			if(fcntl(fd, F_SETLKW, &second_lock) == -1)
			{
				perror("parent: second locking");
				exit(1);
			}
			printf("parent: second locked record\n");
	}
	return 0;
}
