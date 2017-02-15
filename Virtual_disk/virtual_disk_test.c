#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <error.h>

int main(int argc, char *argv[])
{
    char buf[256] = {0};
    int fd = open(argv[1], O_RDWR);
    int num = 0;

    printf("open %s\n", argv[1]);
    if(fd == -1)
    {
        perror("open");
        return -1;
    }
    printf("write:%s\n", argv[2]);

    num = write(fd, argv[2], strlen(argv[2]));
    printf("write num=%d\n", num);

    lseek(fd, 0, SEEK_SET);

    num = read(fd, buf, sizeof(buf));

    printf("num=%d, read:%s\n", num, buf);
   
    ioctl(fd, 1, 0);

    lseek(fd, 0, SEEK_SET);

    num = read(fd, buf, sizeof(buf));

    printf("after clear num=%d, read:%s\n", num, buf);
  
    close(fd);

    return 0;
}
