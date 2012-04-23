/*
 * at_ddsetex.c
 *
 *  Created on: Apr 20, 2012
 *      Author: carlos
 */
#include <string.h>
#include "at_ddsetex.h"

void at_ddsetex_assign(str_t *value, at_command_t *cmd)
{
	STR_ALLOC(cmd->str_cmd, sizeof("AT^DDSETEX=") + value->len + 3 /* \r\n\0 */);
	snprintf(cmd->str_cmd.s, cmd->str_cmd.len, "AT^DDSETEX=%s\r\n", value->s);

	cmd->id				= AT_DDSETEX;
	cmd->type			= AT_TYPE_ASSIGN;
}
