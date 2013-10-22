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

#define _GNU_SOURCE

/*
 * Config file structure:
 * 
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "configparser.h"

void configparser_dump_section(Config_t* config, Config_Section_t cs) {
	unsigned int i;
	Config_SectionItems_t cSectionItems;
	
	printf("Section id #%i\n",cs);
	if (config->sections[cs]==NULL) {
		printf("NULL\n");
	} else if (config->sections[cs]->count==0) {
		printf("empty\n");
	} else {
		cSectionItems=*(config->sections[cs]);
		
		printf("key                  | value\n");
		for (i=0;i<cSectionItems.count;i++) {
			printf("%20s | %s\n",cSectionItems.items[i]->name,cSectionItems.items[i]->value);
		}
	}
}

/*
 * free section given by pointer and also free items if there are any
 */
void configparser_section_free(Config_SectionItems_t** section) {
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
void configparser_section_malloc(Config_t* config, Config_Section_t cs) {
	if (cs!=CONFIG_SECTION_UNKNOWN) {
		configparser_section_free(&config->sections[cs]);
		config->sections[cs]=(Config_SectionItems_t*)malloc(sizeof(Config_SectionItems_t));
	}
	
	//init section
	config->sections[cs]->count=0;
}
void configparser_init(Config_t* config) {
	unsigned int cSIDX;
	
	for (cSIDX=0;cSIDX<CONFIG_SECTION_COUNT;cSIDX++) {
		config->sections[cSIDX]=NULL;
	}
}

/**
 * copy items from item-chain to sections item-list
 */
void configparser_copy_items(
															Config_SectionItems_t* section,
															Item_Chain_t** item_chain,
															unsigned int count
														) {
	Item_Chain_t* cItemChain=*item_chain;
	Item_Chain_t* tItemChain;
	unsigned int sectionItemCount=0;
	
	if (cItemChain!=NULL) {
		section->items=(Config_Entry_t**)malloc(sizeof(Config_Entry_t*)*count);
		while(count--) {
			if (cItemChain==NULL) {
				break;
			}
			/* copy current items to section */
			if (cItemChain->item!=NULL) {
				section->items[sectionItemCount]=cItemChain->item;
				//section->items[sectionItemCount]->
				sectionItemCount++;
				
				/* detach from chain */
				cItemChain->item=NULL;
			}
			
			/* move to next item */
			tItemChain=cItemChain;
			cItemChain=cItemChain->next;
			free(tItemChain);
		}
		section->count=sectionItemCount;
		(*item_chain)=NULL;
	}
}

int configparser_read(char* filename,Config_t* config) {
	FILE* fp;
	char config_line[MAX_LINE_LENGTH];
	char cfg_name[MAX_CFG_NAME_LENGTH];
	char cfg_value[MAX_CFG_VALUE_LENGTH];
	unsigned int line_len;
	unsigned int cSectionIdx;
	unsigned int cSectionItemCount=0;
	Item_Chain_t* fItemChain=NULL; /* first item */
	Item_Chain_t* tItemChain=NULL; /* temporary item */
	
	Config_Section_t cConfigSection;
	
	fp = fopen (filename, "rt");
	
	if (fp==NULL) {
		perror("configfile");
		return -1;
	} else {
		if (config->debug) {
			printf("== config parser ==\n");
		}
		while( fgets(config_line, MAX_LINE_LENGTH, fp)!=NULL ) {
			line_len=strlen(config_line);
			if ((config_line[0]=='[') && (config_line[line_len-2]==']')) {
				/* handle section lines */
				config_line[line_len-2]=0;
				sscanf(config_line, "[%s", cfg_name);
				if (config->debug) {
					printf("\nNew section found: %s\n",cfg_name);
				}
				
				/* previous section found, now copy items */
				if (cConfigSection!=CONFIG_SECTION_UNKNOWN) {
					configparser_copy_items(config->sections[cConfigSection],&fItemChain,cSectionItemCount);
				}
				cConfigSection=CONFIG_SECTION_UNKNOWN;
				cSectionIdx=CONFIG_SECTION_COUNT;
				while(cSectionIdx--) {
					if (strcasestr(cfg_name,CONFIG_SECTION_MAP[cSectionIdx])) {
						cConfigSection=cSectionIdx;
						break;
					}
				}
				configparser_section_malloc(config,cSectionIdx);
			} else {
				/* handle key-value pairs */
				if (cConfigSection!=CONFIG_SECTION_UNKNOWN) {
					if (sscanf(config_line, "%s = %s", cfg_name,cfg_value)==2) {
						if (config->debug) {
							printf("%s = %s\n", cfg_name,cfg_value);
						}
						cSectionItemCount++;
						if (fItemChain==NULL) {
							fItemChain=malloc(sizeof(Item_Chain_t));
							fItemChain->item=malloc(sizeof(Config_Entry_t));
							tItemChain=fItemChain;
						} else {
							tItemChain->next=malloc(sizeof(Item_Chain_t));
							tItemChain=tItemChain->next;
							tItemChain->item=malloc(sizeof(Config_Entry_t));
						}
						tItemChain->next=NULL;
						
						/* copy name / value into item */
						tItemChain->item->name=malloc(strlen(cfg_name)+1);
						strcpy(tItemChain->item->name,cfg_name);
						
						tItemChain->item->value=malloc(strlen(cfg_value)+1);
						strcpy(tItemChain->item->value,cfg_value);
					}
				}
			}
		}
		fclose(fp);
		if (config->debug) {
			printf("== ==\n");
		}

		if (cConfigSection!=CONFIG_SECTION_UNKNOWN) {
			/* end of file, now copy items */
			configparser_copy_items(config->sections[cConfigSection],&fItemChain,cSectionItemCount);
		}
	}
	return 0;
}

char* configparser_get_value(Config_t* config,Config_Section_t cs,char* key) {
	unsigned int i;
	Config_SectionItems_t cSectionItems;
	
	if (config->sections[cs]==NULL) {
		return NULL;
	} else if (config->sections[cs]->count==0) {
		return NULL;
	} else {
		cSectionItems=*(config->sections[cs]);
		
		for (i=0;i<cSectionItems.count;i++) {
			if (strcasestr(cSectionItems.items[i]->name,key)) {
				return cSectionItems.items[i]->value;
			}
		}
	}
	return NULL;
}

Config_SectionItems_t* configparser_get_section(Config_t* config,Config_Section_t cs) {
	return config->sections[cs];
}

#endif
