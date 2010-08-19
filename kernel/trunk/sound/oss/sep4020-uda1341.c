/* linux/linux/sound/oss/sep4020-uda1341.c
*
* Copyright (c) 2009 prochip company
*	http://www.prochip.com.cn
*	aokikyon@gmail.com.cn
*
* sep4020 I2S-uda1341 driver.
*
* Changelog:
*
*	2008-11-07 	begin to write sep4020-uda1341.c according to s3c2410-uda1341.c
*	2008-11-08 	sep4020 使用PWM提供UDA1341的SYSCLK信号 不需要获取芯片主频，删除掉和系统时钟相关的代码/
*			但是PWM依然和CPU时钟相关，目前sep4020主频固定在88MHz PWM采8分频 PWM输出为11MHz 在256fs下 采样率11MHz／256＝42.96k 暂时这么设定
*	2008-11-09 	修改初始化iis函数 修改初始化dma函数 设置iis的采样率固定为44.1kHz
*	2009-04-23 	加入采样率调整功能，可选4中采样率 8000\11025\22050\44100 ，默认44100
*	2009-04-28 	加入录音功能，录音有杂音, 8000 sounds good
*	2009-05-09	修订格式，没有使用paltform device注册
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/


//#define DEBUG		//两个调试开关
//#define DEBUG_READ

#include <linux/init.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/soundcard.h>	
#include <linux/pm.h>
#include <linux/sound.h>
#include <linux/poll.h>	
#include <linux/dma-mapping.h>
#include <asm/dma-mapping.h>
#include <asm/arch/hardware.h>	
#include <asm/arch/irqs.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <asm/dma.h>
#include <asm/arch/dma.h>		

#ifdef DEBUG
#define DPRINTK printk
#else
#define DPRINTK( x... )
#endif

/************************************************* 
     define I2S_T registers
*************************************************/
#define   HALF_FIFO 4
#define   MUTE(X)       (X << 25)
#define   RESET_1(X)    (X << 24)
#define   ALIGN(X)      (X << 23)
#define   WS(X)         (X << 22)
#define   WID(X)        (X << 20)
#define   DIR(X)        (X << 19)
#define   MODE(X)       (X << 18)
#define   MONO(X)       (X << 17)
#define   STOP(X)       (X << 16)
#define   DIV(X)        X

#define DMA_BUF_WR 1
#define DMA_BUF_RD 0

#define DEF_VOLUME 65	

/* UDA1341 Register bits */
#define UDA1341_ADDR 0x14
#define UDA1341_REG_DATA0 (UDA1341_ADDR + 0)
#define UDA1341_REG_STATUS (UDA1341_ADDR + 2)

/* status control */
#define STAT0 (0x00)
#define STAT0_RST (1 << 6)
#define STAT0_SC_MASK (3 << 4)
#define STAT0_SC_512FS (0 << 4)
#define STAT0_SC_384FS (1 << 4)
#define STAT0_SC_256FS (2 << 4)
#define STAT0_IF_MASK (7 << 1)
#define STAT0_IF_I2S (0 << 1)
#define STAT0_IF_LSB16 (1 << 1)
#define STAT0_IF_LSB18 (2 << 1)
#define STAT0_IF_LSB20 (3 << 1)
#define STAT0_IF_MSB (4 << 1)
#define STAT0_IF_LSB16MSB (5 << 1)
#define STAT0_IF_LSB18MSB (6 << 1)
#define STAT0_IF_LSB20MSB (7 << 1)
#define STAT0_DC_FILTER (1 << 0)
#define STAT0_DC_NO_FILTER (0 << 0)

#define STAT1 (0x80)
#define STAT1_DAC_GAIN (1 << 6) /* gain of DAC */
#define STAT1_ADC_GAIN (1 << 5) /* gain of ADC */
#define STAT1_ADC_POL (1 << 4) /* polarity of ADC */
#define STAT1_DAC_POL (1 << 3) /* polarity of DAC */
#define STAT1_DBL_SPD (1 << 2) /* double speed playback */
#define STAT1_ADC_ON (1 << 1) /* ADC powered */
#define STAT1_DAC_ON (1 << 0) /* DAC powered */

/* data0 direct control */
#define DATA0 (0x00)
#define DATA0_VOLUME_MASK (0x3f)
#define DATA0_VOLUME(x) (x)

#define DATA1 (0x40)
#define DATA1_BASS(x) ((x) << 2)
#define DATA1_BASS_MASK (15 << 2)
#define DATA1_TREBLE(x) ((x))
#define DATA1_TREBLE_MASK (3)

#define DATA2 (0x80)
#define DATA2_PEAKAFTER (0x1 << 5)
#define DATA2_DEEMP_NONE (0x0 << 3)
#define DATA2_DEEMP_32KHz (0x1 << 3)
#define DATA2_DEEMP_44KHz (0x2 << 3)
#define DATA2_DEEMP_48KHz (0x3 << 3)
#define DATA2_MUTE (0x1 << 2)
#define DATA2_FILTER_FLAT (0x0 << 0)
#define DATA2_FILTER_MIN (0x1 << 0)
#define DATA2_FILTER_MAX (0x3 << 0)

/* data0 extend control */
#define EXTADDR(n) (0xc0 | (n))
#define EXTDATA(d) (0xe0 | (d))

#define EXT0 0
#define EXT0_CH1_GAIN(x) (x)
#define EXT1 1
#define EXT1_CH2_GAIN(x) (x)
#define EXT2 2
#define EXT2_MIC_GAIN_MASK (7 << 2)
#define EXT2_MIC_GAIN(x) ((x) << 2)
#define EXT2_MIXMODE_DOUBLEDIFF (0)
#define EXT2_MIXMODE_CH1 (1)
#define EXT2_MIXMODE_CH2 (2)
#define EXT2_MIXMODE_MIX (3)
#define EXT4 4
#define EXT4_AGC_ENABLE (1 << 4)
#define EXT4_INPUT_GAIN_MASK (3)
#define EXT4_INPUT_GAIN(x) ((x) & 3)
#define EXT5 5
#define EXT5_INPUT_GAIN(x) ((x) >> 2)
#define EXT6 6
#define EXT6_AGC_CONSTANT_MASK (7 << 2)
#define EXT6_AGC_CONSTANT(x) ((x) << 2)
#define EXT6_AGC_LEVEL_MASK (3)
#define EXT6_AGC_LEVEL(x) (x)

//default setting
#define AUDIO_NAME "UDA1341"
#define AUDIO_NAME_VERBOSE "UDA1341 audio driver"

#define AUDIO_FMT_MASK (AFMT_S16_LE)
#define AUDIO_FMT_DEFAULT (AFMT_S16_LE)

#define AUDIO_CHANNELS_DEFAULT 2
#define AUDIO_RATE_DEFAULT 44100

