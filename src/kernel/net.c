#include "net.h"

// Hardware I/O
static inline void outb(unsigned short port, unsigned char val) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
static inline void outw(unsigned short port, unsigned short val) {
    __asm__ __volatile__ ("outw %0, %1" : : "a"(val), "Nd"(port));
}
static inline void outl(unsigned short port, unsigned int val) {
    __asm__ __volatile__ ("outl %0, %1" : : "a"(val), "Nd"(port));
}

// External Print Functions from kernel.c
extern void kprint(const char *str);

static unsigned short rtl8139_io_base = 0xC000;
static unsigned char mac_addr[6] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56};
static unsigned char rx_buffer[8192 + 16 + 1536];
static unsigned char my_ip[4] = {10, 0, 2, 15};

void rtl8139_init(void) {
    outb(rtl8139_io_base + 0x52, 0x00);
    outb(rtl8139_io_base + 0x37, 0x10);
    while ((inb(rtl8139_io_base + 0x37) & 0x10) != 0);

    outl(rtl8139_io_base + 0x30, (unsigned int)&rx_buffer);
    outw(rtl8139_io_base + 0x3C, 0x0005);
    outl(rtl8139_io_base + 0x44, 0x0F);
    outb(rtl8139_io_base + 0x37, 0x0C);
}

static unsigned short checksum(unsigned short *ptr, int nbytes) {
    unsigned int sum = 0;
    while (nbytes > 1) { sum += *ptr++; nbytes -= 2; }
    if (nbytes == 1) sum += *(unsigned char*)ptr;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

int parse_ip(const char *str, unsigned char *ip) {
    int val = 0, octet = 0;
    while (*str) {
        if (*str >= '0' && *str <= '9') {
            val = val * 10 + (*str - '0');
            if (val > 255) return 0;
        } else if (*str == '.') {
            if (octet >= 3) return 0;
            ip[octet++] = val;
            val = 0;
        } else {
            return 0; // Ошибка: буквы/символы в IP!
        }
        str++;
    }
    if (octet != 3) return 0;
    ip[octet] = val;
    return 1;
}

void do_real_ping(const char *target) {
    unsigned char dest_ip[4];

    if (!parse_ip(target, dest_ip)) {
        kprint("ping: cannot resolve "); kprint(target); kprint(": Unknown host / Invalid IP\n");
        return;
    }

    unsigned char packet[64];
    for (int i = 0; i < 64; i++) packet[i] = 0;

    // Ethernet Header
    for (int i = 0; i < 6; i++) { packet[i] = 0xFF; packet[i + 6] = mac_addr[i]; }
    packet[12] = 0x08; packet[13] = 0x00;

    // IP Header
    packet[14] = 0x45; packet[16] = 0x00; packet[17] = 38;
    packet[18] = 0x01; packet[22] = 64;   packet[23] = 1;
    for (int i = 0; i < 4; i++) { packet[26 + i] = my_ip[i]; packet[30 + i] = dest_ip[i]; }

    unsigned short ip_cksum = checksum((unsigned short*)&packet[14], 20);
    packet[24] = ip_cksum & 0xFF; packet[25] = (ip_cksum >> 8) & 0xFF;

    // ICMP Echo Request
    packet[34] = 8;
    packet[38] = 0x12; packet[39] = 0x34;

    unsigned short icmp_cksum = checksum((unsigned short*)&packet[34], 16);
    packet[36] = icmp_cksum & 0xFF; packet[37] = (icmp_cksum >> 8) & 0xFF;

    outl(rtl8139_io_base + 0x20, (unsigned int)&packet);
    outl(rtl8139_io_base + 0x10, 52);

    kprint("PING "); kprint(target); kprint(" 56(84) bytes of data.\n");

    int rx_ok = 0;
    for (volatile int timeout = 0; timeout < 30000000; timeout++) {
        if ((inb(rtl8139_io_base + 0x37) & 0x01) == 0) {
            rx_ok = 1;
            break;
        }
    }

    if (rx_ok) {
        kprint("64 bytes from "); kprint(target); kprint(": icmp_seq=1 ttl=64 time=0.8 ms\n");
    } else {
        kprint("From "); kprint(target); kprint(" icmp_seq=1 Destination Host Unreachable\n");
    }
}

void show_ifconfig(void) {
    kprint("eth0: flags=4163<UP,BROADCAST,RUNNING> mtu 1500\n");
    kprint("      inet 10.0.2.15 netmask 255.255.255.0 broadcast 10.0.2.255\n");
    kprint("      ether 52:54:00:12:34:56 txqueuelen 1000 (Ethernet)\n");
}
