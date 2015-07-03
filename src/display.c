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
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>


#include "display.h"
int g_SerialFD;

void create_paket(struct display_cmd * cmd, unsigned char ** buffer, int * size)
{
	int fake_size = 7 + cmd->data_size; // 1 Startbyte, 2 CRC Bytes, 1 Byte Laenge, 1 Byte jeweils Command, Address und Nummer  
	int real_size = fake_size;
	unsigned char * buf = NULL;
	unsigned short crc;
	static int g_s = 3;
	int i = 0;
	
	for (i = 0; i < cmd->data_size; i++)
	{
		if (cmd->data[i] == 0x02 || cmd->data[i] == 0x01)
		{
			real_size++;
		}
	}
	printf("\nSize: %i Real Size: %i Fake Size: %i\n", fake_size - 1, real_size, fake_size);

	// HACK!!!!!!!!!!!!!!!!!!!!!!!!!!
	buf = (unsigned char *)malloc(real_size);///malloc(real_size);

	printf("DONE\n");
	buf[0] = 0x02;
	buf[1] = fake_size - 1;
	buf[2] = cmd->address;
	buf[3] = g_s++;
	buf[4] = cmd->command;

	if (cmd->data_size > 0)
	{
		memcpy(&buf[5], cmd->data, cmd->data_size);
	}

	crc = calcCRC16LC(&buf[1], fake_size - 3);

	if (cmd->data_size > 0)
	{
		unsigned char pos = 0;
		for (i = 0; i < cmd->data_size; i++)
		{
			if (cmd->data[i] == 0x01)
			{
				buf[5+pos] = 0x01;
				pos++;
				buf[5+pos] = 0x03;
				pos++;
			} else if (cmd->data[i] == 0x02 )
			{
				buf[5+pos] = 0x01;
				pos++;
				buf[5+pos] = 0x04;
				pos++;
			} else {
				buf[5+pos] = cmd->data[i];
				pos++;
			}
		}	
	}
	

	buf[real_size-2] = (0xff00 & crc) >> 8;
	buf[real_size-1] = (0x00ff & crc);

	*buffer = buf;
	*size = real_size;
}

void cleanup_display_cmd(struct display_cmd ** cmd)
{
	if (!*cmd) return;
	free((*cmd)->data);
	free(*cmd);
	*cmd = NULL;
}

void create_display_cmd(struct display_cmd ** cmd)
{
	*cmd = (struct display_cmd *)malloc(sizeof(struct display_cmd));
	memset(*cmd, 0, sizeof(struct display_cmd));
}


unsigned short calcCRC16LC(unsigned char * data, short size)
{
	unsigned short result = 0x0000;
	// Leurocom will die ersten zwei Bytes bei CRC vertauscht
	// Wir sparen Speicher und kopieren nicht das gesamte Paket
	unsigned char tmp = data[0];
	data[0] = data[1];
	data[1] = tmp;
	result = calcCRC16(data, size);
	// Zuruecktauschen
	data[1] = data[0];
	data[0] = tmp;
	return result;
}

// CRC16: Siehe http://de.wikipedia.org/wiki/Zyklische_Redundanzpruefung
unsigned short calcCRC16(unsigned char * data, short size)
{
	// Ende der Daten berechnen
	unsigned char * end = data + (size * sizeof(char));
	// Startwert ist 0
	unsigned short crc = 0x0000;

	int i;

	// Solange bis Ende der Ende der Daten erreicht wurde, berechnen
	for (;data < end; data++)
	{
		for (i = 7; i >= 0; i--)
		{
			// Pruefen ob die ersten beiden Bits von CRC und den Daten uebereinstimmen
			if ((crc >> 15) != ((*data >> i) & 1))
			{
				crc = crc << 1;
				// XOR mit Polynom
				crc ^= 0x1021;
			} else {
				crc = crc << 1;
			}
		}
	}

	return crc;
}

int sReadAnswer()
{
	int fd = g_SerialFD;
	unsigned char c = 0;
	int i = -1;
	int wait = 0;
	unsigned char buf[4];
	printf("\n\nWait: ");
	while (i < 4 && wait < 200)
	{
		int r = read(fd, &c, 1);
		wait++;
		usleep(2000);
		printf(".");
		if (r <= 0) continue;

		if (c == 0x02)
		{
			i = 0;
			buf[0] = c;
		}
		if (i >= 0 && i < 4)
		{
			buf[i++] = c;
		}
	}
	printf(" %i ms\n\n", wait*2);
	usleep(10000);
	if (i != 4) return -1; else return buf[3];
}

