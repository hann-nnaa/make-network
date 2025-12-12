#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>

// チェックサム計算
unsigned short calc_checksum(void *buf, int len);
// 時間差計算（ミリ秒単位）
double time_diff(struct timeval start, struct timeval end);

#endif
