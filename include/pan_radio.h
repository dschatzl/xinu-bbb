/* pan_radio.h - constants and function declarations for 802.15.4 communications */
#ifndef PAN_RADIO_H
#define PAN_RADIO_H

#define PAN_DELIMITER		        0x7E
#define PAN_ESCAPE		        0x7D
#define PAN_XON			        0x11
#define PAN_XOFF		        0x13
#define PAN_MAX_PAYLOAD		        100 	/* Max number of bytes per packet */
#define PAN_MAX_PACKET_SIZE_64          15 + PAN_MAX_PAYLOAD
#define PAN_MAX_PACKET_SIZE_16          9 + PAN_MAX_PAYLOAD
#define PAN_ZERO_PAYLOAD_SIZE           sizeof(char) * 2 + sizeof(uint16)

#define PAN_BUFFER_LENGTH               3 * PAN_MAX_PACKET_SIZE_64

/* PAN API Identifiers */
#define PAN_MODEM_STATUS                0x8A
#define PAN_AT_COMMAND                  0x08
#define PAN_QUEUE_PARAMETER             0x09
#define PAN_AT_COMMAND_RESPONSE         0x88
#define PAN_REMOTE_COMMAND_REQUEST      0x17
#define PAN_REMOTE_COMMAND_RESPONSE     0x97
#define PAN_TRANSMIT_REQUEST_64         0x00
#define PAN_TRANSMIT_REQUEST_16         0x01
#define PAN_TRANSMIT_STATUS             0x89
#define PAN_RECEIVE_64                  0x80
#define PAN_RECEIVE_16                  0x81
#define PAN_RECEIVE_IO_64               0x82
#define PAN_RECEIVE_IO_16               0x83

#define PAN_ERR_MAX_PAYLOAD	        -1
#define PAN_ERR_BUFFER_TOO_SMALL        -2

struct panpkt {
	byte delimiter; /* The delimiter for a PAN packet */
	uint16 length; 	/* The length of the data, excluding checksum */
	byte api_id; 	/* The API ID that this packet refers to */
	byte checksum;	/* The checksum of the data: 0xFF - ((sum of bytes in data) & 0x00FF) */
	byte *data;	/* The packet payload */
};

struct panpkt_tx_status {
        byte delimiter;
        uint16 length;
        byte api_identifier;
        byte frame_id;
        byte status;
        byte checksum;
};

char pan_rx_buf[PAN_BUFFER_LENGTH];
char pan_at_buf[PAN_BUFFER_LENGTH];
sid32 pan_rx_sem;
uint32 pan_radio_rx_head;
uint32 pan_radio_rx_tail;

sid32 pan_at_sem;
uint32 pan_radio_at_head;
uint32 pan_radio_at_tail;

/* Calculates the checksum and sets the checksum value in packet */
void pan_set_checksum(struct panpkt *packet);

/* Writes the packet to the radio, and returns OK */
int pan_write_packet(did32 radio, const struct panpkt *packet);

/* Broadcast data to all radios nearby */
int pan_broadcast(did32 radio, char *data, int length);

/* Read from the AT Command response buffer */
int pan_read_at(did32 radio, struct panpkt *packet, int length); 

/* Read from the Receive response buffer */
int pan_read_rx(did32 radio, struct panpkt *packet, int length);

/* Creates the receive and AT semaphores and sets the radio to API mode 2 */
int pan_radio_init(did32 radio);

/* Listens to the radio and populates the correct PAN radio buffer 
   NB: This is intended to be run in its own process and only one process
   calls this function */
void pan_listen(did32 radio);

/* Returns true if data needs to be escaped */
int pan_needs_escaping(char data);

/* Returns false if the checksum is correct for the provided data */
int pan_verify_checksum(byte checksum, char *data, uint16 length);

/* Fills the provided packet with the correct information. 
NB: The length should be the available size of the storage, and should be enough to hold any potential escaped characters */
int pan_fill_packet_16(int api_id, uint16 storage_length, char *storage, uint16 data_length, char *data, char frame_id, uint16 destination, char disableAck, struct panpkt *packet);

#endif
