/*************************************************************************************************
Software Driver
SST39VF1601/SST39VF1602
16 Mbit Multi-Purpose Flash (MPF+)

June 2003

ABOUT THE SOFTWARE

This application note provides a software driver example for 39VF1601/39VF1602
16 Mbit Multi-Purpose Flash (MPF+) that can be used in any microprocessor based
system.

The SST39VF1601 supports bottom boot block protection, and the SST39VF1602 
supports top boot block protection. The boot block memory area is protected when 
WP# is low and unprotected when WP# is high.

Software driver example routines provided in this document utilize high-level
"C" programming language for broad platform support. In many cases, software 
driver routines can be inserted "as is" into the main body of code being 
developed by the system software developers. Extensive comments are included 
in each routine to describe the function of each routine. The software driver 
routines in "C" can be used with many microprocessors and microcontrollers. 

ABOUT THE SST39VF1601/SST39VF1602

Companion product datasheet for 39VF1601/39VF1602 should be reviewed in 
conjunction with this application note for a complete understanding of the device.

The C code in this document contains the following routines, which are listed
in this order:

Name                    Function
------------------------------------------------------------------
CheckSST39VF160X         Check manufacturer and device ID
CfiQuery                         CFI Query Entry/Exit command sequence
SecIDQuery		         SecID Query Entry/Exit command sequence
EraseOneSector              Erase a sector of 2048 words
EraseOneBlock               Erase a block of 32K words
EraseEntireChip              Erase the contents of the entire chip
ProgramOneWord          Alter data in one word
ProgramOneSector         Alter data in 2048-word sector
ProgramOneBlock          Alter data in 32K-word block
SecIDLockStatus	        Check the Lock Status of Security ID segment
UserSecIDWordPro        Write data into User Security ID Segment
UserSecIDLockOut 	 Lock out the User Security ID Segment
EraseSuspend		        Suspend Sector/Block Erase operation
EraseResume		        Resume Sector/Block Erase operation
CheckToggleReady        End of internal program or erase detection using
                                    Toggle bit
CheckDataPolling           End of internal program or erase detection using
                                    Data# polling

"C" LANGUAGE DRIVERS
 **************************************************************************************/
/**************************************************************************************/
/* Copyright Silicon Storage Technology, Inc. (SST), 1994-2003                 */
/* Example "C" language Driver of 39VF160X 16 Mbit MPF+ Device	         */
/* Nelson Wang, Silicon Storage Technology, Inc.                                    */
/*                                                                                                         */
/* Revision 1.0, June 19, 2003                                                                 */
/*                                                                                                         */
/* This file requires these external "timing"  routines:                                */
/*                                                                                                         */
/*      1.)  Delay10MicroSeconds                                                             */
/*      2.)  Delay30NanoSeconds                                                              */
/**************************************************************************************/

#include "sep4020.h"
#include "norflash.h"


// This sample code uses 0x20000000 as the system_base address.
// The user should modify this address accordingly.

Uint32 system_base = 0x20000000;           // 4GByte System Memory Address.


/************************************************************************************
* 函数名:	   CheckSST39VF160X
* 入口参数:   无 
* 返回参数:   型号或者FALSE
* 函数功能:   读取Flash中保存的型号信息，并与预定希望得到的型号进行比较,
*                  匹配返回型号，否则返回FALSE 
*************************************************************************************/
//查询芯片ID ，命令序列请参见芯片手册

S32 CheckSST39VF160X(void)
{
    WORD SST_id1;
    WORD SST_id2;
    S32  ReturnStatus;
    
    // 发送查询ID命令
    *sysAddress(0x5555) = 0x00AA;  // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;  // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0x0090;  // write data 0x0090 to device addr 0x5555
    
    Delay30NanoSeconds(3);         // Tida Max 150ns for 39VF160X
    
    // 读ID
    SST_id1  = *sysAddress(0x0000);   // get first ID byte
    SST_id2  = *sysAddress(0x0001);   // get second ID byte
    
    // 判断是否是SST 39VF1601
    if ((SST_id1 == SST_ID) && (SST_id2 == SST_39VF1601))
        return(SST_id2);
    else
        ReturnStatus = FALSE;
        
    // 判断是否是 SST 39VF1602
     if ((SST_id1 == SST_ID) && (SST_id2 == SST_39VF1602))
        ReturnStatus = SST_id2;
    else
        ReturnStatus = FALSE;

    // 查询退出，发送命令以恢复到正常操作状态
    *sysAddress(0x5555) = 0x00AA;  // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;  // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0x00F0;  // write data 0x00F0 to device addr 0x5555
    
    Delay30NanoSeconds(5);         // Tida Max 150ns for 39VF160X	
    
    return (ReturnStatus);
}

