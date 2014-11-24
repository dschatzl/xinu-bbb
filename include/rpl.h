#include <xinu.h>

#define RPL_ICMP_TYPE 	155

/* RPL Control Message Codes - defined in RFC 6550, section 6 */
#define RPL_DIS 	0x00	/* DODAG Information Solicitation */
#define RPL_DIO 	0x01	/* DODAG Information Object */
#define RPL_DAO 	0X02	/* Destination Advertisement Object */
#define RPL_DAO_ACK	0x03	/* Destination Advertisement Object Acknowledgment */
#define S_RPL_DIS	0X80	/* Secure DODAG Information Solicitation */
#define S_RPL_DIO	0x81	/* Secure DODAG Information Object */
#define S_RPL_DAO	0x82	/* Secure Destination Advertisement Object */
#define S_RPL_DAO_ACK	0x83	/* Secure Destination Advertisement Object Acknowledgment */
#define RPL_CONS_CK	0x84	/* Consistency Check */

/* RPL Control Message Options */
#define RPL_DIS_PAD1	0x00
#define RPL_DIS_PADN	0x01
#define RPL_DIS_SOL	0x07

#define RPL_DIO_PAD1	0x00
#define RPL_DIO_PADN	0x01
#define RPL_DIO_DMC	0x02
#define RPL_DIO_RI	0x03
#define RPL_DIO_DC	0x04
#define RPL_DIO_PI	0x08

/* RPL Base Object Definitions */
struct rpl_dis_base 
{
	byte 	flags;
	byte 	reserved; /* To be ignored */
};

struct rpl_dio_base
{
	byte 	rpl_instance_id;
	byte 	version;
	int16 	rank;
	byte	g_mop_prf; /* byte with the format <Grounded>0<Mode of Operation - 3 bits><DODAG Preference - 3 bits> */
	byte	dtsn;
	byte	flags;
	byte	reserved; /* To Be Ignored */
	uint64	dodag_id_high;
	uint64	dodag_id_low;
};

struct rpl_dao_base
{
	byte	rpl_instance_id;
	byte	k_d_flags; /* byte with format <K><D><Flags - 6 bits> */
	byte	reserved; /* To Be Ignored */
	byte	dao_instance;
	uint64	dodag_id_high; /* Optional, if present D must be 1 */
	uint64	dodag_id_low; /* Optional, if present D must be 1 */
};
