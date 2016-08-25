/* tcpsynrcvd.c  -  tcpsynrcvd */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpsynrcvd  -  Handle an incoming segment in state SYN-RECEIVED
 *------------------------------------------------------------------------
 */
int32	tcpsynrcvd(
	  struct tcb	*tcbptr,	/* Ptr to a TCB			*/
	  struct netpacket *pkt		/* Ptr to a packet		*/
	)
{
	/* Check for ACK or bas sequence number */

	if (!(pkt->net_tcpcode & TCPF_ACK)
	    || pkt->net_tcpseq != tcbptr->tcb_rnext) {
		return SYSERR;
	}

	/* The following increment of the refernce count is for	*/
	/*    the system because the TCB is now synchronized	*/

	tcbref (tcbptr);

	signal(tcbptr->tcb_rblock);

	/* Change state to ESTABLISHED */

	tcbptr->tcb_state = TCB_ESTD;

	/* Move past SYN */

	tcbptr->tcb_suna++;
	if (tcpdata (tcbptr, pkt)) {
		tcpxmit (tcbptr);
	}

	/* Permit reading */

	if (tcbptr->tcb_readers) {
		tcbptr->tcb_readers--;
		signal (tcbptr->tcb_rblock);
	}

	return OK;
}
