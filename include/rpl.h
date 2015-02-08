/* rpl.h - definitions for the Routing Protocol for Low-power, Lossy Networks */

#define RPL_ICMP_TYPE 		155

#define RPL_NODE_NUM		16	/* TODO: Is this a good number of nodes to support */

#define RPL_MAX_PARENTS		4	/* This is specified by the Path Control field in the Transit Information Option in section 6.7.8 of RFC 6550 */
#define	RPL_MAX_TARGETS_PER_DAO	4	/* Used in rpl_recv_dao, and set arbitrarily */

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
#define RPL_OPT_PAD1		0x00	/* Pad 1 */
#define RPL_OPT_PADN		0x01	/* Pad N */
#define RPL_OPT_SOL		0x07	/* Solicited Information */
#define RPL_OPT_DMC		0x02	/* DAG Metric Container */
#define RPL_OPT_RI		0x03	/* Routing Information */
#define RPL_OPT_DC		0x04	/* DODAG Configuration */
#define RPL_OPT_PI		0x08	/* Prefix Information */
#define RPL_OPT_RT		0x05	/* RPL Target */
#define RPL_OPT_TI		0x06	/* Transit Information */
#define RPL_OPT_RTD		0x09	/* RPL Target Descriptor */

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

/* RPL Modes of Operation */
#define RPL_MOP_NO_DN_RTS	0	/* No downward routes maintained by RPL */
#define RPL_MOP_NON_STORING	1	/* Non-Storing mode of operation */
#define RPL_MOP_STORE_NO_MCAST	2	/* Storing Mode with no Multicast Support */
#define RPL_MOP_STORE_MCAST	3	/* Storing Mode with Multicast Support */

/* DODAG Preference Values */
#define RPL_DAG_MOST_PREF	0x07	/* Most preferred amoung DODAG roots */
#define RPL_DAG_LEAST_PREF	0x00	/* Least preferred amoung DODAG roots */

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
	byte		dodag_id[16];
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
	byte		d_flag_reserved;	/* Lowest order bit describes if DODAG ID is present (will be 0 for now) and the rest are to be ignored */
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
	byte		dodag_id[16];	/* Identifier of the DODAG root */
	uint32		dest_counter;	/* Destination Counter */
};

/* RPL State Structs */
struct rpl_info
{
	byte		rpl_instance_id;
	byte		dodag_id[16];
	byte		dodag_version;
	uint16		rank;
	byte		dao_sequence;
	byte		g_mop_preference;
	byte		dtsn;
};

struct rpl_parent
{
	ifipaddr 	address;
	byte 		preference;
	uint32		path_lifetime;
};

/* RPL Node Entry */
struct rpl_entry
{
	ifipaddr 		node;			/* The node that this entry refers to */
	struct rpl_parent 	parents[RPL_MAX_PARENTS];	/* The parents of this node */
	byte			num_parents;		/* The number of parents this entry has */
};

struct rpl_table
{
	rpl_entry 		nodes[RPL_NUM_NODES];
	byte			num_nodes;
};

extern struct rpl_table rpl_tab[RPL_NUM_NODES];

/* Global current RPL State */
extern struct rpl_info rpl_current;

/* RPL Functions */
void	rpl_init(void);
int32	rpl_send_dis(int32, byte*);
int32	rpl_recv_dio(int32, uint32, struct rpl_dio_base*, char*, uint32);
int32	rpl_send_dao(int32, bool8, char*, uint32);
int32	rpl_recv_dao(int32, uint32, struct rpl_dao_base*, char*, uint32);
int32	rpl_send_dio(int32, char*, uint32, struct ipinfo*);
int32	rpl_recv_dao_ack(int32, uint32, struct rpl_dao_ack_base*, char*, uint32);
