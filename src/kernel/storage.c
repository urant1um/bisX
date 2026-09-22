#include "hardware.h"

extern void kprint(const char *str);

#define ATA_PRIMARY_IO    0x1F0
#define ATA_REG_DATA      0x1F0
#define ATA_REG_FEATURES  0x1F1
#define ATA_REG_SECCOUNT  0x1F2
#define ATA_REG_LBA_LOW   0x1F3
#define ATA_REG_LBA_MID   0x1F4
#define ATA_REG_LBA_HIGH  0x1F5
#define ATA_REG_DEVSEL    0x1F6
#define ATA_REG_COMMAND   0x1F7
#define ATA_REG_STATUS    0x1F7

#define ATA_CMD_READ_PIO  0x20
#define ATA_CMD_WRITE_PIO 0x30
#define ATA_CMD_IDENTIFY  0xEC

void ata_init(void) {
    outb(ATA_PRIMARY_IO + ATA_REG_DEVSEL, 0xA0); // Master drive
    outb(ATA_PRIMARY_IO + ATA_REG_SECCOUNT, 0);
    outb(ATA_PRIMARY_IO + ATA_REG_LBA_LOW, 0);
    outb(ATA_PRIMARY_IO + ATA_REG_LBA_MID, 0);
    outb(ATA_PRIMARY_IO + ATA_REG_LBA_HIGH, 0);
    outb(ATA_PRIMARY_IO + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

    unsigned char status = inb(ATA_PRIMARY_IO + ATA_REG_STATUS);
    if (status == 0) {
        kprint("[ATA] No drive detected.\n");
        return;
    }

    // Безопасное ожидание с таймаутом, чтобы не вешать процессор навсегда!
    int timeout = 100000;
    while ((inb(ATA_PRIMARY_IO + ATA_REG_STATUS) & 0x80) && --timeout > 0);

    if (timeout <= 0) {
        kprint("[ATA] Drive timeout (Not responding, but running).\n");
    } else {
        kprint("[ATA] Hard Drive / SSD initialized successfully (PIO Mode).\n");
    }
}

int ata_read_sector(unsigned int lba, unsigned char *buf) {
    outb(ATA_PRIMARY_IO + ATA_REG_DEVSEL, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_IO + ATA_REG_SECCOUNT, 1);
    outb(ATA_PRIMARY_IO + ATA_REG_LBA_LOW, (lba & 0xFF));
    outb(ATA_PRIMARY_IO + ATA_REG_LBA_MID, ((lba >> 8) & 0xFF));
    outb(ATA_PRIMARY_IO + ATA_REG_LBA_HIGH, ((lba >> 16) & 0xFF));
    outb(ATA_PRIMARY_IO + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

    int timeout = 100000;
    while ((inb(ATA_PRIMARY_IO + ATA_REG_STATUS) & 0x80) && --timeout > 0);
    if (!(inb(ATA_PRIMARY_IO + ATA_REG_STATUS) & 0x08)) return -1;

    unsigned short *ptr = (unsigned short *)buf;
    for (int i = 0; i < 256; i++) {
        ptr[i] = inw(ATA_PRIMARY_IO + ATA_REG_DATA);
    }
    return 0;
}

int ata_write_sector(unsigned int lba, const unsigned char *buf) {
    outb(ATA_PRIMARY_IO + ATA_REG_DEVSEL, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_IO + ATA_REG_SECCOUNT, 1);
    outb(ATA_PRIMARY_IO + ATA_REG_LBA_LOW, (lba & 0xFF));
    outb(ATA_PRIMARY_IO + ATA_REG_LBA_MID, ((lba >> 8) & 0xFF));
    outb(ATA_PRIMARY_IO + ATA_REG_LBA_HIGH, ((lba >> 16) & 0xFF));
    outb(ATA_PRIMARY_IO + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

    int timeout = 100000;
    while ((inb(ATA_PRIMARY_IO + ATA_REG_STATUS) & 0x80) && --timeout > 0);

    unsigned short *ptr = (unsigned short *)buf;
    for (int i = 0; i < 256; i++) {
        outw(ATA_PRIMARY_IO + ATA_REG_DATA, ptr[i]);
    }
    outb(ATA_PRIMARY_IO + ATA_REG_COMMAND, 0xE7); // Flush cache
    timeout = 100000;
    while ((inb(ATA_PRIMARY_IO + ATA_REG_STATUS) & 0x80) && --timeout > 0);
    return 0;
}
