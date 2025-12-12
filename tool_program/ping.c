// コンパイル: gcc ping.c -o ping utils.o
// 実行　: sudo ./ping <IP>

#include "common.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <ip>\n", argv[0]);
        return 1;
    }

    char *target_ip = argv[1];

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(target_ip);

    char send_buf[64];
    char recv_buf[1024];

    struct icmphdr *icmp = (struct icmphdr *)send_buf;
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = getpid();
    icmp->un.echo.sequence = 1;
    memset(send_buf + sizeof(struct icmphdr), 0, 32);

    // チェックサム計算
    icmp->checksum = calc_checksum((unsigned short *)icmp, sizeof(send_buf));

    struct timeval start, end;
    gettimeofday(&start, NULL);

    if (sendto(sock, send_buf, sizeof(send_buf), 0,
               (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("sendto");
        return 1;
    }

    socklen_t len = sizeof(addr);

    if (recvfrom(sock, recv_buf, sizeof(recv_buf), 0,
                 (struct sockaddr *)&addr, &len) < 0) {
        perror("recvfrom");
        return 1;
    }

    gettimeofday(&end, NULL);

    printf("PING %s: time=%.2f ms\n", target_ip, time_diff(start, end));

    return 0;
}
