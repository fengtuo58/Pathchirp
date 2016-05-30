#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
//#include <netinet/in.h>

#include<sys/time.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
/*****************************************************************/

#define  RECV_IP        "172.16.187.254"
#define  RECV_PORT      8080

/*-----------------------------------------------------------------------------
 *  Chirp Parameters Setup
 *-----------------------------------------------------------------------------*/
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
#define  F  1.5
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











/*****************************************************************/
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


int Compute_chirp_queue_delay(Udp_Probe_Pkt* pkt_array,double**pPdelay)
	{
    int ret = 0;

    //check input
    if(NULL==pkt_array){
        perror("pkt_array pointer is NULL");
        return -1;
    }


	int chirp_len = 0;
    chirp_len = pkt_array->hdr.chirp_len;
    double *pkt_array_delay = NULL;
    pkt_array_delay =(double*) malloc(chirp_len * sizeof(double));
			    if(NULL==pkt_array_delay)
				{
	            perror("malloc: pkt_array_delay");
			    PRINT_CHK_FLAG;
			    return -1;
				 }
			    else
				{
	memset(pkt_array_delay, 0, chirp_len * sizeof(double));
				}
    double cur_pkt_snd_time = 0.0;
    double cur_pkt_rcv_time = 0.0;
 
 	double next_pkt_snd_time = 0.0;
    double next_pkt_rcv_time = 0.0;

   	double delta_snd = 0.0;
    double delta_rcv = 0.0;
    Udp_Probe_Pkt *p = pkt_array;
	int i;
    for(i=0;i<chirp_len-1;i++){
//	printf("\nsnd sec:%d,snd usec:%d,nxt snd sec :%d, nxt usec:%d\n", p->hdr.snd_time_sec, p->hdr.snd_time_usec, (p+1)->hdr.snd_time_sec, (p+1)->hdr.snd_time_usec); 
    	cur_pkt_snd_time =  (double) (p->hdr.snd_time_sec) + (double)(p->hdr.snd_time_usec)/1.0E6;
        next_pkt_snd_time =  (double) ((p+1)->hdr.snd_time_sec) + (double)((p+1)->hdr.snd_time_usec)/1.0E6;

//i=endIndex;}/printf("\nrcv sec:%d,rcv usec:%d,nxt rcv sec :%d, nxtrcv usec:%d\n", p->hdr.rcv_time_sec, p->hdr.rcv_time_usec, (p+1)->hdr.rcv_time_sec, (p+1)->hdr.rcv_time_usec) ;

#if  1
        cur_pkt_rcv_time =  (double) (p->hdr.rcv_time_sec) + (double)(p->hdr.rcv_time_usec)/1.0E6;
        next_pkt_rcv_time =  (double) ((p+1)->hdr.rcv_time_sec) + (double)((p+1)->hdr.rcv_time_usec)/1.0E6;

        delta_snd = next_pkt_snd_time - cur_pkt_snd_time;
        delta_rcv = next_pkt_rcv_time - cur_pkt_rcv_time;
#endif
	
//printf("\n %lf, %lf \n", delta_snd, delta_rcv);
	
//	printf("\ndelt=%lf\n", delta_rcv -delta_snd  );
    pkt_array_delay[i]= delta_rcv - delta_snd;
//	printf("\nd=%lf\n ", pkt_array_delay[i]);
		p++;
    }
  // pdelay=pkt_array_delay;
	 *pPdelay=pkt_array_delay;
#if 0
 printf("\n In delay");
  int  k;
 for(k=0; k<=24; k++) { printf("\n %lf  ", pkt_array_delay[k]); printf(" %lf\n  ",(*pPdelay)[k]);   }
   printf("Indelay\n");
#endif
    return 1;
}







int Compute_chirp_queue_transmit(Udp_Probe_Pkt *pkt_array,double*pkt_array_delay,double**p)
{
  
    int chirp_len = 0;
    chirp_len = pkt_array->hdr.chirp_len;

    double *pkt_array_transmit = NULL;
    pkt_array_transmit =(double*) malloc((chirp_len-1) * sizeof(double));
    if(NULL==pkt_array_transmit){
        perror("malloc: pkt_array_transmit");
        PRINT_CHK_FLAG;
        return -1;
    }
    else{
        memset(pkt_array_transmit, 0, chirp_len * sizeof(double));
    }
	int index;
	double*R=pkt_array_transmit;
    *p=R;
	for(index=0; index <= chirp_len-2; index++)
	{
	//	printf("Udp_Probe_Pkt=%d", sizeof(Udp_Probe_Pkt));
	R[index]=BYTETOBIT(sizeof(Udp_Probe_Pkt)) / ( (double) ( (pkt_array+index+1)->hdr.snd_time_sec ) + (double)((pkt_array+index+1)->hdr.snd_time_usec)/1.0E6-  ( (double) ( (pkt_array+index)->hdr.snd_time_sec ) + (double)( (pkt_array+index)->hdr.snd_time_usec)/1.0E6 ));
	printf("\n,[%d]snd_time_usec=%ld,[%d] snd_time_usec=%ld Rate=%lf\n", index, (pkt_array+index)->hdr.snd_time_usec, index+1, (pkt_array+index+1)->hdr.snd_time_usec ,  R[index]);
	}
	return 1;

}

