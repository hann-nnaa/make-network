//コンパイル　: gcc traceroute.c -o traceroute
//実行　: sudo .traceroute <IP>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char*)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: sudo %s <IP>\n", argv[0]);
        return 1;
    }

    int sockfd;
    struct sockaddr_in addr;
    char recvbuf[1024];
    struct icmp icmp_pkt;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &addr.sin_addr);

    printf("Traceroute to %s\n", argv[1]);

    for (int ttl = 1; ttl <= 30; ttl++) {
        setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

        memset(&icmp_pkt, 0, sizeof(icmp_pkt));
        icmp_pkt.icmp_type = ICMP_ECHO;
        icmp_pkt.icmp_code = 0;
        icmp_pkt.icmp_id = getpid() & 0xFFFF;
        icmp_pkt.icmp_seq = ttl;
        icmp_pkt.icmp_cksum = checksum(&icmp_pkt, sizeof(icmp_pkt));

        struct timeval start, end;
        gettimeofday(&start, NULL);

        sendto(sockfd, &icmp_pkt, sizeof(icmp_pkt), 0,
               (struct sockaddr*)&addr, sizeof(addr));

        struct sockaddr_in reply_addr;
        socklen_t addrlen = sizeof(reply_addr);

        recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0,
                 (struct sockaddr*)&reply_addr, &addrlen);

        gettimeofday(&end, NULL);

        long usec = (end.tv_sec - start.tv_sec) * 1000000L +
                    (end.tv_usec - start.tv_usec);

        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &reply_addr.sin_addr, ip_str, sizeof(ip_str));

        printf("%2d  %s  %.2f ms\n", ttl, ip_str, usec / 1000.0);

        if (reply_addr.sin_addr.s_addr == addr.sin_addr.s_addr) {
            printf("Destination reached.\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}
