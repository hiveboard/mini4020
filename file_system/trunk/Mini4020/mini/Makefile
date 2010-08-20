MINI_OBJECTS = mini.o panels.o same.o pwm.o
#CC = gcc
CC = arm-linux-gcc
LIBS = -lpthread -lminigui -lmgext -lts
LINK = $(CC) -o $@ 

mini: $(MINI_OBJECTS)
	$(LINK) $(LIBS) $(MINI_OBJECTS)

%.o: %.c
	@echo '$(CC)-g -c $<'; \
	$(CC) -g -c $< 

clean:
	rm -rf mini *.o *~
