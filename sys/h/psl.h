/*	psl.h	4.1	11/9/80	*/

/*
 * VAX program status longword
 */

#define	PSL_C		0x1		/* carry bit */
#define	PSL_V		0x2		/* overflow bit */
#define	PSL_Z		0x4		/* zero bit */
#define	PSL_N		0x8		/* negative bit */
#define	PSL_ALLCC	0xf		/* all cc bits - unlikely */
#define	PSL_T		0x10		/* trace enable bit */
#define	PSL_IV		0x20		/* integer overflow enable bit */
#define	PSL_FU		0x40		/* floating point underflow enable */
#define	PSL_DV		0x80		/* decimal overflow enable bit */
#define	PSL_IPL		0x1f0000	/* interrupt priority level */
#define	PSL_PRVMOD	0xc00000	/* previous mode (all on is user) */
#define	PSL_CURMOD	0x3000000	/* current mode (all on is user) */
#define	PSL_IS		0x4000000	/* interrupt stack */
#define	PSL_FPD		0x8000000	/* first part done */
#define	PSL_TP		0x40000000	/* trace pending */
#define	PSL_CM		0x80000000	/* compatibility mode */
#define	PSL_USERCLR	0x3c20ff00	/* bits must be clear in user mode */
