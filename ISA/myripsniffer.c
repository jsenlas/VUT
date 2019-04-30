#include "myripsniffer.h"
/*
** Callback function for pcap_loop
** Processes packets and prints them
** make better - create separate functions for printing
*/
void process_packets(
    u_char *args, 
    const struct pcap_pkthdr *header, 
    const u_char *packet 
    )
{
    struct ip *my_ip;               // pointer to the beginning of IP header
    struct ip6_hdr *my_ip6;
    struct ether_header *eptr;      // pointer to the beginning of Ethernet header    
    const struct udphdr *udp_hdr;   // pointer to the beginning of UDP header
    u_int size_ip4;
    u_int size_ip6;
    int rip_hdr_len;
    struct riphdr *rip_hdr;
    struct ripentry *rip_entry;
    struct rip6entry *rip6_entry;
    int rest_of_packet;
    
    char next_hop_buf[INET6_ADDRSTRLEN];

    printf("#%d at %s", ++counter, ctime((const time_t*)&header->ts.tv_sec));
    //printf("\tLength %d, received at %s",header->len,);  
    //    printf("Ethernet address length is %d\n",ETHER_HDR_LEN);
  
    // read the Ethernet header
    eptr = (struct ether_header *) packet;

    // determine whether it is IPv4 or IPv6
    /***************************************************\
    |*****************      IPv4      ******************|
    \***************************************************/
    if (ntohs(eptr->ether_type) == ETHERTYPE_IP) {        
        my_ip = (struct ip*) (packet+SIZE_ETHERNET);        // skip Ethernet header
        size_ip4 = my_ip->ip_hl*4;                           // length of IP header    
        udp_hdr = (struct udphdr *) (packet+SIZE_ETHERNET+size_ip4); // pointer to the UDP header /// prevzate z sniffer.c
        
        printf("IP src = %s at port %d\n",inet_ntoa(my_ip->ip_src), ntohs(udp_hdr->uh_dport));
        
        /***********************    HEADER    ***********************/        
        //printf("%d\n", ntohs(udp_hdr->len));
        rip_hdr_len = ntohs(udp_hdr->len) - UDP_HDR_LEN;
        if (rip_hdr_len < RIP_HDR_LEN){
            fprintf(stderr, "RIP packet of wrong length.\n");
            return;
        }
        rip_hdr = (struct riphdr *) (packet+SIZE_ETHERNET+size_ip4+UDP_HDR_LEN);
        if ((int)rip_hdr->rip_vers == RIP_V1)
        {
            printf("%s\t", "RIPv1");
        }
        else if ((int)rip_hdr->rip_vers == RIP_V2)
        {
            printf("%s\t", "RIPv2");
        }
        else {
            printf("%s\t", "RIPng");   
        }


        if ((int) rip_hdr->rip_cmd == REQUEST)
        {
            printf("%s command\n", "Request");
        }
        else if ((int) rip_hdr->rip_cmd == RESPONSE)
        {
            printf("%s command\n", "Response");   
        }
        else {
            printf("Unknown command\n");
            //return;
        }
        /***********************    ENTRIES    ***********************/
        rip_entry = (ripentry *) (packet+SIZE_ETHERNET+size_ip4+UDP_HDR_LEN+RIP_HDR_LEN); // posun dalej
        rest_of_packet = rip_hdr_len - RIP_HDR_LEN; // velkost zvysku paketu

        while(rest_of_packet >= ENTRY_LEN){     // tocim kym nedojedem do konca paketu            
            if (rip_entry->addr_fam_id == 0xFFFF) // detekujem autentifikaciu
            {
                char passwd[AUTH_LEN+1];

                memcpy(passwd, rip_entry->body.auth, AUTH_LEN);
                printf("Auth. passwd: %s\n", passwd);
            }
            else {  // klasicka entry
                printf("AddressFId: \t%d\n", ntohs(rip_entry->addr_fam_id));
                printf("Route tag: \t%d\n", ntohs(rip_entry->route_tag));                            
                if ((int) rip_hdr->rip_cmd == RESPONSE)
                {
                    if (ntohs(rip_entry->addr_fam_id) == AF_INET){
                        printf("Address: \t%s\n", inet_ntoa(rip_entry->body.addr));    
                    }
                    if ((int)rip_hdr->rip_vers == RIP_V2)
                    {
                        printf("Mask: \t\t%s\n", inet_ntoa(rip_entry->body.mask));
                        printf("Next-hop: \t%s\n", inet_ntoa(rip_entry->body.nhop));
                    }
                }
                printf("Metric: \t%d\n", (int)ntohl(rip_entry->body.metric));
            }
            rest_of_packet -= ENTRY_LEN;
            rip_entry = (ripentry *) ((u_char *) rip_entry + ENTRY_LEN);
            printf("----------------------------------\n");
        }
    }
    /***************************************************\
    |*****************      IPv6      ******************|
    \***************************************************/
    else if (ntohs(eptr->ether_type) == ETHERTYPE_IPV6) {  

        my_ip6 = (struct ip6_hdr*) (packet+SIZE_ETHERNET);        // skip Ethernet header
        size_ip6 = 40;                           // length of IP header    

        struct in6_addr src = my_ip6->ip6_src;

        udp_hdr = (struct udphdr *) (packet+SIZE_ETHERNET+size_ip6); // pointer to the UDP header /// prevzate z sniffer.c
        char ipv6buf[INET6_ADDRSTRLEN];

        inet_ntop(AF_INET6, &src, ipv6buf, INET6_ADDRSTRLEN);
        
        printf("IP src = %s at port %d\n",ipv6buf, ntohs(udp_hdr->uh_dport));

        /***********************    HEADER    ***********************/       

        printf("%s\t", "RIPng");        
        
        rip_hdr = (struct riphdr *) (packet+SIZE_ETHERNET+size_ip6+UDP_HDR_LEN);        

        if ((int) rip_hdr->rip_cmd == REQUEST)
        {
            printf("%s command\n", "Request");
        }
        else if ((int) rip_hdr->rip_cmd == RESPONSE)
        {
            printf("%s command\n", "Response");   
        }
        else {
            printf("Unknown command\n");
            //return;
        }
        /***********************    ENTRIES    ***********************/
        rip6_entry = (rip6entry *) (packet+SIZE_ETHERNET+size_ip6+UDP_HDR_LEN+RIP_HDR_LEN); // posun dalej
        rip_hdr_len = ntohs(udp_hdr->len) - UDP_HDR_LEN;
        if (rip_hdr_len < RIP_HDR_LEN){
            fprintf(stderr, "RIP packet of wrong length.\n");
            return;
        }
        rest_of_packet = rip_hdr_len - RIP_HDR_LEN;
        while(rest_of_packet >= ENTRY6_LEN){     // tocim kym nedojedem do konca paketu            
            if (rip6_entry->metric == 0xFF) // detekujem NEXTHOP
            {
                inet_ntop(AF_INET6, &rip6_entry->prefix, next_hop_buf, INET6_ADDRSTRLEN);
                printf("Next-hop: %s\n", next_hop_buf);
            }
            else {  // klasicka entry                  
                char addrstr[INET6_ADDRSTRLEN];        
                inet_ntop(AF_INET6, &rip6_entry->prefix, addrstr, INET6_ADDRSTRLEN);        
                printf("Prefix: \t%s\n", addrstr);
                printf("Route tag: \t%d\n", (int)(rip6_entry->tag));
                printf("Prefix length: \t%d\n", (int)(rip6_entry->prefix_len));
                printf("Metric: \t%d\n", (int)(rip6_entry->metric));
            }
            rest_of_packet -= ENTRY6_LEN;
            rip6_entry = (rip6entry *) ((u_char *) rip6_entry + ENTRY6_LEN);
            printf("----------------------------------\n");
        }        
    }
    else {
        printf("\tEthernet type 0x%x, not IPv4 nor IPv6\n", ntohs(eptr->ether_type));        
    } 
    //getchar();
    return;
}

