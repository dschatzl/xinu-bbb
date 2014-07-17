/* pan_checksum - pan_set_checksum, pan_verify_checksum */

#include <xinu.h>

void pan_set_checksum(struct panpkt *packet) {
	int i;
        int sum = packet->api_id;
        // The negative 1 comes from the fact that the length includes the command
        for(i = 0; i < packet->length - 1; i++) {
                sum += packet->data[i];
        }
        
        sum = (sum & 0x00FF);
        char checksum = (0xFF - sum);
        if(pan_needs_escaping(checksum)) {
                // TODO What to do here?
        }
        packet->checksum = checksum;
}

int pan_verify_checksum(byte checksum, char *data, uint16 length) {
        int sum = 0;
        
        int i;
        for(i = 0; i < length; i++) {
                sum += data[i];
        }
        
        sum += checksum;
        
        return (sum == 0xFF);
}
