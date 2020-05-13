#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
//-------------------------------
#include<sys/socket.h>
#include<sys/types.h>

#include<linux/if_packet.h>
#include<netinet/in.h>
#include<netinet/if_ether.h>    // for ethernet header
#include<netinet/ip.h>		// for ip header
#include<netinet/udp.h>		// for udp header
#include<netinet/tcp.h>
#include<arpa/inet.h>           // to avoid warning at inet_ntoa
//-------------------------------------------------------------------
#include<net/if.h>

int main(){
    //Time measurement
    time_t start, end;
    double elapsed;
    start = time(NULL);
    int sock_r;
    //Counters for Packets
    int ipcnt = 0, tcpcnt = 0, udpcnt = 0, icmpcnt = 0, dnscnt = 0, httpcnt = 0, httpscnt = 0, quiccnt = 0;
    //For a raw socket, the socket family is AF_PACKET, the socket type is SOCK_RAW and for the protocol, see the if_ether.h header file. To receive all packets, the macro is ETH_P_ALL
    //To receive all packets, the macro is ETH_P_ALL and to receive IP packets, the macro is ETH_P_IP for the protocol field.
    sock_r = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP)); // Reading only IP Packets
    if(sock_r < 0){
        printf("error in socket\n");
        return 0;
    }

    /*const char *opt;
    opt = "lo";    //"enp0s3";
    const int len = strnlen(opt, IFNAMSIZ);
    if (len == IFNAMSIZ) {
        fprintf(stderr, "Too long iface name");
        return 1;
    }
    printf("Setting Socket Options\n");
    if(setsockopt(sock_r, SOL_SOCKET, SO_BINDTODEVICE, opt, len) < 0){
        printf("Set Socket Failed\n");
    }*/
    
    // Making the recvfrom call non-blocking (to make sure that the program does not run infinitely if no packets are received)
    struct timeval read_timeout;
    read_timeout.tv_sec = 30;
    read_timeout.tv_usec = 0;
    setsockopt(sock_r, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

    struct sockaddr saddr;
    unsigned char* buffer = (unsigned char *)malloc(65536);
    while(1){
        end = time(NULL);
        elapsed = difftime(end, start);
        if(elapsed > 30){
            break;
        }
        memset(buffer, 0, 65536);
        int saddr_len = sizeof(saddr);
        //Copy packet to buffer
        int buflen = recvfrom(sock_r, buffer, 65536, 0, &saddr, (socklen_t *)&saddr_len);
        if(buflen<0){
            printf("error in reading recv function\n");
            return 0;
        }
        //Now, extracting the ethernet frame to find IP count
        struct ethhdr *eth = (struct ethhdr *)buffer;
        if(eth->h_proto == 8){
            ipcnt++;
        }
        //After extracting the Ethenet frame, moving on to extract the Network Layer Datagram to find tcp / udp count
        unsigned short iphdrlen;
        struct iphdr *ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));
        iphdrlen = (ip->ihl)*4;
        // ip is the pointer to the network layer datagram
        if((unsigned int)ip->protocol == 1){
            // it is the famous icmp
            icmpcnt++;
        }
        else if((unsigned int)ip->protocol == 6){
            // it is the famous tcp
            tcpcnt++;
            // After extracting the Ip Packet, now extracting the tcp / udp packet
            struct tcphdr *tcp = (struct tcphdr*)(buffer+iphdrlen+sizeof(struct ethhdr));

            if(ntohs(tcp->source) == 80 || ntohs(tcp->dest) == 80){
                httpcnt++;
            }
            else if(ntohs(tcp->source) == 443 || ntohs(tcp->dest) == 443){
                httpscnt++;
            }
            else if(ntohs(tcp->source) == 53 || ntohs(tcp->dest) == 53){
                dnscnt++; // Here, DNS uses TCP for Zone Transfer
            }
        }
        else if((unsigned int)ip->protocol == 17){
            // it is the famous udp
            udpcnt++;
            // After extracting the Ip Packet, now extracting the tcp / udp packet
            struct udphdr *udp = (struct udphdr*)(buffer+iphdrlen+sizeof(struct ethhdr));

            if(ntohs(udp->source) == 80 || ntohs(udp->dest) == 80){
                httpcnt++;
				quiccnt++;
            }
            else if(ntohs(udp->source) == 443 || ntohs(udp->dest) == 443){
				quiccnt++;
            }
            else if(ntohs(udp->source) == 53 || ntohs(udp->dest) == 53){
                dnscnt++; // Here, DNS uses UDP for name queries
            }
        }
    }
    // Opening a csv file to write to it
    FILE *fp = fopen("sniffer_sshekha4.csv", "w");
    fprintf(fp, "protocol,count\n");
    fprintf(fp, "ip,%d\n", ipcnt);
    fprintf(fp, "tcp,%d\n", tcpcnt);
    fprintf(fp, "udp,%d\n", udpcnt);
    fprintf(fp, "dns,%d\n", dnscnt);
    fprintf(fp, "icmp,%d\n", icmpcnt);
    fprintf(fp, "http,%d\n", httpcnt);
    fprintf(fp, "https,%d\n", httpscnt);
    fprintf(fp, "quic,%d\n", quiccnt);
    fclose(fp);

    return 0;
}
