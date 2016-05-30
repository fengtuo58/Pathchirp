/*
 * =====================================================================================
 *
 *       Filename:  path_chrip.h
 *
 *    Description:  pathChirp, a new active probing tool to estimate the avaiable band-
 *                  width on a communication network path.
 *        Version:  0.1
 *        Created:  5/1/2016 
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *
 * =====================================================================================
 */


#ifndef  PATH_CHIRP_H_INC
#define  PATH_CHIRP_H_INC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

/*-----------------------------------------------------------------------------
 *  Network IP Port
 *-----------------------------------------------------------------------------*/
#define  RECV_IP        "127.0.0.1"
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

// ===  Network FUNCTIONS  ======================================================================
int init_send_udp_socket(int *sd, struct sockaddr_in *dst);
int init_recv_udp_socket(int *sd, struct sockaddr_in *dst);
int send_udp_pkt(Udp_Probe_Pkt *pkt, uint32_t pkt_idx, uint32_t chirp_len, int sd, struct sockaddr_in *dst);
int recv_udp_pkt(int sd, Udp_Probe_Pkt *pkt);
int close_socket(int sd);
int print_udp_pkt(Udp_Probe_Pkt *pkt);
int Compute_chirp_queue_delay(Udp_Probe_Pkt* pkt_array,double**p);
int Compute_chirp_queue_transmit(Udp_Probe_Pkt *pkt_array,double*pkt_array_delay,double**p);

// ===  Pathchirp FUNCTIONS  ======================================================================
int compute_probe_pkt_sum(void);
int send_chirp(int sd, struct sockaddr_in * dst);
int recv_chirp(int sd, Udp_Probe_Pkt **p);
int  excursion(Udp_Probe_Pkt *pkt_array,int i,double * pkt_array_delay);
double EstimateBand(Udp_Probe_Pkt*pkt_array,double *pkt_array_delay,double *pkt_array_Rate);
#endif   /* ----- #ifndef PATH_CHIRP_H_INC  ----- */