#define AUDIO_NBFRAGS_DEFAULT 8
#define AUDIO_FRAGSIZE_DEFAULT 8192 	//this for write
#define AUDIO_FRAGSIZE_DEFAULT_READ 16	//this for read

#define S_CLOCK_FREQ 256
#define PCM_ABS(a) (a < 0 ? -a : a)

#define L3C (1<<4)              //GPD4 = L3CLOCK	//add by kyon
#define L3M (1<<3)              //GPD3 = L3MODE
#define L3D (1<<1)              //GPD1 = L3DATA

#define NEXT_BUF(_s_,_b_) { \
(_s_)->_b_##_idx++; \
(_s_)->_b_##_idx %= (_s_)->nbfrags; \
(_s_)->_b_ = (_s_)->buffers + (_s_)->_b_##_idx; }

typedef struct 
{
	int size; 			/* buffer size */
	char *start; 		/* point to actual buffer */
	dma_addr_t dma_addr; 	/* physical buffer address */
	struct semaphore sem; 	/* down before touching the buffer */
	int master; 		/* owner for buffer allocation, contain size when true */
} audio_buf_t;

typedef struct 
{
	audio_buf_t *buffers; 	/* pointer to audio buffer structures */
	audio_buf_t *buf; 	/* current buffer used by read/write */
	u_int buf_idx; 		/* index for the pointer above */
	u_int fragsize; 		/* fragment i.e. buffer size */
	u_int nbfrags; 		/* nbr of fragments */
	dmach_t dma_ch; 		/* DMA channel (channel2 for audio) */
	u_int dma_ok;
} audio_stream_t;

static audio_stream_t output_stream;
static audio_stream_t input_stream; /* input */

static int write_mark = 0; //prevent the first DMA to be unsuccessful
static int read_mark = 0;

static u_int audio_rate;
static int audio_channels;
static int audio_fmt;
static u_int audio_fragsize;
static u_int audio_nbfrags;

static int audio_rd_refcount;
static int audio_wr_refcount;
#define audio_active (audio_rd_refcount | audio_wr_refcount)

static int audio_dev_dsp;
static int audio_dev_mixer;
static int audio_mix_modcnt;

static int uda1341_volume;
static int uda1341_boost;
static int mixer_igain=0x4; /* -6db*/

static void init_sep4020_iis_bus_rx(void);
static void init_sep4020_iis_bus_tx(void);

/**
*uda1341_l3_address-L3 Address
*@data:	L3 address
*
*use gpio to simulate the L3 bus
*/
static void uda1341_l3_address(u8 data)
{
	int i;
	unsigned long flags;
	local_irq_save(flags);

 	*(volatile unsigned long*)(GPIO_PORTD_DATA_V) &= (~(L3D | L3M ) | L3C); //L3D=L, L3M=L(in address mode), L3C=H
	udelay(1);
	for (i = 0; i < 8; i++) 
	{
		if (data & 0x1) 
		{
 			*(volatile unsigned long*)(GPIO_PORTD_DATA_V) &= ~L3C;            //L3C=L
      		*(volatile unsigned long*)(GPIO_PORTD_DATA_V) |= L3D;             //L3D=H             
      		udelay(1);    
      		*(volatile unsigned long*)(GPIO_PORTD_DATA_V) |= L3C;             //L3C=H，L3D=H
      		*(volatile unsigned long*)(GPIO_PORTD_DATA_V) |= L3D;            
		} 
		else 
		{
			*(volatile unsigned long*)(GPIO_PORTD_DATA_V) &= ~L3C;            //L3C=L,L3D=L
      		*(volatile unsigned long*)(GPIO_PORTD_DATA_V) &= ~L3D;
			udelay(1);
			*(volatile unsigned long*)(GPIO_PORTD_DATA_V) |= L3C;             //L3C=H
      		*(volatile unsigned long*)(GPIO_PORTD_DATA_V) &= ~L3D;            //L3D=L
		}
		data >>= 1;
	}
 	*(volatile unsigned long*)(GPIO_PORTD_DATA_V) |= (L3C | L3M);       //L3M=H,L3C=H  

	local_irq_restore(flags);
}

/**
*uda1341_l3_data-L3 Data
*@data:	L3 data
*
*use gpio to simulate the L3 bus
*/
static void uda1341_l3_data(u8 data)
{
	int i;
	unsigned long flags;
	local_irq_save(flags);

	udelay(1);	
	*(volatile unsigned long*)(GPIO_PORTD_DATA_V) |= (L3C | L3M);   //L3M=H(in data transfer mode)
	udelay(1);
	for (i = 0; i < 8; i++) 
	{
		
		if (data & 0x1) 
		{
			*(volatile unsigned long*)(GPIO_PORTD_DATA_V) &= ~L3C;              //L3C=L
           		*(volatile unsigned long*)(GPIO_PORTD_DATA_V) |= L3D;               //L3D=H
			udelay(1);
			*(volatile unsigned long*)(GPIO_PORTD_DATA_V) |= (L3C | L3D);       //L3C=H,L3D=H
		} 
		else 
		{
			*(volatile unsigned long*)(GPIO_PORTD_DATA_V) &= ~L3C;              //L3C=L
           		*(volatile unsigned long*)(GPIO_PORTD_DATA_V) &= ~L3D;              //L3D=L
			udelay(1);
			*(volatile unsigned long*)(GPIO_PORTD_DATA_V) |= L3C;               //L3C=H
           		*(volatile unsigned long*)(GPIO_PORTD_DATA_V) &= ~L3D;              //L3D=L
		}
		data >>= 1;
	}
	*(volatile unsigned long*)(GPIO_PORTD_DATA_V) |= (L3C | L3M);    //L3M=H,L3C=H

	local_irq_restore(flags);
}

/**
*audio_clear_buf-Clear audio buffer
*@s:	audio_stream_t struct
*
*/
static void audio_clear_buf(audio_stream_t * s)
{
	DPRINTK("audio_clear_buf\n");

	if (s->buffers) 
	{
		int frag;
		for (frag = 0; frag < s->nbfrags; frag++) 
		{
			if (!s->buffers[frag].master)
				continue;
			dma_free_coherent(NULL, s->buffers[frag].master, s->buffers[frag].start, s->buffers[frag].dma_addr);
		}
		kfree(s->buffers);
		s->buffers = NULL;
	}
	s->buf_idx = 0;
	s->buf = NULL;
}

