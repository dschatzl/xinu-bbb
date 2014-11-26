#include <xinu.h>

#define RPL_ICMP_TYPE 		155

/* RPL Control Message Codes - defined in RFC 6550, section 6 */
#define RPL_DIS 		0x00	/* DODAG Information Solicitation */
#define RPL_DIO 		0x01	/* DODAG Information Object */
#define RPL_DAO 		0X02	/* Destination Advertisement Object */
#define RPL_DAO_ACK		0x03	/* Destination Advertisement Object Acknowledgment */
#define S_RPL_DIS		0X80	/* Secure DODAG Information Solicitation */
#define S_RPL_DIO		0x81	/* Secure DODAG Information Object */
#define S_RPL_DAO		0x82	/* Secure Destination Advertisement Object */
#define S_RPL_DAO_ACK		0x83	/* Secure Destination Advertisement Object Acknowledgment */
#define RPL_CONS_CK		0x84	/* Consistency Check */

/* RPL Control Message Options */
#define RPL_DIS_PAD1		0x00	/* Pad 1 */
#define RPL_DIS_PADN		0x01	/* Pad N */
#define RPL_DIS_SOL		0x07	/* Solicited Information */

#define RPL_DIO_PAD1		0x00	/* Pad 1 */
#define RPL_DIO_PADN		0x01	/* Pad N */
#define RPL_DIO_DMC		0x02	/* DAG Metric Container */
#define RPL_DIO_RI		0x03	/* Routing Information */
#define RPL_DIO_DC		0x04	/* DODAG Configuration */
#define RPL_DIO_PI		0x08	/* Prefix Information */

#define RPL_DAO_PAD1		0x00	/* Pad 1 */
#define RPL_DAO_PADN		0x01	/* Pad N */
#define RPL_DAO_RT		0x05	/* RPL Target */
#define RPL_DAO_TI		0x06	/* Transit Information */
#define RPL_DAO_RTD		0x09	/* RPL Target Descriptor */

#define RPL_CC_PAD1		0x00	/* Pad 1 */
#define RPL_CC_PADN		0x01	/* Pad N */

/* RPL Constants - Defined in RFC 6550, Section 17 */
#define RPL_BASE_RANK		0
#define RPL_INF_RANK		0xFFFF	/* Maximum rank a node can have */
#define RPL_DEF_INST		0	/* RPL Instance ID used by a node without an overriding policy */
#define RPL_DEF_PATH_CTL_SZ	0	/* RPL Default Path Control Size */
#define RPL_DEF_DIO_INT_MIN	3 	/* RPL Default DIO Interval Min */
#define RPL_DEF_DIO_INT_DOUBL	20	/* RPL Default DIO Interval Doublings */
#define RPL_DEF_DIO_RED_CONST	10	/* RPL Default DIO Redundancy Constant */
#define RPL_DEF_MIN_HOP_RK_INC	256	/* RPL Default Min Hop Rank Increase */
#define RPL_DEF_DAO_DELAY	1	/* RPL Default DAO Deplay */

/* RPL Base Object Definitions */
struct rpl_dis_base 
{
	byte 	flags;
	byte 	reserved; /* To be ignored */
};

struct rpl_dio_base
{
	byte 		rpl_instance_id;
	byte 		version;
	int16 		rank;
	byte		g_mop_prf; 	/* byte with the format <Grounded>0<Mode of Operation - 3 bits><DODAG Preference - 3 bits> */
	byte		dtsn;		/* Destination Advertisement Trigger Sequence Number */
	byte		flags;
	byte		reserved; 	/* To Be Ignored */
	byte[16]	dodag_id;
};

struct rpl_dao_base
{
	byte		rpl_instance_id;
	byte		k_d_flags; 	/* byte with format <K><D - DODAG ID Presents, will always be 0><Flags - 6 bits> */
	byte		reserved; 	/* To Be Ignored */
	byte		dao_instance;
	/* NB: RFC 6550 also allows for an optional DODAG ID to be specified,
	 * but we are not supporting that at this time since we are assuming
	 * that only one DODAG will be present at once */
};

struct rpl_dao_ack_base
{
	byte		rpl_instance_id;
	byte		d_flag_reserved	/* Lowest order bit describes if DODAG ID is present (will be 0 for now) and the rest are to be ignored */
	byte		dao_instance;
	byte		status;
        /* NB: RFC 6550 also allows for an optional DODAG ID to be specified,
	 * but we are not supporting that at this time since we are assuming
	 * that only one DODAG will be present at once */
};

struct rpl_consistency_check_base
{
	byte		rpl_instance_id;
	byte		r_flags;	/* Lowest order bit is 1 if message is a response, 0 otherwise. The rest are reserved for flags, but must be set to 0 by sender */
	uint16		cc_nonce;	/* Set by request, and is the same for the corresponding response */
	byte[16]	dodag_id;	/* Identifier of the DODAG root */
	uint32		dest_counter;	/* Destination Counter */
};
