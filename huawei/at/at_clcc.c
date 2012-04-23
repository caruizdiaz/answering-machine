/*
 * at_clcc.c
 *
 *  Created on: Apr 20, 2012
 *      Author: carlos
 */

#include <string.h>
#include "at_clcc.h"

void at_clcc_query_all(at_command_t *cmd)
{
	STR_ALLOC(cmd->str_cmd, sizeof("AT+CLCC") + 3 /* \r\n\0 */);
	strcpy(cmd->str_cmd.s, "AT+CLCC\r\n");

	cmd->id		= AT_CLCC;
	cmd->type	= AT_TYPE_QUERY_ALL;
}

_bool at_clcc_parse_response(at_io_t *io, at_clcc_resp_t *clcc)
{
	if (io->at_cmd.id != AT_CLCC)
	{
		LOG(L_ERROR, "%s: invalid command type %d\n", __FUNCTION__, io->at_cmd.id);
		return FALSE;
	}

	switch (io->at_cmd.type)
	{
	case AT_TYPE_QUERY_ALL:

		if (!strstr(io->response.s, "+CLCC:")) // no incoming call
		{
			clcc->status	= CS_NOTHING;
			return TRUE;
		}

		bzero(clcc->number, sizeof(clcc->number));
		sscanf(io->response.s, "AT+CLCC\r\n+CLCC: %d,%d,%d,%d,%d,\"+%s\",145", &clcc->idx,
																		&clcc->mode,
																		&clcc->status,
																		&clcc->type,
																		&clcc->multy_party,
																		clcc->number);

		LOG(L_DEBUG, "%s: CLCC -> idx=%d mode=%d status=%d type=%d multiparty=%d number=%s\n", __FUNCTION__,
																				clcc->idx,
																				clcc->mode,
																				clcc->status,
																				clcc->type,
																				clcc->multy_party,
																				clcc->number);
		return TRUE;
	case AT_TYPE_ASSIGN:
	case AT_TYPE_QUERY_VALUE:
		LOG(L_WARNING, "%s: unused type %d\n", __FUNCTION__, io->at_cmd.type);
		break;
	default:
		LOG(L_ERROR, "%s: unknown type %d\n", __FUNCTION__, io->at_cmd.type);
	}

	return FALSE;
}
