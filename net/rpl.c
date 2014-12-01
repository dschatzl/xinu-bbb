#include <xinu.h>

struct rpl_info rpl_current;

uint32 rpl_fill_options(char*, uint32, char*, uint32);

/* -------------------------------------------------------
 * rpl_init - Initializes global RPL variables
 * -------------------------------------------------------
 */
void rpl_init(void)
{
	memset(&rpl_current, 0, sizeof(struct rpl_info));
}

/* --------------------------------------------------------
 * rpl_send_dis - sends out a DIS on the supplied interface
 * --------------------------------------------------------
 */
int32 	rpl_send_dis (
	int32 interface /* Interface to send out a RPL DIO */
	)
{
	struct rpl_dis_base dis;

	/* No solicited information option used
	 * because we assume only one DODAG for now */
	memset(&dis, 0, sizeof(struct rpl_dis_base));

	struct ipinfo ipdata;
	memset(&ipdata, 0, sizeof(struct ipinfo));

	/* Set the destination to be all-RPL-nodes multicast address 
	 * Defined in RFC 6550, Section 6 */
	ipdata.ipdst[15] = 0xFF;
	ipdata.ipdst[14] = 0x02;
	ipdata.ipdst[0] = 0x1A;
	/* TODO: How do I set ipdata.ipsrc? */

	return icmp_send(interface, RPL_ICMP_TYPE, RPL_DIS, &ipdata, (char*)&dis, sizeof(dis));
}

/* --------------------------------------------------------
 * rpl_recv_dio - Receives one DIO and fills opts_buf 
 * with options up to len 
 * -------------------------------------------------------
 */
int32 	rpl_recv_dio (
	int32 	slot,		/* Slot in the ICMP table */
	uint32	timeout, 	/* How long to listen for a DIO */
	struct rpl_dio_base *base, 
	char	*opts_buf, 	/* Where to store the DIO options */
	uint32	len 		/* Length of the options buffer */
	)
{
	uint32 buf_size = len + sizeof(struct rpl_dio_base);
	char full_buf[buf_size];
	memset(full_buf, 0, buf_size);
	int32 bytes_recv = icmp_recv(slot, full_buf, buf_size, timeout);
	if(bytes_recv == SYSERR)
	{
		return SYSERR;
	}

	memcpy(base, full_buf, sizeof(struct rpl_dio_base));
	
	if(opts_buf != NULL)
	{
		uint32 bytes_written = rpl_fill_options(
					(full_buf + sizeof(struct rpl_dio_base)), 
					bytes_recv - sizeof(struct rpl_dio_base),
					opts_buf, len);

		return (bytes_written + sizeof(struct rpl_dio_base));
	} else 
	{
		return sizeof(struct rpl_dio_base);
	}
}

/* -------------------------------------------------------
 * recv_dao_ack - Receives DAO-ACK and copies as much of
 * the included options into the options buffer 
 * -------------------------------------------------------
 */
int32   recv_dao_ack (
        int32                   slot,		/* The ICMP slot */
	uint32                  timeout,	/* How long to wait before timeout */
	struct rpl_dao_ack_base *base,		/* RPL DAO-ACK base */
	char                    *opts_buf,	/* RPL Options buffer */
	uint32                  len		/* The length of opts_buf */
	)
{
	uint32 buf_size = len + sizeof(struct rpl_dao_ack_base);
	char full_buf[buf_size];
	memset(full_buf, 0, buf_size);
	int32 bytes_recv = icmp_recv(slot, full_buf, buf_size, timeout);
	if(bytes_recv == SYSERR)
	{
		return SYSERR;
	}

	memcpy(base, full_buf, sizeof(struct rpl_dao_ack_base));

	if(opts_buf != NULL)
	{
		uint32 bytes_written = rpl_fill_options(
					full_buf + sizeof(struct rpl_dao_ack_base),
					bytes_recv - sizeof(struct rpl_dao_ack_base),
					opts_buf, len);
		return (bytes_written + sizeof(struct rpl_dao_ack_base));
	} else 
	{
		return sizeof(struct rpl_dao_ack_base);	
	}
}

/* -------------------------------------------------------------
 * rpl_fill_options - Fills the buffer with all options,
 * except for pad options
 * -------------------------------------------------------------
 */
uint32	rpl_fill_options (
	char *opt_src, 	/* Option source buffer */
	uint32 src_len, /* Option source buffer length */
	char *opt_dst, 	/* Option destination buffer */
	uint32 dst_len	/* Option destination buffer length */
	)
{
	uint32 bytes_written = 0;
	uint32 buf_position = 0;
	int32 bytes_remaining = src_len;
	while(bytes_remaining > 0 && bytes_written < dst_len)
	{
		byte option_type = opt_src[buf_position];
		byte opt_len;
		switch(option_type)
		{
			case RPL_OPT_PAD1:
				buf_position++;
				bytes_remaining--;
				break;
			case RPL_OPT_PADN:
				opt_len = opt_src[buf_position + 1];
				/* Skip the pad bytes.
				 * Note that pad_len encompasses the number of blank octets
				 * as well as the option type and length bytes.
				 */
				buf_position += opt_len;
				bytes_remaining -= opt_len;
				break;
			default:
				opt_len = opt_src[buf_position + 1];
				if((bytes_written + opt_len + 2) < dst_len)
				{
					/* Copy the option type, length and the actual option value */
					memcpy((opt_dst + bytes_written), (opt_src + buf_position), opt_len + 2);
					bytes_written += opt_len + 2;
					bytes_remaining -= opt_len + 2;
					buf_position += opt_len + 2;
				} else 
				{
					/* return when the option can't be copied to the buffer */
					return bytes_written;
				}
		}

	}

	return bytes_written;
}

int32   rpl_send_dao (  
	int32   interface,      /* The interface on which to send the DAO */
	bool8   expect_ack,     /* True if expecting an DAO-ACK */                                     
	char    *options,       /* The options to send along with the DAO */
	uint32  len             /* The length of the options buffer */                                 
	)
{
	struct rpl_dao_base base;                                                                      
	memset(&base, 0, sizeof(struct rpl_dao_base));                                                 
	base.rpl_instance_id = rpl_current.rpl_instance_id;
	base.dao_instance = rpl_current.dao_sequence++;

	if(expect_ack)
	{
		base.k_d_flags = 1;
		/* The D flag is never set because we aren't supporting the optional DODAG ID for now */
	}

	char data[len + sizeof(struct rpl_dao_base)];
	memcpy(data, &base, sizeof(struct rpl_dao_base));
	memcpy((data + sizeof(struct rpl_dao_base)), options, len);

	struct ipinfo ipdata;
	memset(&ipdata, 0, sizeof(struct ipinfo));
	memcpy(ipdata.ipdst, &(rpl_current.dodag_id), 16);
	/* TODO: How to get ipsrc? */

	return icmp_send(interface, RPL_ICMP_TYPE, RPL_DAO, &ipdata, data, len + sizeof(struct rpl_dao_base));
}
