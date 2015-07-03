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

#include <stdio.h>


void dDrawLine(unsigned char * buffer, short x1, short y1, short x2, short y2)
{
}

void dDrawDot(unsigned char * buffer, short x, short y)
{
	short shift = 0;
	if (y > 8)
	{	
		shift = 128;
		y -= 8;
	}
	buffer[shift + x] |= 1 << (7 - y);
}

void dClear(unsigned char * buffer)
{
	int i;
	for (i = 0; i < 256; i++) buffer[i] = 0;
}

void dInvert(unsigned char * buffer)
{
	int i;
	for(i = 0; i < 256; i++) buffer[i] = ~buffer[i]; 
}

void printBuffer(unsigned char * buffer)
{
	int l,x,i;
	for (l = 0; l < 2; l++) for (i = 7; i >= 0; i--)
	{
		for (x = 0; x < 128; x++)
		{
			(buffer[x+128*l] >> i) & 0x01 ? printf("0") : printf(" ");
		}
		printf("\n");
	}
}


