/*
   rdesktop: A Remote Desktop Protocol client.
   Entrypoint and utility functions
   Copyright (C) Matthew Chapman 1999-2003

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

//#include <stdarg.h>		/* va_list va_start va_end */
#include <unistd.h>		/* read close getuid getgid getpid getppid gethostname */
#include <fcntl.h>		/* open */
#include <pwd.h>		/* getpwuid */
//#include <termios.h>		/* tcgetattr tcsetattr */
#include <sys/stat.h>		/* stat */
#include <sys/time.h>		/* gettimeofday */
#include <sys/times.h>		/* times */
#include <errno.h>
#include "rdesktop.h"

#ifdef EGD_SOCKET
#include <sys/socket.h>		/* socket connect */
#include <sys/un.h>		/* sockaddr_un */
#endif

//#ifdef WITH_OPENSSL
#include <openssl/md5.h>
//#else
//#include "crypto/md5.h"
//#endif

/*extern "C" */void ui_main_loop(void);

int g_argc;
char ** g_argv;

char title[32] = "";
char username[16] = "j";
char hostname[16] = "sharp";
char keymapname[16];
int keylayout = 0x409;		/* Defaults to US keyboard layout */
int g_width = 800;		/* If width or height are reset to zero, the geometry will
				   be fetched from _NET_WORKAREA */
int g_height = 600;
int tcp_port_rdp = TCP_PORT_RDP;
int server_bpp = 8;
int win_button_size = 0;	/* If zero, disable single app mode */
BOOL bitmap_compression = True;
BOOL sendmotion = True;
BOOL orders = True;
BOOL encryption = True;
BOOL desktop_save = True;
BOOL fullscreen = False;
BOOL grab_keyboard = True;
BOOL hide_decorations = False;
extern BOOL owncolmap;

/* report an unimplemented protocol feature */
void
unimpl(char *format, ...)
{
}

/* malloc; exit if out of memory */
void *
xmalloc(int size)
{
   void *mem = malloc(size);
	if (mem == NULL)
	{
		error("xmalloc %d\n", size);
		exit(1);
	}
	return mem;
}

/* realloc; exit if out of memory */
void *
xrealloc(void *oldmem, int size)
{
	void *mem = realloc(oldmem, size);
	if (mem == NULL)
	{
		error("xrealloc %d\n", size);
		exit(1);
	}
	return mem;
}

/* free */
void
xfree(void *mem)
{
	free(mem);
}

/* report an error */
void
error(char *format, ...)
{
}

/* report a warning */
void
warning(char *format, ...)
{
}

/* Generate a 32-byte random for the secure transport code. */
void
generate_random(uint8 * random)
{
	struct stat st;
	struct tms tmsbuf;
	MD5_CTX md5;
	uint32 *r;
	int fd, n;

	/* If we have a kernel random device, try that first */
	if (((fd = open("/dev/urandom", O_RDONLY)) != -1)
	    || ((fd = open("/dev/random", O_RDONLY)) != -1))
	{
		n = read(fd, random, 32);
		close(fd);
		if (n == 32)
			return;
	}
	/* Otherwise use whatever entropy we can gather - ideas welcome. */
	r = (uint32 *) random;
	r[0] = (getpid()) | (getppid() << 16);
	r[1] = (getuid()) | (getgid() << 16);
	r[2] = times(&tmsbuf);	/* system uptime (clocks) */
	gettimeofday((struct timeval *) &r[3], NULL);	/* sec and usec */
	stat("/tmp", &st);
	r[5] = st.st_atime;
	r[6] = st.st_mtime;
	r[7] = st.st_ctime;

	/* Hash both halves with MD5 to obscure possible patterns */
	MD5_Init(&md5);
	MD5_Update(&md5, random, 16);
	MD5_Final(random, &md5);
	MD5_Update(&md5, random + 16, 16);
	MD5_Final(random + 16, &md5);
}

int
load_licence(unsigned char **data)
{
	char *path;
	char *home;
	struct stat st;
	int fd;

	home = getenv("HOME");
	if (home == NULL)
		return -1;

	path = (char*)xmalloc(strlen(home) + strlen(hostname) + 20);
	sprintf(path, "%s/.rdesktop/licence.%s", home, hostname);

	fd = open(path, O_RDONLY);
	xfree(path);
	if (fd == -1)
		return -1;

	if (fstat(fd, &st))
		return -1;

	*data = (unsigned char*)xmalloc(st.st_size);
	return read(fd, *data, st.st_size);
}

