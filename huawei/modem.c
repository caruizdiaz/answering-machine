/*
 * at_command.c
 *
 *  Created on: Apr 18, 2012
 *      Author: carlos
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "../util.h"
#include "modem.h"


static _bool modem_open(char *file, port_t *modem);

static _bool modem_open(char *file, port_t *modem)
{
	int fd		= open(file, O_RDWR | O_NOCTTY | O_NONBLOCK);
	int flags 	= fcntl(fd, F_GETFD);

	modem->fd	= -1;

	if(flags == -1 || fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1)
	{
		LOG(L_ERROR, "%s: %s\n", __FUNCTION__, strerror(errno));
		return FALSE;
	}

	STR_ALLOC(modem->file, strlen(file) + 1);
	strcpy(modem->file.s, file);
	modem->fd	= fd;
	pthread_mutex_init(&modem->lock, NULL);

	return TRUE;
}

_bool modem_open_audio(char *file, port_t *modem)
{
	return modem_open(file, modem);
}

_bool modem_open_datafile(char *file, port_t *modem)
{
	struct termios  term_attr;

	if (!modem_open(file, modem))
		return FALSE;

	term_attr.c_cflag 		= B115200 | CS8 | CREAD | CRTSCTS;
	term_attr.c_iflag 		= 0;
	term_attr.c_oflag 		= 0;
	term_attr.c_lflag 		= 0;
	term_attr.c_cc[VMIN] 	= 1;
	term_attr.c_cc[VTIME] 	= 0;

	if (tcsetattr(modem->fd, TCSAFLUSH | TCSANOW, &term_attr) > 0)
	{
		LOG(L_ERROR, "%s: %s\n", __FUNCTION__, strerror(errno));
		return FALSE;
	}

	return TRUE;
}

_bool modem_write(port_t *modem, at_io_t *io)
{
	pthread_mutex_lock(&modem->lock);
	_bool succeded	= FALSE;
	int n			= 0;

	if (modem->fd <= 0)
	{
		LOG(L_ERROR, "%s: modem file descriptor in unusable state\n", __FUNCTION__);
		goto exit_func;
	}

	n = write(modem->fd, io->at_cmd.str_cmd.s, io->at_cmd.str_cmd.len);

	if (n != io->at_cmd.str_cmd.len)
	{
		LOG(L_ERROR, "%s: written only %d byte(s) of %d bytes in buffer\n", __FUNCTION__, n, io->at_cmd.str_cmd.len);
		goto exit_func;
	}

	STR_ALLOC(io->response, RESPONSE_BUFFER);

	sleep(1);

	n = read(modem->fd, io->response.s, RESPONSE_BUFFER);

	io->response.len	= 0;
	if (n < 0)
	{
		LOG(L_ERROR, "%s: timeout waiting for answer to '%s'\n", __FUNCTION__, io->at_cmd.str_cmd.s);
		goto exit_func;
	}
	else if (n == 0)
	{
		LOG(L_ERROR, "%s: empty response from modem\n", __FUNCTION__ );
		goto exit_func;
	}

	io->response.s[n]	= '\0';
	io->response.len	= n;

//	printf("-> resp: %s len %d\n", io->response.s, io->response.len);

	/*if (strstr(io->response.s, "ERROR"))
	{
		LOG(L_ERROR, "%s: error executing command '%s'\n", __FUNCTION__, io->at_cmd.str_cmd.s);
		goto exit_func;
	}*/

	succeded	= TRUE;

exit_func:
	pthread_mutex_unlock(&modem->lock);
	return succeded;
}