int excursion(Udp_Probe_Pkt *pkt_array,int i,double *pkt_array_delay/*record the delay time arrays*/)
{
	int j=i+1;
	double max_q=0;
	while((j<= pkt_array->hdr.chirp_len-2) && ((pkt_array_delay[j] - pkt_array_delay[i]) > max_q/F)) 
	{
		max_q=( max_q > pkt_array_delay[j]-pkt_array_delay[i] )? max_q : (pkt_array_delay[j]-pkt_array_delay[i]);
		j++;
	}
//	printf("\n.j\n") ;

	if(j >= pkt_array->hdr.chirp_len-2) return j;
	if(j-i >= L) 
		return j;
	else 
		return i;

}
double EstimateBand(Udp_Probe_Pkt *pkt_array,double *pkt_array_delay,double *pkt_array_Rate)
{
    	int s;
	 double E[pkt_array->hdr.chirp_len-1];
	 memset(E,0,pkt_array->hdr.chirp_len-1);
	 int i=0,ChirpLength=pkt_array->hdr.chirp_len;
	 int endIndex,l=ChirpLength-2;
 //printf("\n.\n") ;

	 while (i<=ChirpLength-3)
	{
		if(pkt_array_delay[i] < pkt_array_delay[i+1]) 
		{	
//printf("\n%d\n", i) ;
			endIndex=excursion(pkt_array, i, pkt_array_delay);
		//	printf("\nendindex=%d\n", endIndex);
//printf("\n.\n") ;
			if( (endIndex > i) && (endIndex <= ChirpLength-2))
			 {	
				for (s=i; s<endIndex-1; s++)
					{	
						if ( pkt_array_delay[s] <= pkt_array_delay[s+1] ) E[s]=pkt_array_Rate[s];  
					}	
			 }			

			 if (endIndex == ChirpLength-1)
			 {	
					for ( s=i; s<ChirpLength-1; s++) E[s]=pkt_array_Rate[i];
				    	l=i;	  
			}
		   if(endIndex==i) {endIndex++;}
		   i=endIndex;
		}		
      else
	      i++;
	 }
//printf("\n.\n") ;
	 double sumDelay=0;
	 double D=0,deltak=0;
//     l=ChirpLength-2;//fengtuo2016530
	 for(s=0; s<ChirpLength-2; s++)
	 {
		 if(E[s]==0) 
		 {
				 deltak=  (double) ( (pkt_array+s+1)->hdr.snd_time_sec ) + (double)((pkt_array+s+1)->hdr.snd_time_usec)/1.0E6-  ( (double) ( (pkt_array+s)->hdr.snd_time_sec ) + (double)( (pkt_array+s)->hdr.snd_time_usec)/1.0E6 );
				 D+=pkt_array_Rate[l]*deltak;
				sumDelay+=deltak;
		}
		 else
		{
				 deltak=  (double) ( (pkt_array+s+1)->hdr.snd_time_sec ) + (double)((pkt_array+s+1)->hdr.snd_time_usec)/1.0E6-  ( (double) ( (pkt_array+s)->hdr.snd_time_sec ) + (double)( (pkt_array+s)->hdr.snd_time_usec)/1.0E6 );
				 D+=E[s]*deltak;
				 sumDelay+=deltak; 

		 }			
	 }
//printf("sumDealy=%d,D=%d",sumDelay,D);
	 D=D/sumDelay;
// printf("D=%f", D);
	 return D;
}