void
save_licence(unsigned char *data, int length)
{
	char *fpath;		/* file path for licence */
	char *fname, *fnamewrk;	/* file name for licence .inkl path. */
	char *home;
	uint32 y;
	struct flock fnfl;
	int fnfd, fnwrkfd, i, wlen;
	struct stream s, *s_ptr;
	uint32 len;

	/* Construct a stream, so that we can use macros to extract the
	 * licence.
	 */
	s_ptr = &s;
	s_ptr->p = data;
	/* Skip first two bytes */
	in_uint16(s_ptr, len);

	/* Skip three strings */
	for (i = 0; i < 3; i++)
	{
		in_uint32(s_ptr, len);
		s_ptr->p += len;
		/* Make sure that we won't be past the end of data after
		 * reading the next length value
		 */
		if ((s_ptr->p) + 4 > data + length)
		{
			printf("Error in parsing licence key.\n");
			printf("Strings %d end value %x > supplied length (%x)\n", i,
			       (unsigned int) s_ptr->p, (unsigned int) data + length);
			return;
		}
	}
	in_uint32(s_ptr, len);
	if (s_ptr->p + len > data + length)
	{
		printf("Error in parsing licence key.\n");
		printf("End of licence %x > supplied length (%x)\n",
		       (unsigned int) s_ptr->p + len, (unsigned int) data + length);
		return;
	}

	home = getenv("HOME");
	if (home == NULL)
		return;

	/* set and create the directory -- if it doesn't exist. */
	fpath = (char*)xmalloc(strlen(home) + 11);
	STRNCPY(fpath, home, strlen(home) + 1);

	sprintf(fpath, "%s/.rdesktop", fpath);
	if (mkdir(fpath, 0700) == -1 && errno != EEXIST)
	{
		perror("mkdir");
		exit(1);
	}

	/* set the real licence filename, and put a write lock on it. */
	fname = (char*)xmalloc(strlen(fpath) + strlen(hostname) + 10);
	sprintf(fname, "%s/licence.%s", fpath, hostname);
	fnfd = open(fname, O_RDONLY);
	if (fnfd != -1)
	{
		fnfl.l_type = F_WRLCK;
		fnfl.l_whence = SEEK_SET;
		fnfl.l_start = 0;
		fnfl.l_len = 1;
		fcntl(fnfd, F_SETLK, &fnfl);
	}

	/* create a temporary licence file */
	fnamewrk = (char*)xmalloc(strlen(fname) + 12);
	for (y = 0;; y++)
	{
		sprintf(fnamewrk, "%s.%lu", fname, (long unsigned int) y);
		fnwrkfd = open(fnamewrk, O_WRONLY | O_CREAT | O_EXCL, 0600);
		if (fnwrkfd == -1)
		{
			if (errno == EINTR || errno == EEXIST)
				continue;
			perror("create");
			exit(1);
		}
		break;
	}
	/* write to the licence file */
	for (y = 0; y < len;)
	{
		do
		{
			wlen = write(fnwrkfd, s_ptr->p + y, len - y);
		}
		while (wlen == -1 && errno == EINTR);
		if (wlen < 1)
		{
			perror("write");
			unlink(fnamewrk);
			exit(1);
		}
		y += wlen;
	}

	/* close the file and rename it to fname */
	if (close(fnwrkfd) == -1)
	{
		perror("close");
		unlink(fnamewrk);
		exit(1);
	}
	if (rename(fnamewrk, fname) == -1)
	{
		perror("rename");
		unlink(fnamewrk);
		exit(1);
	}
	/* close the file lock on fname */
	if (fnfd != -1)
	{
		fnfl.l_type = F_UNLCK;
		fnfl.l_whence = SEEK_SET;
		fnfl.l_start = 0;
		fnfl.l_len = 1;
		fcntl(fnfd, F_SETLK, &fnfl);
		close(fnfd);
	}
	free(fnamewrk);
	free(fpath);
	free(fname);
}
