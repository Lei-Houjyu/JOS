#include <kern/pci.h>

#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

volatile uint32_t *E1000;

int attach_function(struct pci_func *pcif);

#endif	// JOS_KERN_E1000_H
