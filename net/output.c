#include "ns.h"
#include <inc/lib.h>

extern union Nsipc nsipcbuf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver
    while (1)
    {
        cprintf("start...\n");
        if (sys_ipc_recv(&nsipcbuf) < 0)
            return;
        cprintf("data:%s@%d len:%d\n", nsipcbuf.pkt.jp_data, (int)nsipcbuf.pkt.jp_data, nsipcbuf.pkt.jp_len);

        while (sys_transmit((uint8_t *)nsipcbuf.pkt.jp_data, nsipcbuf.pkt.jp_len))
            ;
        cprintf("done\n");

    }
}