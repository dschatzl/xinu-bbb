/* pan_radio.c - pan_read(), pan_write() */

#include <xinu.h>

int pan_needs_escaping(char data) {
	return (data == PAN_DELIMITER ||
		data == PAN_ESCAPE ||
		data == PAN_XON ||
		data == PAN_XOFF);
}

byte pan_calculate_checksum(char* data, int length) {
	int i;
	uint8 value;
	for(i = 0; i < length; i++) {
		value += data[i];
	}

	return (0xFF - value);
}

int pan_write_packet(did32 radio, struct panpkt packet) {
	char pk[5 + packet.length];

	pk[0] = packet.delimiter;
	pk[1] = packet.length >> 2;
	pk[2] = packet.length & 0x00FF;
	pk[3] = packet.command;
	int i;
	for(i = 0; i < packet.length; i++) {
		pk[4 + i] = *(packet.data + i);
	}

	pk[5 + packet.length - 1] = packet.checksum;

	return write(radio, pk, sizeof(pk));
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

	// Add in room for destination address and options
	escapedLength += 3;

	char escapedData[escapedLength];

	// For now, the destination is 0xFFFF, which means broadcast
	escapedData[0] = 0xFF;
	escapedData[1] = 0xFF;

	// For now, options are all zero
	escapedData[2] = 0;

	// Escape all characters that need to
	int loc = 3;
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
	packet.checksum = pan_calculate_checksum(escapedData, escapedLength);

	return pan_write_packet(radio, packet);
}
