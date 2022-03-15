//Definition der Klasse CProc51

#ifndef _PROC51
  #define _PROC51

#include "..\core\Proc.h"
#include "..\core\reginfo.h"

//Recordtypdefinitionen

#define REC_CODE         0x06
#define REC_DEBUG        0x12
#define REC_SRCNAME      0x24
#define REC_DEBUG_KEIL   0x22
#define REC_TYPEDEF      0x20 
#define REC_SCOPE        0x10
#define REC_MODHEAD      0x02
#define REC_MODEND       0x04
#define REC_START        0x70

#define AKKU  0xE0
#define BREG  0xF0
#define SPT   0x81
#define DPL   0x82
#define DPH   0x83
#define PSW   0xD0
#define PORT2 0xA0
#define PORT3 0xB0
#define IE0   0xA8
#define IP0   0xB8
#define TCON  0x88
#define TMOD  0x89
#define TL0   0x8A
#define TH0   0x8C
#define TL1   0x8B
#define TH1   0x8D
#define SCON  0x98
#define S0BUF 0x99
  #define  PARITY  0x01
  #define  CARRY   0x80
  #define  OV      0x04
  #define  AC      0x40
  #define  REGBANK 0x18


extern USHORT PC;

class CProc51 : public CProc
{
public: 	
  const char* GetProcessorName(){ return "8051 - Standard";};   
  int ExecNextCmd( );	
  void Init(HWND hMainWnd,int language=_ENG);
  BOOL WriteMemBlock(ULONG addr,UCHAR* bp,USHORT len,ULONG memspec=0);  
  BOOL IsSerChar;
  void SetMeasurePoint(CMeasurePoint* pmpt);  
  ULONG GetDestinationAddress(ULONG addr,ULONG memspec=0);  
  ULONG GetRegOffset(int index=0);
  int enabledInt;
  int intReq;
  int tracelen;  
  void EnableTraceLog(LPCSTR logname=NULL,int tracetype=0,ULONG addr=0);
  void WriteCurrentASMTraceRecord(USHORT pc);  
  ULONG GetCyclCnt(BOOL bDelete=FALSE);
  int GetNextMemMapping(mcfg_t* pMemCfg,int blockindex=-1);
  BOOL DeleteMem(int blockindex=0,char memspec=0);
  int CreateMemInRange(ULONG memspec=0,ULONG startaddr=0, ULONG endaddr=0);
  void ResetCPU();
  BOOL IsMemAtAddress(ULONG addr, ULONG memspec=0);
  BOOL GetPointer(ULONG* pointeraddr,ULONG* pointerval,
                        CTypdesc* pointtyp=NULL,
                        USHORT pointmem=0, ULONG* pointtomem=NULL,BOOL orderHL=TRUE);
  ULONG SetMemAtAddr(ULONG addr,ULONG* pval,ULONG memspec=0);
  ULONG GetMemSize(ULONG mempec=0);
  int GetMemAlignment(){ return 0;};  // Speicherbelegung H/L
  BOOL GetMemFromAddr(ULONG addr,ULONG* valp,ULONG memspec=0);
  BOOL IsReturn(ULONG addr);
  ULONG GetStepOverAddr(ULONG addr);	
  BOOL ClrTempBkpt(ULONG addr);
  USHORT GetBkptFormat(ULONG addr,int memspec=0);
  inline void RestoreBkpt(ULONG addr) { return; };
  inline void RestoreOpcode(ULONG addr) { return; };
  ULONG IsBreakpointAtAddr(ULONG addr,ULONG fmt=BKPT_CODE,int memspec=0);
  BOOL RemoveBreakpoint(ULONG addr,int memspec=0);
  BOOL SetBreakpoint(ULONG addr,USHORT fmt=BKPT_CODE,int memspec=0);
  BOOL GetNextBreakpoint(POSITION& pos, bkpt_t* bkpt);	  
  CPtrList bkpts;
  CRegdef* GetNextRegister(int index=0); //liefert einen Pointer auf das  nächste Register
  BOOL ObExt;
  ULONG GetStartUpAddress(){return(0);};
  void SetProgramCounter(ULONG pc);
  inline ULONG GetProgramCounter(){return (ULONG)PC;};
  
  BOOL TrackStack;	  
   
  CProc51();
  ~CProc51();
  UINT Reassemble( ULONG code, LPSTR pms, HANDLE hMod=0);
  void Init51(unsigned char val=0);

private:
	int SetSPT(int diff);
	static const char SReg510[128][6];
	static const char SBit510[128][6];
protected:
  void HandleTimers(int ticks);
	void GenerateTXInt();
	int defaultMemcfg;
	void PackMemCfg();
	void SetDefaultMemCfg();
	CPtrList memcfgs;
 	int SetDataMem(USHORT addr, USHORT val);
	int SetXdataMem(USHORT addr, USHORT val);
	int SetIdataMem(USHORT addr, USHORT val);
	int GetDataMem(USHORT addr, USHORT*val);
	int GetIdataMem(USHORT addr, USHORT*val);
	int GetXdataMem(USHORT addr, USHORT*val);
	int GetCodeMem(USHORT addr, USHORT*val);
	int ExecNextCmd1();
	void GetRegName(char* ,int regno, HANDLE hMod=0);
	void GetBitName(char* ,int bitno);
	void SetBit(int  bitno,BOOL bitstate);
	BOOL GetBitState(int bitno,BOOL clr);
  inline void SetParity( int akku );
	CRegInfo regs;
};

#endif //_PROC51