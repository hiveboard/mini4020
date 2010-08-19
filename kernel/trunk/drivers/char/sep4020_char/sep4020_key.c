#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <asm/semaphore.h>
#include <linux/delay.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <linux/time.h>

#define KEY_MAJOR 254 /*主设备号*/
#define MAX_KEY_BUF 16
#define KEY_XNUM 5 
#define KEY_YNUM 5
#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_UNSURE 2
#define KEY_TIMER_DELAY1 4
#define KEY_TIMER_DELAY2 10

struct KEY_DEV{    
	unsigned int keystatus;     
	unsigned char buf[MAX_KEY_BUF];     
	unsigned long head,tail;     
	struct cdev cdev;
};

DECLARE_MUTEX_LOCKED(key_press);
//static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

int keynum =0;
char a[17] = {};
int strlength=0;
int real_keynum = 0;
char KEY_MAP[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y'};
				

struct KEY_DEV *key_dev;
static struct timer_list key_timer;

static void unmaskkey(void)
{
	enable_irq(INTSRC_EXTINT0);
	enable_irq(INTSRC_EXTINT1);
	enable_irq(INTSRC_EXTINT2);
	enable_irq(INTSRC_EXTINT3);
	enable_irq(INTSRC_EXTINT4);
}

static void maskkey(void)
{
	disable_irq(INTSRC_EXTINT0);
	disable_irq(INTSRC_EXTINT1);
	disable_irq(INTSRC_EXTINT2);
	disable_irq(INTSRC_EXTINT3);
	disable_irq(INTSRC_EXTINT4);
}

static void write_row(int index, int HighLow)
{
	switch (index)
	{
		case 0:
			if (HighLow) 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V |= 0x01;
			}
			else 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V &= ~0x01;
			}
			break;
    	case 1: 
			if (HighLow) 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V |= 0x02;
			}
			else 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V &= ~0x02;
			}
			break;
    	case 2: 
			if (HighLow) 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V |= 0x04;
			}
			else 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V &= ~0x04;
			}
			break;
		case 3:
			if (HighLow) 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V |= 0x08;
			}
			else 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V &= ~0x08;
			}
			break;
		case 4: 
			if (HighLow) 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V |= 0x10;
			}
			else 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V &= ~0x10;
			}
			break;
		default:
			break;
	}
}

static void keyevent(void)
{
	int i = 0;
	int col = 0, row = 0;
	int init_col = 0;
	int col_count = 0, row_count = 0, delay;	
	int view;
	view = *(volatile unsigned long*)(INTC_ISR_V);
	init_col = (*(volatile unsigned long*)(INTC_ISR_V)>>1) & 0x001F;
	for (i=0; i<5; i++)    
	{
		if ((init_col >> i) & 0x01 )  
		{
			col = i ;
			col_count++;
		}
	}
	if ((!read_col(col)))
	{
		for (i=0; i<5; i++)   	//通过对轮流PD0~PD4输出高电平；获取行数。
                { 
			write_row(i,1);
			for (delay=0; delay<100; delay++); //delay
			if (read_col(col))
      		        { 
      		row = i ;
        		row_count++;
      		        }
      		write_row(i, 0);
                } 
        } 
        
	if (col_count==1 && row_count==1)
	{
		keynum = col * 5 + row;		
	}
}

static int read_col(int index)
{
	return (*(volatile unsigned long*)GPIO_PORTA_DATA_V >> index) & 0x1;
}

static void sep4020_key_setup(void)
{
	*(volatile unsigned long*)GPIO_PORTD_SEL_V  |= 0x1F ;      //for common use
	*(volatile unsigned long*)GPIO_PORTD_DIR_V  &= (~0x1F);    //0 stands for OUT
	*(volatile unsigned long*)GPIO_PORTD_DATA_V &= (~0x1F);    

	*(volatile unsigned long*)GPIO_PORTA_SEL_V |= 0x001F ;     //for common use
	*(volatile unsigned long*)GPIO_PORTA_DIR_V |= 0x001F ;     //1 stands for in
	*(volatile unsigned long*)GPIO_PORTA_INTRCTL_V |= 0x03ff;  //低电平触发
	*(volatile unsigned long*)GPIO_PORTA_INCTL_V |= 0x001F;    //做通用用途输入时可配置该寄存器。选择外部中断输入时相应位置1
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V |= 0x001F;  //相应位配置为1时表示将清除对应中断为产生的中断
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V = 0x0000;       
//	unmaskkey();
}

static irqreturn_t sep4020_key_irqhandler(int irq, void *dev_id, struct pt_regs *reg)
{
	maskkey();
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V |= 0x001F;
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V = 0x0000;

	key_dev->keystatus = KEY_UNSURE;
	key_timer.expires = jiffies + KEY_TIMER_DELAY1;
	add_timer(&key_timer);

	return IRQ_HANDLED;
}

