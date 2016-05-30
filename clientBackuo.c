#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(50001);
    servaddr.sin_addr.s_addr = inet_addr("172.16.187.254");

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    char sendline[100];
    sprintf(sendline, "Hello, world!");

    write(sockfd, sendline, strlen(sendline));

    close(sockfd);

    return 1;
}
