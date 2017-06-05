#include <kern/pci.h>

#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

/* Constants */
#define MAX_TX_PKT_LEN 1518
#define MAX_TX_DESC_N  64

/* Registers */
#define E1000_TDBAL    ( 0x03800 / 4 )  /* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH    ( 0x03804 / 4 )  /* TX Descriptor Base Address High - RW */
#define E1000_TDLEN    ( 0x03808 / 4 )  /* TX Descriptor Length - RW */
#define E1000_TDH      ( 0x03810 / 4 )  /* TX Descriptor Head - RW */
#define E1000_TDT      ( 0x03818 / 4 )  /* TX Descripotr Tail - RW */
#define E1000_TCTL     ( 0x00400 / 4 )  /* TX Control - RW */
#define E1000_TCTL_RST    ( 0x00000001 / 4 )    /* software reset */
#define E1000_TCTL_EN     ( 0x00000002 )    /* enable tx */
#define E1000_TCTL_BCE    ( 0x00000004 )    /* busy check enable */
#define E1000_TCTL_PSP    ( 0x00000008 )    /* pad short packets */
#define E1000_TCTL_CT     ( 0x00000ff0 )    /* collision threshold */
#define E1000_TCTL_COLD   ( 0x003ff000 )    /* collision distance */
#define E1000_TCTL_SWXOFF ( 0x00400000 )    /* SW Xoff transmission */
#define E1000_TCTL_PBE    ( 0x00800000 )    /* Packet Burst Enable */
#define E1000_TCTL_RTLC   ( 0x01000000 )    /* Re-transmit on late collision */
#define E1000_TCTL_NRTU   ( 0x02000000 )    /* No Re-transmit on underrun */
#define E1000_TCTL_MULR   ( 0x10000000 )    /* Multiple request support */
#define E1000_TIPG       ( 0x00410 / 4 )  /* TX Inter-packet gap -RW */
#define E1000_TIPG_IPGT  ( 0xA )          // Transmit Time
#define E1000_TIPG_IPGR1 ( 0x4 << 10 ) // IPG Receive Time 1
#define E1000_TIPG_IPGR2 ( 0x6 << 20 ) // IPG Receive Time 2
#define E1000_TXD_STAT_DD    0x00000001 /* Descriptor Done */
#define E1000_TXD_CMD_RS     0x00000008 /* Report Status */
#define E1000_TXD_CMD_EOP    0x00000001 /* End of Packet */



volatile uint32_t *E1000;

/* Data Structures*/
struct tx_desc {
    uint64_t addr;
    uint16_t length;
    uint8_t cso;
    uint8_t cmd;
    uint8_t status;
    uint8_t css;
    uint16_t special;
} __attribute__((packed));

struct tx_pkt {
    uint8_t content[MAX_TX_PKT_LEN];
};

struct tx_desc tx_desc_array[MAX_TX_DESC_N] __attribute__((aligned(16)));
struct tx_pkt tx_pkt_buffer[MAX_TX_DESC_N] __attribute__((aligned(16)));

/* Functions */
int attach_function(struct pci_func *pcif);
int transmit(uint8_t *data, int len);

#endif	// JOS_KERN_E1000_H
