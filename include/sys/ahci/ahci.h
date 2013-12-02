#ifndef _AHCI_H
#define _AHCI_H
#include<defs.h>

#define PAGE_ALIGNED 0xffff

#define HBA_PORT_CMD_ST 0x1
#define HBA_PORT_CMD_FRE 0x10
#define HBA_PORT_CMD_FR 0x4000
#define HBA_PORT_CMD_CR 0x8000
#define HBA_PORT_IS_TFES 0x40000000
#define RESET_CMD_ST 0xfffffffe
#define RESET_CMD_FRE 0xffffffef
#define HBA_MEM_CAP_NP 0x1f
#define HBA_MEM_CAP_NCS 0x1f00
#define HBA_MEM_CAP_S64A 0x80000000

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATAPI 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATA 4

#define	AHCI_BASE	0x8000000	// 80M

typedef volatile struct tagHBA_PORT{
DWORD	clb;		// 0x00, command list base address, 1K-byte aligned
DWORD	clbu;		// 0x04, command list base address upper 32 bits
DWORD	fb;		// 0x08, FIS base address, 256-byte aligned
DWORD	fbu;		// 0x0C, FIS base address upper 32 bits
DWORD	is;		// 0x10, interrupt status
DWORD	ie;		// 0x14, interrupt enable
DWORD	cmd;		// 0x18, command and status
DWORD	rsv0;		// 0x1C, Reserved
DWORD	tfd;		// 0x20, task file data
DWORD	sig;		// 0x24, signature
DWORD	ssts;		// 0x28, SATA status (SCR0:SStatus)
DWORD	sctl;		// 0x2C, SATA control (SCR2:SControl)
DWORD	serr;		// 0x30, SATA error (SCR1:SError)
DWORD	sact;		// 0x34, SATA active (SCR3:SActive)
DWORD	ci;		// 0x38, command issue
DWORD	sntf;		// 0x3C, SATA notification (SCR4:SNotification)
DWORD	fbs;		// 0x40, FIS-based switch control
DWORD	rsv1[11];	// 0x44 ~ 0x6F, Reserved
DWORD	vendor[4];	// 0x70 ~ 0x7F, vendor specific
}HBA_PORT;

typedef volatile struct tagHBA_MEM{
// 0x00 - 0x2B, Generic Host Control
DWORD	cap;		// 0x00, Host capability
DWORD	ghc;		// 0x04, Global host control
DWORD	is;		// 0x08, Interrupt status
DWORD	pi;		// 0x0C, Port implemented
DWORD	vs;		// 0x10, Version
DWORD	ccc_ctl;	// 0x14, Command completion coalescing control
DWORD	ccc_pts;	// 0x18, Command completion coalescing ports
DWORD	em_loc;		// 0x1C, Enclosure management location
DWORD	em_ctl;		// 0x20, Enclosure management control
DWORD	cap2;		// 0x24, Host capabilities extended
DWORD	bohc;		// 0x28, BIOS/OS handoff control and status
// 0x2C - 0x9F, Reserved
BYTE	rsv[0xA0-0x2C];
// 0xA0 - 0xFF, Vendor specific registers
BYTE	vendor[0x100-0xA0];
// 0x100 - 0x10FF, Port control registers
HBA_PORT	ports[32];	// 1 ~ 32
}HBA_MEM;

typedef struct tagHBA_CMD_HEADER{
// DW0
BYTE	cfl:5;		// Command FIS length in DWORDS, 2 ~ 16
BYTE	a:1;		// ATAPI
BYTE	w:1;		// Write, 1: H2D, 0: D2H
BYTE	p:1;		// Prefetchable

BYTE	r:1;		// Reset
BYTE	b:1;		// BIST
BYTE	c:1;		// Clear busy upon R_OK
BYTE	rsv0:1;		// Reserved
BYTE	pmp:4;		// Port multiplier port

WORD	prdtl;		// Physical region descriptor table length in entries

// DW1
volatile
DWORD	prdbc;		// Physical region descriptor byte count transferred

// DW2, 3
DWORD	ctba;		// Command table descriptor base address
DWORD	ctbau;		// Command table descriptor base address upper 32 bits

// DW4 - 7
DWORD	rsv1[4];	// Reserved
}HBA_CMD_HEADER;


typedef struct tagHBA_PRDT_ENTRY{
DWORD	dba;		// Data base address
DWORD	dbau;		// Data base address upper 32 bits
DWORD	rsv0;		// Reserved

// DW3
DWORD	dbc:22;		// Byte count, 4M max
DWORD	rsv1:9;		// Reserved
DWORD	i:1;		// Interrupt on completion
}HBA_PRDT_ENTRY;


typedef struct tagHBA_CMD_TBL{
// 0x00
BYTE	cfis[64];	// Command FIS

// 0x40
BYTE	acmd[16];	// ATAPI command, 12 or 16 bytes

// 0x50
BYTE	rsv[48];	// Reserved

// 0x80
HBA_PRDT_ENTRY	prdt_entry[8];	// Physical region descriptor table entries, 0 ~ 65535
}HBA_CMD_TBL;

int setup_ahci();

int ahcicommands();

#endif
