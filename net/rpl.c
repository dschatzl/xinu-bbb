#include <xinu.h>

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
	
	/* Fill out the options buffer if there are options */
	if(opts_buf != NULL)
	{
		int32 bytes_remaining = bytes_recv - sizeof(struct rpl_dio_base);
		uint32 buf_position = sizeof(struct rpl_dio_base);
		uint32 bytes_written = 0;

		while(bytes_remaining > 0 && bytes_written < len)
		{
			byte option_type = full_buf[buf_position];
			switch(option_type)
			{
				case RPL_DIS_PAD1:
					buf_position += sizeof(byte);
					break;
				case RPL_DIS_PADN:
					char pad_len = full_buf[buf_position + sizeof(byte)];
					/* The additional 1 comes from skipping over type */
					buf_position += (pad_len + 1) * sizeof(byte);
					break;
				default:
					char opt_len = full_buf[buf_position + sizeof(byte)];
					/* Make sure there is room in opts_buf */
					if(bytes_written + opt_len < len)
					{
						memcpy((opts_buf + bytes_written), (full_buf + buf_position), opt_len);
					}
					bytes_written += opt_len;
					/* The additional 1 comes from skipping over type */
					buf_position += (opt_len + 1) * sizeof(byte);
			}
		}
	}

	return bytes_recv;
}
