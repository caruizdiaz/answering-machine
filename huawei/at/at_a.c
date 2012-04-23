/*
 * at_a.c
 *
 *  Created on: Apr 20, 2012
 *      Author: carlos
 */

#include <string.h>
#include "at_a.h"

void at_a_assign(str_t *unused, at_command_t *cmd)
{
	STR_ALLOC(cmd->str_cmd, sizeof("ATA") + 3 /* \r\n\0 */);
	strcpy(cmd->str_cmd.s, "ATA\r\n");

	cmd->id		= AT_A;
	cmd->type	= AT_TYPE_ASSIGN;
}