/************************************************************************************
* 函数名:	   CfiQuery
* 入口参数:   src （所指向区域用来保存从Flash中读取的CFI信息） 
* 返回参数:   无
* 函数功能:   从Flash中读取CFI信息，并通过Src返回
*************************************************************************************/
void CfiQuery(WORD *Src)
{
    WORD index;
    
    //  发送查询CFI命令
    *sysAddress(0x5555) = 0x00AA;  // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;  // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0x0098;  // write data 0x0098 to device addr 0x5555
    
    Delay30NanoSeconds(5);        // insert delay time = Tida
    
    //  读取CFI
    for ( index = 0x0010; index <= 0x001A; index++)
    {
        *Src = *sysAddress(index);              //将 CFI 信息存储到数组中
        ++Src;
    }
    
    // 查询退出，发送命令以恢复到正常操作状态
    *sysAddress(0x5555) = 0x00AA;  // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;  // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0x00F0;  // write data 0x00F0 to device addr 0x5555
    
    Delay30NanoSeconds(5);         // insert delay time = Tida
	
}

/************************************************************************************
* 函数名:	   SecIDQuery
* 入口参数:   SST_SecID （用来保存查询得到的SST安全ID），
*                  Ueser_SecID (用来保存查询得到的User安全ID)
* 返回参数:   无
* 函数功能:   从Flash中查询SST和User安全信息，并返回
*************************************************************************************/
void SecIDQuery(WORD *SST_SecID, WORD *User_SecID)
{
    WORD index;
    
    //  发送 SecID 查询命令
    *sysAddress(0x5555) = 0x00AA;  // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;  // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0x0088;  // write data 0x0088 to device addr 0x5555
    
    Delay30NanoSeconds(5);      // insert delay time = Tida
    
    //  Perform all Security ID operations here:
    //  SST programmed segment is from address 000000H--000007H,
    //  User programmed segment is from address 000010H--000017H.
    
    for ( index = 0x0000; index <= 0x0007; index++)
    {
        *SST_SecID = *sysAddress(index);
        ++SST_SecID;
        *User_SecID = *sysAddress(index+0x0010);
        ++User_SecID;    // Security query data is stored in user-defined memory space.
    }
    
    // 查询退出，发送命令以恢复到正常操作状态
    *sysAddress(0x5555) = 0x00AA;  // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;  // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0x00F0;  // write data 0x00F0 to device addr 0x5555
    
    Delay30NanoSeconds(5);      // insert delay time = Tida
       	 
}

/************************************************************************************
* 函数名:	   EraseOneSector
* 入口参数:   Dst （擦除操作的开始地址）
* 返回参数:   ReturnStatus （表示擦除操作是否完成）
* 函数功能:   擦除从Dst地址开始擦除一个Sector大小的空间 
*************************************************************************************/
S32 EraseOneSector(Uint32 Dst)
{
    Uint32 DestBuf = Dst;
    S32  ReturnStatus;

    //  发送擦除命令
    *sysAddress(0x5555) = 0x00AA;     // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;     // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0x0080;     // write data 0x0080 to device addr 0x5555
    *sysAddress(0x5555) = 0x00AA;     // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;     // write data 0x0055 to device addr 0x2AAA
    *sysAddress(DestBuf) = 0x0030;    // write data 0x0030 to device sector addr
    
    ReturnStatus = CheckToggleReady(DestBuf);	// wait for TOGGLE bit ready
    return ReturnStatus;
    
}

/************************************************************************************
* 函数名:	   EraseOneBlock
* 入口参数:   Dst （擦除操作的开始地址）
* 返回参数:   ReturnStatus （表示擦除操作是否完成）
* 函数功能:   从Dst地址开始擦除一个Block大小的空间  
*************************************************************************************/
S32 EraseOneBlock (Uint32 Dst)
{
    Uint32 DestBuf = Dst;
    S32 ReturnStatus;
    
    // 发送擦除命令
    *sysAddress(0x5555) = 0x00AA;     // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;     // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0x0080;     // write data 0x0080 to device addr 0x5555
    *sysAddress(0x5555) = 0x00AA;     // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;     // write data 0x0055 to device addr 0x2AAA
    *sysAddress(DestBuf) = 0x0050;    // write data 0x0050 to device sector addr
    
    ReturnStatus = CheckToggleReady(DestBuf);	  	// wait for TOGGLE bit ready
    
    return ReturnStatus;
}


