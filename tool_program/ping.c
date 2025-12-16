// コンパイル: gcc ping.c -o ping utils.o
// 実行　: sudo ./ping <IP>

#include "common.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <ip>\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    /* 受信タイムアウト 3秒 */
    struct timeval tv = {3, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &addr.sin_addr);

    /* ICMP Echo Request 作成 */
    char send_buf[64];
    memset(send_buf, 0, sizeof(send_buf));

    struct icmphdr *icmp = (struct icmphdr *)send_buf;
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = htons(getpid() & 0xFFFF);
    icmp->un.echo.sequence = htons(1);
    icmp->checksum = 0;

    icmp->checksum = calc_checksum((unsigned short *)icmp, sizeof(send_buf));

    struct timeval start, end;
    gettimeofday(&start, NULL);

    if (sendto(sock, send_buf, sizeof(send_buf), 0,
               (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("sendto");
        return 1;
    }

    /* ===== ここが重要：受信ループ ===== */
    char recv_buf[1024];
    while (1) {
        ssize_t n = recvfrom(sock, recv_buf, sizeof(recv_buf), 0, NULL, NULL);
        if (n < 0) {
            perror("recvfrom");
            close(sock);
            return 1;
        }

        /* IP ヘッダをスキップ */
        struct iphdr *ip = (struct iphdr *)recv_buf;
        int ip_hdr_len = ip->ihl * 4;

        struct icmphdr *recv_icmp =
            (struct icmphdr *)(recv_buf + ip_hdr_len);

        /* 自分宛の Echo Reply か確認 */
        if (recv_icmp->type == ICMP_ECHOREPLY &&
            recv_icmp->un.echo.id == htons(getpid() & 0xFFFF)) {

            gettimeofday(&end, NULL);
            printf("PING %s: time=%.2f ms\n",
                   argv[1], time_diff(start, end));
            break;
        }
        /* 関係ない ICMP は無視して待ち続ける */
    }

    close(sock);
    return 0;
}
