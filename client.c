#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include<sys/time.h>
#include<stdlib.h>
#include<unistd.h>
#include <netinet/in.h>
/******************************.h***********************/
#define  PKT_PAYLOAD_SIZE   1000
#define  PKT_SIZE PKT_PAYLOAD_SIZE+28
#define  MAX_PKT_SIZE   2048
#define  MAX_CHIRP_LEN  200

#define  LOWEST_BITRATE 1 //Mbps
#define  HIGHEST_BITRATE 100 //Mbps
#define  SPREAD_FACTOR  1.2
#define  DECREASE_FACTOR 1.5 
#define  SIGNATURE_REGION_LEN 5

#define  BYTETOBIT(M)   (M*8.0)
#define  PRINT_CHK_FLAG  do{printf("%s ChkFlag-> %s:%d\n",__TIME__,__FILE__,__LINE__);}while(0);
#define  F  1.2
#define  L   5
typedef struct{
    uint32_t pkt_idx;
    uint32_t chirp_len;
    uint32_t pkt_len;
    uint32_t snd_time_sec;
    uint32_t snd_time_usec;
    uint32_t rcv_time_sec;
    uint32_t rcv_time_usec;
}Udp_Probe_Pkt_Header;

typedef struct{
    Udp_Probe_Pkt_Header hdr;
    char payload[PKT_PAYLOAD_SIZE];
}Udp_Probe_Pkt;
#define Tcp_Probe_Pkt Udp_Probe_Pkt

/******************************.h***********************/
int compute_probe_pkt_sum(void)
{
    int pkt_num = 0;

    double inter_arrival = 0;
    inter_arrival =BYTETOBIT( (double)sizeof(Udp_Probe_Pkt) )  / (LOWEST_BITRATE * 1.0E6);

    double min_inter_arrival = 0;
    min_inter_arrival =BYTETOBIT ((double)sizeof(Udp_Probe_Pkt) ) / (HIGHEST_BITRATE * 1.0E6);

    while(inter_arrival > min_inter_arrival){
        pkt_num ++;
        inter_arrival = inter_arrival/SPREAD_FACTOR;
    }

    return pkt_num;
}

void send_tcp_pkt(Tcp_Probe_Pkt *pkt, uint32_t pkt_idx, uint32_t chirp_len, 
                  int sd)
{

int ret = 0;

   // uint32_t crc = 0;
#if 0
    if(NULL==pkt||NULL==dst){
        PRINT_CHK_FLAG;
        return -1;
    }
#endif
    //Get the current timestamp of the computer.
    struct timeval cur_timestamp;
    ret = gettimeofday(&cur_timestamp, (struct timezone *) 0);

    //Assembly the snd pkt
    memset(pkt, 0, sizeof(Udp_Probe_Pkt));

    pkt->hdr.pkt_idx = pkt_idx;
    pkt->hdr.pkt_len = sizeof(Tcp_Probe_Pkt);
    pkt->hdr.chirp_len = chirp_len;
    pkt->hdr.snd_time_sec =  ((uint32_t) cur_timestamp.tv_sec);
    pkt->hdr.snd_time_usec = ((uint32_t) cur_timestamp.tv_usec);
    memset(&(pkt->payload), 'Z', PKT_PAYLOAD_SIZE);

    	   //Sending a pkt
    int cc = 0;
	  //  cc = sendto(sd, (char *)pkt, sizeof(Tcp_Probe_Pkt), 0,
	  //  (struct sockaddr *) dst, sizeof(struct sockaddr_in));
     cc = write(sd, pkt,  sizeof(Tcp_Probe_Pkt));
    if (cc<0){
        fprintf(stderr, "Sending udp: Packet idx: %d of Chirp idx %d.\n", pkt_idx, chirp_len);
   //     return -1;
    }else{
     //   return 1;
    }






}

void send_chirp(int sd)
{
    int ret = 0;
    double max_send_interval = 0;
    max_send_interval = BYTETOBIT( (double)sizeof(Tcp_Probe_Pkt) ) / (LOWEST_BITRATE * 1.0E6);
    double sleep_time = 0;
    sleep_time = max_send_interval * SPREAD_FACTOR;
    int pkt_num = 0;
    pkt_num = compute_probe_pkt_sum();
    Tcp_Probe_Pkt snd_pkt;
    memset(&snd_pkt, 0, sizeof(Tcp_Probe_Pkt));
    send_tcp_pkt(&snd_pkt, 0, pkt_num, sd);
    sleep(1);
	int i;
    for( i=1; i<= pkt_num; i++){
         send_tcp_pkt(&snd_pkt, i, pkt_num, sd);
        usleep( (int)(sleep_time*1.0E6) );
        sleep_time = sleep_time / SPREAD_FACTOR;
       // print_udp_pkt(&snd_pkt);
    }
   // return 1;
}








/*******************************.h************************/

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(50001);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    //char sendline[100];
   // sprintf(sendline, "Hello, world!");
   // generateData();
   // write(sockfd, sendline, strlen(sendline));

   send_chirp(sockfd);




    close(sockfd);

    return 1;
}