/**
*audio_setup_buf-Setup audio buffer
*@s:	audio_stream_t struct
*
*/
static int audio_setup_buf(audio_stream_t * s)
{
	int frag;
	int dmasize = 0;
	char *dmabuf = 0;
	dma_addr_t dmaphys = 0;		//dma physics address

	if (s->buffers)
	return -EBUSY;

	s->nbfrags = audio_nbfrags;
	s->fragsize = audio_fragsize;

	s->buffers = (audio_buf_t *)
	kmalloc(sizeof(audio_buf_t) * s->nbfrags, GFP_KERNEL);
	if (!s->buffers)
		goto err;
	
	memset(s->buffers, 0, sizeof(audio_buf_t) * s->nbfrags);

	for (frag = 0; frag < s->nbfrags; frag++) 
	{
		audio_buf_t *b = &s->buffers[frag];

		if (!dmasize) 
		{
			dmasize = (s->nbfrags - frag) * s->fragsize;
			do {
				dmabuf = dma_alloc_coherent(NULL, dmasize, &dmaphys, GFP_KERNEL|GFP_DMA);
				if (!dmabuf)
				dmasize -= s->fragsize;
			   } 
			while (!dmabuf && dmasize);

			if (!dmabuf)
				goto err;
		
			b->master = dmasize;
		}

		b->start = dmabuf;
		b->dma_addr = dmaphys;
		sema_init(&b->sem, 1);
		DPRINTK("buf %d: start %p dma %d\n", frag, b->start, b->dma_addr);

		dmabuf += s->fragsize;
		dmaphys += s->fragsize;
		dmasize -= s->fragsize;
	}

	s->buf_idx = 0;
	s->buf = &s->buffers[0];
	return 0;

	err:
	printk(AUDIO_NAME ": unable to allocate audio memory\n ");
	audio_clear_buf(s);
	return -ENOMEM;
}

//这两个函数是DMA中断返回函数，以后可能加入DMA中断功能，所以先留着
/* 
static void audio_dmaout_done_callback(s3c2410_dma_chan_t *ch, void *buf, int size,s3c2410_dma_buffresult_t result)
{
	audio_buf_t *b = (audio_buf_t *) buf;
	up(&b->sem);
	wake_up(&b->sem.wait);
}

static void audio_dmain_done_callback(s3c2410_dma_chan_t *ch, void *buf, int size,s3c2410_dma_buffresult_t result)
{
	audio_buf_t *b = (audio_buf_t *) buf;
	b->size = size;
	up(&b->sem);
	wake_up(&b->sem.wait);
}
*/

/**
*audio_sync
*@file:	file struct
*
* using when write
* 这个函数的功能没有实现
*/
static int audio_sync(struct file *file)
{
	audio_stream_t *s = &output_stream;
	audio_buf_t *b = s->buf;

	DPRINTK("audio_sync\n");

	if (!s->buffers)
		return 0;

	if (b->size != 0) 
	{
		down(&b->sem);
		//s3c2410_dma_enqueue(s->dma_ch, (void *) b, b->dma_addr, b->size); //need modify by kyon
		b->size = 0;
		NEXT_BUF(s, buf);
	}

	b = s->buffers + ((s->nbfrags + s->buf_idx - 1) % s->nbfrags);
	if (down_interruptible(&b->sem))
		return -EINTR;
	up(&b->sem);

	return 0;
}

/**copy_from_user_mono_stereo
*@to:		kernel handle
*@from:	user	 handle
*@count:	
*
* 内联函数，从用户态拷贝数据,仅仅用于单声道，sep4020的I2S配置为双声道，这个函数用不到
*/
static inline int copy_from_user_mono_stereo(char *to, const char *from, int count)
{
	u_int *dst = (u_int *)to;
	const char *end = from + count;

	if (access_ok(VERIFY_READ, from, count))		// check user space
		return -EFAULT;

	if ((int)from & 0x2) 
	{
		u_int v;
		__get_user(v, (const u_short *)from); from += 2;
		*dst++ = v | (v << 16);
	}

	while (from < end-2) 
	{
		u_int v, x, y;
		__get_user(v, (const u_int *)from); from += 4;
		x = v << 16;
		x |= x >> 16;
		y = v >> 16;
		y |= y << 16;
		*dst++ = x;
		*dst++ = y;
	}
	
	if (from < end) 
	{
		u_int v;
		__get_user(v, (const u_short *)from);
		*dst = v | (v << 16);
	}

	return 0;
}

/**sep4020_audio_write
*@file:	file struct	
*@buffer:	
*@count:
*@ppos：	ppos是这次对文件进行操作的起始位置
*
* 
*/
static ssize_t sep4020_audio_write(struct file *file, const char *buffer,size_t count, loff_t * ppos)
{
	#ifdef DEBUG
 	printk("sep4020_audio_write! count=%d\n", count);
	#endif		
	const char *buffer0 = buffer;
	audio_stream_t *s = &output_stream;
	int chunksize, ret = 0;

	switch (file->f_flags & O_ACCMODE) 
	{
		case O_WRONLY:
			break;	
		case O_RDWR:			//sep4020 does not support O_RDWR
		default:
		return -EPERM;
	}

	if (!s->buffers && audio_setup_buf(s))
		return -ENOMEM;
	
	count &= ~0x03;				//DMA data must be 4 byte allied

	while (count > 0) 
	{
		audio_buf_t *b = s->buf;	//定义结构的指针变量b，指向当前缓冲区指针

		if (file->f_flags & O_NONBLOCK) 
		{
			ret = -EAGAIN;
			if (down_trylock(&b->sem))
				break;
		} 
		else 
		{
			ret = -ERESTARTSYS;
			if (down_interruptible(&b->sem))
				break;
		}

		if (audio_channels == 2) 				//双声道
		{		
			chunksize = s->fragsize - b->size;		//b->size缓冲区地址的偏移量 一开始这个值为0
			if (chunksize > count)
				chunksize = count;
			DPRINTK("write %d to %d\n", chunksize, s->buf_idx);
			if (copy_from_user(b->start + b->size, buffer, chunksize)) 
			{
				up(&b->sem);
				return -EFAULT;
			}
			b->size += chunksize;
		} 
		else 								//单声道
		{
			chunksize = (s->fragsize - b->size) >> 1;

			if (chunksize > count)
				chunksize = count;
			DPRINTK("write %d to %d\n", chunksize*2, s->buf_idx);
			if (copy_from_user_mono_stereo(b->start + b->size,buffer, chunksize)) 
			{
				up(&b->sem);
				return -EFAULT;
			}
			b->size += chunksize*2;
		}

		buffer += chunksize;
		count -= chunksize;
		if (b->size < s->fragsize) 
		{
			up(&b->sem);
			break;
		}

		while(write_mark)						//第一次不用判断DMA是否就绪
		{
			if(((*(volatile unsigned long*)DMAC_INTTCSTATUS_V) & 0x2 ) == 0x2) break;
		}
		/*清除要用的dma通道（dma1）上的传输完成中断寄存器*/
		*(volatile unsigned long*)(DMAC_INTTCCLEAR_V) = 0x2; 
		*(volatile unsigned long*)(DMAC_INTTCCLEAR_V) = 0x0;	
		write_mark = 1;

		*(volatile unsigned long*)DMAC_C1CONTROL_V  = ((AUDIO_FRAGSIZE_DEFAULT>>2)<<14) + (1<<12) + (2<<9) + (2<<6) + (3<<3) + 3;
		*(volatile unsigned long*)DMAC_C1SRCADDR_V  = b->dma_addr;
 		*(volatile unsigned long*)DMAC_C1DESTADDR_V  = I2S_DATA ;
 		*(volatile unsigned long*)DMAC_C1CONFIGURATION_V = 0x200b ; //iis
		
		up(&b->sem);	
		wake_up(&b->sem.wait);
		#ifdef DEBUG
 		printk("dma1 done! b->size=%d\n",b->size);
		#endif
	
		b->size = 0;
		
		NEXT_BUF(s, buf);
		#ifdef DEBUG
 		printk("next buf!\n");
		#endif
		
	}
	if ((buffer - buffer0))
	ret = buffer - buffer0;
	DPRINTK("audio_write : end count=%d\n\n", ret);
	return ret;
}

