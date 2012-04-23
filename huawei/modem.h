/*
 * at_command.h
 *
 *  Created on: Apr 19, 2012
 *      Author: carlos
 */

#ifndef MODEM_H_
#define MODEM_H_

#include <pthread.h>

#define FRAME_SIZE 			320
#define RESPONSE_BUFFER		100

typedef _bool (*parse_response_fptr)(str_t *at_cmd, str_t *response);

typedef struct pcm_data
{
	char *data;
	int length;
} pcm_data_t;

typedef enum at_command_id
{
	AT_D = 1,
	AT_A,
	AT_CHUP,
	AT_CVOICE,
	AT_DDSETEX,
	AT_CLCC
} at_command_id_t;

typedef enum at_command_type
{
	AT_TYPE_QUERY_VALUE = 1,
	AT_TYPE_QUERY_ALL,
	AT_TYPE_ASSIGN,
} at_command_type_t;


typedef struct at_command
{
	at_command_id_t id;
	str_t str_cmd;
	at_command_type_t type;
} at_command_t;

typedef struct at_io
{
	at_command_t at_cmd;
	str_t response;
	void *parsed_response;
} at_io_t;

typedef struct port
{
	str_t file;
	int fd;
	int frame_period;
	pthread_mutex_t lock;

} port_t;

_bool modem_write(port_t *modem, at_io_t *io);
_bool modem_open_audio(char *file, port_t *modem);
_bool modem_open_datafile(char *file, port_t *modem);

#endif /* AT_COMMAND_H_ */
