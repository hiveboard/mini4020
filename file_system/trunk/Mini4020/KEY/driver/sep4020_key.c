#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/spinlock_types.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <asm/semaphore.h>
#include <asm/types.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/hardware.h>

#define KEY_MAJOR 254 /* 主设备号*/
#define MAX_KEY_BUF 16         //按键缓冲区的大小
#define KEY_XNUM 3   //行数
#define KEY_YNUM 3   //列数

#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_UNSURE 2

#define KEY_TIMER_DELAY_JUDGE 4     //判断是否有按键按下   jiffes
#define KEY_TIMER_DELAY_LONGTOUCH 20  //判断是否长时间按下  jiffes

char key_map[]= {'y','c','b','m','v','h','x','w','r'};

						

static void key_timer_handler(unsigned long arg);
static int sep4020_request_irqs(void);
static void sep4020_free_irqs(void);
static irqreturn_t sep4020_key_irqhandler(int irq, void *dev_id, struct pt_regs *reg);
								
struct keydev
{    
	unsigned int keystatus;          //按键状态    
	unsigned char buf[MAX_KEY_BUF];   //按键缓冲区  
	unsigned int write,read;         //按键缓冲区头和尾
	wait_queue_head_t wq;            //等待队列
	struct cdev cdev;
} ;

DECLARE_MUTEX_LOCKED(key_press);		

struct keydev *key_dev;     //键盘结构体

struct timer_list key_timer;  //定时器


static void unmaskkey(void)   //开启键盘中断
{
	enable_irq(INTSRC_EXTINT0);
}


static void maskkey(void)     //关闭键盘中断
{
	disable_irq(INTSRC_EXTINT0);
}

static void sep4020_key_setup(void)
{
    maskkey();
        
    *(volatile unsigned long*)GPIO_PORTE_SEL_V  |= 0x038 ;      //通用用途
    *(volatile unsigned long*)GPIO_PORTE_DIR_V  &= (~0x038);    //输出方式
    *(volatile unsigned long*)GPIO_PORTE_DATA_V |= 0x038;       //输出拉高
		 
    *(volatile unsigned long*)GPIO_PORTA_SEL_V |= 0x01 ;       //通用用途
    *(volatile unsigned long*)GPIO_PORTA_DIR_V |= 0x01 ;       //输入
    *(volatile unsigned long*)GPIO_PORTA_INTRCTL_V |= 0x03;    //中断类型为低电平触发
    *(volatile unsigned long*)GPIO_PORTA_INCTL_V |= 0x01;      //中断输入方式
    *(volatile unsigned long*)GPIO_PORTA_INTRCLR_V |= 0x01;    //清除中断
    *(volatile unsigned long*)GPIO_PORTA_INTRCLR_V = 0x00;     //清除中断
        
    unmaskkey();
}


static void write_row(int index,int HighLow)
{ 
    switch(index)
    {
        case 0:
            if(HighLow) *(volatile unsigned long*)GPIO_PORTE_DATA_V |= 0x8; 
                else *(volatile unsigned long*)GPIO_PORTE_DATA_V &= ~0x8;
            break;
        case 1: 
            if(HighLow) *(volatile unsigned long*)GPIO_PORTE_DATA_V |= 0x10;
                else *(volatile unsigned long*)GPIO_PORTE_DATA_V &= ~0x10;
            break;
        case 2:
            if(HighLow) *(volatile unsigned long*)GPIO_PORTE_DATA_V |= 0x20;
                else *(volatile unsigned long*)GPIO_PORTE_DATA_V &= ~0x20;
            break;
        
        default:
            break;
    }
        
    return ;
}

static int ReadCol(void)										
{
    return (( *(volatile unsigned long*)GPIO_PORTA_DATA_V ) & 0x1);
}

void row_input(int x)
{   
	 switch(x)
    {
        case 0:
            *(volatile unsigned long*)GPIO_PORTE_DIR_V  |=  0x030;
            break;
            
        case 1: 
            *(volatile unsigned long*)GPIO_PORTE_DIR_V  |=  0x028;
            break;
           
        case 2:
           	*(volatile unsigned long*)GPIO_PORTE_DIR_V  |=  0x018;
            break;
            
        case 10:
            *(volatile unsigned long*)GPIO_PORTE_DIR_V  &= ~0x038;
			
            break;
                        
        default:
            break;
    }
        
    
}

