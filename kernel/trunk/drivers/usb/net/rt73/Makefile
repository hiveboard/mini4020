#CROSS-COMPILE = arm-linux-
#CC=$(CROSS-COMPILE)gcc

#obj-m := rt73.o
obj-$(CONFIG_RT73) := rt73.o
 
rt73-objs :=  rtmp_main.o mlme.o connect.o rtusb_bulk.o rtusb_io.o sync.o assoc.o auth.o auth_rsp.o rtusb_data.o rtmp_init.o  sanity.o rtmp_wep.o rtmp_info.o rtmp_tkip.o wpa.o md5.o
 
#KDIR    := /home/at91sam9261ek/linux-2.6.20
#PWD     := $(shell pwd)
#default: 
#	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.symvers test