int sDisplayImage(unsigned char nr)
{
	
	struct display_cmd * c =  NULL;
	unsigned char * buffer = NULL;
	int size = 0;
	int ret;
	
	create_display_cmd(&c);
	c->address = DISPLAY1;

	c->command = M10_CMD_INIT_TEXT;
	char info[2] = { 0x01, 0x00 };
	c->data = info;
	c->data_size = 2;
	create_paket(c, &buffer, &size);
	write(g_SerialFD, buffer, size);
	free(buffer);
	ret = sReadAnswer();
	//printf("\nRead answer: %i\n", sReadAnswer());
	
	c->command = M10_CMD_SEND_TEXT;
	char init[] = { M10_TC_GRAPHIC, nr, 0x00, 0xff, 0x00, 0xff };
	c->data = init;
	c->data_size = sizeof(init);
	create_paket(c, &buffer, &size);
	write(g_SerialFD, buffer, size);
	ret = sReadAnswer();
	//printf("\nRead answer: %i\n", sReadAnswer());
	free(buffer);

	free(c);
}

int sUploadImage(unsigned char nr, unsigned char image[256])
{
	struct display_cmd * c =  NULL;
	unsigned char * buffer = NULL;
	int size = 0;

	int ret = -1;
	int i;
	
	create_display_cmd(&c);
	c->address = DISPLAY1;

	c->command = M10_CMD_INIT_GRAPHIC;
	char infor[6] = { 0x04, 0x00, nr, 128, 2, 1 };
	c->data = infor;
	c->data_size = 6;
	create_paket(c, &buffer, &size);
	write(g_SerialFD, buffer, size);
	free(buffer);
	
	ret = sReadAnswer();
	//printf("\nRead answer: %i\n", ret);

	c->command = M10_CMD_SEND_GRAPHIC;
	c->data_size = 64;

	for (i = 0; i < 4; i++)
	{
		c->data = &(image[64*i]);
		create_paket(c, &buffer, &size);
		write(g_SerialFD, buffer, size);
		ret = sReadAnswer();
		//printf("\nRead answer: %i\n", sReadAnswer());
		free(buffer);
	}

	free(c);
}

int sDisplayText(unsigned char * text)
{
	struct display_cmd * c =  NULL;
	unsigned char * buffer = NULL;
	int size = 0;
	int ret;
	create_display_cmd(&c);
	c->address = DISPLAY1;
	
	c->command = M10_CMD_INIT_TEXT;
	char info[2] = { 0x01, 0x00 };
	c->data = info;
	c->data_size = 2;
	create_paket(c, &buffer, &size);
	write(g_SerialFD, buffer, size);
	free(buffer);
	ret = sReadAnswer();
	//printf("\nRead answer: %i\n", sReadAnswer());

	c->command = M10_CMD_SEND_TEXT;
	unsigned char infopart1[] = { M10_TC_TXT_LAUF_LINKS, 0x01, 0x01, 0x08, 0x01, 0x00, 0x00, 0x01};
	unsigned char infopart2[] = { 0xfe, 0xff };
	
	int textlength = strlen(text);
	if (textlength > 64) textlength = 64;
	int infolength = textlength  + sizeof(infopart1) + sizeof(infopart2);
	unsigned char * infotext = malloc(infolength);

	memset(infotext, 0, infolength);

	memcpy(infotext, infopart1, sizeof(infopart1));
	memcpy(&(infotext[sizeof(infopart1)]), text, textlength);
	memcpy(&(infotext[sizeof(infopart1)+textlength]), infopart2, sizeof(infopart2));

	c->data = infotext;
	c->data_size = infolength;
	
	create_paket(c, &buffer, &size);
	
	write(g_SerialFD, buffer, size);
	ret = sReadAnswer();
	//printf("\nRead answer: %i\n", sReadAnswer());
	
	free(buffer);
	free(infotext);
	free(c);
}

int sDisplayCOMOK()
{
	struct display_cmd * c =  NULL;
	unsigned char * buffer = NULL;
	int size = 0;
	int ret;
	create_display_cmd(&c);
	c->address = DISPLAY1;
	
	c->command = M10_CMD_COM_OK;
	create_paket(c, &buffer, &size);

	write(g_SerialFD, buffer, size);
	
	free(buffer);
	free(c);
	ret = sReadAnswer();
}

int sOpenDisplay(unsigned char * port)
{
	struct termios oldtio, newtio;
	g_SerialFD = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);

	if (!g_SerialFD) exit(EXIT_FAILURE);
	
	tcgetattr(g_SerialFD, &oldtio);
	newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD/* | CRTSCTS*/;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VMIN]=1;
	newtio.c_cc[VTIME]=0;
	tcflush(g_SerialFD, TCIFLUSH);
	tcsetattr(g_SerialFD, TCSANOW, &newtio);
	return g_SerialFD;
}

void sCloseDisplay()
{
	close(g_SerialFD);
}

/*
	


*/

/*	c->command = M10_CMD_STATUS;
	c->data_size = 0;
	c->data = NULL;
	create_paket(c, &buffer, &size);
	for (cc = 0; cc < size; cc++) printf("%x\t", buffer[cc]);
	write(fd, buffer, size);
	free(buffer);
	sleep(1);
	printf("\nRead answer: %i\n", read_answer(fd));
*/

