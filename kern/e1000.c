#include <kern/e1000.h>
#include <inc/memlayout.h>
#include <kern/pmap.h>

// LAB 6: Your driver code here
int attach_function(struct pci_func *pcif) {
    pci_func_enable(pcif);
    
    boot_map_region(kern_pgdir, KSTACKTOP, pcif->reg_size[0], pcif->reg_base[0], PTE_PCD | PTE_PWT | PTE_W);



    return 1;
}
