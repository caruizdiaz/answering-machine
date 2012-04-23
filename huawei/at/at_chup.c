/*
 * at_chup.c
 *
 *  Created on: Apr 20, 2012
 *      Author: carlos
 */

#include <string.h>
#include "at_chup.h"

void at_chup_assign(str_t *unused, at_command_t *cmd)
{
	STR_ALLOC(cmd->str_cmd, sizeof("AT+CHUP") + 3 /* \r\n\0 */);
	strcpy(cmd->str_cmd.s, "AT+CHUP\r\n");

	cmd->id		= AT_CHUP;
	cmd->type	= AT_TYPE_ASSIGN;
}
