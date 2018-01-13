/*
 * main.c
 *
 *  Created on: Jan 12, 2018
 *      Author: johns
 */

#include <stdio.h>

#include "xparameters.h"

#include "lwipopts.h"
//#include "ten_gignetif.h"
#include "lwip/init.h"
#include "lwip/netif.h"

#include "lwip/init.h"

#include "lwip/debug.h"

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"
//#include "lwip/timeouts.h"

#include "lwip/stats.h"

#include "lwip/ip.h"
#include "lwip/ip_addr.h"
#include "lwip/ip_frag.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "tapif.h"
#include "netif/etharp.h"

#include "udpecho_raw.h"
#include "tcpecho_raw.h"

static ip_addr_t ipaddr, netmask, gw;

int main()
{
    print("Hello World from arty_lwip\r\n");

    struct netif netif;
//    int ch;
//    char ip_str[16] = {0}, nm_str[16] = {0}, gw_str[16] = {0};

    IP4_ADDR(&gw, 192, 168, 0, 1);
    IP4_ADDR(&ipaddr, 192, 168, 0, 182);
    IP4_ADDR(&netmask, 255, 255, 255, 0);

    lwip_init();

    netif_add(&netif, &ipaddr, &netmask, &gw, NULL, tapif_init, ethernet_input);

    netif_set_default(&netif);
    netif_set_up(&netif);
    udpecho_raw_init();
    tcpecho_raw_init();

    print("Entering while loop\r\n");
    while(1)
    {
        // For Proof-of-Concept, we will load one packet
        // from the tapif_select function
        // The actual version will behave differently, there are some
        // warnings to not call certain functions from an Interrupt
        // callback.  So that means I will have to populate a certain
        // data structure and to check it from a select function
        // and call that function from there.
        tapif_select(&netif);
    }

	return 0;
}