/**sep4020_audio_read
*@file:	file struct	
*@buffer:	
*@count:
*@ppos：	ppos是这次对文件进行操作的起始位置
*
* 
*/
static ssize_t sep4020_audio_read(struct file *file, char *buffer,size_t count, loff_t * ppos)
{
	#ifdef DEBUG
	printk("audio_read: count=%d\n", count);
	#endif	

	const char *buffer0 = buffer;
	audio_stream_t *s = &input_stream;
	int chunksize, ret = 0;

	if (!s->buffers) 				//just build the buffer
	{
		int i;
		if (audio_setup_buf(s))
			return -ENOMEM;
		
		for (i = 0; i < s->nbfrags; i++) 
		{
			audio_buf_t *b = s->buf;
			down(&b->sem);

			while(read_mark)		//first we should not check the read_mark
			{
				if(((*(volatile unsigned long*)DMAC_INTTCSTATUS_V) & 0x2 ) == 0x2) break;
			}
			/*清除要用的dma通道（dma1）上的传输完成中断寄存器*/
			*(volatile unsigned long*)(DMAC_INTTCCLEAR_V) = 0x2; 
			*(volatile unsigned long*)(DMAC_INTTCCLEAR_V) = 0x0;	
			read_mark = 1;
		
			*(volatile unsigned long*)DMAC_C1CONTROL_V  = ((AUDIO_FRAGSIZE_DEFAULT_READ>>2)<<14) + (1<<12) + (2<<9) + (2<<6) + (3<<3) + 3;
			*(volatile unsigned long*)DMAC_C1SRCADDR_V  = I2S_DATA;
 			*(volatile unsigned long*)DMAC_C1DESTADDR_V  = b->dma_addr;
			*(volatile unsigned long*)DMAC_C1CONFIGURATION_V = 0x20d ; 
		
			up(&b->sem);			//this should be placed in the interrupt recall function
			wake_up(&b->sem.wait);
			b->size = s->fragsize;
			NEXT_BUF(s, buf);
		}
	}
		
	while (count > 0) 
	{
		audio_buf_t *b = s->buf;
		/* Wait for a buffer to become full */
		if (file->f_flags & O_NONBLOCK) 
		{
			ret = -EAGAIN;
			if (down_trylock(&b->sem))
				break;
		} 
		else 
		{
			ret = -ERESTARTSYS;
			if (down_interruptible(&b->sem))
			break;
		}

		chunksize = b->size;
		if (chunksize > count)	chunksize = count;

		if (copy_to_user(buffer, b->start + s->fragsize - b->size,chunksize)) 
		{
			up(&b->sem);
			return -EFAULT;
		}

		b->size -= chunksize;

		buffer += chunksize;
		count -= chunksize;
		if (b->size > 0) 
		{
			up(&b->sem);
			break;
		}

		/* Make current buffer available for DMA again */
		while(read_mark)
		{
			if(((*(volatile unsigned long*)DMAC_INTTCSTATUS_V) & 0x2 ) == 0x2) break;
		}
		/*清除要用的dma通道（dma1）上的传输完成中断寄存器*/
		*(volatile unsigned long*)(DMAC_INTTCCLEAR_V) = 0x2; 
		*(volatile unsigned long*)(DMAC_INTTCCLEAR_V) = 0x0;	
		
		*(volatile unsigned long*)DMAC_C1CONTROL_V  = ((AUDIO_FRAGSIZE_DEFAULT_READ>>2)<<14) + (1<<12) + (2<<9) +(2<<6) + (3<<3) + 3;
		*(volatile unsigned long*)DMAC_C1SRCADDR_V  = I2S_DATA;
 		*(volatile unsigned long*)DMAC_C1DESTADDR_V  = b->dma_addr;
 		*(volatile unsigned long*)DMAC_C1CONFIGURATION_V = 0x20d ; //iis
		
		b->size = s->fragsize;
		up(&b->sem);
		wake_up(&b->sem.wait);

		NEXT_BUF(s, buf);
	}

	if ((buffer - buffer0))
	ret = buffer - buffer0;

	return ret;
}

/**sep4020_audio_poll
*@file:	file struct	
*@buffer:	poll_table_struct struct
*
* 		这个函数未作修改
*/
static unsigned int sep4020_audio_poll(struct file *file, struct poll_table_struct *wait)
{
	unsigned int mask = 0;
	int i;

	DPRINTK("audio_poll(): mode=%s\n", (file->f_mode & FMODE_WRITE) ? "w" : "");

	if (file->f_mode & FMODE_READ) 
	{
		if (!input_stream.buffers && audio_setup_buf(&input_stream))
		return -ENOMEM;
		poll_wait(file, &input_stream.buf->sem.wait, wait);

		for (i = 0; i < input_stream.nbfrags; i++) 
		{
			if (atomic_read(&input_stream.buffers[i].sem.count) > 0)
			mask |= POLLIN | POLLWRNORM;
			break;
		}
	}

	if (file->f_mode & FMODE_WRITE) 
	{
		if (!output_stream.buffers && audio_setup_buf(&output_stream))
		return -ENOMEM;
		poll_wait(file, &output_stream.buf->sem.wait, wait);

		for (i = 0; i < output_stream.nbfrags; i++) 
		{
			if (atomic_read(&output_stream.buffers[i].sem.count) > 0)
			mask |= POLLOUT | POLLWRNORM;
			break;
		}
	}

	DPRINTK("audio_poll() returned mask of %s\n", (mask & POLLOUT) ? "w" : "");
	return mask;
}

/**sep4020_audio_poll
*@file:	file struct	
*@offset:	
*@origin:
* 		函数功能未实现
*/
static loff_t sep4020_audio_llseek(struct file *file, loff_t offset, int origin)
{
	return -ESPIPE;
}

