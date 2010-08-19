#
# Makefile for the linux kernel.
#

# Object file lists.

obj-y			:= irq.o mm.o time.o Devices.o clock.o dma.o
obj-m			:=
obj-n			:=
obj-			:=

obj-$(CONFIG_ARCH_4020) +=4020.o 
obj-$(CONFIG_LEDS)          += $(leds-y)
