#include "myls.h"

#define BUFSIZE 255

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage wrong !\n");
        exit(1);
    }

    int sfd, dfd;

    sfd = open(argv[1], O_RDONLY);
    if (sfd < 0)
    {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    dfd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (dfd < 0)
    {
        close(sfd);
        perror("open()");
        exit(EXIT_FAILURE);
    }
    int len, ret, pos;
    char buf[BUFSIZE];
    while (1)
    {
        len = read(sfd, buf, BUFSIZE);
        if (len < 0)
        {
            perror("read()");
            break;
        }
        if (len == 0)
            break;
        pos = 0;
        while (len > 0)
        {
            ret = write(dfd, buf + pos, len);
            if (ret < 0)
            {
                perror("write()");
                close(sfd);
                close(dfd);
                exit(1);
            }
            pos += ret;
            len -= ret;
        }

        close(dfd);
        close(sfd);

        exit(0);
    }
}
