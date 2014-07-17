/* pan_radio_listener - pan_listen */

#include <xinu.h>

int pan_write_packet_to_buffer(char *src, char *dest, int length) {
        int destIdx;
        if(dest == pan_rx_buf) {
                destIdx = pan_radio_rx_tail;
        } else if(dest == pan_at_buf) {
                destIdx = pan_radio_at_tail;
        } else {
                return SYSERR;
        }
        int srcIdx;
        for(srcIdx = 0; srcIdx < length; srcIdx++) {
                dest[destIdx] = src[srcIdx];
                destIdx = destIdx + 1;
                if(destIdx >= PAN_BUFFER_LENGTH) {
                        destIdx = 0;
                }
        }
        
        if(dest == pan_rx_buf) {
                pan_radio_rx_tail = destIdx;
        } else if(dest == pan_at_buf) {
                pan_radio_at_tail = destIdx;
        }
        
        return OK;
}

void pan_listen(did32 radio) {
        char buf[PAN_MAX_PACKET_SIZE_64];
        while(1) {
                char delimiter;
                int count = read(radio, &delimiter, sizeof(char));
                if(delimiter != PAN_DELIMITER) {
                        continue;
                }
                
                buf[0] = delimiter;
                uint16 length;
                count = read(radio, &length, sizeof(uint16));
                buf[1] = (length & 0xFF00) >> 2;
                buf[2] = (length & 0x00FF);
                length = ntohs(length);
                
                char api_identifier = 0;
                count = read(radio, &api_identifier, sizeof(char));
                buf[3] = api_identifier;

                memset(&buf[4], 0, length - 1);
                count = read(radio, &buf[4], length - 1); // Length includes the API Identifier, but we've already read that part
                if(count != length - 1) {
                        // Malformed packet...ignore
                        continue;
                }
                
                char checksum = 0;
                count = read(radio, &checksum, sizeof(char));
                int verify = pan_verify_checksum(checksum, &buf[3], length);
                if(verify != 0) {
                        // Malformed packet...ignore
                        continue;
                }                
                
                if(api_identifier == PAN_AT_COMMAND_RESPONSE) {
                        if(pan_write_packet_to_buffer(buf, pan_at_buf, length + 4)) {
                                signal(pan_at_sem);
                        }
                } else if(api_identifier == PAN_RECEIVE_64 || api_identifier == PAN_RECEIVE_16) {
                        if(pan_write_packet_to_buffer(buf, pan_rx_buf, length + 4)) {
                                signal(pan_rx_sem);
                        }
                } else {
                        // Unexpected identifier...ignore
                        continue;
                }             
        }
}
