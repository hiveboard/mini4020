usb:gui_usb.c panels.h panels.c
	gcc -o pc_usb gui_usb.c panels.h panels.c -lpthread -lminigui
	arm-linux-gcc -o arm_usb gui_usb.c panels.h panels.c -lpthread -lminigui -lts
clean:
	rm -rf *~ pc_usb arm_usb
