/*
 * Copyright (C) 2013 Alexander Krause <alexander.krause@ed-solutions.de>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef _CONFIGPARSER_H_
#define _CONFIGPARSER_H_

#include <stdint.h>
#include <stdbool.h>

const char* CONFIG_SECTION_MAP[CONFIG_SECTION_COUNT];

typedef struct Config_Entry {
	char* name;
	char* value;
} Config_Entry_t;

typedef struct Item_Chain {
	Config_Entry_t* item;
	void* next;
} Item_Chain_t;

typedef struct Config_SectionItems {
	unsigned int count;
	Config_Entry_t** items;
} Config_SectionItems_t;

typedef struct Config {
	bool debug;
	Config_SectionItems_t* sections[CONFIG_SECTION_COUNT];
} Config_t;


/* function prototypes */
int configparser_read(char*,Config_t*);
void configparser_init(Config_t*);
void configparser_section_malloc(Config_t*, Config_Section_t);
void configparser_section_free(Config_SectionItems_t** section);
void configparser_dump_section(Config_t*, Config_Section_t);

char* configparser_get_value(Config_t*,Config_Section_t,char*);

Config_SectionItems_t* configparser_get_section(Config_t*,Config_Section_t);

#define MAX_LINE_LENGTH				256
#define MAX_CFG_NAME_LENGTH		128
#define MAX_CFG_VALUE_LENGTH	128

#endif
