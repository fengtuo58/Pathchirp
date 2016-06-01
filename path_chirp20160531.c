/*
 * =====================================================================================
 *
 *       Filename:  path_chirp.c
 *
 *    Description:  pathChirp, a new active probing tool to estimate the avaiable band-
 *                  width on a communication network path.
 *        Version:  0.1
 *        Created:  5/1/2016 
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */
#include "path_chirp.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_send_udp_socket(int *sd, struct sockaddr_in *dst)
 *  Description:  
 * =====================================================================================
 */

int init_send_udp_socket(int *sd, struct sockaddr_in *dst)
{
    int ret = 0;

    if(NULL==dst||NULL==sd){
        PRINT_CHK_FLAG;
        return -1;
    }

    *sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(*sd < 0){
        perror("Init udp socket: socket().\n" );
        return -1;
    }

    dst->sin_family = AF_INET;
    dst->sin_port = htons(RECV_PORT);

    ret = inet_aton(RECV_IP, &(dst->sin_addr));
    if(ret == 0){
        perror("Init udp socket: inet_aton()\n" );
        return -1;
    }

    return 1;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_recv_udp_socket()
 *  Description:  
 * =====================================================================================
 */
int init_recv_udp_socket(int *sd, struct sockaddr_in *dst)
{
    int ret = 0;

    if(NULL==dst||NULL==sd){
        PRINT_CHK_FLAG;
        return -1;
    }

    *sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(*sd < 0){
        perror("Init udp socket: socket().\n" );
        return -1;
    }

    dst->sin_family = AF_INET;
    dst->sin_port = htons(RECV_PORT);
    dst->sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(*sd, (struct sockaddr *)dst, sizeof(struct sockaddr_in));
    if(-1==ret){
        perror("Init recv_udp_socket: bind().\n");
        return -1;
    }

    return 1;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  send_udp_pkt()
 *  Description:  
 * =====================================================================================
 */
int send_udp_pkt(Udp_Probe_Pkt *pkt, uint32_t pkt_idx, uint32_t chirp_len, 
                 int sd, struct sockaddr_in *dst)
{
    int ret = 0;

   // uint32_t crc = 0;

    if(NULL==pkt||NULL==dst){
        PRINT_CHK_FLAG;
        return -1;
    }

    //Get the current timestamp of the computer.
    struct timeval cur_timestamp;
    ret = gettimeofday(&cur_timestamp, (struct timezone *) 0);

    //Assembly the snd pkt
    memset(pkt, 0, sizeof(Udp_Probe_Pkt));

    pkt->hdr.pkt_idx = pkt_idx;
    pkt->hdr.pkt_len = sizeof(Udp_Probe_Pkt);
    pkt->hdr.chirp_len = chirp_len;
    pkt->hdr.snd_time_sec =  ((uint32_t) cur_timestamp.tv_sec);
    pkt->hdr.snd_time_usec = ((uint32_t) cur_timestamp.tv_usec);
    memset(&(pkt->payload), 'Z', PKT_PAYLOAD_SIZE);

    //Sending a pkt
    int cc = 0;
    cc = sendto(sd, (char *)pkt, sizeof(Udp_Probe_Pkt), 0,
            (struct sockaddr *) dst, sizeof(struct sockaddr_in));
    if (cc<0){
        fprintf(stderr, "Sending udp: Packet idx: %d of Chirp idx %d.\n", pkt_idx, chirp_len);
        return -1;
    }else{
        return 1;
    }
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  recv_udp_pkt()
 *  Description:  
 * =====================================================================================
 */
int recv_udp_pkt(int sd, Udp_Probe_Pkt *pkt)
{
    int ret = 0;

    char buff[MAX_PKT_SIZE];
    struct sockaddr_in src;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    if(sd<0||NULL==pkt){
        PRINT_CHK_FLAG;
        return -1;
    }

    memset(buff, 0, MAX_PKT_SIZE);
  //  ret = recvfrom(sd, buff, MAX_PKT_SIZE-1, 0, (struct sockaddr*)&src, &addrlen);
   ret = recvfrom(sd, pkt, sizeof(Udp_Probe_Pkt), 0, (struct sockaddr*)&src, &addrlen);

    if(ret<-1){
        perror("recv_udp_pkt():recvfrom().\n");
        return -1;
    }
    else{
        struct timeval cur_timestamp;
        ret = gettimeofday(&cur_timestamp, (struct timezone *) 0);
	
	
	                                         	//interface time
	//	memset(pkt, 0, sizeof(Udp_Probe_Pkt));
	 // memcpy(pkt, buff, sizeof(Udp_Probe_Pkt));
//printf("in recv_udp_pkt ,%ld, %ld", )
        pkt->hdr.rcv_time_sec =  ((uint32_t) cur_timestamp.tv_sec);
        pkt->hdr.rcv_time_usec = ((uint32_t) cur_timestamp.tv_usec);
    }

    return 1;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  int close_socket(sd)
 *  Description:  
 * =====================================================================================
 */
int close_socket(int sd)
{
    if(sd>0){
        close(sd);
        return 1;
    }
    else{
        return -1;
    }
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  print_udp_pkt()
 *  Description:  
 * =====================================================================================
 */
int print_udp_pkt(Udp_Probe_Pkt *pkt)
{
    if(NULL==pkt){
        PRINT_CHK_FLAG;
        perror("The pointer of pkt is NULL.");
        return -1;
    }
    else{
        printf("\n-----------------------\n");
        printf("pkt_idx:\t %d\n", pkt->hdr.pkt_idx);
        printf("chirp_len:\t %d\n", pkt->hdr.chirp_len);
        printf("pkt_len:\t %d\n", pkt->hdr.pkt_len);
        printf("snd_time_sec:\t %u\n", pkt->hdr.snd_time_sec);
        printf("snd_time_usec:\t %u\n", pkt->hdr.snd_time_usec);
        printf("rcv_time_sec:\t %u\n", pkt->hdr.rcv_time_sec);
        printf("rcv_time_usec:\t %u\n", pkt->hdr.rcv_time_usec);
        return 1;
    }
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  compute_probe_pkt_sum()
 *  Description:  
 * =====================================================================================
 */
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

/*
 * ===  FUNCTION  ======================================================================
 *         Name: send_chirp()
 *  Description:
 * =====================================================================================
 */
int send_chirp(int sd, struct sockaddr_in * dst)
{
    int ret = 0;

    double max_send_interval = 0;
    max_send_interval = BYTETOBIT( (double)sizeof(Udp_Probe_Pkt) ) / (LOWEST_BITRATE * 1.0E6);

    double sleep_time = 0;
    sleep_time = max_send_interval * SPREAD_FACTOR;

    int pkt_num = 0;
    pkt_num = compute_probe_pkt_sum();

    Udp_Probe_Pkt snd_pkt;
    memset(&snd_pkt, 0, sizeof(Udp_Probe_Pkt));

    //Send the start message to the receiver and wait 1 sec
    //so that the receiver can allocate memeory to hold
    //the following packets.
    ret = send_udp_pkt(&snd_pkt, 0, pkt_num, sd, dst);
    sleep(1);

    for(int i=1; i<= pkt_num; i++){
        ret = send_udp_pkt(&snd_pkt, i, pkt_num, sd, dst);
        usleep(int(sleep_time*1.0E6));
        sleep_time = sleep_time / SPREAD_FACTOR;
        print_udp_pkt(&snd_pkt);
    }

    return 1;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  int recv_chirp()
 *  Description:  
 * =====================================================================================
 */
int recv_chirp(int sd, Udp_Probe_Pkt ** ppkt_array)
{
    int ret = 0;
    
    //waiting for start msg from the sender.
    int chirp_len = 0;
    int pkt_size = sizeof(Udp_Probe_Pkt);

    Udp_Probe_Pkt rcv_pkt;
    Udp_Probe_Pkt *p = NULL;
//printf("p=%ld", p);
    while(1){

        ret = recv_udp_pkt(sd, &rcv_pkt);
        if(0==rcv_pkt.hdr.pkt_idx 
                && MAX_CHIRP_LEN>rcv_pkt.hdr.chirp_len 
                && 1< rcv_pkt.hdr.chirp_len){
            chirp_len = rcv_pkt.hdr.chirp_len;
            p = (Udp_Probe_Pkt*)malloc((chirp_len+1) * pkt_size);

            if (NULL==p){
                perror("malloc.");
                PRINT_CHK_FLAG;
                return -1;
            }
            else{
                memset(p, 0, (chirp_len+1)* pkt_size);
        //        PRINT_CHK_FLAG;
                break;
            }
                
        }
		else 
		{
			printf("\n%ld,%ld\n", rcv_pkt.hdr.pkt_idx, rcv_pkt.hdr.chirp_len);
		return -1;
		}
	}
		
     for(int i=0;i<chirp_len;i++) 
	 {
	    // ret=recv_udp_pkt(sd,&rcv_pkt);//fengutuo2016530
    	//memcpy((void*)(p+i),(void*)&rcv_pkt,pkt_size);//fengtuo2016530
       ret=recv_udp_pkt(sd, (p+i));

	 }

	 *ppkt_array=p;
//	 printf("*ppkt_array=%d ", *ppkt_array); 
//	 char s;
//	 scanf("%c",s);
//	 printf("\n*ppkt_array= %lf\n", *ppkt_array);
	 return 1;
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

        cur_pkt_snd_time =  (double) ((p+i)->hdr.snd_time_sec) + (double)((p+i)->hdr.snd_time_usec)/1.0E6;
        next_pkt_snd_time =  (double) ((p+i+1)->hdr.snd_time_sec) + (double)((p+i+1)->hdr.snd_time_usec)/1.0E6;


        cur_pkt_rcv_time =  (double) ((p+i)->hdr.rcv_time_sec) + (double)((p+i)->hdr.rcv_time_usec)/1.0E6;
        next_pkt_rcv_time =  (double) ((p+1+i)->hdr.rcv_time_sec) + (double)((p+1+i)->hdr.rcv_time_usec)/1.0E6;

        delta_snd = next_pkt_snd_time - cur_pkt_snd_time;
        delta_rcv = next_pkt_rcv_time - cur_pkt_rcv_time;

        *(pkt_array_delay+i) = delta_rcv - delta_snd;
//			printf("\n delta_delay :%lf\n ", *(pkt_array_delay+i));
    }
  // pdelay=pkt_array_delay;
 *pPdelay=pkt_array_delay;

    return 1;
}







int Compute_chirp_queue_transmit(Udp_Probe_Pkt *pkt_array,double*pkt_array_delay,double**p)
{
  
    int chirp_len = 0;
    chirp_len = pkt_array->hdr.chirp_len;

    double *pkt_array_transmit = NULL;
    pkt_array_transmit =(double*) malloc(chirp_len * sizeof(double));
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
	for(index=0;index <= chirp_len-2;index++)
	{
         R[index]=BYTETOBIT(sizeof(Udp_Probe_Pkt)) / ( (double) ( (pkt_array+index+1)->hdr.snd_time_sec ) + (double)((pkt_array+index+1)->hdr.snd_time_usec)/1.0E6-  ( (double) ( (pkt_array+index)->hdr.snd_time_sec ) + (double)( (pkt_array+index)->hdr.snd_time_usec)/1.0E6 ));
//         printf("\n,[%d]snd_time_usec=%ld,[%d] snd_time_usec=%ld Rate=%lf\n", index, (pkt_array+index)->hdr.snd_time_usec, index+1, (pkt_array+index+1)->hdr.snd_time_usec ,  R[index]);
	}


    //	R[index]=sizeof(Udp_Probe_Pkt)/pkt_array_delay[index];
	return 1;

	}



int excursion(Udp_Probe_Pkt *pkt_array,int i,double *pkt_array_delay/*record the delay time arrays*/)
{
	int j=i+1;
	double max_q=0;
	while(( j<= pkt_array->hdr.chirp_len-2) && ( (pkt_array_delay[j]-pkt_array_delay[i]) > max_q/F ) )
	{
		max_q=( max_q > (pkt_array_delay[j]-pkt_array_delay[i]) )? max_q:pkt_array_delay[j]-pkt_array_delay[i];
		j++;
	}
	if(j >= pkt_array->hdr.chirp_len-2) return j;
	if(j-i>=L) 
		return j;
	else 
		return i;

}
double EstimateBand(Udp_Probe_Pkt *pkt_array,double *pkt_array_delay,double *pkt_array_Rate)
{
	int s;
 double E[pkt_array->hdr.chirp_len-1];
 memset(E,0,pkt_array->hdr.chirp_len-1);
 int i=1,ChirpLength=pkt_array->hdr.chirp_len;
 int endIndex,l = ChirpLength-2;
 while (i <= ChirpLength-3)
 {
if(pkt_array_delay[i] < pkt_array_delay[i+1]) {
   endIndex=excursion(pkt_array,i,pkt_array_delay);
   if( endIndex>i && endIndex <=ChirpLength-2 )
   {
	   for (s=i;s<endIndex-1;s++)
	   {
		   if ( pkt_array_delay[s] < pkt_array_delay[s+1] ) E[s]=pkt_array_Rate[s]; 
			  printf("E[%d]:%lf", s, E[s]); 
	   }
    }

   if (endIndex==ChirpLength-1)
   {
		   for ( s=i;s<ChirpLength;s++)
		   {
			   E[s-1]=pkt_array_Rate[i];
			   printf("E[%d]:%lf", s, E[s]); 
		   }
		   l=i;	  
   }
   if(endIndex == i) {endIndex++;}
	i=endIndex;
 }
else
	i++;
 }
#if 0
 for(int j = 0; j < pkt_array->hdr.chirp_len-1; j++ )
   printf("E[%d]:%lf", j,  E[j]);
printf("\n");
#endif
 double sumDelay=0;
 double D=0,deltak=0;
 for(s = 0; s <= ChirpLength-2; s++)
 {
	 if(E[s] == 0)
 {  //D+=pkt_array_Rate[l]*pkt_array_delay[s];sumDelay+=pkt_array_delay[s];
 deltak=  (double) ( (pkt_array+s+1)->hdr.snd_time_sec ) + (double)((pkt_array+s+1)->hdr.snd_time_usec)/1.0E6-  ( (double) ( (pkt_array+s)->hdr.snd_time_sec ) + (double)( (pkt_array+s)->hdr.snd_time_usec)/1.0E6 );
				 D+=pkt_array_Rate[l]*deltak;
				sumDelay+=deltak;
      			printf("E[%d]= %lf", s, deltak*pkt_array_Rate[l]);

}
 else
 {	//D+=E[s]*pkt_array_delay[s];sumDelay+=pkt_array_delay[s]; }

deltak=  (double) ( (pkt_array+s+1)->hdr.snd_time_sec ) + (double)((pkt_array+s+1)->hdr.snd_time_usec)/1.0E6-  ( (double) ( (pkt_array+s)->hdr.snd_time_sec ) + (double)( (pkt_array+s)->hdr.snd_time_usec)/1.0E6 );
				 D+=E[s]*deltak;
				 sumDelay+=deltak;
//	printf("E[s]==0 deta= %lf", deltak);


 }
}

 D = D / sumDelay;

printf("sumDely=%lf,D=%lf",sumDelay,D);
 return D;
}








