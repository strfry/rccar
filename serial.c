#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>

int		ttyfd = -1;

void 
serial_init(char *ttypath)
{
	struct termios	options;
	ttyfd = open(ttypath, O_RDWR | O_NOCTTY);

	if (ttyfd < 0) {
		perror("open");
		exit(1);
	}
	ioctl(ttyfd, TIOCEXCL);


//OSX Hack: set fd to blocking again:
	int		flags = fcntl(ttyfd, F_GETFL, 0);
	fcntl(ttyfd, F_SETFL, flags | ~O_NONBLOCK);

	struct termios	mode;
	tcgetattr(ttyfd, &mode);
	
	cfmakeraw(&mode);
    

	mode.c_iflag = IGNPAR;
	mode.c_oflag = 0;
	mode.c_cflag = CLOCAL | CREAD | CS8 | B9600;
	mode.c_lflag = 0;
	mode.c_cc[VMIN] = 0;
	mode.c_cc[VTIME] = 0;

	tcflush(ttyfd, TCIOFLUSH);
	tcsetattr(ttyfd, TCSANOW, &mode);
}

//void		serial_write(char *c, int len);
void 
serial_writebyte(char c)
{
	write(ttyfd, &c, 1);
}

void serial_write(char* c, int len)
{
//	printf("%c(%d) %c(%d) %c(%d) %c(%d)%x%x\n", c[0], c[4], c[1], c[5], c[2], c[6], c[3], c[7], c[8], c[9]);
	write(ttyfd, c, len);
}

int serial_selectread()
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 40000;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(ttyfd, &fds);
    int ret = select(ttyfd + 1, &fds, 0, 0, &tv);

    if (ret <= 0) {
        return 0;
    }

    char readChar;
    read(ttyfd, &readChar, 1);

    return readChar;
}
