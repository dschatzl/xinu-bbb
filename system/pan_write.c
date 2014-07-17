/* pan_radio_write.c -  */

#include <xinu.h>

int pan_needs_escaping(char data) {
	return (data == PAN_DELIMITER ||
		data == PAN_ESCAPE ||
		data == PAN_XON ||
		data == PAN_XOFF);
}

int pan_write_packet(did32 radio, const struct panpkt *packet) {
	char pk[4 + packet->length];

	pk[0] = packet->delimiter;
	pk[1] = (packet->length & 0xFF00) >> 2;
	pk[2] = packet->length & 0x00FF;
	pk[3] = packet->api_id;
	int i;
	for(i = 0; i < packet->length - 1; i++) {
		pk[4 + i] = *(packet->data + i);
	}

	pk[4 + packet->length - 1] = packet->checksum;

	int result = write(radio, pk, sizeof(pk));

	return result;
}
