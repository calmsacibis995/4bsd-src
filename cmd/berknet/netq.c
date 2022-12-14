/* netq - print the netq send queue */
/* netq [-a] [mach] */
/* must be setuid root */

# include "defs.h"
# define STSIZE 150

static FILE *df;
static char jname[16], printlong;
static struct table {
	char name[16];
	long filesize;
	} stack[STSIZE], temp;
static int stptr = 0;
static char mach,visit[26];
static char netcmd1[] =	NETCMD1;
static int hisuid,sumj;
static long sumb;
static struct stat statbuf;
static struct direct dirbuf;

char _sobuf[BUFSIZ];
main(argc,argv)
  char **argv; {
	int i;
	setbuf(stdout,_sobuf);
	hisuid = getuid();
	hisuid = uidmask(hisuid);
	if(stat(netcmd,&statbuf) >= 0)
		if((statbuf.st_mode & 07) == 0){
			printf("Network is down\n");
			exit(EX_UNAVAILABLE);
			}
	else if(stat(netcmd1,&statbuf) >= 0)
		if((statbuf.st_mode & 07) == 0){
			printf("Network is down\n");
			exit(EX_UNAVAILABLE);
			}
	while(argc > 1){
		switch(argv[1][0]){
		case '-': printlong++; break;
		default: mach = lookup(argv[1]);
			if(mach > 0 && machtype[mach-'a'] == 0)mach = 0;
			break;
		}
		argc--, argv++;
		}
	if(mach){
		mach = gothru(local,mach); /* list to directly conn. machine */
		if(mach == 0){
			fprintf(stderr,"That machine not directly connected.\n");
			exit(EX_USAGE);
		}
		senddir[strlen(senddir)-1] = mach;
		if(chdir(senddir) < 0){
			perror(senddir);
			exit(EX_OSFILE);
		}
		pdir(senddir);
	}
	else for(i = 'a'; i <= 'z'; i++)
		if((mach = gothru(local,i)) && !visit[mach - 'a']){
			visit[mach - 'a'] = 1;
			senddir[strlen(senddir)-1] = mach;
			if(chdir(senddir) < 0)continue;
			pdir(senddir);
			printf("---\n");
			}
	fflush(stdout);
	}
static pdir(str)
  char *str; {
	int i;
	char more = 0, *cp;
	int (*compar)();
	df = fopen(str,"r");
	if(df == NULL){
		perror(str);
		exit(EX_OSFILE);
		}
	stptr = 0;
	while(fread(&dirbuf,1,sizeof dirbuf,df)==sizeof dirbuf){
		if(dirbuf.d_ino == 0 
		|| dirbuf.d_name[0] != 'c'
		|| dirbuf.d_name[1] != 'f'
		|| stat(dirbuf.d_name,&statbuf) < 0)
			continue;
		if(mach != dirbuf.d_name[2])continue;
		dirbuf.d_name[0] = 'd';
		if(stat(dirbuf.d_name,&statbuf) < 0)continue;

		if(!insert(dirbuf.d_name,getsize(&statbuf))){
			more++;
			break;
			}
		}
	if(stptr == 0){
		printf("Network queue to/thru %s is empty.\n",longname(mach));
		return;
		}
	cp = (char *)&(stack[0].name[0]);
	sort(cp,stptr,sizeof temp,compar);
	printf("Network queue to/thru %s:\n",longname(mach));
	printf(
	"From       To           Len  Code   Time          Command\n");
	for(i = 0; i < stptr; i++){ /* screen size */
		strcpy(jname,stack[i].name);
		jname[0] = 'd';
		if(stat(jname,&statbuf) < 0)
			continue;
		if(printlong || guid(statbuf.st_uid,statbuf.st_gid) == hisuid)
			process();
		else summarize(i);
		}
	printsum();
	if(more)printf("   ... more ...\n");
	}
summarize(i){
	sumb += stack[i].filesize;
	sumj++;
	}
printsum(){
	if(sumj != 0){
		printf("%d request%s, %ld bytes\n",
			sumj,(sumj > 1 ? "s" : ""),sumb);
		sumj = 0;
		sumb = 0L;
		}
	}