/**sep4020_mixer_ioctl
*@inode:	inode struct
*@file:	file struct	
*@cmd:	command
*@arg:	argument
*
*		/dev/mixer 设备接口函数,这个函数我没有作修改 		
*/
static int sep4020_mixer_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret;
	long val = 0;

	switch (cmd) 
	{
	case SOUND_MIXER_INFO:
		{
			mixer_info info;
			strncpy(info.id, "UDA1341", sizeof(info.id));
			strncpy(info.name,"Philips UDA1341", sizeof(info.name));
			info.modify_counter = audio_mix_modcnt;
			return copy_to_user((void *)arg, &info, sizeof(info));
		}

	case SOUND_OLD_MIXER_INFO:
		{
		_old_mixer_info info;
		strncpy(info.id, "UDA1341", sizeof(info.id));
		strncpy(info.name,"Philips UDA1341", sizeof(info.name));
		return copy_to_user((void *)arg, &info, sizeof(info));
		}

	case SOUND_MIXER_READ_STEREODEVS:
		return put_user(0, (long *) arg);

	case SOUND_MIXER_READ_CAPS:
		val = SOUND_CAP_EXCL_INPUT;
		return put_user(val, (long *) arg);

	case SOUND_MIXER_WRITE_VOLUME:
		ret = get_user(val, (long *) arg);
		if (ret)
		return ret;
		uda1341_volume = 63 - (((val & 0xff) + 1) * 63) / 100;
		uda1341_l3_address(UDA1341_REG_DATA0);
		uda1341_l3_data(uda1341_volume);
		break;

	case SOUND_MIXER_READ_VOLUME:
		val = ((63 - uda1341_volume) * 100) / 63;
		val |= val << 8;
		return put_user(val, (long *) arg);

	case SOUND_MIXER_READ_IGAIN:
		val = ((31- mixer_igain) * 100) / 31;
		return put_user(val, (int *) arg);

	case SOUND_MIXER_WRITE_IGAIN:
		ret = get_user(val, (int *) arg);
		if (ret)
		return ret;
		mixer_igain = 31 - (val * 31 / 100);
		/* use mixer gain channel 1*/
		uda1341_l3_address(UDA1341_REG_DATA0);
		uda1341_l3_data(EXTADDR(EXT0));
		uda1341_l3_data(EXTDATA(EXT0_CH1_GAIN(mixer_igain)));
		break;

	default:
		DPRINTK("mixer ioctl %u unknown\n", cmd);
		return -ENOSYS;
	}

	audio_mix_modcnt++;
	return 0;
}

/**audio_set_dsp_speed
*@val
*	放音时使用这个函数来调整采样率
*	added 090422		
*/
static long audio_set_dsp_speed(long val)
{
#ifdef DEBUG
printk("audio_set_dsp_speed\n");
#endif	

switch (val)
{
case 8000:
	*(volatile unsigned long*)(I2S_CTRL_V) = MUTE(0) + RESET_1(0) + ALIGN(0) + WS(0) + WID(1) + DIR(0)+ MODE(0) + MONO(0) + STOP(1) + DIV(0xaf);	
	*(volatile unsigned long*)(I2S_INT_V) = 0x04;	//发送中断始能	
	udelay(10);
 	*(volatile unsigned long*)(I2S_CTRL_V) &= 0Xfffeffff; //发送
	*(volatile unsigned long*)PWM4_DIV_V =0x17; 	
	break;	

case 11025:
	*(volatile unsigned long*)(I2S_CTRL_V) = MUTE(0) + RESET_1(0) + ALIGN(0) + WS(0) + WID(1) + DIR(0)+ MODE(0) + MONO(0) + STOP(1) + DIV(0x7f);	
	*(volatile unsigned long*)(I2S_INT_V) = 0x04;	//发送中断始能	
	udelay(10);
 	*(volatile unsigned long*)(I2S_CTRL_V) &= 0Xfffeffff; //发送
	*(volatile unsigned long*)PWM4_DIV_V =0x11; 	
	break;

case 22050:
	*(volatile unsigned long*)(I2S_CTRL_V) = MUTE(0) + RESET_1(0) + ALIGN(0) + WS(0) + WID(1) + DIR(0)+ MODE(0) + MONO(0) + STOP(1) + DIV(0x3f);	
	*(volatile unsigned long*)(I2S_INT_V) = 0x04;	//发送中断始能	
	udelay(10);
 	*(volatile unsigned long*)(I2S_CTRL_V) &= 0Xfffeffff; //发送
	*(volatile unsigned long*)PWM4_DIV_V =0x8; 			
	break;
	//defualt is 44100	
	
default:
	val = 44100;
	*(volatile unsigned long*)(I2S_CTRL_V) = MUTE(0) + RESET_1(0) + ALIGN(0) + WS(0) + WID(1) + DIR(0)+ MODE(0) + MONO(0) + STOP(1) + DIV(0x1f);	
	*(volatile unsigned long*)(I2S_INT_V) = 0x04;	//发送中断始能	
	udelay(10);
 	*(volatile unsigned long*)(I2S_CTRL_V) &= 0Xfffeffff; //发送
	*(volatile unsigned long*)PWM4_DIV_V =0x4; 	//88MHz/(4*2)=11Mhz 11M/256fs=42.96k
}
	
return (audio_rate = val);
}

/**audio_set_dsp_speed_read
*@val
*	录音时使用这个函数来调整采样率
*	added 090422		
*/
static long audio_set_dsp_speed_read(long val)
{
#ifdef DEBUG
printk("audio_set_dsp_speed_read\n");
#endif	

switch (val)
{
case 8000:
	*(volatile unsigned long*)(I2S_CTRL_V) = MUTE(0) + RESET_1(0) + ALIGN(0) + WS(0) + WID(1) + DIR(1)+ MODE(0) + MONO(0) + STOP(1) + DIV(0xaf);	
	*(volatile unsigned long*)(I2S_INT_V) = 0x02;	//发送中断始能	
	udelay(10);
 	*(volatile unsigned long*)(I2S_CTRL_V) &= 0Xfffeffff; //发送
	*(volatile unsigned long*)PWM4_DIV_V =0x17; 	
	break;	

case 11025:
	*(volatile unsigned long*)(I2S_CTRL_V) = MUTE(0) + RESET_1(0) + ALIGN(0) + WS(0) + WID(1) + DIR(1)+ MODE(0) + MONO(0) + STOP(1) + DIV(0x7f);	
	*(volatile unsigned long*)(I2S_INT_V) = 0x02;	//发送中断始能	
	udelay(10);
 	*(volatile unsigned long*)(I2S_CTRL_V) &= 0Xfffeffff; //发送
	*(volatile unsigned long*)PWM4_DIV_V =0x11; 	
	break;

case 22050:
	*(volatile unsigned long*)(I2S_CTRL_V) = MUTE(0) + RESET_1(0) + ALIGN(0) + WS(0) + WID(1) + DIR(1)+ MODE(0) + MONO(0) + STOP(1) + DIV(0x3f);	
	*(volatile unsigned long*)(I2S_INT_V) = 0x02;	//发送中断始能	
	udelay(10);
 	*(volatile unsigned long*)(I2S_CTRL_V) &= 0Xfffeffff; //发送
	*(volatile unsigned long*)PWM4_DIV_V =0x8; 			
	break;
	//defualt is 44100	
	
default:
	val = 44100;
	*(volatile unsigned long*)(I2S_CTRL_V) = MUTE(0) + RESET_1(0) + ALIGN(0) + WS(0) + WID(1) + DIR(1)+ MODE(0) + MONO(0) + STOP(1) + DIV(0x1f);	
	*(volatile unsigned long*)(I2S_INT_V) = 0x02;	//发送中断始能	
	udelay(10);
 	*(volatile unsigned long*)(I2S_CTRL_V) &= 0Xfffeffff; //发送
	*(volatile unsigned long*)PWM4_DIV_V =0x4; 	//88MHz/(4*2)=11Mhz 11M/256fs=42.96k
}
	
return (audio_rate = val);
}