static void key_timer_handler(void)
{
	int j = 0;
	j = *(volatile unsigned long*)GPIO_PORTA_DATA_V;
	if ((j&0x1f) != 0x1f)
	{
		if (key_dev->keystatus == KEY_UNSURE)
		{
			key_dev->keystatus = KEY_DOWN;
			key_timer.expires = jiffies + KEY_TIMER_DELAY2;
		   keyevent();
         real_keynum = keynum;
         key_dev->buf[key_dev->head++ % MAX_KEY_BUF] = KEY_MAP[real_keynum];
         up(&key_press);
     		add_timer(&key_timer);							
		}
		else
		{
			key_timer.expires = jiffies + KEY_TIMER_DELAY2;
			add_timer(&key_timer);
		}
	}
	else
	{
		if (key_dev->keystatus == KEY_DOWN) 		
		{
			key_dev->keystatus = KEY_UP;
		 	key_dev->buf[key_dev->head++ % MAX_KEY_BUF] = KEY_MAP[real_keynum] | 0x80;
		 	up(&key_press);
		}
	
		unmaskkey();
	}	
}

static ssize_t sep4020_key_read(struct file *filp, char __user *buff, size_t size, loff_t *ppos)
{
	int m=0;
	unsigned long err;
	int i;
 	retry:		
   	if ((key_dev->head) > (key_dev->tail)) 
		{
    		m = (key_dev->head) - (key_dev->tail);
        	if (size > m)
      			{
				size = m;
			}
      	for (i=1; i<(size+1); i++)
			{
      		a[i] = key_dev->buf[key_dev->tail++ % MAX_KEY_BUF];
      			}
			err = copy_to_user(buff, a, (size+1)*1);
			return err ? -EFAULT : 0;
		}
		else 
		{
			down_interruptible(&key_press);
			goto retry;
		}
	return sizeof(unsigned char);
}

static ssize_t sep4020_key_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	return 0;
}

int sep4020_key_open(struct inode *inode, struct file *filp)
{
	sep4020_key_setup();
	return 0;
}

int sep4020_key_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static const struct file_operations sep4020_key_fops = 
{
	.owner = THIS_MODULE,
	.read  = sep4020_key_read,
	.write = sep4020_key_write,
	.open  = sep4020_key_open,
	.release = sep4020_key_release,
};

static int sep4020_request_irqs(void)
{
	if (request_irq(INTSRC_EXTINT0,sep4020_key_irqhandler,SA_INTERRUPT,"4020KEY",NULL))	
	{
		printk("request button irq0 failed!\n");
		return -1;			
	}
	if (request_irq(INTSRC_EXTINT1,sep4020_key_irqhandler,SA_INTERRUPT,"4020KEY",NULL))	
	{
		printk("request button irq1 failed!\n");
		return -1;			
	}
	if (request_irq(INTSRC_EXTINT2,sep4020_key_irqhandler,SA_INTERRUPT,"4020KEY",NULL))	
	{
		printk("request button irq2 failed!\n");
		return -1;			
	}
	if (request_irq(INTSRC_EXTINT3,sep4020_key_irqhandler,SA_INTERRUPT,"4020KEY",NULL))	
	{
		printk("request button irq3 failed!\n");
		return -1;			
	}
	if (request_irq(INTSRC_EXTINT4,sep4020_key_irqhandler,SA_INTERRUPT,"4020KEY",NULL))	
	{
		printk("request button irq4 failed!\n");
		return -1;			
	}
	return 0;
}

static void sep4020_free_irqs(void)
{
	free_irq(INTSRC_EXTINT0,NULL);
	free_irq(INTSRC_EXTINT1,NULL);
	free_irq(INTSRC_EXTINT2,NULL);
	free_irq(INTSRC_EXTINT3,NULL);
	free_irq(INTSRC_EXTINT4,NULL);
}

static int __init sep4020_key_init(void)
{
	int err,result;
	dev_t devno = MKDEV(KEY_MAJOR, 0);
	if (KEY_MAJOR)
	{
		result = register_chrdev_region(devno, 1, "sep4020_key");
	}
	else
	{
		result = alloc_chrdev_region(&devno, 0, 1, "sep4020_key");		
	}
	if (result < 0)
	{
		return result;
	}
	
	key_dev = kmalloc(sizeof(struct KEY_DEV), GFP_KERNEL);
	if (!key_dev)
	{
		result = -ENOMEM;
		goto fail_malloc;
	}
	memset(key_dev,0,sizeof(struct KEY_DEV));
	
	sep4020_request_irqs();
	key_dev->head = key_dev->tail = 0; 
	
	//sep4020_key_setup();
	cdev_init(&(key_dev->cdev), &sep4020_key_fops);
	key_dev->cdev.owner = THIS_MODULE;
	key_dev->keystatus = KEY_UP;
   setup_timer(&key_timer,key_timer_handler,0);   

	err = cdev_add(&key_dev->cdev, devno, 1);
	if(err)
	{
		printk("adding err\r\n");
	}	
	return 0;
	
	fail_malloc: unregister_chrdev_region(devno, 1);
	return result;
}


static void __exit sep4020_key_exit(void)
{
	sep4020_free_irqs();
	cdev_del(&key_dev->cdev);
	kfree(key_dev);
	unregister_chrdev_region(MKDEV(KEY_MAJOR, 0), 1);
}

module_init(sep4020_key_init);
module_exit(sep4020_key_exit);

MODULE_AUTHOR("Leeming Zhang");
MODULE_LICENSE("GPL");

