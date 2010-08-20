led:led.c gui_led.c
	gcc -o pc_gui_led gui_led.c -lpthread -lminigui
	arm-linux-gcc -o arm_gui_led gui_led.c -lpthread -lminigui
	arm-linux-gcc -o arm_led led.c
clean:
	rm -rf arm_led pc_gui_led arm_gui_led *~  ./driver/*~
	
