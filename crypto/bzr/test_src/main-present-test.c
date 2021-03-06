/* main-present-test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * present test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include <present.h>
#include "cli.h"
#include "performance_test.h"
#include "bcal-performance.h"
#include "bcal-nessie.h"
#include "bcal_present.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

char* algo_name = "Present";

const bcdesc_t* algolist[] PROGMEM = {
	(bcdesc_t*)&present_desc,
	NULL
};
/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void present_genctx_dummy(uint8_t* key, uint16_t keysize_b, present_ctx_t* ctx){
	present_init(key, keysize_b, ctx);
}

void testrun_nessie_present(void){
	bcal_nessie_multiple(algolist);
}

void testrun_selfenc(uint8_t* key, uint8_t* buffer){
	present_ctx_t ctx;
	cli_putstr_P(PSTR("\r\nkey   : "));
	cli_hexdump(key, 10);
	cli_putstr_P(PSTR("\r\nplain : "));
	cli_hexdump(buffer, 8);
	present_init(key, 80, &ctx);
	present_enc(buffer, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump(buffer, 8);
	present_dec(buffer, &ctx);
	cli_putstr_P(PSTR("\r\nplain : "));
	cli_hexdump(buffer, 8);
	cli_putstr_P(PSTR("\r\n"));
}

void testrun_self_present(void){
	uint8_t buffer[8], key[10];
	cli_putstr_P(PSTR("\r\n\r\n=== Testvectors from the paper ===\r\n"));
	
	memset(buffer, 0, 8);
	memset(key, 0, 10);
	testrun_selfenc(key, buffer);
	
	memset(buffer, 0, 8);
	memset(key, 0xFF, 10);
	testrun_selfenc(key, buffer);
	
	memset(buffer, 0xFF, 8);
	memset(key, 0, 10);
	testrun_selfenc(key, buffer);
	
	memset(buffer, 0xFF, 8);
	memset(key, 0xFF, 10);
	testrun_selfenc(key, buffer);
	
}

void testrun_performance_present(void){
	bcal_performance_multiple(algolist);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_present},
	{ test_str,        NULL, testrun_self_present},
	{ performance_str, NULL, testrun_performance_present},
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl},
	{ NULL,            NULL, NULL}
};

int main (void){
	DEBUG_INIT();
	
	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;	 	
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}
