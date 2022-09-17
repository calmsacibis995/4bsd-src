/* Copyright (c) 1979 Regents of the University of California */

static char sccsid[] = "@(#)RELTGT.c 1.1 10/29/80";

#include "h00vars.h"

RELTGT(bytecnt, left, right)

	int		bytecnt;
	register long	*left;
	register long	*right;
{
	register int	longcnt;

	longcnt = bytecnt >> 2;
	do	{
		if ((*right & ~*left) != 0)
			return FALSE;
		if ((*left++ & ~*right++) != 0)
			goto geq;
	} while (--longcnt);
	return FALSE;
geq:
	while (--longcnt) {
		if ((*right++ & ~*left++) != 0) 
			return FALSE;
	}
	return TRUE;
}
