all:
	cc -O -z -d2 ls.c -o ls
	cc -O -z ucbls.c -o ucbls
install:
	install -s ls ${DESTDIR}/bin/ls
	install -s ucbls ${DESTDIR}/usr/ucb/ls
	rm -f ${DESTDIR}/usr/ucb/l
	ln ${DESTDIR}/usr/ucb/ls ${DESTDIR}/usr/ucb/l
clean:
	rm -f ls ucbls
