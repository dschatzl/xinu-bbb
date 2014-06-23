/* pan_radio.h - constants and function declarations for 802.15.4 communications */

#define PAN_DELIMITER		0x7E
#define PAN_ESCAPE		0x7D
#define PAN_XON			0x11
#define PAN_XOFF		0x13
#define PAN_MAX_PAYLOAD		100 	/* Max number of bytes per packet */

#define PAN_ERR_MAX_PAYLOAD	-1

#define PAN_API_TRANSMIT_16	0x01

struct panpkt {
	byte delimiter; /* The delimiter for a PAN packet */
	uint16 length; 	/* The length of the data, excluding checksum */
	byte command; 	/* The command that this packet refers to */
	byte checksum;	/* The checksum of the data: 0xFF - ((sum of bytes in data) & 0x00FF) */
	byte *data;	/* The packet payload */
};

int pan_write_packet(did32 radio, struct panpkt packet);
int pan_broadcast(did32 radio, char *data, int length);
int pan_read(did32 radio, char *buffer, int length);
