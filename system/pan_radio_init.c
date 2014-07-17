/* pan_radio_init.c - pan_radio_init() */

#include <xinu.h>

int pan_radio_init(did32 radio) {
        pan_rx_sem = semcreate(0);
        if(pan_rx_sem == SYSERR) {
                return SYSERR;
        }
        pan_at_sem = semcreate(0);
        if(pan_at_sem == SYSERR) {
                return SYSERR;
        }
        pan_radio_at_head = 0;
        pan_radio_at_tail = 0;
        
        pan_radio_rx_head = 0;
        pan_radio_rx_tail = 0;
        
        write(radio, "+++", 3);
        
        char buf[256];
        int i;
        for(i = 0; i < 256; i++) {
                buf[i] = 0;
        }
        
        int count;
	count = 0;
	while(count == 0) {
	        count = read(radio, buf, 0);
	}
	if(buf[0] != 'O' || buf[1] != 'K') {
	        return SYSERR;
	}
	
	count = 0;
	buf[0] = 0;
	buf[1] = 0;
	write(radio, "ATRE\r", 5);
	while(count == 0) {
	        count = read(radio, buf, 0);
	}
	if(buf[0] != 'O' || buf[1] != 'K') {
	        return SYSERR;
	}
	
	count = 0;
	buf[0] = 0;
	buf[1] = 0;
	write(radio, "ATIDFFFF\r", 9);
	while(count == 0) {
	      count = read(radio, buf, 0);
	}
	if(buf[0] != 'O' || buf[1] != 'K') {
	        return SYSERR;
	}
	
	count = 0;
	buf[0] = 0;
	buf[1] = 0;
	write(radio, "ATCHB\r", 6);
	while(count == 0) {
	        count = read(radio, buf, 0);
	}
	if(buf[0] != 'O' || buf[1] != 'K') {
	        return SYSERR;
	}
	
	count = 0;
	buf[0] = 0;
	buf[1] = 0;
	write(radio, "ATMM1\r", 6);
	while(count == 0) {
	        count = read(radio, buf, 0);
	}
	if(buf[0] != 'O' || buf[1] != 'K') {
	        return SYSERR;
	}
	
	count = 0;
	buf[0] = 0;
	buf[1] = 0;
	write(radio, "ATMY0103\r", 9);
	while(count == 0) {
	        count = read(radio, buf, 0);
	}
	if(buf[0] != 'O' || buf[1] != 'K') {
	        return SYSERR;
	}

	count = 0;
	buf[0] = 0;
	buf[1] = 0;
	write(radio, "ATAP2\r", 6);
	while(count == 0) {
	        count = read(radio, buf, 0);
	}
	if(buf[0] != 'O' || buf[1] != 'K') {
	        return SYSERR;
	}
	
	count = 0;
	buf[0] = 0;
	buf[1] = 0;
	write(radio, "ATAC\r", 5);
	while(count == 0) {
	        count = read(radio, buf, 0);
	}
	if(buf[0] != 'O' || buf[1] != 'K') {
	        return SYSERR;
	}
	
	count = 0;
	buf[0] = 0;
	buf[1] = 0;
	write(radio, "ATWR\r", 5);
	while(count == 0) {
	        count = read(radio, buf, 0);
	}
	if(buf[0] != 'O' || buf[1] != 'K') {
	        return SYSERR;
	}
        
        count = 0;
        buf[0] = 0;
        buf[1] = 0;
        write(radio, "ATCN\r", 5);
        while(count == 0) {
	        count = read(radio, buf, 0);
	}
        
        resume(create(pan_listen, 1024, 20, "pan_listen", 1, radio));

	return OK;
}
