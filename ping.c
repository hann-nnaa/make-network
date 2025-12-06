//コンパイル: gcc ping.c -o ping
//実行　: sudo .ping <IP>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>

unsigned short checksum(void *b, int len){
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for(sum = 0; len > 1; len -= 2){
        sum += *buf++;
    }
    if(len == 1){
        sum += *(unsigned char*)buf;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

long get_time_diff(struct timeval *start, struct timeval *end){
    return (end->tv_sec - start->tv_sec) * 1000000L + (end->tv_usec - start->tv_usec);
}

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Usage: sudo %s <IP>\n", argv[0]);
        return 1;
    }

    int sockfd;
    struct sockaddr_in addr;
    struct icmp icmp_pkt;
    char recvbuf[1024];

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &addr.sin_addr);

    memset(&icmp_pkt, 0, sizeof(icmp_pkt));
    icmp_pkt.icmp_type = ICMP_ECHO;
    icmp_pkt.icmp_code = 0;
    icmp_pkt.icmp_id = getpid() & 0xFFFF;
    icmp_pkt.icmp_seq = 0;
    icmp_pkt.icmp_cksum = checksum(&icmp_pkt, sizeof(icmp_pkt));

    struct timeval start, end;

    gettimeofday(&start, NULL);
    sendto(sockfd, &icmp_pkt, sizeof(icmp_pkt), 0,
           (struct sockaddr*)&addr, sizeof(addr));

    recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, NULL, NULL);
    gettimeofday(&end, NULL);

    long rtt = get_time_diff(&start, &end);
    printf("Reply from %s: time=%.2f ms\n", argv[1], rtt / 1000.0);

    close(sockfd);
    return 0;
}