int read_col(int x)
{
	int i,j;
	int col = x;
	
	for(i=0;i<x;i++)
	{
		if(((*(volatile unsigned long*)GPIO_PORTE_DATA_V)>>(i+3)) & 0x1)  col=i;
	}
	for(j=x+1;j<KEY_YNUM;j++)
	{
		if(((*(volatile unsigned long*)GPIO_PORTE_DATA_V)>>(j+3)) & 0x1)  col=j;
	}
	return col;
}


static int keyevent(void)
{
	 int i,result,row,col;
    int keynum = -1;
       
    write_row(0,0);
	 write_row(1,0);
	 write_row(2,0);
	    
    for (i=0; i<KEY_XNUM; i++)         
	     {
		   write_row(i,1);
         udelay(500);
            
         if(!ReadCol())
                       {
             row = i ;
             row_input(i);
             col = read_col(i);
             keynum = col*3 + row;
             printk("row=%ld,col=%ld,keynum=%ld,i=%d \n",row,col,keynum,i );
             row_input(10);			        
                        }
            
        	write_row(i,0);
             }
     if (keynum<9 && keynum>=0)
            {
			return keynum;
           }
	 else
	   {
	      printk("key errors\n");
	 		return -1;
	   }        
	
   return -1;
	
}


static int sep4020_key_open(struct inode *inode, struct file *filp)
{
    sep4020_key_setup();  //设置键盘设备
    init_waitqueue_head(&(key_dev->wq));
    return 0;
}

static ssize_t sep4020_key_read(struct file *filp, char __user *buff, size_t size, loff_t *ppos)
{
	int total_num = 0;
	unsigned long err;
	int i;
	char buffer[16];
     
	retry:
			
   	if ((key_dev->write) != (key_dev->read))    //当前缓冲队列中有数据
		{
			
			if ((key_dev->write) > (key_dev->read)) 
			{	
				total_num = (key_dev->write) - (key_dev->read);
			}
			else 
			{
				total_num = (16 - key_dev->read) + key_dev->write;
			}
			
			if (size > total_num)
			{
				size = total_num;
				
			}

		//	buffer[0] = size;
			for (i=0; i< size; i++)
			{
				buffer[i] = key_dev->buf[key_dev->read];
				if (++(key_dev->read) == MAX_KEY_BUF)
				{
					key_dev->read = 0;
				}
			}
			err = copy_to_user(buff, buffer, size);
	      
			return err ? -EFAULT : 0;
		}
		else      //当前缓冲队列中没有数据
		{     
			if(filp->f_flags & O_NONBLOCK)    //假如用户采用的是非堵塞方式读取
		      {
				 
             return -EAGAIN;
			}
				

			//用户采用阻塞方式读取，调用该函数使进程睡眠
			//interruptible_sleep_on(&(key_dev->wq)); 
 		   down_interruptible(&key_press);

			goto retry;
		}
}

static ssize_t sep4020_key_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	return 0;
}


static int sep4020_key_release(struct inode *inode, struct file *filp)
{
	return 0;
}


static int sep4020_request_irqs(void)
{
	if(request_irq(INTSRC_EXTINT0,sep4020_key_irqhandler,SA_INTERRUPT,"4020KEY",NULL))	
	{
		printk("request button irq0 failed!\n");
      free_irq(INTSRC_EXTINT0,NULL);
		return -1;			
	}
	
	return 0;
}

static void sep4020_free_irqs(void)
{
	free_irq(INTSRC_EXTINT0,NULL);
}

