#include "path_chirp.h"

int main(int argc,char* argv[])
{
    int ret = 0;
    
    int func_flag = 0;

    int pkt_sum = 0;
    pkt_sum = compute_probe_pkt_sum();

    printf("PKT NUM:%d\n", pkt_sum);
   printf("Input func_flag(0:send; 1:recv):");
    scanf("%d", &func_flag);
/*
	if(argc!=2)
{   
	printf("error!please input: ./%s <args>      0:send,1:receive.",argv[0]);
	exit(0);
}
*/
//Sending.
    if (0==func_flag){
        printf("Sending.\n");

        int sd = 0;
        struct sockaddr_in dst;
        
        ret = init_send_udp_socket(&sd, &dst);

        ret = send_chirp(sd, &dst);

        ret = close_socket(sd);
    }

    //Receiving.
    if(1==func_flag){
        printf("Receiving.\n");
        int sd = 0;
        struct sockaddr_in dst;
        
        ret = init_recv_udp_socket(&sd, &dst);

        Udp_Probe_Pkt *pkt_array = NULL;

        //malloc inside recv_chirp
        ret = recv_chirp(sd,(  Udp_Probe_Pkt **) &pkt_array);

        //print 
        if(NULL!=pkt_array){
            int pkt_sum = pkt_array->hdr.chirp_len;
            for(int i=0; i<pkt_sum; i++){
                print_udp_pkt(pkt_array+i);
            }
        }
/*
        if(NULL!=pkt_array){
            free(pkt_array);
            pkt_array = NULL;
        }
*/

        close_socket(sd);
#if 0	
		for(int i=0;i<pkt_array->hdr.chirp_len;i++)
		{
	
           printf("idx:%d snd_time_sec:\t %u\n",pkt_array[i].hdr.pkt_idx, pkt_array[i].hdr.snd_time_sec);

		   printf("idx:%d snd_time_usec:\t %u\n",pkt_array[i].hdr.pkt_idx, pkt_array[i].hdr.snd_time_usec);

		   printf("idx:%d rcv_time_sec:\t %u\n",pkt_array[i].hdr.pkt_idx, pkt_array[i].hdr.rcv_time_sec);

		   printf("idx:%d rcv_time_usec:\t %u\n", pkt_array[i].hdr.pkt_idx,pkt_array[i].hdr.rcv_time_usec);
		}
#endif
		double*pDelay=0;
	    Compute_chirp_queue_delay(pkt_array,&pDelay);
		printf("\n");
#if 0
		for(int i=0;i<25;i++)  printf(" %f ",pDelay[i]);
#endif
		double*pRate=0;
		Compute_chirp_queue_transmit(pkt_array,pDelay,&pRate);
double estimate	=	EstimateBand(pkt_array,pDelay,pRate);
printf("\n estimate=%fbps\n",estimate);



    }
  
}

