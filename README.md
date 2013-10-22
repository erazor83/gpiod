gpiod
=====

GPIOd - configures GPIO and can call executables depending on inputs


Purpose
==========
Personally I've had the issue with some embedded boards to somehow evaluate
input changes on GPIOs which may came from a button press. For example a button
to gracefully shut-down the system.

Sure, this can be done by any other simple python script or whatever, but I wanted it to
be small and effective. Somehow like the acpid is listening for ACPI-events, this tool 
should wait for GPIO changes.

So thats why I wrote the gpiod.

Dependencies
==========
Actually just the libc ;-) .

<pre>
Aria25 gpiod # ldd gpiod
	libc.so.6 => /lib/libc.so.6 (0xb6e98000)
	/lib/ld-linux.so.3 (0xb6fce000)
</pre>

Configuration file
==========
The syntax is pretty simple and is similar to .ini files. Lines starting with a # will be ignored.

There are 4 sections which are expressed via brackets:
  * [GPIOd]
  * [Setup]
  * [Aliases]
  * [Handler]

**GPIOd** is the general section for the daemon, currently only having one option to set up the sys-fs path of gpio.

**Setup** includes what GPIOs should be exported and if these are inputs or outputs.

**Alises** is required since after export, some GPIOs could have a name which does not really 
relate to the GPIO index number.

**Handlers** lists what should be done on input changes.


Example file
===============
<pre>
[GPIOd]
gpio_path = /sys/class/gpio

[Setup]
#leds: green, yellow, red
22 = output,0
23 = output,0
24 = output,0

#halt button
4 = input

92 = output
93 = output

[Aliases]
22 = pioA22
23 = pioA23
24 = pioA24
4 = pioA4

92 = pioC28
93 = pioC29

[Handler]


</pre>
