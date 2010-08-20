sd:gui_sd.c panels.h panels.c
	gcc -o pc_sd gui_sd.c panels.h panels.c -lpthread -lminigui
	arm-linux-gcc -o arm_sd gui_sd.c panels.h panels.c -lpthread -lminigui -lts
clean:
	rm -rf *~ pc_sd arm_sd