/************************************************************************************
* 函数名:	   EraseEntireChip
* 入口参数:   无
* 返回参数:   无
* 函数功能:   擦除整个Flash 的空间 
*************************************************************************************/
S32 EraseEntireChip(void)
{
    U16 data ;
    S32 i ;
    
    //  整块擦除，详细命令序列请参照NorFLash芯片手册
    *(U16 *)(sysAddress(0x5555)) = 0xAAAA;     // write data 0x00AA to device addr 0x5555
    *(U16 *)(sysAddress(0x2AAA)) = 0x5555;     // write data 0x0055 to device addr 0x2AAA
    *(U16 *)(sysAddress(0x5555)) = 0x8080;     // write data 0x0080 to device addr 0x5555
    *(U16 *)(sysAddress(0x5555)) = 0xAAAA;     // write data 0x00AA to device addr 0x5555
    *(U16 *)(sysAddress(0x2AAA)) = 0x5555;     // write data 0x0055 to device addr 0x2AAA
    *(U16 *)(sysAddress(0x5555)) = 0x1010;     // write data 0x0010 to device addr 0x5555
    
    Delay10MicroSeconds(5000);                  // 等待一个 Tsce，具体时间请参照NorFlash芯片手册，Flash状态寄存器
    
     //整页读取一次，是否全 1     
     for (i=0; i<200000; i++) ;
     {
        data = *(U16 *)(0X20000000 +i );
        
        if(data != 0xffff )
        { 
            printf("error at addr %ld \n", (0X20000000 +i ));
            return E_HA ;  
        }
    }

    return 0 ;  
}


/************************************************************************************
* 函数名:	   ProgramOneWord
* 入口参数:   SrcWord  指向源数据地址 ，Dst  指向Flash中目的地址偏移
* 返回参数:   ReturnStatus （表示写操作是否完成）
* 函数功能:   从源地址SrcWord地址处取一个WORD（16位）写到Flash，偏移地址为Dst地址处 
*************************************************************************************/
S32 ProgramOneWord (WORD *SrcWord,   Uint32 Dst)
{
    Uint32 DestBuf = Dst;
    WORD *SourceBuf = SrcWord;
    S32 ReturnStatus;
    
    *sysAddress(0x5555) = 0xAAAA;           // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x5555;          // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0xA0A0;          // write data 0x00A0 to device addr 0x5555
    *(volatile U16*)(system_base|DestBuf) = *SourceBuf;       // transfer the WORD to destination
    
    ReturnStatus = CheckToggleReady(DestBuf);               // wait for TOGGLE bit ready
    return ReturnStatus;
    
}

/************************************************************************************
* 函数名:	   ProgramOneSector
* 入口参数:   SrcWord  指向源数据地址 ，Dst  指向Flash中目的地址偏移
* 返回参数:   ReturnStatus （表示写操作是否完成）
* 函数功能:   从源地址SrcWord地址处取一个Sector字的数据写到Flash中起始偏移地址为为Dst处   
*************************************************************************************/
S32 ProgramOneSector (WORD *Src, Uint32 Dst)
{
    WORD *SourceBuf;
    Uint32 DestBuf;
    S32 Index, ReturnStatus;
    
    SourceBuf = Src;
    DestBuf = Dst;
    ReturnStatus = EraseOneSector(DestBuf);	// erase the sector first
    
    if (!ReturnStatus)
    {
        return ReturnStatus;
    }
    
    for (Index = 0; Index < SECTOR_SIZE; Index++)
    {
        ReturnStatus = ProgramOneWord( SourceBuf, DestBuf);  // transfer data from source to destination
        ++DestBuf;
        ++SourceBuf;
        
        if (!ReturnStatus)
        {
            return ReturnStatus;
        }
    }
    
     return ReturnStatus;
      
}


