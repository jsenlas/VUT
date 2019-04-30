#ifndef MYRIPSNIFFERH
#define MYRIPSNIFFERH

#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pcap.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>
#include <time.h>

#define SIZE_ETHERNET 14
#define UDP_HDR_LEN 8
#define RIP_HDR_LEN 4

#define RIP_V1 1
#define RIP_V2 2
#define REQUEST 1
#define RESPONSE 2
#define ENTRY_LEN 20
#define AUTH_LEN 16
#define ENTRY6_LEN 20

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define ERR_PARAM_HANDLER 8

#define MAXBYTES2CAPTURE 2048 
#define PROMISCUOUS 1
#define WAITTOMS 512
#define SNAPLEN 512
#define NOOPTIMIZE 0

int counter = 0;

typedef struct riphdr 
{
    uint8_t rip_cmd;
    uint8_t rip_vers;
    uint16_t rip_mbz;
} riphdr;

typedef struct ripentry
{
    uint16_t addr_fam_id;
    uint16_t route_tag;
    union body 
    {
        struct
        {
            u_char auth[16];  
        };
        struct
        {
            struct in_addr addr;    // ip address
            struct in_addr mask;    // maska siete
            struct in_addr nhop;    // next hop adresa
            uint32_t metric;        // metrika <16
        };
    } body;
} ripentry;

typedef struct rip6entry
{
    struct in6_addr prefix; // prefix or nexthop address
    uint16_t tag;
    uint8_t prefix_len;    
    uint8_t metric;
} rip6entry;

void process_packets(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
int sniff(char *interface, char *errbuf);


#endif