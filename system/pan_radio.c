/* pan_radio.c - pan_read(), pan_write() */

#include <xinu.h>

int pan_needs_escaping(char data) {
	return (data == PAN_DELIMITER ||
		data == PAN_ESCAPE ||
		data == PAN_XON ||
		data == PAN_XOFF);
}

void pan_set_checksum(struct panpkt *packet) {
	int i;
        int sum = packet->command;
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

int pan_read(did32 radio, struct panpkt *packet, int length) {
        char buf[100];
        
        int result = read(radio, buf, length);
        
        if(result <= 0) {
                return SYSERR;
        }
        
        
}

int pan_radio_init(did32 radio) {
        write(radio, "+++", 3);
        
        char buf[100];
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
	write(radio, "ATAP 2\r", 7);
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
	if(buf[0] != 'O' || buf[1] != 'K') {
	        return SYSERR;
	}
	
	return OK;
}

int pan_write_packet(did32 radio, const struct panpkt *packet) {
	char pk[3 + packet->length];

	pk[0] = packet->delimiter;
	pk[1] = (packet->length & 0xFF00) >> 2;
	pk[2] = packet->length & 0x00FF;
	pk[3] = packet->command;
	int i;
	for(i = 0; i < packet->length; i++) {
		pk[4 + i] = *(packet->data + i);
	}

	pk[3 + packet->length - 1] = packet->checksum;

        kprintf("About to write...");
	int result = write(radio, pk, sizeof(pk));
	kprintf("done: %d\n", result);
	struct panpkt_tx_status status;
	char buf[100];
	kprintf("About to read...");
	int bytesRead = read(radio, &buf, sizeof(status));
        kprintf("%d bytes read: ", bytesRead);
        int counter = 0;
        while(counter < bytesRead) {
                kprintf("%x ", buf[counter++]);
        }
	return result;
}

int pan_broadcast(did32 radio, char *data, int length) {
	// Find out length of data including escaped characters
	int escapedLength = length;
	int i;
	for(i = 0; i < length; i++) {
		if(pan_needs_escaping(data[i])) {
			escapedLength++;
		}
	}

	if(escapedLength > PAN_MAX_PAYLOAD) {
		return PAN_ERR_MAX_PAYLOAD;  // Should I return an error or only write the first 100 bytes?
	}

	// Add in room for checksum, API Identifier, frame ID, destination address and options
	escapedLength += 6;

	char escapedData[escapedLength];

        // For now, set the frame id to 1
        escapedData[0] = 1;
	// For now, the destination is 0xFFFF, which means broadcast
	escapedData[1] = 0xFF;
	escapedData[2] = 0xFF;

	// For now, disable ACK
	escapedData[3] = 0x0001;

	// Escape all characters that need to
	int loc = 4;
	for(i = 0; i < length; i++) {
		if(pan_needs_escaping(data[i])) {
			escapedData[loc++] = PAN_ESCAPE;
			escapedData[loc++] = data[i] ^ 0x20; // As per spec, escaped characters are to be XOR'd with 0x20
		} else {
			escapedData[loc++] = data[i];
		}
	}

	// Construct the PAN packet and write it to the radio
	struct panpkt packet;
	packet.delimiter = PAN_DELIMITER;
	packet.command = PAN_API_TRANSMIT_16;
	packet.length = escapedLength;
	packet.data = escapedData;
	pan_set_checksum(&packet);

	return pan_write_packet(radio, &packet);
}