/************************************************************************************
* 函数名:	   ProgramOneBlock
* 入口参数:   SrcWord  指向源数据地址 ，Dst  指向Flash中目的地址偏移
* 返回参数:   ReturnStatus （表示写操作是否完成）
* 函数功能:   从源地址SrcWord地址处取一个BLock字的数据写到Flash中起始偏移地址为为Dst处   
*************************************************************************************/
S32 ProgramOneBlock (WORD *Src, U32 Dst)
{
    WORD *SourceBuf;
    WORD *DestBuf;
    S32 Index;
    S32 ReturnStatus;
    
    SourceBuf = Src;
    DestBuf = (WORD *)Dst;
    ReturnStatus = EraseOneBlock((U32)(Dst>>1));	// erase the block first
    
     if (!ReturnStatus)
    {
        printf("Chip Erased Fail! \n");
        return ReturnStatus;
    }
    
    for (Index = 0; Index < BLOCK_SIZE; Index++)
    { 
        ReturnStatus = ProgramOneWord( SourceBuf, (Uint32)DestBuf);
        ++DestBuf;
        ++SourceBuf;
        
        if (!ReturnStatus)
        {
            printf("Chip Programed Fail! \n");
            return ReturnStatus;
        }
    }
    
    return ReturnStatus;

}


/************************************************************************************
* 函数名:	   SecIDLockStatus
* 入口参数:   无
* 返回参数:   1 （SecID字段被LOCK）,0 （SecID字段UNLOCK）
* 函数功能:   查询Flash中SecID字段的LOCK状态   
*************************************************************************************/
S32 SecIDLockStatus(void)
{
    WORD SecID_Status;
    
    //  Issue the Sec ID Entry code to 39VF160X
    *sysAddress(0x5555) = 0x00AA;  // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;  // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0x0088;  // write data 0x0088 to device addr 0x5555
    
    Delay30NanoSeconds(5);         // insert delay time = Tida
    
    //  Read Lock Status of SecID segment
    SecID_Status = *sysAddress(0x00FF);
    SecID_Status &= 0x0008; // Unlocked: DQ3=1; Locked: DQ3=0
    
    // Issue the Sec ID Exit code thus returning the 39VF160X ,to the read operating mode
     *sysAddress(0x5555) = 0x00AA;   // write data 0x00AA to device addr 0x5555
     *sysAddress(0x2AAA) = 0x0055;  // write data 0x0055 to device addr 0x2AAA
     *sysAddress(0x5555) = 0x00F0;  // write data 0x00F0 to device addr 0x5555
     
     Delay30NanoSeconds(5);         // insert delay time = Tida
     
      if (!SecID_Status)
      {
        return TRUE;		      // SecID segment is Locked
      }
      
      return FALSE;   		     // SecID segment is Unlocked
}


/************************************************************************************
* 函数名:	   UserSecIDWordPro
* 入口参数:   SrcWord   (待写数据源地址) ，Dst   (写入Flash中的目的地址) ，Length (待
*                  写数据的字长度) 
* 返回参数:   1 （写成功）,0 （写失败）
* 函数功能:   写Flash中的User_SecID段    
*************************************************************************************/
S32 UserSecIDWordPro (WORD *SrcWord, WORD *Dst, S32 length)
{
    WORD *DestBuf = Dst;
    WORD *SourceBuf = SrcWord;
    S32 test, index=length;
    
    test = SecIDLockStatus ();  // Check whether the SecID is Locked or not
    if (test)                     // TRUE: SecID is Locked
    {
        return FALSE;
    }
    
    while (index--)
    {
        *sysAddress(0x5555) = 0x00AA; // write data 0x00AA to device addr 0x5555
        *sysAddress(0x2AAA) = 0x0055; // write data 0x0055 to device addr 0x2AAA
        *sysAddress(0x5555) = 0x00A5; // write data 0x00A5 to device addr 0x5555
        *sysAddress((U32)DestBuf) = *SourceBuf; // transfer the WORD to destination
        ++DestBuf;
        ++SourceBuf;
        
        // Read the toggle bit to detect end-of-write for the Sec ID.Do Not use Data# Polling for UserSecIDWordPro.
        test = CheckToggleReady((Uint32)DestBuf);   // wait for TOGGLE bit to get ready
        if (!test)
        {
            return FALSE;   // SecID Word-Program failed!
        }
    
    }
    
    return TRUE;

}


