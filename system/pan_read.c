/* pan_read.c - pan_read_at, pan_read_rx */

#include <xinu.h>

int pan_read_buffer(did32 radio, struct panpkt *packet, int length, char *src, int start) {
        char delimiter = src[start];
        
        if(delimiter != PAN_DELIMITER) {
                return SYSERR;
        }
        
        int j;
        for(j = 0; j < 14; j++) {
                kprintf("%02X ", src[j]);
        }
        
        int currentPosition = (start + sizeof(char));
        uint16 pktLength = (src[currentPosition] << 2) | src[(currentPosition + sizeof(char))];
        currentPosition += sizeof(uint16);
        
        if(pktLength > length) {
                return PAN_ERR_BUFFER_TOO_SMALL;
        }
        
        packet->delimiter = delimiter;
        packet->length = pktLength;
        packet->api_id = src[currentPosition];
        currentPosition = (currentPosition + sizeof(char));
        
        int i;
        for(i = 0; i < pktLength - 1; i++) {
                packet->data[i] = src[currentPosition];
                currentPosition = (currentPosition + sizeof(char));
        }
        
        packet->checksum = src[currentPosition];
        
        return OK;
}

int pan_read_at(did32 radio, struct panpkt *packet, int length) {
        wait(pan_at_sem);
        int start = pan_radio_at_head;
        
        int result = pan_read_buffer(radio, packet, length, pan_at_buf, start);
        if(result) {
                pan_radio_at_head = (pan_radio_at_head + length + PAN_ZERO_PAYLOAD_SIZE) % PAN_BUFFER_LENGTH;
        }
        
        return result;
}

int pan_read_rx(did32 radio, struct panpkt *packet, int length) {
        wait(pan_rx_sem);
        int start = pan_radio_rx_head;
        
        int result = pan_read_buffer(radio, packet, length, pan_rx_buf, start);
        if(result) {
                pan_radio_rx_head = (pan_radio_rx_head + length + PAN_ZERO_PAYLOAD_SIZE);
        }
        
        return result;
}
