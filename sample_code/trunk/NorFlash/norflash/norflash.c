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
* ������:	   CheckSST39VF160X
* ��ڲ���:   �� 
* ���ز���:   �ͺŻ���FALSE
* ��������:   ��ȡFlash�б�����ͺ���Ϣ������Ԥ��ϣ���õ����ͺŽ��бȽ�,
*                  ƥ�䷵���ͺţ����򷵻�FALSE 
*************************************************************************************/
//��ѯоƬID ������������μ�оƬ�ֲ�

S32 CheckSST39VF160X(void)
{
    WORD SST_id1;
    WORD SST_id2;
    S32  ReturnStatus;
    
    // ���Ͳ�ѯID����
    *sysAddress(0x5555) = 0x00AA;  // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;  // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0x0090;  // write data 0x0090 to device addr 0x5555
    
    Delay30NanoSeconds(3);         // Tida Max 150ns for 39VF160X
    
    // ��ID
    SST_id1  = *sysAddress(0x0000);   // get first ID byte
    SST_id2  = *sysAddress(0x0001);   // get second ID byte
    
    // �ж��Ƿ���SST 39VF1601
    if ((SST_id1 == SST_ID) && (SST_id2 == SST_39VF1601))
        return(SST_id2);
    else
        ReturnStatus = FALSE;
        
    // �ж��Ƿ��� SST 39VF1602
     if ((SST_id1 == SST_ID) && (SST_id2 == SST_39VF1602))
        ReturnStatus = SST_id2;
    else
        ReturnStatus = FALSE;

    // ��ѯ�˳������������Իָ�����������״̬
    *sysAddress(0x5555) = 0x00AA;  // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;  // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0x00F0;  // write data 0x00F0 to device addr 0x5555
    
    Delay30NanoSeconds(5);         // Tida Max 150ns for 39VF160X	
    
    return (ReturnStatus);
}

/************************************************************************************
* ������:	   CfiQuery
* ��ڲ���:   src ����ָ���������������Flash�ж�ȡ��CFI��Ϣ�� 
* ���ز���:   ��
* ��������:   ��Flash�ж�ȡCFI��Ϣ����ͨ��Src����
*************************************************************************************/
void CfiQuery(WORD *Src)
{
    WORD index;
    
    //  ���Ͳ�ѯCFI����
    *sysAddress(0x5555) = 0x00AA;  // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;  // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0x0098;  // write data 0x0098 to device addr 0x5555
    
    Delay30NanoSeconds(5);        // insert delay time = Tida
    
    //  ��ȡCFI
    for ( index = 0x0010; index <= 0x001A; index++)
    {
        *Src = *sysAddress(index);              //�� CFI ��Ϣ�洢��������
        ++Src;
    }
    
    // ��ѯ�˳������������Իָ�����������״̬
    *sysAddress(0x5555) = 0x00AA;  // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;  // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0x00F0;  // write data 0x00F0 to device addr 0x5555
    
    Delay30NanoSeconds(5);         // insert delay time = Tida
	
}

/************************************************************************************
* ������:	   SecIDQuery
* ��ڲ���:   SST_SecID �����������ѯ�õ���SST��ȫID����
*                  Ueser_SecID (���������ѯ�õ���User��ȫID)
* ���ز���:   ��
* ��������:   ��Flash�в�ѯSST��User��ȫ��Ϣ��������
*************************************************************************************/
void SecIDQuery(WORD *SST_SecID, WORD *User_SecID)
{
    WORD index;
    
    //  ���� SecID ��ѯ����
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
    
    // ��ѯ�˳������������Իָ�����������״̬
    *sysAddress(0x5555) = 0x00AA;  // write data 0x00AA to device addr 0x5555
    *sysAddress(0x2AAA) = 0x0055;  // write data 0x0055 to device addr 0x2AAA
    *sysAddress(0x5555) = 0x00F0;  // write data 0x00F0 to device addr 0x5555
    
    Delay30NanoSeconds(5);      // insert delay time = Tida
       	 
}

