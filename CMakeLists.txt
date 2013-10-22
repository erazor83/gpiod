CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(gpiod)

SET(CMAKE_C_FLAGS "-I -g -Wall")

ADD_EXECUTABLE(gpiod gpiod.c inc/gpio_helpers.c)
