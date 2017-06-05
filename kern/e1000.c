#include <kern/e1000.h>
#include <inc/memlayout.h>
#include <inc/string.h>
#include <kern/pmap.h>

// LAB 6: Your driver code here
int attach_function(struct pci_func *pcif) {
    pci_func_enable(pcif);
    
    boot_map_region(kern_pgdir, KSTACKTOP, pcif->reg_size[0], pcif->reg_base[0], PTE_PCD | PTE_PWT | PTE_W);
    E1000 = (uint32_t *)KSTACKTOP;

    memset(tx_desc_array, 0, sizeof(struct tx_desc) * MAX_TX_DESC_N);
    memset(tx_pkt_buffer, 0, sizeof(struct tx_pkt)  * MAX_TX_DESC_N);
    int i;
    for (i = 0; i < MAX_TX_DESC_N; i++) {
        tx_desc_array[i].addr = PADDR(tx_pkt_buffer[i].content);
        tx_desc_array[i].status |= E1000_TXD_STAT_DD;
    }

    E1000[E1000_TDBAL] = PADDR(tx_desc_array);
    E1000[E1000_TDBAH] = 0;
    E1000[E1000_TDLEN] = sizeof(struct tx_desc) * MAX_TX_DESC_N;
    E1000[E1000_TDH] = 0;
    E1000[E1000_TDT] = 0;
    E1000[E1000_TCTL] |= E1000_TCTL_EN;
    E1000[E1000_TCTL] |= E1000_TCTL_PSP;
    E1000[E1000_TCTL] |= E1000_TCTL_CT;
    E1000[E1000_TCTL] |= E1000_TCTL_COLD;
    E1000[E1000_TIPG]  = 0;
    E1000[E1000_TIPG] |= E1000_TIPG_IPGT;
    E1000[E1000_TIPG] |= E1000_TIPG_IPGR1;
    E1000[E1000_TIPG] |= E1000_TIPG_IPGR2;

    return 1;
}

int transmit(uint8_t *data, int len) {
    if (len <= 0 || len > MAX_TX_PKT_LEN)
        return -1;
    int tail = E1000[E1000_TDT];
    if (!(tx_desc_array[tail].status & E1000_TXD_STAT_DD))
        return -1;
    memmove(tx_pkt_buffer[tail].content, data, len);
    tx_desc_array[tail].cmd |= E1000_TXD_CMD_RS;
    
    E1000[E1000_TDT] = (tail + 1) % MAX_TX_DESC_N;

    return 0;
}
