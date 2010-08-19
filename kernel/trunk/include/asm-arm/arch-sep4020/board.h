#ifndef __ASM_ARCH_BOARD_H
#define __ASM_ARCH_BOARD_H
//#include <asm/arch/init.h>
 /* Ethernet */
struct sep_eth_data {
         u8              phy_irq_pin;    /* PHY IRQ */
         u8              is_rmii;        /* using RMII interface? */
  };
extern void __init sep_add_device_eth(struct sep_eth_data *data);
 
/* LCD Controller */
 struct sep4020fb_val {
	unsigned int	defval;
	unsigned int	min;
	unsigned int	max;
};

struct sep4020fb_hw {
	unsigned long	sep_ssa;
	unsigned long	sep_size;
	unsigned long	sep_pcr;
	unsigned long	sep_hcr;
	unsigned long	sep_vcr;
	unsigned long	sep_pwmr;
	unsigned long	sep_lecr;
	unsigned long	sep_dmacr;
	unsigned long	sep_lcdisren;
	unsigned long	sep_lcdisr;
};

struct sep4020fb_mach_info {
//	unsigned char	fixed_syncs;	/* do not update sync/border */

	/* Screen size */
	int		width;
	int		height;

	/* Screen info */
	struct sep4020fb_val xres;
	struct sep4020fb_val yres;
	struct sep4020fb_val bpp;

	/* lcd configuration registers */
	struct sep4020fb_hw  regs;

	/* GPIOs */

	/*unsigned long	gpcup;
	unsigned long	gpcup_mask;
	unsigned long	gpccon;
	unsigned long	gpccon_mask;
	unsigned long	gpdup;
	unsigned long	gpdup_mask;
	unsigned long	gpdcon;
	unsigned long	gpdcon_mask;
*/
	/* lpc3600 control register */
	//unsigned long	lpcsel;
};

extern void __init sep4020_fb_set_platdata(struct sep4020fb_mach_info *);
/* MMC / SD */
struct sep_mmc_data {
	u8		det_pin;	/* card detect IRQ */
	u8    slot_b;	/* uses Slot B */
	u8    wire4;	/* (SD) supports DAT0..DAT3 */
	u8		wp_pin;		/* (SD) writeprotect detect */
	u8		vcc_pin;	/* power switching (high == on) */
};
extern void __init sep_add_device_mmc(struct sep_mmc_data *data);

#endif



 
