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

    memset(rv_desc_array, 0, sizeof(struct rv_desc) * MAX_RV_DESC_N);
    memset(rv_pkt_buffer, 0, sizeof(struct rv_pkt)  * MAX_RV_DESC_N);
    for (i = 0; i < MAX_RV_DESC_N; i++)
        rv_desc_array[i].addr = PADDR(rv_pkt_buffer[i].content);

    E1000[E1000_RAL] = 0x12005452;
    E1000[E1000_RAH] = 0x00005634;
    E1000[E1000_RAH] |= E1000_RAH_AV;

    E1000[E1000_RDBAL] = PADDR(rv_desc_array);
    E1000[E1000_RDBAH] = 0;
    E1000[E1000_RDLEN] = sizeof(struct rv_desc) * MAX_RV_DESC_N;
    E1000[E1000_RDH]   = 1;
    E1000[E1000_RDT]   = 0;
    
    E1000[E1000_RCTL] = E1000_RCTL_EN;
    E1000[E1000_RCTL] &= ~E1000_RCTL_LPE;
    E1000[E1000_RCTL] &= ~E1000_RCTL_LBM;
    E1000[E1000_RCTL] &= ~E1000_RCTL_RDMTS;
    E1000[E1000_RCTL] &= ~E1000_RCTL_MO;
    E1000[E1000_RCTL] |= E1000_RCTL_BAM;
    E1000[E1000_RCTL] &= ~E1000_RCTL_BSIZE;
    E1000[E1000_RCTL] |= E1000_RCTL_SECRC;

    return 1;
}

int transmit(uint8_t *data, int len) {
    if (len <= 0 || len > MAX_TX_PKT_LEN)
        return -1;
    int tail = E1000[E1000_TDT];
    if (!(tx_desc_array[tail].status & E1000_TXD_STAT_DD))
        return -1;
    memmove(tx_pkt_buffer[tail].content, data, len);
    tx_desc_array[tail].length = len;
    tx_desc_array[tail].status &= ~E1000_TXD_STAT_DD;
    tx_desc_array[tail].cmd |= E1000_TXD_CMD_RS;
    tx_desc_array[tail].cmd |= E1000_TXD_CMD_EOP;


    E1000[E1000_TDT] = (tail + 1) % MAX_TX_DESC_N;

    return 0;
}

int receive(uint8_t *data) {
    uint32_t rdt = (E1000[E1000_RDT] + 1) % MAX_RV_DESC_N;
    if ((rv_desc_array[rdt].status & E1000_RXD_STAT_DD) == 0)
        return -1;
    if ((rv_desc_array[rdt].status & E1000_RXD_STAT_EOP) == 0)
        panic("e1000_receive: exception");
    uint32_t len = rv_desc_array[rdt].length;
    memmove(data, rv_pkt_buffer[rdt].content, len);
    rv_desc_array[rdt].status &= ~E1000_RXD_STAT_DD;
    rv_desc_array[rdt].status &= ~E1000_RXD_STAT_EOP;
    E1000[E1000_RDT] = rdt;
    return len;
    // uint32_t tail = (E1000[E1000_RDT] + 1) % MAX_RV_DESC_N;
    // if (!(rv_desc_array[tail].status & E1000_RXD_STAT_DD))
    //     return -1;
    // uint32_t len = rv_desc_array[tail].length;
    // memmove(data, rv_pkt_buffer[tail].content, len);
    // rv_desc_array[tail].status &= ~E1000_RXD_STAT_DD;
    // rv_desc_array[tail].status &= ~E1000_RXD_STAT_EOP;
    // E1000[E1000_RDT] = tail;
    // return len;
}
