static	char sccsid[] = "%Z%%M% %I% %G%";
/*
 * adb - main command loop and error/interrupt handling
 */
#include "defs.h"

MSG		NOEOR;

INT		mkfault;
INT		executing;
INT		infile;
CHAR		*lp;
L_INT		maxoff;
L_INT		maxpos;
ADDR		sigint;
ADDR		sigqit;
INT		wtflag;
L_INT		maxfile;
STRING		errflg;
L_INT		exitflg;

CHAR		lastc;
INT		eof;

INT		lastcom;

long	maxoff = MAXOFF;
long	maxpos = MAXPOS;

main(argc, argv)
	register char **argv;
	int argc;
{

	mkioptab();
	while (argc>1 && eqstr("-w", argv[1])) {
		wtflag = 2;		/* suitable for open() */
		argc--, argv++;
	}
	if (argc > 1)
		symfil = argv[1];
	if (argc > 2)
		corfil = argv[2];
	xargc = argc;
	setsym(); setcor(); setvar();

	if ((sigint=signal(SIGINT,SIG_IGN)) != SIG_IGN) {
		sigint = fault;
		signal(SIGINT, fault);
	}
	sigqit = signal(SIGQUIT, SIG_IGN);
	setexit();
	if (executing)
		delbp();
	executing = 0;
	for (;;) {
		flushbuf();
		if (errflg) {
			printf("%s\n", errflg);
			exitflg = errflg;
			errflg = 0;
		}
		if (mkfault) {
			mkfault=0;
			printc('\n');
			prints(DBNAME);
		}
		lp=0; rdc(); lp--;
		if (eof) {
			if (infile) {
				iclose(); eof=0; reset();
			} else
				done();
		} else
			exitflg = 0;
		command(0, lastcom);
		if (lp && lastc!='\n')
			error(NOEOR);
	}
}

done()
{
	endpcs();
	exit(exitflg);
}

L_INT
round(a,b)
REG L_INT a, b;
{
	REG L_INT w;
	w = (a/b)*b;
	IF a!=w THEN w += b; FI
	return(w);
}

/*
 * If there has been an error or a fault, take the error.
 */
chkerr()
{
	if (errflg || mkfault)
		error(errflg);
}

/*
 * An error occurred; save the message for later printing,
 * close open files, and reset to main command loop.
 */
error(n)
	char *n;
{
	errflg = n;
	iclose(); oclose();
	reset();
}

/*
 * An interrupt occurred; reset the interrupt
 * catch, seek to the end of the current file
 * and remember that there was a fault.
 */
fault(a)
{
	signal(a, fault);
	lseek(infile, 0L, 2);
	mkfault++;
}
