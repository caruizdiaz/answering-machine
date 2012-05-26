/*
 * main.c
 *
 *  Created on: Apr 16, 2012
 *      Author: carlos
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"
#include "huawei/modem.h"
#include "huawei/at/at_clcc.h"
#include "huawei/at/at_a.h"
#include "huawei/at/at_cvoice.h"
#include "huawei/at/at_ddsetex.h"
#include "huawei/at/at_chup.h"

static void print_usage();
static _bool get_pcm_frames(char *audio_file, pcm_data_t *audio);
static _bool get_modem_frame_rate(port_t *port);
static void check_modem(port_t *data_port, port_t *audio_port, pcm_data_t *audio);
static void play_message(port_t *port, pcm_data_t *audio);

int main(int argc, char **argv)
{
	at_cvoice_resp_t *cvoice = malloc(sizeof(at_cvoice_resp_t));

	sscanf("AT^CVOICE\r\n^CVOICE:0,8000,16,20\r\nOK\r\n\0", "AT^CVOICE\r\n^CVOICE:%d,%d,%d,%d", &cvoice->mode,
															&cvoice->sampling_rate,
															&cvoice->data_bit,
															&cvoice->ptime);
	LOG(L_DEBUG, "%s: CVOICE -> sampling rate=%d data_bit=%d frame_period=%d\n", __FUNCTION__,
																					cvoice->sampling_rate,
																						cvoice->data_bit,
																						cvoice->ptime);

	at_io_t io;

	at_cvoice_query_all(&io.at_cmd);

	/*io.response.s = "AT^CVOICE\r\n^CVOICE:0,8000,16,20\r\nOK\r\n";
	io.response.len	= 44;

	at_cvoice_parse_response(&io, cvoice);

	//return 0;*/
	port_t data_port, audio_port;
	pcm_data_t audio;

	if (argc != 4)
	{
		print_usage();
		exit(1);
	}

	if (!modem_open_datafile(argv[1], &data_port))
		exit(1);

	if (!modem_open_audio(argv[2], &audio_port))
		exit(1);

	if (!get_modem_frame_rate(&data_port))
		exit(1);

	if (!get_pcm_frames(argv[3], &audio))
		exit(1);

	check_modem(&data_port, &audio_port, &audio);

}

static void print_usage()
{

}
/*
static _bool get_modem_info(port_t *data_port)
{
	at_io_t io;
	at_cvoice_resp_t cvoice;

	if (!get_pcm_frames(data_port))
		return FALSE;
}*/

static _bool get_pcm_frames(char *audio_file, pcm_data_t *audio)
{
	int file_length		= 0,
		silent_frame	= 0,
		bytes_read		= 0;

	FILE *file		= fopen(audio_file, "rb");

	if (file == NULL)
	{
		LOG(L_ERROR, "%s: error opening file %s. %s\n", __FUNCTION__, audio_file, strerror(errno));
		return FALSE;
	}

	fseek(file, 0, SEEK_END);
	file_length	= ftell(file);
	rewind(file);
//	file_length = 252000;

	silent_frame	= FRAME_SIZE - (file_length % FRAME_SIZE);
	audio->data		= malloc(file_length + silent_frame);
	audio->length	= file_length + silent_frame;

	if ((bytes_read	= fread(audio->data, 1, file_length, file)) != file_length)
	{
		//printf("br %d, l %d\n", bytes_read, file_length);
		LOG(L_ERROR, "%s: couldn't read the whole audio file. %s\n", __FUNCTION__, strerror(errno));
		return FALSE;
	}

	memset(&audio->data[file_length], '\0', silent_frame);

	return TRUE;
}

static _bool get_modem_frame_rate(port_t *port)
{
	at_io_t io;
	at_cvoice_resp_t cvoice;

	at_cvoice_query_all(&io.at_cmd);

	if (!modem_write(port, &io))
	{
		LOG(L_ERROR, "%s: error writing to modem\n", __FUNCTION__);
		return FALSE;
	}

	printf("response: %s\n", io.response.s);

	if (!at_cvoice_parse_response(&io, &cvoice))
	{
		LOG(L_ERROR, "%s: error getting frame rate\n", __FUNCTION__);
		return FALSE;
	}

	port->frame_period	= cvoice.ptime;

	return TRUE;
}

static void play_message(port_t *port, pcm_data_t *audio)
{
	int n		= 0,
		offset	= 0,
		i		= 0;

	for(i = 0; i <= audio->length; i += FRAME_SIZE)
	{
		n		= write(port->fd, &audio->data[offset], FRAME_SIZE);

		if (n < FRAME_SIZE)
		{
			LOG(L_DEBUG, "%s: wrote only %d of %d bytes\n", __FUNCTION__, n, FRAME_SIZE);
		}

		offset	+= FRAME_SIZE;

		//usleep(port->frame_period * 10000);
		usleep(20000);
	}
}

static void check_modem(port_t *data_port, port_t *audio_port, pcm_data_t *audio)
{
	_bool incoming	= FALSE;

	for(;;)
	{
		at_io_t io;
		at_clcc_resp_t clcc;

		sleep(1);

		at_clcc_query_all(&io.at_cmd);

		if (!modem_write(data_port, &io))
		{
			LOG(L_ERROR, "%s: error writing to modem\n", __FUNCTION__);
			continue;
		}

		if (!at_clcc_parse_response(&io, &clcc))
		{
			LOG(L_ERROR, "%s: error parsing response\n", __FUNCTION__);
			continue;
		}

		if (clcc.status == CS_INCOMING)
		{
			str_t a_port	= {"2", 1};

			LOG(L_DEBUG, "%s: incoming call. Preparing to answer.\n", __FUNCTION__);

			incoming	= TRUE;

			STR_FREE(io.at_cmd.str_cmd);

			at_a_assign(NULL, &io.at_cmd);

			if (!modem_write(data_port, &io))
				continue;

			at_ddsetex_assign(&a_port, &io.at_cmd);

			if (!modem_write(data_port, &io))
				continue;
		}
		else if(clcc.status == CS_ACTIVE)
		{
			if (!incoming)
			{
				LOG(L_DEBUG, "%s: active call not originated by modem, ignoring\n", __FUNCTION__);
				continue;
			}
			else
				LOG(L_DEBUG, "%s: call was answered\n", __FUNCTION__);

			incoming	= FALSE;

			play_message(audio_port, audio);

			at_chup_assign(NULL, &io.at_cmd);

			if (!modem_write(data_port, &io))
				continue;
		}
	}
}

