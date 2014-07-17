/* pan_fill_packet.c - pan_fill_packet */

#include <xinu.h>

int pan_fill_packet_16(int api_id, uint16 storage_length, char *storage, uint16 data_length, char *data, char frame_id, uint16 destination, char disableAck, struct panpkt *packet) {
        // Find out length of data including escaped characters
	int escapedLength = data_length;
	int i;
	for(i = 0; i < data_length; i++) {
		if(pan_needs_escaping(data[i])) {
			escapedLength++;
		}
	}

	if(escapedLength > PAN_MAX_PAYLOAD) {
		return PAN_ERR_MAX_PAYLOAD;  // Should I return an error or only write the first 100 bytes?
	} else if(escapedLength > storage_length) {
	        return PAN_ERR_BUFFER_TOO_SMALL;
	}

	// Add in room for API Identifier, frame ID, destination address and options
	escapedLength += 5;
	
	storage[0] = frame_id;
	storage[1] = (destination & 0xFF00) >> sizeof(uint16);
	storage[2] = (destination & 0x00FF);
	
	// Escape all characters that need to
	int loc = 4;
	for(i = 0; i < data_length; i++) {
		if(pan_needs_escaping(data[i])) {
			storage[loc++] = PAN_ESCAPE;
			storage[loc++] = data[i] ^ 0x20; // As per spec, escaped characters are to be XOR'd with 0x20
		} else {
			storage[loc++] = data[i];
		}
	}
	
	packet->delimiter = PAN_DELIMITER;
	packet->api_id = api_id;
	packet->length = escapedLength;
	packet->data = storage;
	pan_set_checksum(packet);
	
	return OK;
}
