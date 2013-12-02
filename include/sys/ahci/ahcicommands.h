#ifndef _AHCICOMMANDS_H
#define _AHCICOMMANDS_H

#include<defs.h>
#include<sys/ahci/ahci.h>

#define ATA_CMD_READ_DMA 0xC8
#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_CMD_WRITE_DMA 0xCA
#define ATA_CMD_WRITE_DMA_EX 0x35

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08

#define LOBYTE(w) ((BYTE)(w))
#define HIBYTE(w) ((BYTE)(((WORD)(w) >> 8) & 0xFF))

int read_ahci(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, WORD *buffer); 
int write_ahci(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, WORD *buffer); 

#endif
