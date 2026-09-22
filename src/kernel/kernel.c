#include "hardware.h"
#include "commands.h"

#define VGA_BASE 0xB8000
#define VGA_W 80
#define VGA_H 25

static int cursor_x = 0, cursor_y = 0;

void kclear(void) {
    unsigned short *vga = (unsigned short *)VGA_BASE;
    for (int i = 0; i < VGA_W * VGA_H; i++) vga[i] = (unsigned short)' ' | (0x0F << 8);
    cursor_x = 0; cursor_y = 0;
}

void kputchar_col(char c, unsigned char col) {
    unsigned short *vga = (unsigned short *)VGA_BASE;
    if (c == '\n') {
        cursor_x = 0; cursor_y++;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            vga[cursor_y * VGA_W + cursor_x] = ' ' | (col << 8);
        }
    } else {
        vga[cursor_y * VGA_W + cursor_x] = (unsigned short)c | ((unsigned short)col << 8);
        cursor_x++;
    }
    if (cursor_x >= VGA_W) { cursor_x = 0; cursor_y++; }
    if (cursor_y >= VGA_H) {
        for (int i = 0; i < (VGA_H - 1) * VGA_W; i++) vga[i] = vga[i + VGA_W];
        for (int i = (VGA_H - 1) * VGA_W; i < VGA_H * VGA_W; i++) vga[i] = ' ' | (col << 8);
        cursor_y = VGA_H - 1;
    }
}

void kputchar(char c) { kputchar_col(c, 0x0F); }
void kprint(const char *str) { while (*str) kputchar(*str++); }
void kprint_col(const char *str, unsigned char col) { while (*str) kputchar_col(*str++, col); }

// Строгий опрос PS/2 клавиатуры с отсечением Break-кодов (без звездочек!)
static const char scancode_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`', 0,
    '\\','z','x','c','v','b','n','m',',','.','/', 0, 0, 0, ' '
};

char kgetchar(void) {
    while (1) {
        if (inb(0x64) & 1) {
            unsigned char scancode = inb(0x60);
            if (scancode & 0x80) continue; // Игнорируем отпускание клавиши
            char c = scancode_ascii[scancode];
            if (c != 0) return c;
        }
    }
}

void print_banner(void) {
    kprint_col("   /\\_/\\  \n", 0x0E);
    kprint_col("  ( o.o )   ", 0x0E); kprint_col("bisX OS Bare-Metal Enterprise Server v2.5\n", 0x0B);
    kprint_col("   > ^ <    ", 0x0E); kprint_col("Drivers: RTL8139 NIC, ATA HDD/SSD, PS/2 Active\n\n", 0x0A);
}

void bisX_main(void) {
    kclear();
    
    // Инициализация реального оборудования
    rtl8139_init();
    ata_init();
    
    print_banner();

    char input_buf[128];
    int buf_idx = 0;

    while (1) {
        kprint_col("root@bisX-server", 0x0A);
        kprint(":");
        kprint_col("~", 0x09);
        kprint("# ");

        buf_idx = 0;
        while (1) {
            char c = kgetchar();
            if (c == '\n') {
                kputchar('\n');
                input_buf[buf_idx] = 0;
                execute_cmd(input_buf);
                break;
            } else if (c == '\b') {
                if (buf_idx > 0) {
                    buf_idx--;
                    kputchar('\b');
                }
            } else {
                if (buf_idx < 127) {
                    input_buf[buf_idx++] = c;
                    kputchar(c);
                }
            }
        }
    }
}
