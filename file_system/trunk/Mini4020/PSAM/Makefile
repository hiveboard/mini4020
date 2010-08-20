psam:psam.c gui_psam.c
	#gcc -o pc_gui_psam gui_psam.c -lpthread -lminigui
	arm-linux-gcc -o arm_gui_psam gui_psam.c -lpthread -lminigui
	arm-linux-gcc -o arm_psam psam.c
clean:
	rm -rf arm_psam pc_gui_psam arm_gui_psam *~  ./driver/*~
	
