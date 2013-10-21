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

#define GPIOD_SECTION_COUNT		3
const char* CPIOD_SECTION_MAP[GPIOD_SECTION_COUNT];

typedef struct GPIO_Config_Entry {
	char* name;
	char* value;
} GPIO_Config_Entry_t;

typedef struct GPIO_Config_EntryList {
	unsigned int count;
	GPIO_Config_Entry_t* items[];
} GPIO_Config_EntryList_t;

typedef struct GPIOd_Config {
	bool debug;
	GPIO_Config_EntryList_t* section[GPIOD_SECTION_COUNT];
} GPIOD_Config_t;


typedef enum ConfigSection{
	GPIOD_CONFIG_SECTION_UNKNOWN			=-1,
	GPIOD_CONFIG_SECTION_GPIOD				= 0,
	GPIOD_CONFIG_SECTION_GPIO_SETUP		= 1,
	GPIOD_CONFIG_SECTION_HANDLER			= 2
} GPIOD_Config_Section_t;

/* function prototypes */
int configparser_read(char*,GPIOD_Config_t*);
void configparser_init(GPIOD_Config_t*);
void configparser_section_malloc(GPIOD_Config_t*, GPIOD_Config_Section_t);
void configparser_section_free(GPIO_Config_EntryList_t** section);

#define MAX_LINE_LENGTH				256
#define MAX_CFG_NAME_LENGTH		128
#define MAX_CFG_VALUE_LENGTH	128

#endif
