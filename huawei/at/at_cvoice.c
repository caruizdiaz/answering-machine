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
static int _matchgrp_no;
static _bool _initialized	= FALSE;

_bool at_cvoice_initialize()
{
	if (regcomp(&_cvoice_regex, CVOICE_QUERY_ALL_PATTERN, REG_EXTENDED))
	{
		LOG(L_ERROR, "%s: CVOICE_QUERY_ALL_PATTERN compilation failed\n", __FUNCTION__);
		return FALSE;
	}

	_initialized		= TRUE;
	_matchgrp_no	= _cvoice_regex.re_nsub  + 1;

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
	int ret_code, i;
	char *error_message	= NULL;
	regmatch_t matches[_matchgrp_no];

	if (io->at_cmd.id != AT_CVOICE)
	{
		LOG(L_ERROR, "%s: invalid command type %d\n", __FUNCTION__, io->at_cmd.id);
		return FALSE;
	}

	switch (io->at_cmd.type)
	{
	case AT_TYPE_QUERY_ALL:

		if (!_initialized)
			if (!at_cvoice_initialize())
			{
				LOG(L_ERROR, "Error initializing pattern '%s'\n", CVOICE_QUERY_ALL_PATTERN);
				return FALSE;
			}


		ret_code	= regexec(&_cvoice_regex,
								io->response.s,
								_matchgrp_no,
								matches,
								0);

		switch(ret_code)
		{
			case 0:
				break;
			case REG_NOMATCH:
				error_message	= "String didn't match";
				break;
			case REG_ESPACE:
				error_message	= "No memory left";
				break;
			default:
				error_message	= "Unknown error";
				break;
		}

		if (ret_code != 0)
		{
			LOG(L_ERROR, "%s\n", error_message);
			return FALSE;
		}

		char result[50];
		bzero(result, 50);
		strncpy(result, &io->response.s[matches[0].rm_so], matches[0].rm_eo - matches[0].rm_so);

		printf("%s\n", result);

		sscanf(result, "CVOICE:0,%d,%d,%d", &cvoice->sampling_rate, &cvoice->data_bit, &cvoice->ptime);

		/*

		for (i = 0; i < _matchgrp_no; i++)	// first match is the the whole string
		{
			regmatch_t match 	= matches[i];
			int length			= match.rm_eo - match.rm_so;
			char result[50];

			bzero(result, sizeof(result));
			//printf("l %d %d %d\n", match.rm_so, match.rm_eo, length);

			printf("--> %.*s\n", length, &io->response.s[match.rm_so]);
			strncpy(result, &io->response.s[match.rm_so], length);

			switch(i)
			{
			case 1:
				cvoice->mode		= atoi(result);
				break;
			case 2:
				cvoice->sampling_rate	= atoi(result);
				break;
			case 3:
				cvoice->data_bit	= atoi(result);
				break;
			case 4:
				cvoice->ptime		= atoi(result);
				break;
			}
			//memcpy(result, io->response.s, length);



			printf("result %s\n", result);
		}
		*/
		LOG(L_DEBUG, "%s: CVOICE -> sampling rate=%d data_bit=%d ptime=%d\n", __FUNCTION__,
																					cvoice->sampling_rate,
																					cvoice->data_bit,
																					cvoice->ptime);
		cvoice->ptime	= 20;
		return TRUE;
		break;
	case AT_TYPE_ASSIGN:
	case AT_TYPE_QUERY_VALUE:
		LOG(L_WARNING, "%s: unused type %d\n", __FUNCTION__, io->at_cmd.type);
		break;
	default:
		LOG(L_ERROR, "%s: unknown type %d\n", __FUNCTION__, io->at_cmd.type);
		break;
	}

	return FALSE;
}