/************************************************************************************
* ������:	   EraseOneSector
* ��ڲ���:   Dst �����������Ŀ�ʼ��ַ��
* ���ز���:   ReturnStatus ����ʾ���������Ƿ���ɣ�
* ��������:   ������Dst��ַ��ʼ����һ��Sector��С�Ŀռ� 
*************************************************************************************/
S32 EraseOneSector(Uint32 Dst)
{
    Uint32 DestBuf = Dst;
    S32  ReturnStatus;

    //  ���Ͳ�������
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
* ������:	   EraseOneBlock
* ��ڲ���:   Dst �����������Ŀ�ʼ��ַ��
* ���ز���:   ReturnStatus ����ʾ���������Ƿ���ɣ�
* ��������:   ��Dst��ַ��ʼ����һ��Block��С�Ŀռ�  
*************************************************************************************/
S32 EraseOneBlock (Uint32 Dst)
{
    Uint32 DestBuf = Dst;
    S32 ReturnStatus;
    
    // ���Ͳ�������
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
* ������:	   EraseEntireChip
* ��ڲ���:   ��
* ���ز���:   ��
* ��������:   ��������Flash �Ŀռ� 
*************************************************************************************/
S32 EraseEntireChip(void)
{
    U16 data ;
    S32 i ;
    
    //  �����������ϸ�������������NorFLashоƬ�ֲ�
    *(U16 *)(sysAddress(0x5555)) = 0xAAAA;     // write data 0x00AA to device addr 0x5555
    *(U16 *)(sysAddress(0x2AAA)) = 0x5555;     // write data 0x0055 to device addr 0x2AAA
    *(U16 *)(sysAddress(0x5555)) = 0x8080;     // write data 0x0080 to device addr 0x5555
    *(U16 *)(sysAddress(0x5555)) = 0xAAAA;     // write data 0x00AA to device addr 0x5555
    *(U16 *)(sysAddress(0x2AAA)) = 0x5555;     // write data 0x0055 to device addr 0x2AAA
    *(U16 *)(sysAddress(0x5555)) = 0x1010;     // write data 0x0010 to device addr 0x5555
    
    Delay10MicroSeconds(5000);                  // �ȴ�һ�� Tsce������ʱ�������NorFlashоƬ�ֲᣬFlash״̬�Ĵ���
    
     //��ҳ��ȡһ�Σ��Ƿ�ȫ 1     
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
* ������:	   ProgramOneWord
* ��ڲ���:   SrcWord  ָ��Դ���ݵ�ַ ��Dst  ָ��Flash��Ŀ�ĵ�ַƫ��
* ���ز���:   ReturnStatus ����ʾд�����Ƿ���ɣ�
* ��������:   ��Դ��ַSrcWord��ַ��ȡһ��WORD��16λ��д��Flash��ƫ�Ƶ�ַΪDst��ַ�� 
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
* ������:	   ProgramOneSector
* ��ڲ���:   SrcWord  ָ��Դ���ݵ�ַ ��Dst  ָ��Flash��Ŀ�ĵ�ַƫ��
* ���ز���:   ReturnStatus ����ʾд�����Ƿ���ɣ�
* ��������:   ��Դ��ַSrcWord��ַ��ȡһ��Sector�ֵ�����д��Flash����ʼƫ�Ƶ�ַΪΪDst��   
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
* ������:	   ProgramOneBlock
* ��ڲ���:   SrcWord  ָ��Դ���ݵ�ַ ��Dst  ָ��Flash��Ŀ�ĵ�ַƫ��
* ���ز���:   ReturnStatus ����ʾд�����Ƿ���ɣ�
* ��������:   ��Դ��ַSrcWord��ַ��ȡһ��BLock�ֵ�����д��Flash����ʼƫ�Ƶ�ַΪΪDst��   
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
* ������:	   SecIDLockStatus
* ��ڲ���:   ��
* ���ز���:   1 ��SecID�ֶα�LOCK��,0 ��SecID�ֶ�UNLOCK��
* ��������:   ��ѯFlash��SecID�ֶε�LOCK״̬   
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
* ������:	   UserSecIDWordPro
* ��ڲ���:   SrcWord   (��д����Դ��ַ) ��Dst   (д��Flash�е�Ŀ�ĵ�ַ) ��Length (��
*                  д���ݵ��ֳ���) 
* ���ز���:   1 ��д�ɹ���,0 ��дʧ�ܣ�
* ��������:   дFlash�е�User_SecID��    
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
* ������:	   UserSecIDLockOut
* ��ڲ���:   �� 
* ���ز���:   ��
* ��������:   дFlash�е�User_SecID��   
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
* ������:	   EraseSuspend
* ��ڲ���:   �� 
* ���ز���:   ��
* ��������:   ��ֹ����һ�� Sector/Block�� (����39VF160X���ṩ������)    
*************************************************************************************/
void EraseSuspend (void)
{
    *sysAddress(0x5555) = 0x00B0;        // write data 0x00B0 to any addr, i.e. 0x5555
    
     // The device automatically enters read mode , typically within 20 us after the Erase-Suspend command issued.
     Delay10MicroSeconds(2);  	
}


/************************************************************************************
* ������:	   EraseResume
* ��ڲ���:   �� 
* ���ز���:   ��
* ��������:   ���Ѳ���һ�� Sector/Block (����39VF160X���ṩ������)    
*************************************************************************************/
void EraseResume (void)
{
    *sysAddress(0x5555) = 0x0030;          // write data 0x0030 to any addr, i.e. 0x5555
}


/************************************************************************************
* ������:	   CheckToggleReady 
* ��ڲ���:   Dst  ��������Ŀ�ĵ�ַƫ�ƣ� 
* ���ز���:   1��Ŀ�ĵ�ַ�����ڽ���д�Ͳ�����������0��Ŀ�ĵ�ַ���У�
* ��������:   ���Flash��ƫ��ΪDst��ַ���Ƿ����ڽ���д���߲�������    
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
* ������:	   CheckDataPolling 
* ��ڲ���:   Dst  must already be set-up by the caller 
*                  TrueData 	this is the original (true) data  
* ���ز���:   TRUE   Data polling success ,FALSE  Time out 
* ��������:   During the internal program cycle, any attempt to read DQ7 of the
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
* ������:	   Check 
* ��ڲ���:   head1  �����бȽϵĵ�ַ1�� ,Head2  �����бȽϵĵ�ַ2��,Num����Ҫ
*                  �Ƚϵ����ݵ��ֳ��ȣ�
*                  TrueData 	this is the original (true) data  
* ���ز���:   0 ������һ�£�,1 �����ݲ�һ�£� 
* ��������:   �Ƚ�������ַ�������ݣ�һ����������Flash����д��Flash�е������Ƿ�
*                  ��Դ����һ��  
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