//コンパイル　: gcc traceroute.c -o traceroute
//実行　: sudo .traceroute <IP>

#include "common.h"

#define MAX_TTL 30
#define TIMEOUT_SEC 1

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <target>\n", argv[0]);
        return 1;
    }

    char *target = argv[1];
    int sockfd;
    struct sockaddr_in addr;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    if (inet_pton(AF_INET, target, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    printf("Traceroute to %s (%s), %d hops max\n\n", target, target, MAX_TTL);

    for (int ttl = 1; ttl <= MAX_TTL; ttl++) {

        // TTL 設定
        if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
            perror("setsockopt");
            return 1;
        }

        struct icmp icmp_packet;
        memset(&icmp_packet, 0, sizeof(icmp_packet));
        icmp_packet.icmp_type = ICMP_ECHO;
        icmp_packet.icmp_code = 0;
        icmp_packet.icmp_id = getpid();
        icmp_packet.icmp_seq = ttl;
        icmp_packet.icmp_cksum = calc_checksum(&icmp_packet, sizeof(icmp_packet));

        struct timeval start, end;
        gettimeofday(&start, NULL);

        if (sendto(sockfd, &icmp_packet, sizeof(icmp_packet), 0,
                   (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("sendto");
            continue;
        }

        // タイムアウト設定
        struct timeval timeout;
        timeout.tv_sec = TIMEOUT_SEC;
        timeout.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        unsigned char buf[1024];
        struct sockaddr_in from;
        socklen_t from_len = sizeof(from);

        int res = recvfrom(sockfd, buf, sizeof(buf), 0,
                           (struct sockaddr *)&from, &from_len);

        gettimeofday(&end, NULL);

        if (res < 0) {
            printf("%2d  *  (timeout)\n", ttl);
            continue;
        }

        // IPヘッダ分をスキップ
        struct ip *ip_hdr = (struct ip *)buf;
        int ip_header_len = ip_hdr->ip_hl << 2;

        struct icmp *icmp_res = (struct icmp *)(buf + ip_header_len);

        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &from.sin_addr, ip_str, sizeof(ip_str));

        double rtt = time_diff(start, end);

        printf("%2d  %-15s  %.3f ms\n", ttl, ip_str, rtt);

        // Echo Reply = 目的地到達
        if (icmp_res->icmp_type == ICMP_ECHOREPLY) {
            printf("\nReached destination.\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}
