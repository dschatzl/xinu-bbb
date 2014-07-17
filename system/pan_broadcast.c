/* pan_broadcast.c - pan_broadcast */

#include <xinu.h>

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

	// Add in room for API Identifier, frame ID, destination address and options
	escapedLength += 5;

	char escapedData[escapedLength];

        // For now, set the frame id to 1
        escapedData[0] = 1;
	// For now, the destination is 0xFFFF, which means broadcast
	escapedData[1] = 0xFF;
	escapedData[2] = 0xFF;

	// For now, disable ACK
	escapedData[3] = 0x01;

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
	packet.api_id = PAN_TRANSMIT_REQUEST_16;
	packet.length = escapedLength;
	packet.data = escapedData;
	pan_set_checksum(&packet);

	return pan_write_packet(radio, &packet);
}
