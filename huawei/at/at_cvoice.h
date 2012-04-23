/*
 * at_cvoice.h
 *
 *  Created on: Apr 20, 2012
 *      Author: carlos
 */

#ifndef AT_CVOICE_H_
#define AT_CVOICE_H_

#include "../../util.h"
#include "../modem.h"

#define CVOICE_QUERY_ALL_PATTERN "^CVOICE:([0-9]+),([0-9]+),([0-9]+),([0-9]+)"

typedef struct at_cvoice_resp
{
	int mode;
	int sampling_rate;
	int data_bit;
	int frame_period;
} at_cvoice_resp_t;

void at_cvoice_assign(str_t *value, at_command_t *cmd);
_bool at_cvoice_parse_response(at_io_t *io, at_cvoice_resp_t *cvoice);
void at_cvoice_query_all(at_command_t *cmd);

#endif /* AT_CVOICE_H_ */
