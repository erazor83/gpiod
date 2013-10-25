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

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>


typedef enum ConfigSection{
	CONFIG_SECTION_UNKNOWN						=-1,
	GPIOD_CONFIG_SECTION_GPIOD				= 0,
	GPIOD_CONFIG_SECTION_GPIO_SETUP		= 1,
	GPIOD_CONFIG_SECTION_GPIO_ALIASES	= 2,
	GPIOD_CONFIG_SECTION_HANDLER			= 3
} Config_Section_t;

#define CONFIG_SECTION_COUNT		4

const char* CONFIG_SECTION_MAP[] = {
	"GPIOd",
	"Setup",
	"Aliases",
	"Handler"
};


#include "inc/configparser.c"

#include "inc/gpio_helpers.h"

#define VERSION_STRING  "0.0.1"

#define CONFIG_PATH			"/etc/gpiod.conf"

#define DEFAULT_GPIO_PATH				"/sys/class/gpio/"

#define print_err(str) fputs(str,stderr)

void print_help(void) {
	printf("gpiod, version %s\n",VERSION_STRING);
	printf("  - setup GPIOs and manage input-changes\n");
	printf("usage: gpiod [-d] [-w] [-h]\n");
	printf("  d : show debug info\n");
	printf("  w : watch inputs for changes\n");
	printf("  h : show this help\n");
}

Config_t GPIOd_Config;

int main(int argc, char *argv[]) {
	bool debug=false;
	bool watch_inputs=false;

	char* gpio_path;
	unsigned int i;
	
	Config_SectionItems_t* cConfigSection;
	char* tGPIO_id;
	char* tGPIO_alias;
	char* tGPIO_name;
	char* tValue;
	char* tValue2;
	
	struct pollfd *fdset;
	int *GPIOfds;
	unsigned int fd_count;
	
	char buf[128];
	
	/* pretty simple arg parsing */
	while (argc--) {
		//printf("argv: %s\n",argv[argc]);
		if        (strstr(argv[argc],"-h")!=NULL){
			print_help();
			return 0;
		} else if (strstr(argv[argc],"-d")!=NULL) {
			debug=true;
		} else if (argc>0){
			print_help();
			return -1;
		}
	}
	
		
	configparser_init(&GPIOd_Config);
	GPIOd_Config.debug=debug;

	if (debug) {
		printf("Trying to read config %s \n",CONFIG_PATH);
	}
	if (configparser_read(CONFIG_PATH,&GPIOd_Config)!=0) {
		print_err("error while reading config\n");
	}


	//configparser_dump_section(&GPIOd_Config,GPIOD_CONFIG_SECTION_GPIOD);
	
	gpio_path=configparser_get_value(&GPIOd_Config,GPIOD_CONFIG_SECTION_GPIOD,"gpio_path");
	//configparser_dump_section(&GPIOd_Config,GPIOD_CONFIG_SECTION_GPIOD);
	
	if (gpio_path==NULL) {
		gpio_path=malloc(sizeof(DEFAULT_GPIO_PATH));
		strcpy(gpio_path,DEFAULT_GPIO_PATH);
	}
	
	if (debug) {
		printf("\ngpio_path=%s\n",gpio_path);
		
	}
	strcpy(SYSFS_GPIO_DIR,gpio_path);
	//setup gpios
	if (debug) {
		printf("Setting up GPIOs...\n");
	}
	//configparser_dump_section(&GPIOd_Config,GPIOD_CONFIG_SECTION_GPIO_SETUP);
	cConfigSection=GPIOd_Config.sections[GPIOD_CONFIG_SECTION_GPIO_SETUP];
	if (cConfigSection!=NULL) {
		for (i=0;i<(cConfigSection->count);i++) {
			tGPIO_id=cConfigSection->items[i]->name;
			tGPIO_alias=configparser_get_value(&GPIOd_Config,GPIOD_CONFIG_SECTION_GPIO_ALIASES,tGPIO_id);
			if (tGPIO_alias!=NULL) {
				tGPIO_name=tGPIO_alias;
			} else {
				tGPIO_name=tGPIO_id;
			}
			
			if (tGPIO_name!=NULL) {
				tValue=cConfigSection->items[i]->value;
				if (debug) {
					printf("%s -> %s\n",tGPIO_name,tValue);
				}
				gpio_export(tGPIO_id);
				if				(strcasestr(tValue,"input")) {
					gpio_set_dir(tGPIO_name, 0);
					/* also manage edges*/
					if				(strcasestr(tValue,"rising")) {
						gpio_set_edge(tGPIO_name, "rising");
					} else if (strcasestr(tValue,"falling")) {
						gpio_set_edge(tGPIO_name, "falling");
					}
				} else if (strcasestr(tValue,"output")) {
					gpio_set_dir(tGPIO_name, 1);
					tValue2=strchr(tValue,',');
					if (tValue2!=NULL) {
						// , found, next comes the value
						gpio_set_value(tGPIO_name, atol(tValue2+1));
					}
				}
			}
		}
		//configparser_dump_section(&GPIOd_Config,GPIOD_CONFIG_SECTION_GPIOD);
	} else {
		printf("No \"GPIO-setup\" section");
	}

  if (!watch_inputs) {
		//we're done
		return 0;
	}

	//watch inputs
	if (debug) {
		printf("Creating GPIO-Watchlist...");
	}
	//configparser_dump_section(&GPIOd_Config,GPIOD_CONFIG_SECTION_HANDLER);
	cConfigSection=GPIOd_Config.sections[GPIOD_CONFIG_SECTION_HANDLER];
	if (cConfigSection!=NULL) {
		fd_count=cConfigSection->count+1;
		fdset=malloc(fd_count*sizeof(struct pollfd));
		memset((void*)fdset, 0, fd_count);
		
		GPIOfds=malloc(cConfigSection->count*sizeof(int));
		memset((void*)GPIOfds, 0, fd_count-1);
		
		fdset[0].fd = STDIN_FILENO;
		fdset[0].events = POLLIN;
		
		for (i=0;i<(cConfigSection->count);i++) {
			tGPIO_id=cConfigSection->items[i]->name;
			tGPIO_alias=configparser_get_value(&GPIOd_Config,GPIOD_CONFIG_SECTION_GPIO_ALIASES,tGPIO_id);
			if (tGPIO_alias!=NULL) {
				tGPIO_name=tGPIO_alias;
			} else {
				tGPIO_name=tGPIO_id;
			}
			if (tGPIO_name!=NULL) {
				GPIOfds[i]=gpio_fd_open(tGPIO_name);
				fdset[i+1].fd = GPIOfds[i];
				fdset[i+1].events = POLLPRI;
				tValue=cConfigSection->items[i]->value;
				if (debug) {
					printf("Adding fp=%i for GPIO#%s",GPIOfds[i],tGPIO_id);
				}
			}
		}

		
		while (1) {
			i = poll(fdset, fd_count, 1000);      
			
			if (i < 0) {
				printf("\npoll() failed!\n");
				return -1;
			}
				
			if (i == 0) {
				printf(".");
			}
							
			if (fdset[1].revents & POLLPRI) {
				//len = read(fdset[1].fd, buf, 128);
				//printf("\npoll() GPIO %d interrupt occurred\n", gpio);
			}

			if (fdset[0].revents & POLLIN) {
				(void)read(fdset[0].fd, buf, 1);
				printf("\npoll() stdin read 0x%2.2X\n", (unsigned int) buf[0]);
			}

			fflush(stdout);
		}

	//gpio_fd_close(gpio_fd);
	} else {
		printf("No \"Handler\" section");
	}

	return 0;
}