/**sep4020_audio_ioctl
*@inode:	inode struct
*@file:	file struct	
*@cmd:	command
*@arg:	argument
*
*		/dev/dsp 设备接口函数 		
*/
static int sep4020_audio_ioctl(struct inode *inode, struct file *file, uint cmd, ulong arg)
{
	#ifdef DEBUG
 	printk("sep4020_audio_ioctl,cmd=0x%x\n",cmd);
	#endif	
	long val;

	switch (cmd) 
	{
	case SNDCTL_DSP_SETFMT:
		get_user(val, (long *) arg);
		if (val & AUDIO_FMT_MASK) 
		{
			audio_fmt = val;
			break;
		} 
		else
		return -EINVAL;

	case SNDCTL_DSP_CHANNELS:
	case SNDCTL_DSP_STEREO:
		get_user(val, (long *) arg);
		if (cmd == SNDCTL_DSP_STEREO)
			val = val ? 2 : 1;
		if (val != 1 && val != 2)
			return -EINVAL;
		audio_channels = val;
		break;

	case SOUND_PCM_READ_CHANNELS:
		put_user(audio_channels, (long *) arg);
		break;

	case SNDCTL_DSP_SPEED:
		get_user(val, (long *) arg);
		if ((file->f_mode & FMODE_READ))		//判断改调用哪个函数来修改采样率
		{
			val = audio_set_dsp_speed_read(val);
		}
		else
		{
			val = audio_set_dsp_speed(val);
		}
	
		if (val < 0)
			return -EINVAL;
		put_user(val, (long *) arg);
		break;

	case SOUND_PCM_READ_RATE:
		put_user(audio_rate, (long *) arg);
	break;

	case SNDCTL_DSP_GETFMTS:
		put_user(AUDIO_FMT_MASK, (long *) arg);
	break;

	case SNDCTL_DSP_GETBLKSIZE:
		if(file->f_mode & FMODE_WRITE)
			return put_user(audio_fragsize, (long *) arg);
		else
			return put_user(audio_fragsize, (int *) arg);

	case SNDCTL_DSP_SETFRAGMENT:
		if (file->f_mode & FMODE_WRITE) 
		{
			if (output_stream.buffers)
				return -EBUSY;
			get_user(val, (long *) arg);
			audio_fragsize = 1 << (val & 0xFFFF);
			if (audio_fragsize < 16)
				audio_fragsize = 16;
			if (audio_fragsize > 16384)
				audio_fragsize = 16384;
			audio_nbfrags = (val >> 16) & 0x7FFF;
			if (audio_nbfrags < 2)
				audio_nbfrags = 2;
			if (audio_nbfrags * audio_fragsize > 128 * 1024)
				audio_nbfrags = 128 * 1024 / audio_fragsize;
			if (audio_setup_buf(&output_stream))
				return -ENOMEM;
		}
		if (file->f_mode & FMODE_READ) 
		{
			if (input_stream.buffers)
				return -EBUSY;
			get_user(val, (int *) arg);
			audio_fragsize = 1 << (val & 0xFFFF);
			if (audio_fragsize < 16)
				audio_fragsize = 16;
			if (audio_fragsize > 16384)
				audio_fragsize = 16384;
			audio_nbfrags = (val >> 16) & 0x7FFF;
			if (audio_nbfrags < 2)
				audio_nbfrags = 2;
			if (audio_nbfrags * audio_fragsize > 128 * 1024)
				audio_nbfrags = 128 * 1024 / audio_fragsize;
			if (audio_setup_buf(&input_stream))
				return -ENOMEM;
		}
	break;

	case SNDCTL_DSP_SYNC:
		return audio_sync(file);

	case SNDCTL_DSP_GETOSPACE:
		{
		audio_stream_t *s = &output_stream;
		audio_buf_info *inf = (audio_buf_info *) arg;
		int err = access_ok(VERIFY_WRITE, inf, sizeof(*inf));
		int i;
		int frags = 0, bytes = 0;

		if (err)
			return err;
		for (i = 0; i < s->nbfrags; i++) 
		{
			if (atomic_read(&s->buffers[i].sem.count) > 0) 
			{
				if (s->buffers[i].size == 0) frags++;
				bytes += s->fragsize - s->buffers[i].size;
			}
		}
		put_user(frags, &inf->fragments);
		put_user(s->nbfrags, &inf->fragstotal);
		put_user(s->fragsize, &inf->fragsize);
		put_user(bytes, &inf->bytes);
		break;
		}

	case SNDCTL_DSP_GETISPACE:
		{
		audio_stream_t *s = &input_stream;
		audio_buf_info *inf = (audio_buf_info *) arg;
		int err = access_ok(VERIFY_WRITE, inf, sizeof(*inf));
		int i;
		int frags = 0, bytes = 0;

		if (!(file->f_mode & FMODE_READ))
			return -EINVAL;

		if (err)
			return err;
		for(i = 0; i < s->nbfrags; i++)
		{
			if (atomic_read(&s->buffers[i].sem.count) > 0)
			{
				if (s->buffers[i].size == s->fragsize)
					frags++;
				bytes += s->buffers[i].size;
			}
		}
		put_user(frags, &inf->fragments);
		put_user(s->nbfrags, &inf->fragstotal);
		put_user(s->fragsize, &inf->fragsize);
		put_user(bytes, &inf->bytes);
		break;
	}

	case SNDCTL_DSP_RESET:
		if (file->f_mode & FMODE_READ) 
		{
			audio_clear_buf(&input_stream);
		}
		if (file->f_mode & FMODE_WRITE) 
		{
			audio_clear_buf(&output_stream);
		}
	return 0;
	case SNDCTL_DSP_NONBLOCK:
		file->f_flags |= O_NONBLOCK;
		return 0;
	case SNDCTL_DSP_POST:
	case SNDCTL_DSP_SUBDIVIDE:
	case SNDCTL_DSP_GETCAPS:
	case SNDCTL_DSP_GETTRIGGER:
	case SNDCTL_DSP_SETTRIGGER:
	case SNDCTL_DSP_GETIPTR:
	case SNDCTL_DSP_GETOPTR:
	case SNDCTL_DSP_MAPINBUF:
	case SNDCTL_DSP_MAPOUTBUF:
	case SNDCTL_DSP_SETSYNCRO:
	case SNDCTL_DSP_SETDUPLEX:
		return -ENOSYS;
	
	default:
		return sep4020_mixer_ioctl(inode, file, cmd, arg);
	}

	return 0;
}

