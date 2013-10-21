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

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "inc/configparser.h"

#define VERSION_STRING  "0.0.1"
#define CONFIG_PATH			"/etc/gpiod.conf"

#define print_err(str) fputs(str,stderr)

void print_help(void) {
	printf("gpiod, version %s\n",VERSION_STRING);
	printf("  - setup GPIOs and manage input-changes\n");
	printf("usage: gpiod [-d] [-h]\n");
	printf("  d : show debug info\n");
	printf("  h : show this help\n");
}

GPIOD_Config_t GPIOd_Config;

int main(int argc, char *argv[]) {
	bool debug=false;
	
	
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

	
}
