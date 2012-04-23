/*
 * at_clcc.h
 *
 *  Created on: Apr 20, 2012
 *      Author: carlos
 */

#ifndef AT_CLCC_H_
#define AT_CLCC_H_

#include "../../util.h"
#include "../modem.h"

typedef enum call_direction
{
	CD_MO,
	CD_MT
} call_direction_t;

typedef enum call_status
{
	CS_ACTIVE,
	CS_ONHOLD,
	CS_DIALING,
	CS_ALERTING,
	CS_INCOMING,
	CS_WAITING,

	CS_NOTHING = 999
} call_status_t;

typedef enum call_type
{
	CT_VOICE,
	CT_DATA,
	CT_FAX
} call_type_t;

typedef enum call_multiparty
{
	CM_NON_MULTIPARTY,
	CM_MULTIPARTY
} call_multiparty_t;


typedef struct at_clcc_resp
{
	int idx;
	call_direction_t mode;
	call_status_t status;
	call_type_t type;
	call_multiparty_t multy_party;
	char number[50];
} at_clcc_resp_t;


void at_clcc_query_all(at_command_t *cmd);
_bool at_clcc_parse_response(at_io_t *io, at_clcc_resp_t *clcc);

#endif /* AT_CLCC_H_ */
