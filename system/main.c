/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>
void radputc(char c) {

	struct uart_csreg *csrptr = devtab[RADIO].dvcsr;

	while((csrptr->lsr & UART_LSR_THRE) == 0);

	csrptr->buffer = c;
}
char radgetc() {

	struct uart_csreg *csrptr = devtab[RADIO].dvcsr;
	uint32 buffer;

	while((csrptr->lsr & UART_LSR_DR) == 0);

	buffer = csrptr->buffer;
	kprintf("%x", buffer);
	return (char)buffer;
}

int32	main(void)

{
	uint32	retval;
//	byte	mac1[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
//	char	buf[1500];
//	struct	netpacket pkt;
//	int32	i;
//	bool8	done;

	kprintf("\n\n###########################################################\n\n");

        kprintf("About to init radio...");
        int initok = pan_radio_init(RADIO);
        kprintf("done. Init returned %d\n", initok);
        
        struct panpkt test;
        char data[256];
        test.data = data;
        int readok = pan_read_rx(RADIO, &test, 256);
        if(readok == OK) {
                kprintf("packet data: ");
                int i;
                for(i = 0; i < 256; i++) {
                        kprintf("%X ", test.data[i]);
                }
        }
        
//	while(1) {
//		char buf[100];
//		int count;
//		count = 0;
//		while(count == 0) {
//			count = read(RADIO, buf, 0);
//		}
//		buf[count] = '\0';
//		kprintf("%s\n", buf);
//		count = read(CONSOLE, buf, 99);
//		kprintf("read %d\n", count);
//		//buf[count-1] = '\r';
//		write(RADIO, buf, count);
		//count = read(RADIO, buf, 3);
		//buf[count] = '\0';
		//kprintf("%s\n", buf);
//	}
	//write(CONSOLE, "test\n", 5);
	//nread = read(CONSOLE, buf, 10);
	//buf[nread] = '\0';
	//kprintf("read from CONSOLE: %s\n", buf);
	//uint32 *ptr = 0x44e0006c;
	/*ptr = 0x44e00c08;
	kprintf("pmper1 %x\n", *ptr);
	ptr = 0x44e00c0c;
	kprintf("pmper2 %x\n", *ptr);
	ptr = 0x44e00490;
	kprintf("cm_clkmode_dpll_core %x\n", *ptr);
	ptr = 0x44e00000;
	kprintf("l4lsclkstctrl %x\n", *ptr);
	ptr = 0x44e00060;
	*ptr = 0x2;
	kprintf("l4ls %x\n", *ptr);
	ptr = 0x44e00000;
	kprintf("l4lsclkstctrl %x\n", *ptr);*/
	//ptr = 0x44e0006c;
	//*ptr = 0x2;
	//kprintf("Uart1 clk %x\n", *ptr);
	//struct uart_csreg *csrptr = devtab[RADIO].dvcsr;
	//kprintf("accessing uart 2 registers\n");
	//kprintf("uart2 lcr %x\n", csrptr->lcr);
	//kprintf("\n...creating a shell\n");
	recvclr();
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		retval = receive();
		sleepms(200);
		kprintf("\n\n\rMain process recreating shell\n\n\r");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
}

void radio_read() {

	char c;
	kprintf("radio_read process\n");
	while(1) {
		read(RADIO, &c, 1);
		kprintf("%c", c);
		if(c == '\r')
			kprintf("\n");
	}
}

void radio_write() {

	char buf[100];
	uint32 count;
	kprintf("radio_write process\n");
	while(1) {
		count = read(CONSOLE, buf, 99);
		//kprintf("read from console %d\n", count);
		if((buf[0] == '+') && (buf[1] == '+') && (buf[2] == '+')) {
			count = 3;
		}
		write(RADIO, buf, count);
	}
}