/************************************************************************************
* 函数名:	   UserSecIDLockOut
* 入口参数:   无 
* 返回参数:   无
* 函数功能:   写Flash中的User_SecID段   
*************************************************************************************/
void UserSecIDLockOut (void)
{
    *sysAddress(0x5555) = 0x00AA; // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055; // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0x0085; // write data 0x0085 to device addr 0x5555
    *sysAddress(0x00FF) = 0x0000; // write data 0x0000 to any addr
    
    Delay10MicroSeconds(1);	// Wait for Word-Program timeout, Tbp=10us
}


/************************************************************************************
* 函数名:	   EraseSuspend
* 入口参数:   无 
* 返回参数:   无
* 函数功能:   禁止擦除一个 Sector/Block的 (根据39VF160X所提供的命令)    
*************************************************************************************/
void EraseSuspend (void)
{
    *sysAddress(0x5555) = 0x00B0;        // write data 0x00B0 to any addr, i.e. 0x5555
    
     // The device automatically enters read mode , typically within 20 us after the Erase-Suspend command issued.
     Delay10MicroSeconds(2);  	
}


/************************************************************************************
* 函数名:	   EraseResume
* 入口参数:   无 
* 返回参数:   无
* 函数功能:   唤醒擦除一个 Sector/Block (根据39VF160X所提供的命令)    
*************************************************************************************/
void EraseResume (void)
{
    *sysAddress(0x5555) = 0x0030;          // write data 0x0030 to any addr, i.e. 0x5555
}


/************************************************************************************
* 函数名:	   CheckToggleReady 
* 入口参数:   Dst  （待检查的目的地址偏移） 
* 返回参数:   1（目的地址处不在进行写和擦除操作），0（目的地址空闲）
* 函数功能:   检查Flash中偏移为Dst地址处是否正在进行写或者擦除操作    
*************************************************************************************/
S32 CheckToggleReady (Uint32 Dst)
{
    WORD PreData;
    WORD CurrData;
    unsigned long TimeOut = 0;
    
    PreData = *sysAddress(Dst);
    PreData = PreData & 0x0040;       // read DQ6
    while (TimeOut < MAX_TIMEOUT)	 // MAX_TIMEOUT=0x07FFFFFF
    {
        CurrData = *sysAddress(Dst);
        CurrData = CurrData & 0x0040;    // read DQ6 again
        
        if (PreData == CurrData)
        {
            return TRUE;
        }
        
        PreData = CurrData;
        TimeOut++;
    }
    
    return FALSE;
       
}
/************************************************************************************
* 函数名:	   CheckDataPolling 
* 入口参数:   Dst  must already be set-up by the caller 
*                  TrueData 	this is the original (true) data  
* 返回参数:   TRUE   Data polling success ,FALSE  Time out 
* 函数功能:   During the internal program cycle, any attempt to read DQ7 of the
*                  last byte loaded during the page/byte-load cycle will receive the  
*                  complement of the true data.  Once the program cycle is completed, 
*                 DQ7 will show true data. 
*************************************************************************************/
S32 CheckDataPolling (Uint32 Dst, WORD TrueData)
{
    WORD CurrData;
    U32 TimeOut = 0;
    
    TrueData = TrueData & 0x0080; 	// read D7
    
    while (TimeOut < MAX_TIMEOUT)	// MAX_TIMEOUT=0x07FFFFFF
    {
        CurrData = *sysAddress(Dst);
        CurrData = CurrData & 0x0080;   // read DQ7
        
        if (TrueData == CurrData)
        {
            return TRUE;
        }
        
        TimeOut++;
    }
     
     return FALSE;
}  


/************************************************************************************
* 函数名:	   Check 
* 入口参数:   head1  （进行比较的地址1） ,Head2  （进行比较的地址2）,Num（需要
*                  比较的数据的字长度）
*                  TrueData 	this is the original (true) data  
* 返回参数:   0 （数据一致）,1 （数据不一致） 
* 函数功能:   比较两个地址处的数据，一般用来检查对Flash进行写后Flash中的数据是否
*                  和源数据一致  
*************************************************************************************/
S32 Check(U32 head1, U32 head2, U32 num)
{
    U16 *p1;
    U16 *p2;
    U32 count = num ;
    U16 data;
    
    p1 = (U16 *)head1;
    p2 = (U16 *)head2;
    
    while(count-->0)
    {
        data = *((U16 *)p1++);
        
        if(data!= *((U16 *)p2++))
        {
            return FALSE;
        }
    }
    return TRUE;
    
}