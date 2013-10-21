CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(gpiod)

ADD_EXECUTABLE(gpiod gpiod.c inc/configparser.c inc/gpio_helpers.c)