/**sep4020_audio_open
*@inode:	inode struct
*@file:	file struct	
*
*		/dev/dsp 设备open函数 		
*/
static int sep4020_audio_open(struct inode *inode, struct file *file)
{
	#ifdef DEBUG
 	printk("sep4020_audio_open\n");
	#endif

	int cold = !audio_active;
	
	if ((file->f_flags & O_ACCMODE) == O_RDONLY) 
	{
		if (audio_rd_refcount || audio_wr_refcount)
			return -EBUSY;
		audio_rd_refcount++;
	} 
	else if ((file->f_flags & O_ACCMODE) == O_WRONLY) 
	{
		if (audio_wr_refcount)
		return -EBUSY;
		audio_wr_refcount++;
	}
	else if ((file->f_flags & O_ACCMODE) == O_RDWR) 
	{
		/*sep4020 does not support this mode!*/		
		printk("sep4020 does not support this mode!\n");
		return -EINVAL;

	} 
	else
		return -EINVAL;

	if (cold) 
	{
		audio_rate = AUDIO_RATE_DEFAULT;
		audio_channels = AUDIO_CHANNELS_DEFAULT;
		audio_fragsize = AUDIO_FRAGSIZE_DEFAULT;
		audio_nbfrags = AUDIO_NBFRAGS_DEFAULT;
		if ((file->f_mode & FMODE_WRITE))
		{		
			DPRINTK("init 4020 IIS tx\n");
		
			init_sep4020_iis_bus_tx();		//这里调用iis的tx初始化函数		
			audio_clear_buf(&output_stream);
		}
		if (!output_stream.buffers && audio_setup_buf(&output_stream))    
			return  -ENOMEM;
		if ((file->f_mode & FMODE_READ))
		{
			DPRINTK("init 4020 IIS rx\n");
			
			audio_fragsize = AUDIO_FRAGSIZE_DEFAULT_READ;		//调整为16byte
			init_sep4020_iis_bus_rx();				
			audio_clear_buf(&input_stream);
		}
	}
	return 0;
}

/**sep4020_mixer_open
*@inode:	inode struct
*@file:	file struct	
*
*		/dev/mixer 设备open函数 		
*/
static int sep4020_mixer_open(struct inode *inode, struct file *file)
{
	return 0;
}

/**sep4020_audio_release
*@inode:	inode struct
*@file:	file struct	
*
*		/dev/dsp 设备close关联函数 		
*/
static int sep4020_audio_release(struct inode *inode, struct file *file)
{
	#ifdef DEBUG
 	printk("sep4020_audio_release\n");
	#endif

	if (file->f_mode & FMODE_READ) 
	{
				
		if (audio_rd_refcount == 1)
		{
			#ifdef DEBUG			
			printk("audio_release read release\n");
			#endif
			audio_clear_buf(&input_stream);		
		}		
		
		audio_rd_refcount = 0;
	}
	if(file->f_mode & FMODE_WRITE) 
	{
		
		if (audio_wr_refcount == 1) 
		{
			#ifdef DEBUG			
			printk("audio_release write release\n");
			#endif			
			audio_sync(file);
			audio_clear_buf(&output_stream);
			audio_wr_refcount = 0;
		}
	}
	return 0;
}

/**sep4020_mixer_release
*@inode:	inode struct
*@file:	file struct	
*
*		/dev/mixer 设备close关联函数 		
*/
static int sep4020_mixer_release(struct inode *inode, struct file *file)
{
	return 0;
}


static struct file_operations sep4020_audio_fops = 
{
	llseek: sep4020_audio_llseek,
	write: sep4020_audio_write,
	read: sep4020_audio_read,
	poll: sep4020_audio_poll,
	ioctl: sep4020_audio_ioctl,
	open: sep4020_audio_open,
	release: sep4020_audio_release
};

static struct file_operations sep4020_mixer_fops = 
{
	ioctl: sep4020_mixer_ioctl,
	open: sep4020_mixer_open,
	release: sep4020_mixer_release
};

/**init_uda1341
*
*	初始化uda1341芯片（通过L3 bus控制）		
*/
static void init_uda1341(void)
{	
	#ifdef DEBUG
 	printk("init_uda1341\n");
	#endif

	unsigned long flags;
      uda1341_volume = 62 - ((DEF_VOLUME * 61) / 100);
      uda1341_boost = 0;

      local_irq_save(flags);
	*(volatile unsigned long*)(GPIO_PORTD_DATA_V) &= ~(L3M|L3C|L3D);
	*(volatile unsigned long*)(GPIO_PORTD_DATA_V) |= (L3M|L3C); 		//Start condition : L3M=H, L3C=H
      local_irq_restore(flags);
	
      uda1341_l3_address(UDA1341_REG_STATUS);
      uda1341_l3_data(0x40 | STAT0_SC_256FS | STAT0_IF_I2S | STAT0_DC_FILTER);// 修改为256fs
      uda1341_l3_data(STAT1 | STAT1_ADC_GAIN | STAT1_ADC_ON | STAT1_DAC_ON);

      uda1341_l3_address(UDA1341_REG_DATA0);
      uda1341_l3_data(DATA0 | DATA0_VOLUME(0)); 					// maximum volume
      uda1341_l3_data(DATA1 | DATA1_BASS(uda1341_boost) | DATA1_TREBLE(0));
      uda1341_l3_data((DATA2 | DATA2_DEEMP_NONE ) & ~(DATA2_MUTE));
      uda1341_l3_data(EXTADDR(EXT2));
      uda1341_l3_data(EXTDATA(EXT2_MIC_GAIN(1)) | EXT2_MIXMODE_CH1);		// input channel 1 select(input channel 2 off)

}


