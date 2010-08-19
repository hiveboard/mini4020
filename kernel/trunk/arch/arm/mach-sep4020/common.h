struct sys_timer;
typedef volatile unsigned int * RP;
extern void sep4020_map_io(void);
extern void sep4020_init_irq(void);
extern struct sys_timer sep4020_timer;
