/*
 * at_d.c
 *
 *  Created on: Apr 20, 2012
 *      Author: carlos
 */
#include <string.h>
#include "at_d.h"

void at_d_assign(str_t *value, at_command_t *cmd)
{
	STR_ALLOC(cmd->str_cmd, sizeof("ATD") + value->len + sizeof(";") + 3 /* \r\n\0 */);
	snprintf(cmd->str_cmd.s, cmd->str_cmd.len, "ATD%s;\r\n", value->s);

	cmd->id		= AT_D;
	cmd->type	= AT_TYPE_ASSIGN;
}