/*init_sep4020_iis_bus_rx
*
*	将I2S配置为录音模式		
*/
static void init_sep4020_iis_bus_rx(void)
{
	#ifdef DEBUG
	printk("init_sep4020_iis_bus_tx record\n");
 	#endif

	/*selet the input line*/	
	*(volatile unsigned long*)(GPIO_PORTG_DIR_V)  &= ~(0x1<<11);
	*(volatile unsigned long*)(GPIO_PORTG_SEL_V)  |=  (0x1<<11);
	*(volatile unsigned long*)(GPIO_PORTG_DATA_V) &= ~(0x1<<11);
	
	/*首先复位一次，再进行配置，1f对应88MHz的44.1k*/
	*(volatile unsigned long*)(I2S_CTRL_V) = MUTE(0) + RESET_1(1) + ALIGN(0) + WS(0) + WID(1) + DIR(1)+ MODE(0) + MONO(0) + STOP(1) + DIV(0x1f);
	*(volatile unsigned long*)(I2S_CTRL_V) = MUTE(0) + RESET_1(0) + ALIGN(0) + WS(0) + WID(1) + DIR(1)+ MODE(0) + MONO(0) + STOP(1) + DIV(0x1f);	
	*(volatile unsigned long*)(I2S_INT_V) = 0x02;		//接收中断始能	
	udelay(10);
 	*(volatile unsigned long*)(I2S_CTRL_V) &= 0xfffeffff; //启动

} 

/**init_sep4020_iis_bus_tx
*
*	将I2S配置为放音模式		
*/
static void init_sep4020_iis_bus_tx(void)
{
	#ifdef DEBUG
 	printk("init_sep4020_iis_bus_tx play the music\n");
	#endif

	/*selet the output line*/
	*(volatile unsigned long*)(GPIO_PORTG_DIR_V)  &= ~(0x1<<11);
	*(volatile unsigned long*)(GPIO_PORTG_SEL_V)  |= 0x1<<11;
	*(volatile unsigned long*)(GPIO_PORTG_DATA_V) |= 0x1<<11;
	 
	/*首先复位一次，再进行配置，1f对应88MHz的44.1k*/
	*(volatile unsigned long*)(I2S_CTRL_V) = MUTE(0) + RESET_1(1) + ALIGN(0) + WS(0) + WID(1) + DIR(0)+ MODE(0) + MONO(0) + STOP(1) + DIV(0x1f);
	*(volatile unsigned long*)(I2S_CTRL_V) = MUTE(0) + RESET_1(0) + ALIGN(0) + WS(0) + WID(1) + DIR(0)+ MODE(0) + MONO(0) + STOP(1) + DIV(0x1f);	
	*(volatile unsigned long*)(I2S_INT_V) = 0x04;		//发送中断始能	
	udelay(10);
 	*(volatile unsigned long*)(I2S_CTRL_V) &= 0Xfffeffff; //启动
	
}

/**init_uda1341
*
*	初始化DMA函数，没有使用中断，简化了很多内容	
*/
static int __init audio_init_dma(audio_stream_t * s, char *desc)
{
	#ifdef DEBUG
 	printk("audio_init_dma\n");
	#endif

	s->dma_ok = 1; 
	
	//清除要用的dma通道（dma1）上的传输完成中断寄存器
	*(volatile unsigned long*)(DMAC_INTTCCLEAR_V) = 0X02; 
	*(volatile unsigned long*)(DMAC_INTTCCLEAR_V) = 0X00;
  	//清除要用的dma通道（dma1）上的错误中断寄存器
	*(volatile unsigned long*)(DMAC_INTINTERRCLR_V) = 0x02;
  	*(volatile unsigned long*)(DMAC_INTINTERRCLR_V) = 0x00;

	return 0;
}

/**audio_clear_dma
*@s addio_stream_t struct
*
*	清除DMA函数，DMA依然没有由系统管理	
*/
static int audio_clear_dma(audio_stream_t * s)							
{
	//清除要用的dma通道（dma1）上的传输完成中断寄存器
	*(volatile unsigned long*)(DMAC_INTTCCLEAR_V) = 0X02; 
	*(volatile unsigned long*)(DMAC_INTTCCLEAR_V) = 0X00;
  	//清除要用的dma通道（dma1）上的错误中断寄存器
	*(volatile unsigned long*)(DMAC_INTINTERRCLR_V) = 0x02;
  	*(volatile unsigned long*)(DMAC_INTINTERRCLR_V) = 0x00;
	
	return 0;
}

/**sep4020_uda1341_init
*
*	驱动初始化加载函数
*/
static int __init sep4020_uda1341_init(void) 
{
	#ifdef DEBUG
 	printk("sep4020_uda1341_init\n");
	#endif
	
      memzero(&input_stream, sizeof(audio_stream_t));
      memzero(&output_stream, sizeof(audio_stream_t));

	*(volatile unsigned long*)(GPIO_PORTD_DIR_V) &= ~(0xd<<1);       //GPB[4:1]=00_0 Output(L3CLOCK):Output(L3DATA):Output(L3MODE)   
	*(volatile unsigned long*)(GPIO_PORTD_SEL_V) |= (0xd<<1);        //GPD[4:1] 1 1010 
	
	/*GPIO_PG11控制将处理器的数据信号I2S_SD连接在I2S_DIN或者I2S_DOUT上。
	放音过程种将GPIO_PG11置高，I2S_SD与I2S_DIN相连。*/
	*(volatile unsigned long*)(GPIO_PORTG_DIR_V)  &= ~(0x1<<11);
	*(volatile unsigned long*)(GPIO_PORTG_SEL_V)  |= 0x1<<11;
	*(volatile unsigned long*)(GPIO_PORTG_DATA_V) |= 0x1<<11;

	/*init_PWM 给UDA1341提供CDCLK*/
	*(volatile unsigned long*)PWM4_CTRL_V =0x00;
      *(volatile unsigned long*)PWM4_DIV_V =0x4; 	//88MHz/(4*2)=11Mhz 11M/256fs=42.96k
  	*(volatile unsigned long*)PWM4_PERIOD_V =0x2;	//计数时钟为总线的DIV分频
  	*(volatile unsigned long*)PWM4_DATA_V =0x1;  	//周期为两个计数时钟
  	*(volatile unsigned long*)PWM_ENABLE_V =0x1<<3;	//高电平为一个计数时钟

      //init_sep4020_iis_bus();
      init_uda1341();

	audio_init_dma(&output_stream, "UDA1341 out");
		
      audio_dev_dsp = register_sound_dsp(&sep4020_audio_fops, -1);
	if(audio_dev_dsp==-1)
		goto err;
      
	audio_dev_mixer = register_sound_mixer(&sep4020_mixer_fops, -1);
	if(audio_dev_mixer==-1)
		goto err;


      printk(AUDIO_NAME_VERBOSE " initialized\n");
	return 0;

err:
	printk("UDA1341 fail initialized!\n");
	return -EINVAL;	        

}

/**sep4020_uda1341_exit
*
*	驱动卸载函数
*/
static void __exit sep4020_uda1341_exit(void) 
{
	unregister_sound_dsp(audio_dev_dsp);
      unregister_sound_mixer(audio_dev_mixer);
	audio_clear_dma(&output_stream);
      printk(AUDIO_NAME_VERBOSE " unloaded\n");	
}

module_init(sep4020_uda1341_init);
module_exit(sep4020_uda1341_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("aokikyon@gmail.com");
MODULE_DESCRIPTION("sep4020 uda1341 sound driver");