/***************************************************************************************************************/
void recv_tcp_pkt(Tcp_Probe_Pkt *pkt, int sd)
{

    int ret = 0;

    char buff[MAX_PKT_SIZE];
    struct sockaddr_in src;
    socklen_t addrlen = sizeof(struct sockaddr_in);
#if 1
    if(sd<0||NULL==pkt){
    //    PRINT_CHK_FLAG;
        return -1;
    }
#endif
//	printf("\nidx=%d\n", pkt->hdr.pkt_idx);
    memset(buff, 0, MAX_PKT_SIZE);

//   	ret = read(sd, buff, MAX_PKT_SIZE-1);
       memset(pkt, 0, sizeof(Tcp_Probe_Pkt));

    ret= read(sd, buff, sizeof(Tcp_Probe_Pkt));
    if(ret<-1){
        perror("recv_udp_pkt():read().\n");
        return -1;
    }
    else{
        struct timeval cur_timestamp;
        ret = gettimeofday(&cur_timestamp, (struct timezone *) 0);

       // memset(pkt, 0, sizeof(Tcp_Probe_Pkt));
        memcpy(pkt, buff, sizeof(Tcp_Probe_Pkt));

        pkt->hdr.rcv_time_sec =  ((uint32_t) cur_timestamp.tv_sec);
        pkt->hdr.rcv_time_usec = ((uint32_t) cur_timestamp.tv_usec);
#if 1

    	printf(" \n pkt->hdr.idx=%d",(int) pkt->hdr.pkt_idx);
		printf("  pkt->hdr.snd_time_sec=%d",(int) pkt->hdr.snd_time_sec);
		printf("pkt->hdr.snd_time_usec=%d \n",(int) pkt->hdr.snd_time_usec);

		printf(" \n pkt->hdr.rcv_time_sec=%d", (int)pkt->hdr.rcv_time_sec);
		printf("pkt->hdr.rcv_time_usec=%d\n",(int) pkt->hdr.rcv_time_usec);
#endif
    }

    return 1;



}



void receive_chirp(Tcp_Probe_Pkt**ppkt_array,int sd)
{
	int ret = 0;
    
    //waiting for start msg from the sender.
    int chirp_len = 0;
    int pkt_size = sizeof(Tcp_Probe_Pkt);

    Udp_Probe_Pkt rcv_pkt;
    Udp_Probe_Pkt *p = NULL;

    while(1){

				 recv_tcp_pkt(&rcv_pkt, sd);//gai
				printf("%d\n", rcv_pkt.hdr.chirp_len);
				 if(0==rcv_pkt.hdr.pkt_idx 
                && MAX_CHIRP_LEN>rcv_pkt.hdr.chirp_len 
                && 1< rcv_pkt.hdr.chirp_len)
				 {
							chirp_len = rcv_pkt.hdr.chirp_len;
							p = (Udp_Probe_Pkt*)malloc((chirp_len+1) * pkt_size);

							if (NULL==p){
											 perror("malloc.");
												//   PRINT_CHK_FLAG;
											 return -1;
										}
									 else{
												 memset(p, 0, (chirp_len+1)* pkt_size);
												//  PRINT_CHK_FLAG;
												 break;
										}
                
				 }
				else 
				{
					return -1;
				}
	      }
	int i;	
     for( i=0;i<chirp_len;i++) 
	 {
#if 0
		 recv_tcp_pkt(&rcv_pkt,sd);//gai
		 memcpy((void*)(p+i),(void*)&rcv_pkt,pkt_size);
#endif
	 recv_tcp_pkt(p+i,sd);//fengtuo2016526
  //   printf("i=%d", i);
	 }
	 *ppkt_array=p;
	 return 1;
}

int main(int argc, char **argv)
{
    int listenfd;
    int connfd;
    struct sockaddr_in servaddr;

    listenfd = socket(PF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(50001);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, 10);
    connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
    
    int n;
    char recvline[1024];
    Tcp_Probe_Pkt *pPkt;
   receive_chirp(&pPkt,connfd);
  //  int i=0
#if 0   
 while((n=read(connfd, recvline, 1024)) > 0)
    {

      //  recvline[n] = 0;
      //  printf("%s\n", recvline);
    }
#endif
#if 0
for(n=0; n<26;n++)
{

printf("%d", pPkt[n].hdr.pkt_idx);
printf("%f", pPkt[n].hdr.snd_time_sec);
printf("%f", pPkt[n].hdr.snd_time_usec);
printf("%f",  pPkt[n].hdr.rcv_time_sec);
printf("%f",  pPkt[n].hdr.rcv_time_usec);
printf("\n");

}
#endif
double*pPDelay;

Compute_chirp_queue_delay(pPkt, &pPDelay);
#if 0
for(n=0; n<pPkt->hdr.chirp_len-1; n++)
printf(" %f ", pPDelay[n]);
#endif

double* pTransmit;
Compute_chirp_queue_transmit(pPkt, pPDelay, &pTransmit);
double band=EstimateBand(pPkt,pPDelay,pTransmit);
printf("\nband=%f", band);
    close(connfd);
    close(listenfd);
    return 1;
}