/*
** Prepares everything for sniffing sets interface, filter and loops till end of time
** make better - uncomment testing and use a flag in condition or add an argument
*/
int sniff(char *device, char *errbuf){ 
    pcap_t *deviceHandle = NULL;
    struct bpf_program fp;
    char filter_exp[] = "portrange 520-521 and udp";//port 521 and udp";

    bpf_u_int32 mask;   // Netmask of sniffing address
    bpf_u_int32 net;    // Sniffing IP address


    ////TESTING onf files.pcap
    //if (strcmp(device, ".pcap") != NULL) {
    //    deviceHandle = pcap_open_offline(device, errbuf);
    //    if (deviceHandle == NULL)
    //    {
    //        fprintf(stderr, "Couldn't open device %s.\n%s\n", device, errbuf);
    //        return EXIT_FAILURE;
    //    }
    //}
    //else {
        if (pcap_lookupnet(device, &net, &mask, errbuf) == -1) {
            fprintf(stderr, "Can't get netmask for device %s\n", device);
            net = 0;
            mask = 0;
            return EXIT_FAILURE;
        }
    
        deviceHandle = pcap_open_live(device, SNAPLEN, PROMISCUOUS, WAITTOMS, errbuf);
    
        if (deviceHandle == NULL)
        {
            fprintf(stderr, "Couldn't open device %s.\n%s\n", device, errbuf);
            return EXIT_FAILURE;
        }
    
        if (pcap_compile(deviceHandle, &fp, filter_exp, NOOPTIMIZE, mask))
        {
            return EXIT_FAILURE;         
        } 
    
        if (pcap_setfilter(deviceHandle, &fp) == -1) {
            fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(deviceHandle));
            return EXIT_FAILURE;
        }
    //}

    printf("Sniffing on device %s\n", device);

    if (pcap_loop(deviceHandle, -1, process_packets, NULL) == -1){
        fprintf(stderr, "pcap_loop error\n");
        return EXIT_FAILURE;
    }

    pcap_close(deviceHandle);

    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    char errbuf[PCAP_ERRBUF_SIZE];
    char *device = NULL;
    int opt;

    if (argc == 1)
    {
        device = pcap_lookupdev(errbuf);
        if (device == NULL){
            fprintf(stderr, "There is no divice to choose from.\nERROR: %s\n", errbuf);
            exit(1);
        }
    }    
    else {        
        // Spracovanie parametrov
        while ((opt = getopt(argc, argv, "i:")) != -1){
            switch (opt) {
                case 'i':
                    device = optarg; // interface
                    break;
                case '?':
                    break;
            } // SWITCH
        } // WHICH
    }

    if (device == NULL)
    {
        printf("./myripsniffer -i <interface>\n");
        printf("-i <interface> interface to listen on.\n");
        return ERR_PARAM_HANDLER;
    } 

    printf("Starting sniffer on %s.\n", device);
    return sniff(device, errbuf);
}

/* Through blood and pain, hopefully done. Peace ☯ ☮ (Y) */