process(){
	static struct header hd;
	FILE *look;
	char *cp;

	printsum();
	look = fopen(jname,"r");
	if(look == NULL)
		return;
	readhdfd(&hd,look);
	fclose(look);
	if(hd.hd_snfrom[0] == 0)strcat(hd.hd_addrfrom,"Internal");
	expandcc(hd.hd_sttyname);
	cp = ctime(&statbuf.st_mtime);
	cp[strlen(cp)-9] = 0;
	jname[3] = jname[2];
	printf("%-10s %-10s %5ld %s %s  %-.27s\n",
		hd.hd_addrfrom,hd.hd_addrto,getsize(&statbuf),
		jname+3,cp+4,hd.hd_scmdvirt);
	}
readhdfd(phd,fd)
register struct header *phd;
FILE *fd;
{
	char sbuf[BUFSIZ], parmlist[PARMLIST];
	char *s, cflag;
	int c, i;
	phd->hd_code = phd->hd_mchto = phd->hd_snto[0] = phd->hd_sinfile[0] = 0;
	phd->hd_soutfile[0] = phd->hd_srespfile[0] = phd->hd_snfrom[0] = 0;
	phd->hd_scmdact[0] = 0;
	for(i=0;i<20;i++)phd->hd_sttyname[i] = 0;

	phd->hd_code = ngetc(fd);
	if(phd->hd_code == 0)return;
	phd->hd_mchto = ngetc(fd);
	phd->hd_mchfrom = ngetc(fd);	/* from machine */
	phd->hd_vmajor = ngetc(fd);
	phd->hd_vminor = ngetc(fd);
	ngets(phd->hd_snto,NS,fd);
	ngets(phd->hd_spasswd,20,fd);			/* passwd */
	ngets(phd->hd_sinfile,FNS,fd);
	ngets(phd->hd_soutfile,FNS,fd);
	ngets(phd->hd_srespfile,FNS,fd);
	ngets(phd->hd_snfrom,NS,fd);
	ngets(phd->hd_sttyname,20,fd);
	cflag = ngetc(fd);		
	ngets(sbuf,BUFSIZ,fd);				/* lttytime */
	ngets(parmlist,PARMLIST,fd);			/* jobno */
	parseparmlist(parmlist);
	ngets(sbuf,BUFSIZ,fd);				/* timesent */
	s = phd->hd_scmdact;
	while((c = getc(fd)) != EOF && c != '\n'){
		if(c == '\\')c = getc(fd);
		*s++ = c;
		}
	*s = 0;
	s = phd->hd_scmdvirt;
	while((c = getc(fd)) != EOF && c != '\n'){
		if(c == '\\')c = getc(fd);
		*s++ = c;
		}
	*s = 0;
	if(phd->hd_scmdvirt[0] == 0)strcpy(phd->hd_scmdvirt,phd->hd_scmdact);
	sprintf(phd->hd_addrfrom,"%c:%s",phd->hd_mchfrom,phd->hd_snfrom);
	sprintf(phd->hd_addrto  ,"%c:%s",phd->hd_mchto  ,phd->hd_snto  );
}
ngetc(fd)
FILE *fd;
{
	char b[3];
	if(feof(fd))return(0);
	if(fread(b,1,3,fd) != 3) return(0);
	return(b[0]);
	}
/* read a string s of max length maxlen out of queue file */
ngets(s,maxlen,fd)
int maxlen;
char *s;
FILE *fd;
{
	int i;
	if(feof(fd))return;
	for(;;){
		i = getc(fd);
		if(i == EOF){
			*s = 0;
			return;
			}
		*s = i;
		if(*s == '\\')*s = getc(fd);
		if(*s == ' ')break;
		if(maxlen-- > 0)s++;
		}
	*s = 0;
	getc(fd);
	}
insert(f,t)
  char *f;
  long t; {
	strcpy(stack[stptr].name,f);
	stack[stptr++].filesize = t;
	return(stptr <= STSIZE);
	}
compar(a,b)
  register struct table *a,*b; {
	if(a->filesize < b->filesize)return(-1);
	if(a->filesize > b->filesize)return(1);
	return(0);
	}
sort(){		/* use this cause qsort doesn't work */
	register int i,j;
	for(i=0; i< stptr-1; i++)
		for(j=i+1;j<stptr;j++)
			if(compar(&stack[i],&stack[j]) > 0)
				swap(&stack[i],&stack[j]);
	}
swap(a,b)
  register struct table *a, *b; {
	char str[16];
	long t;
	strcpy(str,a->name);
	t = a->filesize;
	strcpy(a->name,b->name);
	a->filesize = b->filesize;
	strcpy(b->name,str);
	b->filesize = t;
	}
