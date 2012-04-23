/*
 * at_cvoice.c
 *
 *  Created on: Apr 20, 2012
 *      Author: carlos
 */
#include <string.h>
#include <regex.h>
#include "at_cvoice.h"

static regex_t _cvoice_regex;

_bool at_cvoice_initialize()
{
	if (regcomp(&_cvoice_regex, CVOICE_QUERY_ALL_PATTERN, REG_EXTENDED))
	{
		LOG(L_ERROR, "%s: CVOICE_QUERY_ALL_PATTERN compilation failed\n", __FUNCTION__);
		return FALSE;
	}

	return TRUE;
}

void at_cvoice_query_all(at_command_t *cmd)
{
	STR_ALLOC(cmd->str_cmd, sizeof("AT^CVOICE?") + 3 /* \r\n\0 */);
	strcpy(cmd->str_cmd.s, "AT^CVOICE?\r\n");

	cmd->id		= AT_CVOICE;
	cmd->type	= AT_TYPE_QUERY_ALL;
}

_bool at_cvoice_parse_response(at_io_t *io, at_cvoice_resp_t *cvoice)
{
	if (io->at_cmd.id != AT_CVOICE)
	{
		LOG(L_ERROR, "%s: invalid command type %d\n", __FUNCTION__, io->at_cmd.id);
		return FALSE;
	}

	switch (io->at_cmd.type)
	{
	case AT_TYPE_QUERY_ALL:

		sscanf(io->response.s, "AT^CVOICE\r\r\n^CVOICE:%d,%d,%d,%d", &cvoice->mode,
																	&cvoice->sampling_rate,
																	&cvoice->data_bit,
																	&cvoice->frame_period);
														/*
		sscanf(io->response.s, "AT^CVOICE\n\r^CVOICE:%d,%d,%d,%d", &a,
																&b,
																&c,
																&d);
		printf("%s-> fp %d\n", io->response.s, d); */

		LOG(L_DEBUG, "%s: CVOICE -> sampling rate=%d data_bit=%d frame_period=%d\n", __FUNCTION__,
																					cvoice->sampling_rate,
																					cvoice->data_bit,
																					cvoice->frame_period);
		cvoice->frame_period	= 20;
		return TRUE;
		break;
	case AT_TYPE_ASSIGN:
	case AT_TYPE_QUERY_VALUE:
		LOG(L_WARNING, "%s: unused type %d\n", __FUNCTION__, io->at_cmd.type);
		break;
	default:
		LOG(L_ERROR, "%s: unknown type %d\n", __FUNCTION__, io->at_cmd.type);
	}

	return FALSE;
}
