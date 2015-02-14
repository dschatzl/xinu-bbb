#include <xinu.h>

struct 	rpl_info rpl_current;
struct 	rpl_table rpl_tab;

uint32 	rpl_fill_options(char*, uint32, char*, uint32);
void	rpl_add_parent(const char*, uint32);
bool8	rpl_cmp_addr(struct ifipaddr, struct ifipaddr);

/* -------------------------------------------------------
 * rpl_init - Initializes global RPL variables
 * -------------------------------------------------------
 */
void rpl_init(void)
{
	memset(&rpl_current, 0, sizeof(struct rpl_info));

	/* The following sets the default information for border routers */
	byte g_mop_pref = 1; /* Set the G flag */
	g_mop_pref |= (RPL_MOP_NON_STORING << 3); /* Set the default non-storing mode of operation */
	g_mop_pref |= (RPL_DAG_LEAST_PREF << 5); /* By default, the root is least preferred */
	rpl_current.g_mop_preference = g_mop_pref;

	/* Set the DODAG ID to be the IPv6 address of the network interface */
	memcpy(rpl_current.dodag_id, if_tab[0].if_ipucast[0].ipaddr, 16);

	memset(&rpl_tab, 0, sizeof(byte) + sizeof(struct rpl_entry) * RPL_NODE_NUM);
}

/* --------------------------------------------------------
 * rpl_send_dis - sends out a DIS on the supplied interface
 * --------------------------------------------------------
 */
