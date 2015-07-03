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
#include <lua.h>


unsigned char g_SingleBuffer[256];


int l_diDrawImage(lua_State * L)
{
	printf("OK\n");
	FILE * f = fopen("test.dat","rb");
	if (!f) return 0;
	printf("OK\n");
	
	int line = 0;
	int position = 0;
	unsigned char c;
	while (fread(&c, 1, 1, f) > 0)
	{
		if ( line > 15 || position > 128)
		{
			printf("FALSE\n");
			return -1;
		}

		if (c == '\n')
		{
			line++;
			position = 0;
			printf("Line %i\n", line);
		} else {
			if (c == 'x') {
				printf("1\n");
				if (line < 8)
				{
					g_SingleBuffer[position] |= 1 << (7-line);
				} else {
					printf("NOW! %i\n", 128+position);
					g_SingleBuffer[128 + position] |= 1 << (7-(line-8));
				}
			}
			position++;
		}
	}
	
	fclose(f);
}

int l_diDrawDot(lua_State * L)
{
	int n = lua_gettop(L);
	int x, y;
	if (n != 2 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2))
	{
		lua_pushstring(L,"Wrong arguments for function!");
		lua_error(L);
	}
	x = lua_tonumber(L, 1); 
	y = lua_tonumber(L, 2);
	if (x < 0 || y < 0 || x > 127 || y > 15)
	{
		lua_pushstring(L,"Parameters out of range!");
		lua_error(L);
	}
	dDrawDot(g_SingleBuffer, x, y);
	return 0;
}

int l_diSendText(lua_State * L)
{
	int n = lua_gettop(L);
	const unsigned char * msg;
	if (n != 1 || !lua_isstring(L, 1))
	{
		lua_pushstring(L,"Wrong arguments for function!");
		lua_error(L);
	}

	msg = lua_tostring(L, 1);
	if (strlen(msg)>64)
	{
		lua_pushstring(L,"Parameters out of range!");
		lua_error(L);
	}
	sDisplayText(msg);
	return 0;
}

int l_diSend(lua_State * L)
{
	int n = lua_gettop(L);
	if (n != 0)
	{
		lua_pushstring(L,"Wrong number of arguments!");
		lua_error(L);
	}
	sUploadImage(1,g_SingleBuffer);
	sDisplayImage(1);
	return 0;
}

int l_diInvert(lua_State * L)
{
	int n = lua_gettop(L);
	if (n != 0)
	{
		lua_pushstring(L,"Wrong number of arguments!");
		lua_error(L);
	}
	dInvert(g_SingleBuffer);
	return 0;
}

int l_diClear(lua_State * L)
{
	int n = lua_gettop(L);
	if (n != 0)
	{
		lua_pushstring(L, "Wrong number of arguments!");
		lua_error(L);
	}
	dClear(g_SingleBuffer);
	return 0;
}

int runScript(unsigned char * file)
{
	lua_State * l = luaL_newstate();
	luaL_openlibs(l);
	
	lua_pushcfunction(l, l_diClear); lua_setglobal(l, "diClear");
	lua_pushcfunction(l, l_diInvert); lua_setglobal(l, "diInvert");
	lua_pushcfunction(l, l_diSend); lua_setglobal(l, "diSend");
	lua_pushcfunction(l, l_diDrawDot); lua_setglobal(l, "diDrawDot");
	lua_pushcfunction(l, l_diSendText); lua_setglobal(l, "diSendText");
	lua_pushcfunction(l, l_diDrawImage); lua_setglobal(l, "diDrawImage");
	
	luaL_loadfile(l, file);
	
	dClear(g_SingleBuffer);
	
	lua_pcall(l, 0, LUA_MULTRET, 0);
	
	lua_close(l);
}