static void key_timer_handler(unsigned long arg)
{
	int irq_value = 0;
	static int key_value = -1;

	irq_value = (*(volatile unsigned long*)GPIO_PORTA_DATA_V & 0x01);  //读取中断口数值
	
	if (irq_value != 0x01) //如果有低电平，表示键盘仍然有键被按着
	{
		if (key_dev->keystatus == KEY_UNSURE)
		{
			key_dev->keystatus = KEY_DOWN;
			
			key_value = keyevent();      //读取键盘的位置
			
			if (key_value >= 0)
			{		
				 key_dev->buf[key_dev->write] = key_map[key_value];  
					
			    if(++(key_dev->write) == MAX_KEY_BUF)               //按键缓冲区循环存取
			          {
				   
					key_dev->write = 0;
		               	  }
                        }
		    
			//wake_up_interruptible(&(key_dev->wq));
		   up(&key_press);

			//检测是否持续按键
			key_timer.expires = jiffies + KEY_TIMER_DELAY_LONGTOUCH;
			add_timer(&key_timer);	
        						
		}
		else    //一定是键按下 
		{
			key_timer.expires = jiffies + KEY_TIMER_DELAY_LONGTOUCH;    //延迟
			add_timer(&key_timer);
			 	
		}
	}
	else      //键已抬起
	{

		if (key_dev->keystatus == KEY_DOWN)
		{
			key_dev->keystatus = KEY_UP;
		
			if (key_value >= 0)
			{		
			 	key_dev->buf[key_dev->write] = key_map[key_value] | 0x80;  
			 	printk("face\n");		
			 	if(++(key_dev->write) == MAX_KEY_BUF)               //按键缓冲区循环存取
			    	{
				  	key_dev->write = 0;
		               }	
				}
				up(&key_press);
                  }
		//wake_up_interruptible(&(key_dev->wq));
		//打开键盘中断
		 printk("tree\n");
		*(volatile unsigned long*)GPIO_PORTE_DATA_V |= 0x038;       //输出拉高
		unmaskkey();
	}
	
}

static irqreturn_t sep4020_key_irqhandler(int irq, void *dev_id, struct pt_regs *reg)
{
	 //关闭键盘中断
	maskkey();  

	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V |= 0x0001;
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V = 0x0000;    //清除中断  

	key_dev->keystatus = KEY_UNSURE;
	key_timer.expires = jiffies + KEY_TIMER_DELAY_JUDGE;
	add_timer(&key_timer);     //启动定时器
	
	//we will turn on the irq in the timer_handler
	return IRQ_HANDLED;
}

static const struct file_operations sep4020_key_fops = 
{
	.owner = THIS_MODULE,
	.read  = sep4020_key_read,
	.write = sep4020_key_write,
	.open  = sep4020_key_open,
	.release = sep4020_key_release,
};

static int __init sep4020_key_init(void)
{
	int err,result;
	dev_t devno;


	devno = MKDEV(KEY_MAJOR, 0);
	result = register_chrdev_region(devno, 1, "sep4020_key");   //向系统静态申请设备号

	if (result < 0)
		return result;	
	
	key_dev = kmalloc(sizeof(struct keydev), GFP_KERNEL);
	if (key_dev == NULL)
	{
		result = -ENOMEM;
		unregister_chrdev_region(devno, 1);
		return result;
	}
	memset(key_dev,0,sizeof(struct keydev));  //初始化
	
	
   if(sep4020_request_irqs())  //注册中断函数
	{ 
		unregister_chrdev_region(devno,1);
		kfree(key_dev); 
		printk("request key irq failed!\n");
		return -1;
	}

   key_dev->write = key_dev->read = 0;
   key_dev->keystatus = KEY_UP;
  
   setup_timer(&key_timer,key_timer_handler,0); //初始化定时器
	
	cdev_init(&key_dev->cdev, &sep4020_key_fops);
	key_dev->cdev.owner = THIS_MODULE;
	
	  

	//向系统注册该字符设备
	err = cdev_add(&key_dev->cdev, devno, 1);
	if (err)
	{
		printk("key adding err\r\n");
		unregister_chrdev_region(devno,1);
		kfree(key_dev);
		sep4020_free_irqs();
		return err;
	}
   return 0;
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

MODULE_AUTHOR("Lee xiang");
MODULE_LICENSE("GPL");





