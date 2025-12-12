#include "common.h"

// チェックサム計算
unsigned short calc_checksum(void *buf, int len) {
    unsigned int sum = 0;
    unsigned short *data = buf;

    while (len > 1) {
        sum += *data++;
        len -= 2;
    }

    if (len == 1) {
        unsigned short tmp = 0;
        *(unsigned char *)&tmp = *(unsigned char *)data;
        sum += tmp;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return (unsigned short)(~sum);
}

// 時間差計算（ミリ秒単位）
double time_diff(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 +
           (end.tv_usec - start.tv_usec) / 1000.0;
}
