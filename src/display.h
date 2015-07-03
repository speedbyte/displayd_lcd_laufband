/*
 * Copyright (c) 2010, Kristian Lehmann
 * Copyright (c) 2010, Michael Eisele
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#define DISPLAY1 0x04

#define M10_CMD_STATUS 0x03
#define M10_CMD_INIT_TEXT 0x04
#define M10_CMD_SEND_TEXT 0x05
#define M10_CMD_INIT_GRAPHIC 0x07
#define M10_CMD_SEND_GRAPHIC 0x08
#define M10_CMD_INIT_FLASH 0x09
#define M10_CMD_UPDATE_FLASH 0x0a
#define M10_CMD_SEND_PARAMETER 0x0b
#define M10_CMD_SEND_SYSTEM_TIME 0x0c
#define M10_CMD_COM_OK 0x0d
#define M10_CMD_GET_INFORMATION 0x0e

unsigned char NEED_PARAMETERS[] = {
				0, 0, 0, 0, 1,
				1, 0, 1, 1, 1,
				1, 1, 1, 0, 1,
				0
				};


#define M10_TC_SLEEP 0			// Keine Parameter
#define M10_TC_GRAPHIC 1		//
#define M10_TC_TXT_LAUF_LINKS 2 	// 
struct m10_par_lauf
{
	unsigned char a;
	unsigned char b;
	unsigned char abstand_h;
	unsigned char breite_h;
	unsigned char speed;
	unsigned char f;
	unsigned char g;
};
#define M10_TC_TXT_EINSCHIESSEN 3	//
#define M10_TC_TXT_ROLLEN_VON_OBEN 4	//
#define M10_TC_TXT_ROLLEN_VON_UNTEN 5	//
#define M10_TC_CL_ROLLEN_VON_OBEN 6	//
#define M10_TC_CL_ROLLEN_VON_UNTEN 7	//
#define M10_TC_NONE_8 8
#define M10_TC_NONE_9 9
#define M10_TC_NONE_10 10
#define M10_TC_TXT_AUFBLAETTERN 11	//
#define M10_TC_TXT_ZUBLAETTERN 12	//
#define M10_TC_NONE_13 13
#define M10_TC_NONE_14 14
#define M10_TC_TXT_ROLLEN_VON_MITTE 15
#define M10_TC_CL_ROLLEN_VON_MITTE 16
#define M10_TC_AUFRUF_SUB 17
#define M10_TC_ENDE_SUB 18
#define M10_TC_DEFINITION_SUB 19
#define M10_TC_HALT_20 20
#define M10_TC_NONE_21 21
#define M10_TC_NONE_22 22
#define M10_TC_NONE_23 23
#define M10_TC_NONE_24 24
#define M10_TC_NONE_25 25
#define M10_TC_NONE_26 26
#define M10_TC_NONE_27 27
#define M10_TC_NONE_28 28
#define M10_CL_BUCHSTABENWEISE 29


struct display_cmd
{
	unsigned char address;
	unsigned char command;
	unsigned char * data;
	unsigned int data_size;
};

unsigned short calcCRC16(unsigned char * data, short size);
unsigned short calcCRC16LC(unsigned char * data, short size);


#endif
