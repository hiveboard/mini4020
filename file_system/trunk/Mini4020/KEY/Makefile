key:key.c gui_key.c
	gcc -o pc_gui_key gui_key.c -lpthread -lminigui
	arm-linux-gcc -o arm_gui_key gui_key.c -lpthread -lminigui -lts
	arm-linux-gcc -o arm_key key.c
clean:
	rm -rf arm_key pc_gui_key arm_gui_key *~  ./driver/*~
	
