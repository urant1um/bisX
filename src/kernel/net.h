#ifndef NET_H
#define NET_H

void rtl8139_init(void);
int parse_ip(const char *str, unsigned char *ip);
void do_real_ping(const char *target);
void show_ifconfig(void);

#endif
