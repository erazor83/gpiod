/* Copyright (c) 2011, RidgeRun
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the RidgeRun.
 * 4. Neither the name of the RidgeRun nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY RIDGERUN ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL RIDGERUN BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _GPIO_HELPERS_C_
#define _GPIO_HELPERS_C_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

 /****************************************************************
 * Constants
 ****************************************************************/
 
#include "gpio_helpers.h"

#define MAX_BUF 64

/****************************************************************
 * gpio_export
 ****************************************************************/
int gpio_export(char* gpio)
{
	int fd;
	char filename[MAX_SYSFS_FILENAME_LENGTH];
	
	snprintf(filename,sizeof(filename),"%s/export",SYSFS_GPIO_DIR);

	fd = open(filename, O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}
 
	write(fd, gpio, strlen(gpio));
	close(fd);
 
	return 0;
}

/****************************************************************
 * gpio_unexport
 ****************************************************************/
int gpio_unexport(char* gpio)
{
	int fd;
	char filename[MAX_SYSFS_FILENAME_LENGTH];
	
	snprintf(filename,sizeof(filename),"%s/unexport",SYSFS_GPIO_DIR);
 
	fd = open(filename, O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}
 
	write(fd, gpio, strlen(gpio));
	close(fd);
	return 0;
}

/****************************************************************
 * gpio_set_dir
 ****************************************************************/
int gpio_set_dir(char* gpio, unsigned int out_flag)
{
	int fd;
	char filename[MAX_SYSFS_FILENAME_LENGTH];
	
	snprintf(filename,sizeof(filename),"%s/%s/direction",SYSFS_GPIO_DIR,gpio);
 
	fd = open(filename, O_WRONLY);
	if (fd < 0) {
		perror("gpio/direction");
		return fd;
	}
 
	if (out_flag)
		write(fd, "out", 4);
	else
		write(fd, "in", 3);
 
	close(fd);
	return 0;
}

/****************************************************************
 * gpio_set_value
 ****************************************************************/
int gpio_set_value(char* gpio, unsigned int value)
{
	int fd;
	char filename[MAX_SYSFS_FILENAME_LENGTH];
	
	snprintf(filename,sizeof(filename),"%s/%s/value",SYSFS_GPIO_DIR,gpio);
 
	//printf("%s = %i\n",filename,value);
	
	fd = open(filename, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-value");
		return fd;
	}
 
	if (value)
		write(fd, "1", 2);
	else
		write(fd, "0", 2);
 
	close(fd);
	return 0;
}

/****************************************************************
 * gpio_get_value
 ****************************************************************/
int gpio_get_value(char* gpio, unsigned int *value)
{
	int fd;
	char ch;
	char filename[MAX_SYSFS_FILENAME_LENGTH];
	
	snprintf(filename,sizeof(filename),"%s/%s/value",SYSFS_GPIO_DIR,gpio);

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		perror("gpio/get-value");
		return fd;
	}
 
	read(fd, &ch, 1);

	if (ch != '0') {
		*value = 1;
	} else {
		*value = 0;
	}
 
	close(fd);
	return 0;
}


/****************************************************************
 * gpio_set_edge
 ****************************************************************/

int gpio_set_edge(char* gpio, char *edge)
{
	int fd;
	char buf[MAX_BUF];
	char filename[MAX_SYSFS_FILENAME_LENGTH];
	
	snprintf(filename,sizeof(filename),"%s/%s/edge",SYSFS_GPIO_DIR,gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-edge");
		return fd;
	}
 
	write(fd, edge, strlen(edge) + 1); 
	close(fd);
	return 0;
}

/****************************************************************
 * gpio_fd_open
 ****************************************************************/

int gpio_fd_open(char* gpio)
{
	int fd;
	char filename[MAX_SYSFS_FILENAME_LENGTH];

	snprintf(filename,sizeof(filename),"%s/%s/value",SYSFS_GPIO_DIR,gpio);

	fd = open(filename, O_RDONLY | O_NONBLOCK );
	if (fd < 0) {
		perror("gpio/fd_open");
	}
	return fd;
}

/****************************************************************
 * gpio_fd_close
 ****************************************************************/

int gpio_fd_close(int fd)
{
	return close(fd);
}

#endif
