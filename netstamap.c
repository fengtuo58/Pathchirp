#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <string.h>  
#include <stdio.h>  
#include <stdint.h>  
#include <sys/ioctl.h>  
#include <sys/time.h>  
#define SERVPORT 50001 
  
int main(int argc, char **argv)   
{   
     int sockfd;   
     struct sockaddr_in srvAddr;   
     sockfd = socket(AF_INET ,SOCK_DGRAM,0 );   
     if(sockfd< 0 )   
     {   
          printf("socket error\n");   
          return 0;   
     }      
  
     bzero(&srvAddr, sizeof(srvAddr) );   
     srvAddr.sin_family = AF_INET;   
     srvAddr.sin_port   = htons(SERVPORT);   
     srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);   
  
     bind(sockfd, (struct sockaddr*)&srvAddr,sizeof(srvAddr));   
  
     while(1)   
     {      
          struct msghdr msg;   
  
          struct sockaddr_in clientAddr;   
          msg.msg_name = &clientAddr;   
          msg.msg_namelen= sizeof(clientAddr);   
  
          struct iovec iov;   
          msg.msg_iov = &iov;   
          msg.msg_iovlen =1;   
          char text[1024] ;   
          iov.iov_base= text;   
          iov.iov_len = sizeof(text);   
  
          char ctrl[CMSG_SPACE(sizeof(struct timeval))];   
          struct cmsghdr *cmsg=(struct cmsghdr*)&ctrl;   
          msg.msg_control = (caddr_t)ctrl;   
          msg.msg_controllen = sizeof(ctrl);   
  
          int rc = recvmsg(sockfd,&msg,0);   
          if(rc== -1)   
          {   
               printf("recvmsg error\n");   
               return 0;   
          }   
  
          struct timeval tv, tvNow,tvRes;  
  
          if(cmsg->cmsg_level ==SOL_SOCKET&&   
                    cmsg->cmsg_type  ==SCM_TIMESTAMP &&   
                    cmsg->cmsg_len   ==CMSG_LEN(sizeof(tv))   
            )   
          memcpy(&tv,CMSG_DATA(cmsg),sizeof(tv));   
          gettimeofday(&tvNow, NULL);  
          uint64_t ddwNow = tvNow.tv_sec*1000000 + tvNow.tv_usec;  
          uint64_t ddwTv = tv.tv_sec*1000000 + tv.tv_usec;  
  
          printf("Now:%lu Tv:%lu dff:%lu\n",ddwNow,ddwTv,(ddwNow - ddwTv)/1000000);  
          sleep(10);  
     }   
}  