int32 	rpl_send_dis (
	int32 interface, /* Interface to send out a RPL DIO */
	byte dest[16]	/* Destination address */
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
	struct 	rpl_dio_base *base, 
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

/* ----------------------------------------------------------------
 * rpl_recv_dao - Receives one DAO, and fills opts_buf with options
 * with options up to len.  Modifies the rpl_tab to create a new 
 * entry or add a parent to an already existing entry.  If an entry
 * already has RPL_MAX_PARENTS, the parent with the matching 
 * preference is replaced.
 * ---------------------------------------------------------------- */
int32	rpl_recv_dao (
	int32	slot,		/* Slot in the ICMP table */
	uint32	timeout,	/* How long to listen for a DAO */
	struct rpl_dao_base *base,
	char	*opts_buf,	/* Where to store the DAO options */
	uint32	len		/* Length of the options buffer */
	)
{
	uint32 buf_size = len + sizeof(struct rpl_dao_base);
	char full_buf[buf_size];
	memset(full_buf, 0, buf_size);
	int32 bytes_recv = icmp_recv(slot, full_buf, buf_size, timeout);
	if(bytes_recv == SYSERR)
	{
		return SYSERR;
	}

	memcpy(base, full_buf, sizeof(struct rpl_dao_base));
	
	if(opts_buf != NULL)
	{
		uint32 bytes_written = rpl_fill_options(
				full_buf + sizeof(struct rpl_dao_base),
				bytes_recv - sizeof(struct rpl_dao_base),
				opts_buf, len);

		rpl_add_parent(opts_buf, bytes_written);

		return (bytes_written + sizeof(struct rpl_dao_base));
	} else
	{
		return sizeof(struct rpl_dao_base);
	}
}


/* ------------------------------------------------------------------
 * rpl_add_parent - Adds a parent to the target(s) indicated in 
 * buf. A new entry is created if a target is not in rpl_tab.  If 
 * a parent exists for the indicated preference, the old parent is 
 * replaced with the new parent. NB: This only supports 
 * ------------------------------------------------------------------ */
void	rpl_add_parent(
	const char	*buf,	/* The fill DAO option buffer */
	uint32		buf_len	/* The size of the buffer */
	)
{
	struct ifipaddr targets[RPL_MAX_TARGETS_PER_DAO];
	memset(targets, 0, RPL_MAX_TARGETS_PER_DAO * sizeof(struct ifipaddr));

	/* Find all the targets */
	uint32 counter = 0;
	byte target_counter = 0;
	byte opt_type = 0;
	byte opt_len = 0;
	do
	{
		opt_type = *(buf + counter);
		opt_len = *(buf + counter + sizeof(byte));
		
		/* All RPL options have at least two bytes - a type and a length */
		counter += sizeof(byte) + sizeof(byte);
		if(opt_type == RPL_OPT_RT)
		{
			/* The address length starts after one byte from the length */
			int addr_start = counter + sizeof(byte);
			byte addr_len = *(buf + addr_start);

			memcpy(&targets[target_counter++], (buf + addr_start), addr_len);
		}

		counter += opt_len;
	} while (opt_type != RPL_OPT_TI && counter < buf_len);

	/* If for some reason, we have reached the end (or close enough) to the 
		 end of the buffer, just leave -- POSSIBLE SOURCE OF WEIRD BUGS */	
	// TODO: Check this logic, because it is probably not correct
	if(counter >= buf_len)
	{
		return;
	}

	/* Find all the parents */
	struct rpl_parent parents[RPL_MAX_PARENTS];
	memset(parents, 0, RPL_MAX_PARENTS * sizeof(struct rpl_parent));
	byte parent_counter = 0;
	do
	{
		opt_type = *(buf + counter);
		opt_len = *(buf + counter + sizeof(byte));
		
		if(opt_type == RPL_OPT_TI)
		{
			/* The preference byte starts at a three byte offset from the 
				start of the option */
			uint32 opt_pos = counter + sizeof(byte) + sizeof(byte) + sizeof(byte);
			parents[parent_counter].preference = *(buf + opt_pos);
		
			/* The Path lifetime starts on one byte after the preference */
			opt_pos += sizeof(byte) + sizeof(byte);
			parents[parent_counter].path_lifetime = *(buf + opt_pos);
			/* The address starts at an offset of 4 bytes, and has a size of
				option length - offset */
			memcpy(&(parents[parent_counter].address), buf + opt_pos + sizeof(byte), opt_len - 4);
			parent_counter++;
		}

		counter += opt_len + sizeof(byte);
	} while (opt_type == RPL_OPT_TI && counter < buf_len);

	/* Find each target in rpl_tab */
	int i;
	for(i = 0; i < target_counter; i++)
	{
		int j;
		bool8 target_found = FALSE;
		for(j = 0; j < RPL_NODE_NUM; j++)
		{
			if(rpl_cmp_addr(rpl_tab.nodes[j].node, targets[i]) == TRUE)
			{
				target_found = TRUE;
				break;
			}		
		}

		if(target_found)
		{
			struct rpl_entry entry = rpl_tab.nodes[j];
			/* Replace node's parents with the new set */
			memcpy(entry.parents, parents, sizeof(parents));
			entry.num_parents = parent_counter;
		} else 
		{
			/* Add a new entry in rpl_tab */
			rpl_tab.nodes[rpl_tab.num_nodes].num_parents = parent_counter;
			memcpy(&(rpl_tab.nodes[rpl_tab.num_nodes].node), &(targets[i]), sizeof(struct ifipaddr));
			memcpy(&(rpl_tab.nodes[rpl_tab.num_nodes].parents), parents, sizeof(parents));
			rpl_tab.num_nodes++;
		}
	}
}

/* ------------------------------------------------------
 *  rpl_cmp_addr - Compares two IP addresses to see if 
 *	they route to the same exact address
 * ------------------------------------------------------ */
bool8	rpl_cmp_addr(
	struct ifipaddr 	addr1,
	struct ifipaddr 	addr2
	)
{
	if(addr1.ippreflen != addr2.ippreflen)
	{
		return FALSE;
	}

	int i;
	for(i = 0; i < addr1.ippreflen; i++)
	{
		if(addr1.ipaddr[i] != addr2.ipaddr[i])
		{
			return FALSE;
		}
	}

	return TRUE;
}
/* -------------------------------------------------------
 * recv_dao_ack - Receives DAO-ACK and copies as much of
 * the included options into the options buffer 
 * -------------------------------------------------------
 */
int32   rpl_recv_dao_ack (
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

/* ------------------------------------------------------------------------
 * rpl_send_dao - Constructs a DODAG Advertising Object, copies the options 
 * and sends it on the provided interface.
 * ------------------------------------------------------------------------ */
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
	memcpy(ipdata.ipdst, rpl_current.dodag_id, 16);
	/* TODO: How to get ipsrc? */

	return icmp_send(interface, RPL_ICMP_TYPE, RPL_DAO, &ipdata, data, len + sizeof(struct rpl_dao_base));
}

/* ---------------------------------------------------------------------------
 * rpl_send_dio - Constructs a DODAG Information Object, copies the options
 * and sends it on the provided interface.
 * --------------------------------------------------------------------------- */
int32	rpl_send_dio (
	int32 interface, 	/* The interface on which to send the DIO */
	char *options, 		/* The options to send along with the DIO */
	uint32 len,		/* The length of the options buffer */
	struct ipinfo *dest	/* Where to send the DIO */
	)
{
	struct rpl_dio_base base;
	memset(&base, 0, sizeof(struct rpl_dio_base));
	
	base.rpl_instance_id = rpl_current.rpl_instance_id;
	base.version = rpl_current.dodag_version;
	base.rank = rpl_current.rank;
	base.g_mop_prf = rpl_current.g_mop_preference;
	base.dtsn = rpl_current.dtsn;
	memcpy(base.dodag_id, rpl_current.dodag_id, 16);

	char data[len + sizeof(struct rpl_dio_base)];
	memcpy(data, &base, sizeof(struct rpl_dio_base));
	memcpy((data + sizeof(struct rpl_dio_base)), options, len);

	return icmp_send(interface, RPL_ICMP_TYPE, RPL_DIO, dest, data, len + sizeof(struct rpl_dio_base)); 
}
