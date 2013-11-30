#include<sys/ahci/ahci.h>
#include<sys/memory/mm_struct.h>
#include<string.h>
#include<sys/memory/free_phys_pages.h>

HBA_PORT hba_port = {0};
HBA_MEM hba_mem = {0};

static int check_type(HBA_PORT port);
int print_hba_mem(HBA_MEM *hba_mem);
void port_rebase(HBA_PORT *port, int portno, int num_pi);

int setup_ahci(){
	struct kernel_mm_struct *mm = get_kernel_mm();
	printf("mm->start_ahci_mem: %p\n", mm->start_ahci_mem);
	HBA_MEM *hba_mem = (HBA_MEM *)mm->start_ahci_mem;
	printf("hba_mem: %p\n", hba_mem);
	print_hba_mem(hba_mem);
	DWORD mem_pi = hba_mem->pi;
	int num_pi =0;
	DWORD mem_cap_np = (hba_mem->cap)&HBA_MEM_CAP_NP;
	printf("mem_Cap_np: %p",mem_cap_np);
	/* get port details and stop engine */
	for(int i=0;i<=mem_cap_np;i++){
		if(mem_pi&0x1){
			printf("port%d",i);
			DWORD cmd_st = (hba_mem->ports[i].cmd)&HBA_PORT_CMD_ST;
			DWORD cmd_fre = (hba_mem->ports[i].cmd)&HBA_PORT_CMD_FRE;
	//		DWORD cmd_fr = (hba_mem->ports[i].cmd)&HBA_PORT_CMD_FR;
	//		DWORD cmd_cr = (hba_mem->ports[i].cmd)&HBA_PORT_CMD_CR;
	//		printf("cmd.st: %p",cmd_st);//0th cmd bit
	//		printf("cmd.fre: %p",cmd_fre);//4th cmd bit 
	//		printf("cmd.fr: %p",cmd_fr);//14th cmd bit
	//		printf("cmd.cr: %p\n",cmd_cr);//15th cmd bit
			int dt = check_type(hba_mem->ports[i]);
			if (dt == AHCI_DEV_SATA){
				printf("SATA drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SATAPI){
				printf("SATAPI drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SEMB){
				printf("SEMB drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM){
				printf("PM drive found at port %d\n", i);
			}
			else{
				printf("No drive found at port %d\n", i);
			}

			if(cmd_st!=0||cmd_fre!=0){	
				if(cmd_st!=0){
					hba_mem->ports[i].cmd&=RESET_CMD_ST;
				}
				if(cmd_fre!=0){
					(hba_mem->ports[i].cmd)&=RESET_CMD_FRE;
				}
			i--;
			continue;
			}
		}
		mem_pi>>=1;
		num_pi++;
	}
	printf("num_pi: %d\n",num_pi);
	DWORD mem_cap_ncs = (hba_mem->cap)&HBA_MEM_CAP_NCS;
	mem_cap_ncs>>=8;
	mem_cap_ncs++;
	printf("mem_cap_ncs: %p\n",mem_cap_ncs);
	if(((hba_mem->cap)&HBA_MEM_CAP_S64A)!=0){
		printf("64 bit supported\n");	
	}
	else
		printf("64 bit address not supported\n");
	mem_pi = hba_mem->pi; 
	
	/* port rebase */
	for(int i=0;i<=mem_cap_np;i++){
		if(mem_pi&0x1){
			port_rebase(&(hba_mem->ports[i]), i, num_pi);
		}
		mem_pi>>=1;
	}	

	/*start engine */
	for(int i = 0;i<=mem_cap_np;i++){
		if(mem_pi&0x1){
			while(hba_mem->ports[i].cmd & HBA_PORT_CMD_CR);
			hba_mem->ports[i].cmd |= HBA_PORT_CMD_FRE;
			hba_mem->ports[i].cmd |= HBA_PORT_CMD_ST;
		}
		mem_pi>>=1;
		
	}
	return 0;
}

void port_rebase(HBA_PORT *port, int i, int num_pi){
//TODO: we are assuming that number of commandlist is 32
	uint32_t FB_BASE = AHCI_BASE + (num_pi/4 + 1)*4096;//each clb is 1K
	printf("FB_BASE = %p", FB_BASE);
	uint64_t CLB_BASE = FB_BASE + (num_pi/16 + 1)*4096; //each fis structure is 256B
	printf("CLB_BASE = %p", CLB_BASE);
	port->clb = AHCI_BASE + (i<<10);
	port->clbu = 0;
	uint64_t clb = port->clb;
	printf("clb: %p",clb);
	//if(clb&PAGE_ALIGNED){
		uint64_t clb_vir = v_alloc_page_get_phys(&clb, PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR);
		printf("clb virtual address: %p",clb_vir );

		memset((void*)clb_vir, 0, 1024);
		printf("memsetclb");
	//}
	port->fb = FB_BASE + (i<<8);
	port->fbu = 0;
	uint64_t fb = port->fb;
	printf("fb:%p",fb);
	if(fb&PAGE_ALIGNED){
		uint64_t fb_vir = v_alloc_page_get_phys(&fb, PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR);
		memset((void*)fb_vir, 0, 1024);
		printf("memsetfb");
	}
			
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)clb_vir;
	for (int j=0; j<32; j++){
		cmdheader[j].prdtl = 8;
		cmdheader[j].ctba = CLB_BASE + (i<<13) + (j<<8);
		cmdheader[j].ctbau = 0;
		uint64_t ctba = cmdheader[j].ctba;
		printf("ctba: %p", ctba);
		if(ctba&PAGE_ALIGNED){
			uint64_t ctba_vir = v_alloc_page_get_phys(&ctba, PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR); 
			memset((void*)ctba_vir, 0, 1024);
			//printf("after memset ctba");
		}
	}
}

static int check_type(HBA_PORT port)
{
	DWORD ssts = port.ssts;
 
	BYTE ipm = (ssts >> 8) & 0x0F;
	BYTE det = ssts & 0x0F;
  
	if (det != 0x3)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != 0x1)
		return AHCI_DEV_NULL;
   
	switch (port.sig){
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}

int print_hba_mem(HBA_MEM *hba_mem){
	printf("hba_mem->cap: %x\n", hba_mem->cap);
	printf("hba_mem->ghc: %p\n", hba_mem->ghc);
	printf("hba_mem->is: %p\n", hba_mem->is);
	printf("hba_mem->pi: %p\n", hba_mem->pi);
	printf("hba_mem->vs: %p\n", hba_mem->vs);

	return 0;
}
