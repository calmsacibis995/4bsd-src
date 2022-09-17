/* Copyright (c) 1979 Regents of the University of California */

static char sccsid[] = "@(#)PRED.c 1.1 10/29/80";

#include "h01errs.h"

PRED(value, lower, upper)

	int	value;
	int	lower;
	int	upper;
{
	value--;
	if (value < lower || value > upper) {
		ERROR(ERANGE, value);
		return;
	}
	return	value;
}
