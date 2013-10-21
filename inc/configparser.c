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
#ifndef _CONFIGPARSER_C_
#define _CONFIGPARSER_C_

/*
 * Config file structure:
 * 
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "configparser.h"
const char* CPIOD_SECTION_MAP[] = {
	"GPIOd",
	"GPIO-Setup",
	"Handler"
};

/*
 * free section given by pointer and also free items if there are any
 */
void configparser_section_free(GPIO_Config_EntryList_t** section) {
	unsigned int cItem;
	
	if (*section!=NULL) {
		for (cItem=0;cItem<((*section)->count);cItem++) {
			if ((*section)->items[cItem]!=NULL) {
				free((*section)->items[cItem]);
				(*section)->items[cItem]=NULL;
				(*section)->count=0;
			}
		}
		free(*section);
		*section=NULL;
	}
}

/*
 * create a new section with empty item-lists
 */
void configparser_section_malloc(GPIOD_Config_t* config, GPIOD_Config_Section_t cs) {
	if (cs!=GPIOD_CONFIG_SECTION_UNKNOWN) {
		configparser_section_free(&config->section[cs]);
		config->section[cs]=(GPIO_Config_EntryList_t*)malloc(sizeof(GPIO_Config_EntryList_t));
	}
	
	//init section
	config->section[cs]->count=0;
}
void configparser_init(GPIOD_Config_t* config) {
	unsigned int cSIDX;
	
	for (cSIDX=0;cSIDX<GPIOD_SECTION_COUNT;cSIDX++) {
		config->section[cSIDX]=NULL;
	}
}

int configparser_read(char* filename,GPIOD_Config_t* config) {
	FILE* fp;
	char config_line[MAX_LINE_LENGTH];
	char cfg_name[MAX_CFG_NAME_LENGTH];
	char cfg_value[MAX_CFG_VALUE_LENGTH];
	int line_len;
	int cSectionIdx;
	
	GPIOD_Config_Section_t cConfigSection;
	
	fp = fopen (filename, "rt");
	
	if (fp==NULL) {
		return -1;
	} else {
		while( fgets(config_line, MAX_LINE_LENGTH, fp)!=NULL ) {
			line_len=strlen(config_line);
			if ((config_line[0]=='[') && (config_line[line_len-2]==']')) {
				/* handle section lines */
				config_line[line_len-2]=0;
				sscanf(config_line, "[%s", cfg_name);
				if (config->debug) {
					printf("New section found: %s\n",cfg_name);
				}
				cConfigSection=GPIOD_CONFIG_SECTION_UNKNOWN;
				cSectionIdx=GPIOD_SECTION_COUNT;
				while(cSectionIdx--) {
					if (strcasestr(cfg_name,CPIOD_SECTION_MAP[cSectionIdx])) {
						cConfigSection=cSectionIdx;
						break;
					}
				}
				configparser_section_malloc(config,cSectionIdx);
			} else {
				/* handle key-value pairs */
				if (cConfigSection!=GPIOD_CONFIG_SECTION_UNKNOWN) {
					sscanf(config_line, "%s = %s", cfg_name,cfg_value);
					if (config->debug) {
						printf("%s = %s\n", cfg_name,cfg_value);
					}
					/*
					config->section[cConfigSection]->count++;
					
					config->section[cConfigSection]->items;
					*/
				}
			}
		}
		fclose(fp);
	}
}

#endif
