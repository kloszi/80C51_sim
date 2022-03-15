//Implementierung CProc51
#include "stdafx.h"
#include "..\core\Proc.h"
#include "..\core\reginfo.h"
#include "Proc51.h"
#include "procdll.h"
#include "..\core\Typdesc.h"
#include "..\core\coreexport.h"
#include "accessDlg.h"

int accessbkpts;
USHORT PC,pc;
ULONG CyclCnt;
HWND hMWnd;
CFile tracefile;
CArchive* traceptr;
ULONG traceaddr;
BOOL activeMeasure;
CMeasurePoint* measurepoint;
int tracetyp;
BOOL callstackvalid;
int actInt;
int retval;

unsigned short codemem[0x10000];
unsigned char xdatamem[0x10000];
unsigned char xdatamemfmt[0x10000];
unsigned char datamem[256];
unsigned char datamemfmt[256];
unsigned char idatamem[128];
unsigned char idatamemfmt[128];
BOOL extTick0;
BOOL extTick1;

CProc51::~CProc51()
{
  POSITION pos;
  bkpt_t* bp;
  mcfg_t* mp;
  pos=bkpts.GetHeadPosition();
  while(pos)
  {
    bp= (bkpt_t*)bkpts.GetNext(pos);
    delete bp;
  }
  bkpts.RemoveAll();
  pos=memcfgs.GetHeadPosition();
  while(pos)
  {
    mp= (mcfg_t*)memcfgs.GetNext(pos);
    delete mp;
  }
  memcfgs.RemoveAll();
}

CProc51::CProc51()
{
  defaultMemcfg=0x0007;
  SetDefaultMemCfg();
  Init51();
  accessbkpts=0;
  measurepoint=0;
  activeMeasure=FALSE;
  PC=0;
  CyclCnt=0;
  IsSerChar=FALSE;
  TrackStack=TRUE;
  traceptr=0;
  regs.AddReg("AKKU",&datamem[AKKU],BYTE_HEX);
  regs.AddReg("B",&datamem[BREG],BYTE_HEX);
  regs.AddReg("PSW",&datamem[PSW],BYTE_HEX);
  regs.AddReg(" PSW",&datamem[PSW],BYTE_BIN);
  regs.AddReg("SP",&datamem[SPT],BYTE_HEX);
  regs.AddReg("P2",&datamem[PORT2],BYTE_HEX);
  regs.AddReg("DPTR",&datamem[DPL],SHORT_HEX_INVERS);
  regs.AddReg("R0",&datamem[0],BYTE_HEX|REGVARLOC);
  regs.AddReg("R1",&datamem[1],BYTE_HEX|REGVARLOC);
  regs.AddReg("R2",&datamem[2],BYTE_HEX|REGVARLOC);
  regs.AddReg("R3",&datamem[3],BYTE_HEX|REGVARLOC);
  regs.AddReg("R4",&datamem[4],BYTE_HEX|REGVARLOC);
  regs.AddReg("R5",&datamem[5],BYTE_HEX|REGVARLOC);
  regs.AddReg("R6",&datamem[6],BYTE_HEX|REGVARLOC);
  regs.AddReg("R7",&datamem[7],BYTE_HEX|REGVARLOC);
  regs.AddReg("PC",&PC,SHORT_HEX);

  intReq=0;
  actInt=0;
  callstackvalid=1;
  tracetyp=0;
}

void CProc51::SetProgramCounter(ULONG pc)
{
  PC=(USHORT)pc;
}

void CProc51::Init(HWND hMainWnd,int language)
{
  int i;

  theApp.hMainWnd=hMainWnd;
  ::language=language;
  hMWnd=hMainWnd;
  if(hMainWnd)
  {
    //commandID,Menuname,BitmapID,Tooltipstring,Statustext,Windowtyp
    if(language==_GER)
    {
      ::InsertProcMenuItem(IDD_SERSIM,"Terminal",IDB_TERMINAL,"Terminal","Terminal Simulation",DLLWND_DYNAMIC);
      ::InsertProcMenuItem(IDD_INTERRUPT,"Interrupts",IDB_INTERRUPT,"Interrupts","Interrupts",DLLWND_FIX);
      ::InsertProcMenuItem(IDD_TIMER,"Timer",IDB_TIMER,"Timer","8051-Timer",DLLWND_FIX);
    }
    else
    {
      ::InsertProcMenuItem(IDD_SERSIM_E,"Terminal",IDB_TERMINAL,"Terminal","Terminal simulation",DLLWND_DYNAMIC);
      ::InsertProcMenuItem(IDD_INTERRUPT_E,"Interrupts",IDB_INTERRUPT,"Interrupts","Interrupts",DLLWND_FIX);
      ::InsertProcMenuItem(IDD_TIMER,"Timer",IDB_TIMER,"Timer","8051-Timer",DLLWND_FIX);
    }
    for (i=0;i<128;i++)
    {
      if(SReg510[i][2]!='H') //also ein richtiger Name
        ::AddGlobalSymbol(0x80+i,LPSTR(SReg510[i]),(CTypdesc*)T_UCHAR,DATAMEM);
    }
    for (i=0;i<128;i++)
    {
      if( !strchr( LPCSTR(SBit510[i]),'.')) // nur wenn kein punkt im Namen auftaucht
        ::AddGlobalSymbol(0x80+i,LPSTR(SBit510[i]),(CTypdesc*)T_BIT,DATAMEM);
    }
    ::AddGlobalSymbol(0x0000,"reset",(CTypdesc*)T_CODELABEL,CODEMEM);
    ::AddGlobalSymbol(0x0003,"ext0_int",(CTypdesc*)T_CODELABEL,CODEMEM);
    ::AddGlobalSymbol(0x000b,"t0_int",(CTypdesc*)T_CODELABEL,CODEMEM);
    ::AddGlobalSymbol(0x0013,"ext1_int",(CTypdesc*)T_CODELABEL,CODEMEM);
    ::AddGlobalSymbol(0x001b,"t1_int",(CTypdesc*)T_CODELABEL,CODEMEM); 
    ::AddGlobalSymbol(0x0023,"ser_int",(CTypdesc*)T_CODELABEL,CODEMEM); 
    ::AddGlobalSymbol(0x002b,"t2_int",(CTypdesc*)T_CODELABEL,CODEMEM);
  }
  Init51(0xFF);
}

void CProc51::Init51(unsigned char val)
{
  int i;
  unsigned char  *xp;
  unsigned short *cp;

  accessbkpts=0;
  defaultMemcfg=0x0007;
  cp=codemem;
  xp=xdatamem;
  for(i=0;i<0x10000;i++)
  {
    *cp++=val;                  //codespeicher erstmal löschen
    *xp++=val;
  }
  xp=xdatamemfmt;
  for(i=0;i<0x10000;i++)
  {
    *xp++=0;                  //breakpoints erstmal löschen
  }
  for(i=0;i<128;i++)
  {
    datamem[i]=val;            //Direktadressierbaren Speicher löschen
    datamemfmt[i]=0;
    datamem[i+0x80]=0;
    datamemfmt[i+0x80]=0;
    idatamem[i]=val;           //IdataSpeicher löschen
    idatamemfmt[i]=0;
  }
  datamem[SPT]=0x07;
  datamem[0x80]=0xFF;
  datamem[0x90]=0xFF;
  datamem[0xA0]=0xFF;
  datamem[0xB0]=0xFF;
  intReq=0;
  CyclCnt=0;
}


// Reassembliert den übergebenen Maschinencode, Returnwert ist Anzahl der Bytes die
// der Befehl erfordert
// Übergabeparameter sind:
// * der Stand des Codespeichers ab dem die Übersetzeung erfolgen soll
// * der String in das reassemblierte Kommando geschrieben wird.

UINT CProc51::Reassemble( ULONG code, LPSTR pms,HANDLE hMod )
{
  char vstr[200];
  USHORT* cp;
  LONG addr;
  char lbl[200];

  code&=0xFFFF;
  cp=&codemem[code];
  UINT cl=0;
  int opcode=*cp & 0xFF;     //erstes Byte ist Opcode
  if(opcode<0x10)
  {
    switch(opcode & 0x0F)
    {
      case 0x00:
        cl=1;
        strcpy(pms,"nop");
        break;
      case 0x01:
        cl=2;
        addr=(USHORT)cp[1] | (code & 0xF800);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,PUBSYM|LOCSYM,4);
        strcpy(pms,"ajmp ");
        strcat(pms,lbl);
        break;
      case 0x02:
        cl=3;
        addr=(cp[1]<<8)+cp[2];
        ::FindLabel((USHORT)addr,lbl,CODEMEM,PUBSYM|LOCSYM,4);
        strcpy(pms,"ljmp ");
        strcat(pms,lbl);
        break;
      case 0x03:
        cl=1;
        strcpy(pms,"rr   a");
        break;
      case 0x04:
        cl=1;
        strcpy(pms,"inc  a");
        break;
      case 0x05:
        cl=2;
        strcpy(pms,"inc  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x06:
        cl=1;
        strcpy(pms,"inc  @r0");
        break;
      case 0x07:
        cl=1;
        strcpy(pms,"inc  @r1");
        break;
      case 0x08:
        cl=1;
        strcpy(pms,"inc  r0");
        break;
      case 0x09:
        cl=1;
        strcpy(pms,"inc  r1");
        break;
      case 0x0A:
        cl=1;
        strcpy(pms,"inc  r2");
        break;
      case 0x0B:
        cl=1;
        strcpy(pms,"inc  r3");
        break;
      case 0x0C:
        cl=1;
        strcpy(pms,"inc  r4");
        break;
      case 0x0D:
        cl=1;
        strcpy(pms,"inc  r5");
        break;
      case 0x0E:
        cl=1;
        strcpy(pms,"inc  r6");
        break;
      case 0x0F:
        cl=1;
        strcpy(pms,"inc  r7");
        break;
    }
    return(cl);
  }
  if(opcode<0x20)
  {
    switch(opcode & 0x0F)
    {
      case 0x00:
        cl=3;
        strcpy(pms,"jbc  ");
        GetBitName(vstr,cp[1]);
        strcat(pms, vstr);
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)(cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2]) & 0xFF);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcat(pms,",");
        strcat(pms,lbl);
        break;
      case 0x01:
        cl=2;
        addr=(code & 0xF800)+cp[1];
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcpy(pms,"acall ");
        strcat(pms,lbl);
        break;
      case 0x02:
        cl=3;
        addr=(cp[1]<<8)+cp[2];
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcpy(pms,"lcall ");
        strcat(pms,lbl);
        break;
      case 0x03:
        cl=1;
        strcpy(pms,"rrc  a");
        break;
      case 0x04:
        cl=1;
        strcpy(pms,"dec  a");
        break;
      case 0x05:
        cl=2;
        strcpy(pms,"dec  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x06:
        cl=1;
        strcpy(pms,"dec  @r0");
        break;
      case 0x07:
        cl=1;
        strcpy(pms,"dec  @r1");
        break;
      case 0x08:
        cl=1;
        strcpy(pms,"dec  r0");
        break;
      case 0x09:
        cl=1;
        strcpy(pms,"dec  r1");
        break;
      case 0x0A:
        cl=1;
        strcpy(pms,"dec  r2");
        break;
      case 0x0B:
        cl=1;
        strcpy(pms,"dec  r3");
        break;
      case 0x0C:
        cl=1;
        strcpy(pms,"dec  r4");
        break;
      case 0x0D:
        cl=1;
        strcpy(pms,"dec  r5");
        break;
      case 0x0E:
        cl=1;
        strcpy(pms,"dec  r6");
        break;
      case 0x0F:
        cl=1;
        strcpy(pms,"dec  r7");
        break;
    }
    return(cl);
  }
  if(opcode<0x30)
  {
    switch(opcode & 0x0F)
    {
      case 0x00:
        cl=3;
        strcpy(pms,"jb   ");
        GetBitName(vstr,cp[1]);
        strcat(pms, vstr);
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)(cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2])&0xFF);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcat(pms,",");
        strcat(pms,lbl);
        break;
      case 0x01:
        cl=2;
        addr=cp[1]+0x100 +(code & 0xF800);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcpy(pms,"ajmp ");
        strcat(pms,lbl);
        break;
      case 0x02:
        cl=1;
        strcpy(pms,"ret");
        break;
      case 0x03:
        cl=1;
        strcpy(pms,"rl   a");
        break;
      case 0x04:
        cl=2;
        strcpy(pms,"add  a,#");
        sprintf(vstr,"%2.2XH",cp[1]);
        strcat(pms,vstr);
        break;
      case 0x05:
        cl=2;
        strcpy(pms,"add  a,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x06:
        cl=1;
        strcpy(pms,"add  a,@r0");
        break;
      case 0x07:
        cl=1;
        strcpy(pms,"add  a,@r1");
        break;
      case 0x08:
        cl=1;
        strcpy(pms,"add  a,r0");
        break;
      case 0x09:
        cl=1;
        strcpy(pms,"add  a,r1");
        break;
      case 0x0A:
        cl=1;
        strcpy(pms,"add  a,r2");
        break;
      case 0x0B:
        cl=1;
        strcpy(pms,"add  a,r3");
        break;
      case 0x0C:
        cl=1;
        strcpy(pms,"add  a,r4");
        break;
      case 0x0D:
        cl=1;
        strcpy(pms,"add  a,r5");
        break;
      case 0x0E:
        cl=1;
        strcpy(pms,"add  a,r6");
        break;
      case 0x0F:
        cl=1;
        strcpy(pms,"add  a,r7");
        break;
    }
    return(cl);
  }
  if(opcode<0x40)
  {
    switch(opcode & 0x0F)
    {
      case 0x00:
        cl=3;
        strcpy(pms,"jnb  ");
        GetBitName(vstr,cp[1]);
        strcat(pms, vstr);
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)(cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2])&0xFF);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcat(pms,",");
        strcat(pms,lbl);
        break;
      case 0x01:
        cl=2;
        addr=(code & 0xF800)+cp[1]+0x100;
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcpy(pms,"acall ");
        strcat(pms,lbl);
        break;
      case 0x02:
        cl=1;
        strcpy(pms,"reti");
        break;
      case 0x03:
        cl=1;
        strcpy(pms,"rlc  a");
        break;
      case 0x04:
        cl=2;
        strcpy(pms,"addc a,#");
        sprintf(vstr,"%2.2XH",cp[1]);
        strcat(pms,vstr);
        break;
      case 0x05:
        cl=2;
        strcpy(pms,"addc a,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x06:
        cl=1;
        strcpy(pms,"addc a,@r0");
        break;
      case 0x07:
        cl=1;
        strcpy(pms,"addc a,@r1");
        break;
      case 0x08:
        cl=1;
        strcpy(pms,"addc a,r0");
        break;
      case 0x09:
        cl=1;
        strcpy(pms,"addc a,r1");
        break;
      case 0x0A:
        cl=1;
        strcpy(pms,"addc a,r2");
        break;
      case 0x0B:
        cl=1;
        strcpy(pms,"addc a,r3");
        break;
      case 0x0C:
        cl=1;
        strcpy(pms,"addc a,r4");
        break;
      case 0x0D:
        cl=1;
        strcpy(pms,"addc a,r5");
        break;
      case 0x0E:
        cl=1;
        strcpy(pms,"addc a,r6");
        break;
      case 0x0F:
        cl=1;
        strcpy(pms,"addc a,r7");
        break;
    }
    return(cl);
  }
  if(opcode<0x50)
  {
    switch(opcode & 0x0F)
    {
      case 0x00:
        cl=2;
        strcpy(pms,"jc   ");
        GetBitName(vstr,cp[1]);
        if(cp[1]<0x80)
          addr=(LONG)(code+2)+(LONG)((CHAR)cp[1]);
        else
          addr=(LONG)(code+2)-((-(LONG)cp[1])&0xFF);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcat(pms,lbl);
        break;
      case 0x01:
        cl=2;
        addr=cp[1]+0x200 +(code & 0xF800);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcpy(pms,"ajmp ");
        strcat(pms,lbl);
        break;
      case 0x02:
        cl=2;
        strcpy(pms,"orl  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        strcat(pms,",a");
        break;
      case 0x03:
        cl=3;
        strcpy(pms,"orl  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        sprintf(vstr,",#%2.2X",cp[2]);
        strcat(pms,vstr);
        break;
      case 0x04:
        cl=2;
        strcpy(pms,"orl  a,#");
        sprintf(vstr,"%2.2X",cp[1]);
        strcat(pms,vstr);
        break;
      case 0x05:
        cl=2;
        strcpy(pms,"orl  a,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x06:
        cl=1;
        strcpy(pms,"orl  a,@r0");
        break;
      case 0x07:
        cl=1;
        strcpy(pms,"orl  a,@r1");
        break;
      case 0x08:
        cl=1;
        strcpy(pms,"orl  a,r0");
        break;
      case 0x09:
        cl=1;
        strcpy(pms,"orl  a,r1");
        break;
      case 0x0A:
        cl=1;
        strcpy(pms,"orl  a,r2");
        break;
      case 0x0B:
        cl=1;
        strcpy(pms,"orl  a,r3");
        break;
      case 0x0C:
        cl=1;
        strcpy(pms,"orl  a,r4");
        break;
      case 0x0D:
        cl=1;
        strcpy(pms,"orl  a,r5");
        break;
      case 0x0E:
        cl=1;
        strcpy(pms,"orl  a,r6");
        break;
      case 0x0F:
        cl=1;
        strcpy(pms,"orl  a,r7");
        break;
    }
    return(cl);
  }
  if(opcode<0x60)
  {
    switch(opcode & 0x0F)
    {
      case 0x00:
        cl=2;
        strcpy(pms,"jnc  ");
        GetBitName(vstr,cp[1]);
        if(cp[1]<0x80)
          addr=(LONG)(code+2)+(LONG)((CHAR)cp[1]);
        else
          addr=(LONG)(code+2)-((-(LONG)cp[1])&0xFF);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcat(pms,lbl);
        break;
      case 0x01:
        cl=2;
        addr=(code & 0xF800)+cp[1]+0x200;
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcpy(pms,"acall ");
        strcat(pms,lbl);
        break;
      case 0x02:
        cl=2;
        strcpy(pms,"anl  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        strcat(pms,",A");
        break;
      case 0x03:
        cl=3;
        strcpy(pms,"anl  ");
        GetRegName(vstr,cp[1]);
        strcat(pms,vstr);
        sprintf(vstr,",#%2.2X",cp[2]);
        strcat(pms,vstr);
        break;
      case 0x04:
        cl=2;
        sprintf(vstr,"anl  a,#%2.2X",cp[1]);
        strcpy(pms,vstr);
        break;
      case 0x05:
        cl=2;
        strcpy(pms,"anl  a,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x06:
        cl=1;
        strcpy(pms,"anl  a,@r0");
        break;
      case 0x07:
        cl=1;
        strcpy(pms,"anl  a,@r1");
        break;
      case 0x08:
        cl=1;
        strcpy(pms,"anl  a,r0");
        break;
      case 0x09:
        cl=1;
        strcpy(pms,"anl  a,r1");
        break;
      case 0x0A:
        cl=1;
        strcpy(pms,"anl  a,r2");
        break;
      case 0x0B:
        cl=1;
        strcpy(pms,"anl  a,r3");
        break;
      case 0x0C:
        cl=1;
        strcpy(pms,"anl  a,r4");
        break;
      case 0x0D:
        cl=1;
        strcpy(pms,"anl  a,r5");
        break;
      case 0x0E:
        cl=1;
        strcpy(pms,"anl  a,r6");
        break;
      case 0x0F:
        cl=1;
        strcpy(pms,"anl  a,r7");
        break;
    }
    return(cl);
  }
  if(opcode<0x70)
  {
    switch(opcode & 0x0F)
    {
      case 0x00:
        cl=2;
        strcpy(pms,"jz   ");
        GetBitName(vstr,cp[1]);
        if(cp[1]<0x80)
          addr=(LONG)(code+2)+(LONG)((CHAR)cp[1]);
        else
          addr=(LONG)(code+2)-((-(LONG)cp[1])&0xFF);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcat(pms,lbl);
        break;
      case 0x01:
        cl=2;
        addr=cp[1]+0x300 +(code & 0xF800);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcpy(pms,"ajmp ");
        strcat(pms,lbl);
        break;
      case 0x02:
        cl=2;
        strcpy(pms,"xrl  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        strcat(pms,",a");
        break;
      case 0x03:
        cl=3;
        strcpy(pms,"xrl  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        sprintf(vstr,",#%2.2X",cp[2]);
        strcat(pms,vstr);
        break;
      case 0x04:
        cl=2;
        strcpy(pms,"xrl  a,#");
        sprintf(vstr,"%2.2X",cp[1]);
        strcat(pms,vstr);
        break;
      case 0x05:
        cl=2;
        strcpy(pms,"xrl  a,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x06:
        cl=1;
        strcpy(pms,"xrl  a,@r0");
        break;
      case 0x07:
        cl=1;
        strcpy(pms,"xrl  a,@r1");
        break;
      case 0x08:
        cl=1;
        strcpy(pms,"xrl  a,r0");
        break;
      case 0x09:
        cl=1;
        strcpy(pms,"xrl  a,r1");
        break;
      case 0x0A:
        cl=1;
        strcpy(pms,"xrl  a,r2");
        break;
      case 0x0B:
        cl=1;
        strcpy(pms,"xrl  a,r3");
        break;
      case 0x0C:
        cl=1;
        strcpy(pms,"xrl  a,r4");
        break;
      case 0x0D:
        cl=1;
        strcpy(pms,"xrl  a,r5");
        break;
      case 0x0E:
        cl=1;
        strcpy(pms,"xrl  a,r6");
        break;
      case 0x0F:
        cl=1;
        strcpy(pms,"xrl  a,r7");
        break;
    }
    return(cl);
  }
  if(opcode<0x80)
  {
    switch(opcode & 0x0F)
    {
      case 0x00:
        cl=2;
        strcpy(pms,"jnz  ");
        GetBitName(vstr,cp[1]);
        if(cp[1]<0x80)
          addr=(LONG)(code+2)+(LONG)((CHAR)cp[1]);
        else
          addr=(LONG)(code+2)-((-(LONG)cp[1])&0xFF);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcat(pms,lbl);
        break;
      case 0x01:
        cl=2;
        addr=(code & 0xF800)+cp[1]+0x300;
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcpy(pms,"acall ");
        strcat(pms,lbl);
        break;
      case 0x02:
        cl=2;
        strcpy(pms,"orl  C,");
        GetBitName(vstr,cp[1]);
        strcat(pms,vstr);
        break;
      case 0x03:
        cl=1;
        strcpy(pms,"jmp  @a+dptr");
        break;
      case 0x04:
        cl=2;
        sprintf(vstr,"mov  a,#%2.2XH",cp[1]);
        strcpy(pms,vstr);
        break;
      case 0x05:
        cl=3;
        strcpy(pms,"mov  ");
        GetRegName(vstr,cp[1],hMod),
        strcat(pms,vstr);
        sprintf(vstr,",#%2.2XH",cp[2]);
        strcat(pms,vstr);
        break;
      case 0x06:
        cl=2;
        sprintf(vstr,"mov  @r0,#%2.2XH",cp[1]);
        strcpy(pms,vstr);
        break;
      case 0x07:
        cl=2;
        sprintf(vstr,"mov  @r1,#%2.2XH",cp[1]);
        strcpy(pms,vstr);
        break;
      case 0x08:
        cl=2;
        sprintf(vstr,"mov  r0,#%2.2XH",cp[1]);
        strcpy(pms,vstr);
        break;
      case 0x09:
        cl=2;
        sprintf(vstr,"mov  r1,#%2.2XH",cp[1]);
        strcpy(pms,vstr);
        break;
      case 0x0A:
        cl=2;
        sprintf(vstr,"mov  r2,#%2.2XH",cp[1]);
        strcpy(pms,vstr);
        break;
      case 0x0B:
        cl=2;
        sprintf(vstr,"mov  r3,#%2.2XH",cp[1]);
        strcpy(pms,vstr);
        break;
      case 0x0C:
        cl=2;
        sprintf(vstr,"mov  r4,#%2.2XH",cp[1]);
        strcpy(pms,vstr);
        break;
      case 0x0D:
        cl=2;
        sprintf(vstr,"mov  r5,#%2.2XH",cp[1]);
        strcpy(pms,vstr);
        break;
      case 0x0E:
        cl=2;
        sprintf(vstr,"mov  r6,#%2.2XH",cp[1]);
        strcpy(pms,vstr);
        break;
      case 0x0F:
        cl=2;
        sprintf(vstr,"mov  r7,#%2.2XH",cp[1]);
        strcpy(pms,vstr);
        break;
    }
    return(cl);
  }
  if(opcode<0x90)
  {
    switch(opcode & 0x0F)
    {
      case 0x00:
        cl=2;
        strcpy(pms,"sjmp ");
        if(cp[1]<0x80)
          addr=(LONG)(code+2)+(LONG)((CHAR)cp[1]);
        else
          addr=(LONG)(code+2)-((-(LONG)cp[1])&0xFF);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcat(pms,lbl);
        break;
      case 0x01:
        cl=2;
        addr=cp[1]+0x400 +(code & 0xF800);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcpy(pms,"ajmp ");
        strcat(pms,lbl);
        break;
      case 0x02:
        cl=2;
        strcpy(pms,"anl  C,");
        GetBitName(vstr,cp[1]);
        strcat(pms,vstr);
        break;
      case 0x03:
        cl=1;
        strcpy(pms,"movc a,@a+pc");
        break;
      case 0x04:
        cl=1;
        strcpy(pms,"div  ab");
        break;
      case 0x05:
        cl=3;
        strcpy(pms,"mov  ");
        GetRegName(vstr,cp[2],hMod);
        strcat(pms,vstr);
        strcat(pms,",");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x06:
        cl=2;
        strcpy(pms,"mov  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        strcat(pms,",@r0");
        break;
      case 0x07:
        cl=2;
        strcpy(pms,"mov  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        strcat(pms,",@r1");
        break;
      case 0x08:
        cl=2;
        strcpy(pms,"mov  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        strcat(pms,",r0");
        break;
      case 0x09:
        cl=2;
        strcpy(pms,"mov  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        strcat(pms,",r1");
        break;
      case 0x0A:
        cl=2;
        strcpy(pms,"mov  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        strcat(pms,",r2");
        break;
      case 0x0B:
        cl=2;
        strcpy(pms,"mov  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        strcat(pms,",r3");
        break;
      case 0x0C:
        cl=2;
        strcpy(pms,"mov  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        strcat(pms,",r4");
        break;
      case 0x0D:
        cl=2;
        strcpy(pms,"mov  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        strcat(pms,",r5");
        break;
      case 0x0E:
        cl=2;
        strcpy(pms,"mov  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        strcat(pms,",r6");
        break;
      case 0x0F:
        cl=2;
        strcpy(pms,"mov  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        strcat(pms,",r7");
        break;
    }
    return(cl);
  }
  if(opcode<0xA0)
  {
    switch(opcode & 0x0F)
    {
      case 0x00:
        cl=3;
        sprintf(vstr,"mov  dptr,#%4.4XH",(USHORT)((cp[1]<<8)+cp[2]));
        strcpy(pms,vstr);
        break;
      case 0x01:
        cl=2;
        addr=(code & 0xF800)+cp[1]+0x400;
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcpy(pms,"acall ");
        strcat(pms,lbl);
        break;
      case 0x02:
        cl=2;
        strcpy(pms,"mov  ");
        GetBitName(vstr,cp[1]);
        strcat(pms,vstr);
        strcat(pms,",C");
        break;
      case 0x03:
        cl=1;
        strcpy(pms,"movc a,@a+dptr");
        break;
      case 0x04:
        cl=2;
        sprintf(vstr,"subb  a,#%2.2XH",cp[1]);
        strcpy(pms,vstr);
        break;
      case 0x05:
        cl=2;
        strcpy(pms,"subb a,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x06:
        cl=1;
        strcpy(pms,"subb a,@r0");
        break;
      case 0x07:
        cl=1;
        strcpy(pms,"subb a,@r1");
        break;
      case 0x08:
        cl=1;
        strcpy(pms,"subb a,r0");
        break;
      case 0x09:
        cl=1;
        strcpy(pms,"subb a,r1");
        break;
      case 0x0A:
        cl=1;
        strcpy(pms,"subb a,r2");
        break;
      case 0x0B:
        cl=1;
        strcpy(pms,"subb a,r3");
        break;
      case 0x0C:
        cl=1;
        strcpy(pms,"subb a,r4");
        break;
      case 0x0D:
        cl=1;
        strcpy(pms,"subb a,r5");
        break;
      case 0x0E:
        cl=1;
        strcpy(pms,"subb a,r6");
        break;
      case 0x0F:
        cl=1;
        strcpy(pms,"subb a,r7");
        break;
    }
    return(cl);
  }
  if(opcode<0xB0)
  {
    switch(opcode & 0x0F)
    {
      case 0x00:
        cl=2;
        sprintf(vstr,"orl  C,%2.2XH",cp[1]);
        strcpy(pms,vstr);
        break;
      case 0x01:
        cl=2;
        addr=cp[1]+0x500 +(code & 0xF800);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcpy(pms,"ajmp ");
        strcat(pms,lbl);
        break;
      case 0x02:
        cl=2;
        strcpy(pms,"mov  C,");
        GetBitName(vstr,cp[1]);
        strcat(pms,vstr);
        break;
      case 0x03:
        cl=1;
        strcpy(pms,"inc  dptr");
        break;
      case 0x04:
        cl=1;
        strcpy(pms,"mul  ab");
        break;
      case 0x05:
        cl=1;
        strcpy(pms,"DB A5H");
        break;
      case 0x06:
        cl=2;
        strcpy(pms,"mov  @r0,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x07:
        cl=2;
        strcpy(pms,"mov  @r1,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x08:
        cl=2;
        strcpy(pms,"mov  r0,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x09:
        cl=2;
        strcpy(pms,"mov  r1,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x0A:
        cl=2;
        strcpy(pms,"mov  r2,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x0B:
        cl=2;
        strcpy(pms,"mov  r3,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x0C:
        cl=2;
        strcpy(pms,"mov  r4,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x0D:
        cl=2;
        strcpy(pms,"mov  r5,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x0E:
        cl=2;
        strcpy(pms,"mov  r6,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x0F:
        cl=2;
        strcpy(pms,"mov  r7,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
    }
    return(cl);
  }
  if(opcode<0xC0)
  {
    switch(opcode & 0x0F)
    {
      case 0x00:
        cl=2;
        sprintf(vstr,"anl  C,/%2.2XH",cp[1]);
        strcpy(pms,vstr);
        break;
      case 0x01:
        cl=2;
        addr=(code & 0xF800)+cp[1]+0x500;
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcpy(pms,"acall ");
        strcat(pms,lbl);
        break;
      case 0x02:
        cl=2;
        strcpy(pms,"cpl  ");
        GetBitName(vstr,cp[1]);
        strcat(pms,vstr);
        break;
      case 0x03:
        cl=1;
        strcpy(pms,"cpl  C");
        break;
      case 0x04:
        cl=3;
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)((CHAR)cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2])&0xFF);
        sprintf(vstr,"cjne a,#%2.2XH,%4.4XH",cp[1],addr);
        strcpy(pms,vstr);
        break;
      case 0x05:
        cl=3;
        strcpy(pms,"cjne a,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)((CHAR)cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2])&0xFF);
        sprintf(vstr,"#%4.4XH",addr);
        strcat(pms,vstr);
        break;
      case 0x06:
        cl=3;
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)((CHAR)cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2])&0xFF);
        sprintf(vstr,"cjne @r0,#%2.2XH,%4.4XH",cp[1],addr);
        strcpy(pms,vstr);
        break;
      case 0x07:
        cl=3;
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)((CHAR)cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2])&0xFF);
        sprintf(vstr,"cjne @r1,#%2.2XH,%4.4XH",cp[1],addr);
        strcpy(pms,vstr);
        break;
      case 0x08:
        cl=3;
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)((CHAR)cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2])&0xFF);
        sprintf(vstr,"cjne r0,#%2.2XH,%4.4XH",cp[1],addr);
        strcpy(pms,vstr);
        break;
      case 0x09:
        cl=3;
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)((CHAR)cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2])&0xFF);
        sprintf(vstr,"cjne r1,#%2.2XH,%4.4XH",cp[1],addr);
        strcpy(pms,vstr);
        break;
      case 0x0A:
        cl=3;
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)((CHAR)cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2])&0xFF);
        sprintf(vstr,"cjne r2,#%2.2XH,%4.4XH",cp[1],addr);
        strcpy(pms,vstr);
        break;
      case 0x0B:
        cl=3;
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)((CHAR)cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2])&0xFF);
        sprintf(vstr,"cjne r3,#%2.2XH,%4.4XH",cp[1],addr);
        strcpy(pms,vstr);
        break;
      case 0x0C:
        cl=3;
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)((CHAR)cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2])&0xFF);
        sprintf(vstr,"cjne r4,#%2.2XH,%4.4XH",cp[1],addr);
        strcpy(pms,vstr);
        break;
      case 0x0D:
        cl=3;
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)((CHAR)cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2])&0xFF);
        sprintf(vstr,"cjne r5,#%2.2XH,%4.4XH",cp[1],addr);
        strcpy(pms,vstr);
        break;
      case 0x0E:
        cl=3;
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)((CHAR)cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2])&0xFF);
        sprintf(vstr,"cjne r6,#%2.2XH,%4.4XH",cp[1],addr);
        strcpy(pms,vstr);
        break;
      case 0x0F:
        cl=3;
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)((CHAR)cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2])&0xFF);
        sprintf(vstr,"cjne r7,#%2.2XH,%4.4XH",cp[1],addr);
        strcpy(pms,vstr);
        break;
    }
    return(cl);
  }
  if(opcode<0xD0)
  {
    switch(opcode & 0x0F)
    {
      case 0x00:
        cl=2;
        strcpy(pms,"push  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x01:
        cl=2;
        addr=cp[1]+0x600 +(code & 0xF800);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcpy(pms,"ajmp ");
        strcat(pms,lbl);
        break;
      case 0x02:
        cl=2;
        strcpy(pms,"clr  ");
        GetBitName(vstr,cp[1]);
        strcat(pms,vstr);
        break;
      case 0x03:
        cl=1;
        strcpy(pms,"clr  C");
        break;
      case 0x04:
        cl=1;
        strcpy(pms,"swap a");
        break;
      case 0x05:
        cl=2;
        strcpy(pms,"xch  a,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x06:
        cl=1;
        strcpy(pms,"xch  a,@r0");
        break;
      case 0x07:
        cl=1;
        strcpy(pms,"xch  a,@r1");
        break;
      case 0x08:
        cl=1;
        strcpy(pms,"xch  a,r0");
        break;
      case 0x09:
        cl=1;
        strcpy(pms,"xch  a,r1");
        break;
      case 0x0A:
        cl=1;
        strcpy(pms,"xch  a,r2");
        break;
      case 0x0B:
        cl=1;
        strcpy(pms,"xch  a,r3");
        break;
      case 0x0C:
        cl=1;
        strcpy(pms,"xch  a,r4");
        break;
      case 0x0D:
        cl=1;
        strcpy(pms,"xch  a,r5");
        break;
      case 0x0E:
        cl=1;
        strcpy(pms,"xch  a,r6");
        break;
      case 0x0F:
        cl=1;
        strcpy(pms,"xch  a,r7");
        break;
    }
    return(cl);
  }
  if(opcode<0xE0)
  {
    switch(opcode & 0x0F)
    {
      case 0x00:
        cl=2;
        strcpy(pms,"pop  ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x01:
        cl=2;
        addr=(code & 0xF800)+cp[1]+0x600;
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcpy(pms,"acall ");
        strcat(pms,lbl);
        break;
      case 0x02:
        cl=2;
        strcpy(pms,"setb ");
        GetBitName(vstr,cp[1]);
        strcat(pms,vstr);
        break;
      case 0x03:
        cl=1;
        strcpy(pms,"setb C");
        break;
      case 0x04:
        cl=1;
        strcpy(pms,"da   a");
        break;
      case 0x05:
        cl=3;
        strcpy(pms,"djnz ");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        if(cp[2]<0x80)
          addr=(LONG)(code+3)+(LONG)((CHAR)cp[2]);
        else
          addr=(LONG)(code+3)-((-(LONG)cp[2])&0xFF);
        ::FindLabel(addr,lbl,CODEMEM,PUBSYM|LOCSYM,4);
        strcat(pms,",");
        strcat(pms,lbl);
        break;
      case 0x06:
        cl=1;
        strcpy(pms,"xchd a,@r0");
        break;
      case 0x07:
        cl=1;
        strcpy(pms,"xchd a,@r1");
        break;
      case 0x08:
        strcpy(pms,"djnz r0,");
        goto r1;
      case 0x09:
        strcpy(pms,"djnz r1,");
        goto r1;
      case 0x0A:
        strcpy(pms,"djnz r2,");
        goto r1;
      case 0x0B:
        strcpy(pms,"djnz r3,");
        goto r1;
      case 0x0C:
        strcpy(pms,"djnz r4,");
        goto r1;
      case 0x0D:
        strcpy(pms,"djnz r5,");
        goto r1;
      case 0x0E:
        strcpy(pms,"djnz r6,");
        goto r1;
      case 0x0F:
        strcpy(pms,"djnz r7,");
r1:
        cl=2;
        if(cp[2]<0x80)
          addr=(LONG)(code+2)+(LONG)((CHAR)cp[1]);
        else
          addr=(LONG)(code+2)-((-(LONG)cp[1])&0xFF);
        ::FindLabel(addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcat(pms,lbl);
        break;
    }

    return(cl);
  }
  if(opcode<0xF0)
  {
    switch(opcode & 0x0F)
    {
      case 0x00:
        cl=1;
        strcpy(pms,"movx a,@dptr");
        break;
      case 0x01:
        cl=2;
        addr=cp[1]+0x700 +(code & 0xF800);
        ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
        strcpy(pms,"ajmp ");
        strcat(pms,lbl);
        break;
      case 0x02:
        cl=1;
        strcpy(pms,"movx a,@r0");
        break;
      case 0x03:
        cl=1;
        strcpy(pms,"movx a,@r1");
        break;
      case 0x04:
        cl=1;
        strcpy(pms,"clr  a");
        break;
      case 0x05:
        cl=2;
        strcpy(pms,"mov  a,");
        GetRegName(vstr,cp[1],hMod);
        strcat(pms,vstr);
        break;
      case 0x06:
        cl=1;
        strcpy(pms,"mov  a,@r0");
        break;
      case 0x07:
        cl=1;
        strcpy(pms,"mov  a,@r1");
        break;
      case 0x08:
        cl=1;
        strcpy(pms,"mov  a,r0");
        break;
      case 0x09:
        cl=1;
        strcpy(pms,"mov  a,r1");
        break;
      case 0x0A:
        cl=1;
        strcpy(pms,"mov  a,r2");
        break;
      case 0x0B:
        cl=1;
        strcpy(pms,"mov  a,r3");
        break;
      case 0x0C:
        cl=1;
        strcpy(pms,"mov  a,r4");
        break;
      case 0x0D:
        cl=1;
        strcpy(pms,"mov  a,r5");
        break;
      case 0x0E:
        cl=1;
        strcpy(pms,"mov  a,r6");
        break;
      case 0x0F:
        cl=1;
        strcpy(pms,"mov  a,r7");
        break;
    }
    return(cl);
  }
  switch(opcode & 0x0F)
  {
    case 0x00:
      cl=1;
      strcpy(pms,"movx @dptr,a");
      break;
    case 0x01:
      cl=2;
      addr=(code & 0xF800)+cp[1]+0x700;
      ::FindLabel((USHORT)addr,lbl,CODEMEM,LOCSYM|PUBSYM,4);
      strcpy(pms,"acall ");
      strcat(pms,lbl);
      break;
    case 0x02:
      cl=1;
      strcpy(pms,"movx @r0,a");
      break;
    case 0x03:
      cl=1;
      strcpy(pms,"movx @r1,a");
      break;
    case 0x04:
      cl=1;
      strcpy(pms,"cpl  a");
      break;
    case 0x05:
      cl=2;
      strcpy(pms,"mov  ");
      GetRegName(vstr,cp[1],hMod);
      strcat(pms,vstr);
      strcat(pms,",a");
      break;
    case 0x06:
      cl=1;
      strcpy(pms,"mov  @r0,a");
      break;
    case 0x07:
      cl=1;
      strcpy(pms,"mov  @r1,a");
      break;
    case 0x08:
      cl=1;
      strcpy(pms,"mov  r0,a");
      break;
    case 0x09:
      cl=1;
      strcpy(pms,"mov  r1,a");
      break;
    case 0x0A:
      cl=1;
      strcpy(pms,"mov  r2,a");
      break;
    case 0x0B:
      cl=1;
      strcpy(pms,"mov  r3,a");
      break;
    case 0x0C:
      cl=1;
      strcpy(pms,"mov  r4,a");
      break;
    case 0x0D:
      cl=1;
      strcpy(pms,"mov  r5,a");
      break;
    case 0x0E:
      cl=1;
      strcpy(pms,"mov  r6,a");
      break;
    case 0x0F:
      cl=1;
      strcpy(pms,"mov  r7,a");
      break;
  }
  return(cl);
}


const char CProc51::SReg510[128][6]=
{
  "P0","SP","DPL","DPH","84H","85H","86H","PCON","TCON","TMOD","TL0","TL1","TH0","TH1","8EH","8FH",
  "P1","91H","92H","93H","94H","95H","96H","97H","SCON","SBUF","9AH","9BH","9CH","9DH","9EH","9FH",
  "P2","A1H","A2H","A3H","A4H","A5H","A6H","A7H","IE","A9H","AAH","ABH","ACH","ADH","AEH","AFH",
  "P3","B1H","B2H","B3H","B4H","B5H","B6H","B7H","IP","B9H","BAH","BBH","BCH","BDH","BEH","BFH",
  "C0H","C1H","C2H","C3H","C4H","C5H","C6H","C7H","T2CON","C9H","CAH","CBH","TL2","TH2","CEH","CFH",
  "PSW","D1H","D2H","D3H","D4H","D5H","D6H","D7H","D8H","D9H","DAH","DBH","DCH","DDH","DEH","DFH",
  "ACC","E1H","E2H","E3H","E4H","E5H","E6H","E7H","E8H","E9H","EAH","EBH","ECH","EDH","EEH","EFH",
  "B","F1H","F2H","F3H","F4H","F5H","F6H","F7H","F8H","F9H","FAH","FBH","FCH","FDH","FEH","FFH"
};

const char CProc51::SBit510[128][6]=
{
  "P0.0","P0.1","P0.2","P0.3","P0.4","P0.5","P0.6","P0.7","IT0","IE0","IT1","IE1","TR0","TF0","TR1","TF1",
  "P1.0","P1.1","P1.2","P1.3","P1.4","P1.5","P1.6","P1.7","RI","TI","RB8","TB8","REN","SM2","SM1","SM0",
  "P2.0","P2.1","P2.2","P2.3","P2.4","P2.5","P2.6","P2.7","EX0","ET0","EX1","ET1","ES","ADH","AEH","EA",
  "RXD","TXD","INT0","INT1","T0","T1","WR","RD","PX0","PT0","PX1","PT1","PS","BDH","BEH","BFH",
  "C0H.0","C0H.1","C0H.2","C0H.3","C0H.4","C0H.5","C0H.6","C0H.7","C8H.0","C8H.1","C8H.2","C8H.3","C8H.4","C8H.5","C8H.6","C8H.7",
  "P","F1","OV","RS0","RS1","F0","AC","CY","D8H.0","D8H.1","D8H.2","D8H.3","D8H.4","D8H.5","D8H.6","D8H.7",
  "ACC.0","ACC.1","ACC.2","ACC.3","ACC.4","ACC.5","ACC.6","ACC.7","P4.0","P4.1","P4.2","P4.3","P4.4","P4.5","P4.6","P4.7",
  "B.0H","B.1","B.2","B.3","B.4","B.5","B.6","B.7","P5.0","P5.1","P5.2","P5.3","P5.4","P5.5","P5.6","P5.7"
};


void CProc51::GetRegName( char* cp, int regno,HANDLE hMod )
{
  char lbl[200];

  if(hMod==0)
  {
    if(regno < 0x80)
      sprintf(cp,"%2.2XH",regno);
    else
      strcpy(cp,SReg510[regno-0x80]);
  }
  else
  {
    if(::FindLabel(regno,lbl,DATAMEM,LOCSYM,2))
      strcpy(cp,lbl);
    else if(regno >= 0x80)
      strcpy(cp,SReg510[regno-0x80]);
    else
      sprintf(cp,"%2.2XH",regno);
  }
}


void CProc51::GetBitName(char* cp,int bitno)
{
  int bitpos;

  if(bitno < 0x80)
  {
    bitpos=bitno%8;
    bitno=0x20+bitno/8;
    sprintf(cp,"%2.2XH.%d",bitno,bitpos);
  }
  else
    strcpy(cp,SBit510[bitno-0x80]);
}

void CProc51::SetBit(int bitno,BOOL bitstate)
{
  USHORT i,n,m;
  UCHAR a;

  if(bitno<128)
  {
    i=0x20 + bitno/8;
    n=bitno%8;
    a=1;
    while(n--)
      a=a<<1;
  }
  else
  {
    bitno-=0x80;
    n=bitno / 8;
    m=bitno % 8;
    i=0x80 + 8*n;
    a=1;
    while(m--)
      a=a<<1;
  }
  if(bitstate)
  {
    if(!accessbkpts)
    {
      datamem[i] |= a;
      if(i==AKKU)
        SetParity(datamem[AKKU]);
    }
    else
    {
      SetDataMem(i,datamem[i]|a);
    }
  }
  else
  {
    if(!accessbkpts)
    {
      datamem[i] &= ~a;
      if(i==AKKU)
        SetParity(datamem[AKKU]);
    }
    else
    {
      SetDataMem(i,datamem[i]& ~a);
    }
  }
  if(i==S0BUF || i==SCON)
  {
    if(i==S0BUF)
      IsSerChar=TRUE;
    else if(datamem[0x98]&2 )  //muss SCON sein und das TI-Bit
      GenerateTXInt();
  }
}


BOOL  CProc51::GetBitState(int bitno,BOOL clr)
{
  BOOL ret;
  USHORT i,n,m;
  UCHAR a;

  if(bitno<128)
  {
    i=0x20 + bitno/8;
    n=bitno%8;
    a=1;
    while(n--)
      a=a<<1;
    if(datamem[i] & a)
      ret=TRUE;
    else
      ret=FALSE;
  }
  else
  {
    bitno-=0x80;
    n=bitno / 8;
    m=bitno % 8;
    i=0x80 + 8*n;
    a=1;
    while(m--)
      a=a<<1;
    if(datamem[i] & a)
      ret=TRUE;
    else
      ret=FALSE;
  }
  if(clr==TRUE)
    datamem[i] &= ~a;  //Bit löschen
  return(ret);
}

// UP  SETPARITY (p)
void  CProc51::SetParity( int akku )
{
  USHORT n,i;

  n=1;
  i=0;
  while(n<0x100)
  {
    if(n & akku)
      i++;
    n=n<<1;
  }
  if(i & 0x01)    //ungerade Parität
    datamem[PSW] |= PARITY;
  else
    datamem[PSW] &=~ PARITY;
}

int CProc51::ExecNextCmd( )
{
  USHORT cmd;
  USHORT n,i,hPC;
  UCHAR a;
  SHORT rel;
  BOOL b;
  ULONG retAddr,calladdr;
  ULONG lastCycleCnt;

  retval=0;
  lastCycleCnt=CyclCnt;
  if(accessbkpts)
  {

    retval=ExecNextCmd1();
    HandleTimers((CyclCnt-lastCycleCnt)/12);
    return retval;
  }
  pc=(USHORT)PC;

iw1:
  if((datamem[0xA8] & 0x80)
     && intReq
     && (actInt^intReq) & (enabledInt=datamem[0xA8]&0x7F | (datamem[0xA8]&0x7F)*256)
     && (((actInt^intReq) & enabledInt)<actInt
     || !actInt))
  {  //Interruptanmeldung die noch nicht bearbeitet wird und höher priorisiert ist
    actInt=0x0001;
    while(!(actInt & intReq))  //den höchst prioren Interrupt raussuchen
      actInt*=2;

    CyclCnt+=36; //das muss noch überprüft werden
    retAddr=PC;
    calladdr=PC;
    retval=SetSPT(+1);
    i=datamem[SPT];  //aktueller Stackzeiger
    if(i>0x7F)
    {
      idatamem[i-0x80]=(UCHAR)(PC & 0xFF);  //Low ablegen
      i++;
    }
    else
      datamem[i++]=(UCHAR)(PC & 0xFF);  //Low ablegen
    retval|=SetSPT(+1);
    if(i>0x7F)
      idatamem[i-0x80]=(UCHAR)(PC>>8);  //High ablegen
    else
      datamem[i]=(UCHAR)(PC>>8);  //High ablegen
    switch(actInt)
    {
      case 0x0001:
      case 0x0100: PC=0x0003;  //extInt0
        if(datamem[TCON] & 0x01)
          datamem[TCON] &= ~0x02; //Anmeldebit rücksetzen wenn FlankenInt
        break;
      case 0x0002:
      case 0x0200: PC=0x000B;  //timer0
        datamem[TCON] &= ~0x20;  //Anmeldebit rücksetzen
        break;
      case 0x0004:
      case 0x0400: PC=0x0013;  //extInt1
        if(datamem[TCON] & 0x04)
          datamem[TCON] &= ~0x08; //Anmeldebit rücksetzen wenn FlankenInt
        break;
      case 0x0008:
      case 0x0800: PC=0x001B;  //timer1
        datamem[TCON] &= ~0x80;     //Anmeldebit rücksetzen
        break;
      case 0x0010:
      case 0x1000: PC=0x0023;  //Serial
        break;
      case 0x0020:
      case 0x2000: PC=0x002B;
        break;
      case 0x0040:
      case 0x4000: PC=0x0033;
        break;
      case 0x0080:
      case 0x8000: PC=0x003B;
        break;
    }
    ::AddToCallStackWnd(calladdr,retAddr);
    intReq &= ~actInt; //den aktuellen Interrupt löschen
    actInt=0;
    HandleTimers(3);
    return retval;
  }

  cmd=codemem[PC];
  if(!(cmd &0xFF00))  //kein Breakpoint
  {
exstart:
    PC++;

    if(cmd<0x10)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //NOP
          CyclCnt+=12;
          break;
        case 0x01: //AJMP Page0
          CyclCnt+=24;
          PC=(PC & 0xF800) + (codemem[PC] & 0xFF);
          break;
        case 0x02: //LJMP adr16
          CyclCnt+=24;
          PC=codemem[PC]*256 + (codemem[PC+1]& 0xFF);
          break;
        case 0x03: //RR A
          CyclCnt+=12;
          a=datamem[AKKU];
          if(a &0x01)
            datamem[AKKU]=(a>>1) | 0x80;
          else
            datamem[AKKU]=a>>1;
          break;
        case 0x04: //INC A
          CyclCnt+=12;
          datamem[AKKU]++;
          a=datamem[AKKU];
          SetParity(a);
          break;
        case 0x05: //INC dadr
          CyclCnt+=12;
          n=(UCHAR)codemem[PC++];
          datamem[n]++;
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          if(n==AKKU)
            SetParity(datamem[AKKU]);
          break;
        case 0x06: //INC @R0
          i=datamem[PSW] & REGBANK; //adresse von R0
          goto w11;
        case 0x07: //INC @R1
          i=(datamem[PSW] & REGBANK) +1;
w11:
          CyclCnt+=12;
          n=datamem[i];             //Ri
          if(n>0x7F)
            idatamem[n-0x80]++;
          else
            datamem[n]++;
          break;
        case 0x08: //INC R0
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          datamem[n]++;
          break;
        case 0x09: //INC R1
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          datamem[n+1]++;
          break;
        case 0x0A: //INC R2
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          datamem[n+2]++;
          break;
        case 0x0B: //INC R3
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          datamem[n+3]++;
          break;
        case 0x0C: //INC R4
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          datamem[n+4]++;
          break;
        case 0x0D: //INC R5
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          datamem[n+5]++;
          break;
        case 0x0E: //INC R6
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          datamem[n+6]++;
          break;
        case 0x0F: //INC R7
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          datamem[n+7]++;
          break;
      }
      goto ex;
    }
    if(cmd<0x20)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //JBC badr,rel
          CyclCnt+=24;
          b=GetBitState((UCHAR)codemem[PC++],TRUE);
          if(b)
          {
            SetParity(datamem[AKKU]);
            rel=(SHORT)((CHAR)(codemem[PC]))+1;
            PC=PC+rel;
          }
          else
            PC++;
          break;
        case 0x01: //ACALL page0
          CyclCnt+=24;
          calladdr=PC-1;
          retAddr=PC+1;
          hPC=PC+1;
          retval=SetSPT(+1);
          i=datamem[SPT];
          if(i>0x7F)
          {
            idatamem[i-0x80]=(UCHAR)(hPC & 0xFF);  //Low ablegen
            i++;
          }
          else
            datamem[i++]=(UCHAR)(hPC & 0xFF);  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            idatamem[i-0x80]=(UCHAR)(hPC>>8);  //High ablegen
          else
            datamem[i]=(UCHAR)(hPC>>8);  //High ablegen

          n=(UCHAR)codemem[PC];
          PC&=0xF800;
          PC|=(USHORT)(n & 0xFF);
          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x02: //LCALL adr16
          CyclCnt+=24;
          retAddr=PC+2;
          calladdr=PC-1;
          hPC=PC+2;
          retval=SetSPT(+1);
          i=datamem[SPT];  //aktueller Stackzeiger
          if(i>0x7F)
          {
            idatamem[i-0x80]=(UCHAR)(hPC & 0xFF);  //Low ablegen
            i++;
          }
          else
            datamem[i++]=(UCHAR)(hPC & 0xFF);  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            idatamem[i-0x80]=(UCHAR)(hPC>>8);  //High ablegen
          else
            datamem[i]=(UCHAR)(hPC>>8);  //High ablegen
          PC=(USHORT)((codemem[PC]&0xFF)<<8) | (codemem[PC+1]&0xFF);

          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x03: //RRC A
          CyclCnt+=12;
          a=datamem[AKKU];
          n=datamem[PSW];
          a=a>>1;
          if(n & CARRY)
            a |= 0x80;

          if(datamem[AKKU] & 1)
            datamem[PSW] |= CARRY;   //carry setzen
          else
            datamem[PSW] &=~CARRY;   //carry löschen
          datamem[AKKU]=a;
          SetParity(a);
          break;
        case 0x04: //DEC A
          CyclCnt+=12;
          datamem[AKKU]--;
          SetParity(datamem[AKKU]);
          break;
        case 0x05: //DEC dadr
          CyclCnt+=24;
          n=(UCHAR)codemem[PC++];
          datamem[n]--;
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          if(n==AKKU)
            SetParity(datamem[AKKU]);
          break;
        case 0x06: //DEC @R0
          i=datamem[PSW] & REGBANK;
          goto w12;
        case 0x07: //DEC @R1
          i=(datamem[PSW] & REGBANK)+1;
w12:
          CyclCnt+=12;
          n=datamem[i];
          if(n>0x7F)
            idatamem[n-0x80]--;
          else
            datamem[n]--;
          break;
        case 0x08: //DEC R0
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          datamem[i]--;
          break;
        case 0x09: //DEC R1
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          datamem[i+1]--;
          break;
        case 0x0A: //DEC R2
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          datamem[i+2]--;
          break;
        case 0x0B: //DEC R3
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          datamem[i+3]--;
          break;
        case 0x0C: //DEC R4
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          datamem[i+4]--;
          break;
        case 0x0D: //DEC R5
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          datamem[i+5]--;
          break;
        case 0x0E: //DEC R6
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          datamem[i+6]--;
          break;
        case 0x0F: //DEC R7
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          datamem[i+7]--;
          break;
      }
      goto ex;
    }
    if(cmd<0x30)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //JB badr,rel
          CyclCnt+=24;
          b=GetBitState((UCHAR)codemem[PC++],FALSE);
          if(b)
          {
            rel=(SHORT)((CHAR)(codemem[PC]))+1;
            PC=PC+rel;
          }
          else
            PC++;
          break;
        case 0x01: //AJMP page1
          CyclCnt+=24;
          PC=(PC & 0xF800) + (codemem[PC]&0xFF)+0x100;
          break;
        case 0x02: //RET
          CyclCnt+=24;
          //high zurückholen
          n=datamem[SPT];
          if(n<0x80)
            PC=(USHORT)(datamem[n]<<8);
          else
            PC=(USHORT)(idatamem[n-0x80]<<8);
          retval=SetSPT(-1);
          //low zurückholen
          n=datamem[SPT];
          if(n<0x80)
            PC+=(USHORT)datamem[n];
          else
            PC+=(USHORT)idatamem[n-0x80];
          retval|=SetSPT(-1);
          ::RemoveFromCallStackWnd(PC);
          break;
        case 0x03: //RL A
          CyclCnt+=12;
          a=datamem[AKKU];
          if(a & 0x80)
            datamem[AKKU]= (a<<1) | 0x01;
          else
            datamem[AKKU]= a<<1;
          SetParity(datamem[AKKU]);
          break;
        case 0x04: //ADD A,#const8
          n=(UCHAR)codemem[PC++];
          goto w0;
        case 0x05: //ADD A,dadr
          n=(USHORT)datamem[(UCHAR)codemem[PC++]];
          goto w0;
        case 0x06: //ADD A,@R0
          n=datamem[PSW] & REGBANK;
          n=datamem[n];  //R0
          if(n>0x7F)
            n=idatamem[n-0x80];
          else
            n=datamem[n];  //@R0
          goto w0;
        case 0x07: //ADD A,@R1
          n=datamem[PSW] & REGBANK;
          n=datamem[n+1];  //R1
          if(n>0x7F)
            n=idatamem[n-0x80];
          else
            n=datamem[n];  //@R1
          goto w0;
        case 0x08: //ADD A,R0
          n=datamem[PSW] & REGBANK;
          n=datamem[n];    //R0
          goto w0;
        case 0x09: //ADD A,R1
          n=datamem[PSW] & REGBANK;
          n=datamem[n+1];  //R1
          goto w0;
        case 0x0A: //ADD A,R2
          n=datamem[PSW] & REGBANK;
          n=datamem[n+2];  //R2
          goto w0;
        case 0x0B: //ADD A,R3
          n=datamem[PSW] & REGBANK;
          n=datamem[n+3];  //R3
          goto w0;
        case 0x0C: //ADD A,R4
          n=datamem[PSW] & REGBANK;
          n=datamem[n+4];  //R4
          goto w0;
        case 0x0D: //ADD A,R5
          n=datamem[PSW] & REGBANK;
          n=datamem[n+5];  //R5
          goto w0;
        case 0x0E: //ADD A,R6
          n=datamem[PSW] & REGBANK;
          n=datamem[n+6];  //R6
          goto w0;
        case 0x0F: //ADD A,R7
          n=datamem[PSW] & REGBANK;
          n=datamem[n+7];  //R7
w0:
          CyclCnt+=12;
          a=datamem[AKKU];
          i=(USHORT)a;
          i+=n;
          datamem[AKKU]=(UCHAR)i;
          if(i & 0x0100)
            datamem[PSW] |= CARRY;  // carry setzen
          else
            datamem[PSW] &= ~CARRY;
          SetParity((UCHAR)i);
          if(  (a<0x80 && (UCHAR)n<0x80 && (UCHAR)i>0x80)
               ||(a>0x80 && (UCHAR)n>0x80 && (UCHAR)i<0x80))
          datamem[PSW] |= OV;
          else
            datamem[PSW] &= ~OV;

          if((a&0x0F)+(n&0x0F)>0x0F )
            datamem[PSW] |= AC;
          else
            datamem[PSW] &= ~AC;
          break;
      }
      goto ex;
    }
    if(cmd<0x40)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //JNB badr,rel
          CyclCnt+=24;
          b=GetBitState((UCHAR)codemem[PC++],FALSE);
          if(!b)
          {
            rel=(SHORT)((CHAR)(codemem[PC]))+1;
            PC=PC+rel;
          }
          else
            PC++;
          break;
        case 0x01: //ACALL page1
          CyclCnt+=24;
          calladdr=PC-1;
          retAddr=PC+1;
          hPC=PC+1;
          retval=SetSPT(+1);
          i=datamem[SPT];
          if(i>0x7F)
          {
            idatamem[i-0x80]=(UCHAR)(hPC & 0xFF);  //Low ablegen
            i++;
          }
          else
            datamem[i++]=(UCHAR)(hPC & 0xFF);  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            idatamem[i-0x80]=(UCHAR)(hPC>>8);  //High ablegen
          else
            datamem[i]=(UCHAR)(hPC>>8);  //High ablegen

          n=(UCHAR)codemem[PC];
          PC&=0xF800;
          PC|=(USHORT)(0x100 + (n & 0xFF));
          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x02: //RETI
          CyclCnt+=24;
          //high zurückholen
          n=datamem[SPT];
          if(n<0x80)
            PC=(USHORT)(datamem[n]<<8);
          else
            PC=(USHORT)(idatamem[n-0x80]<<8);
          retval=SetSPT(-1);
          //low zurückholen
          n=datamem[SPT];
          if(n<0x80)
            PC+=(USHORT)datamem[n];
          else
            PC+=(USHORT)idatamem[n-0x80];
          retval|=SetSPT(-1);
          ::RemoveFromCallStackWnd(PC);
          if(intReq) //es liegen noch Interrupts an
            goto iw1;
          break;
        case 0x03: //RLC A
          CyclCnt+=12;
          a=datamem[AKKU];
          i=datamem[PSW];
          a=a<<1;

          if(i & CARRY)
            a|= 0x01;
          if(datamem[AKKU] & 0x80)
            datamem[PSW] |= CARRY;
          else
            datamem[PSW] &= ~CARRY;
          datamem[AKKU]=a;
          SetParity(a);
          break;
        case 0x04: //ADDC A,#const8
          n=(UCHAR)codemem[PC++];
          goto w1;
        case 0x05: //ADDC A,dadr
          n=(USHORT)datamem[(UCHAR)codemem[PC++]];
          goto w1;
        case 0x06: //ADDC A,@R0
          n=datamem[PSW] & REGBANK;
          n=datamem[n];  //R0
          if(n>0x7F)
            n=idatamem[n-0x80];
          else
            n=datamem[n];  //@R0
          goto w1;
        case 0x07: //ADDC A,@R1
          n=datamem[PSW] & REGBANK;
          n=datamem[n+1];  //R1
          if(n>0x7F)
            n=idatamem[n-0x80];
          else
            n=datamem[n];  //@R1
          goto w1;
        case 0x08: //ADDC A,R0
          n=datamem[PSW] & REGBANK;
          n=datamem[n];    //R0
          goto w1;
        case 0x09: //ADDC A,R1
          n=datamem[PSW] & REGBANK;
          n=datamem[n+1];  //R1
          goto w1;
        case 0x0A: //ADDC A,R2
          n=datamem[PSW] & REGBANK;
          n=datamem[n+2];  //R2
          goto w1;
        case 0x0B: //ADDC A,R3
          n=datamem[PSW] & REGBANK;
          n=datamem[n+3];  //R3
          goto w1;
        case 0x0C: //ADDC A,R4
          n=datamem[PSW] & REGBANK;
          n=datamem[n+4];  //R4
          goto w1;
        case 0x0D: //ADDC A,R5
          n=datamem[PSW] & REGBANK;
          n=datamem[n+5];  //R5
          goto w1;
        case 0x0E: //ADDC A,R6
          n=datamem[PSW] & REGBANK;
          n=datamem[n+6];  //R6
          goto w1;
        case 0x0F: //ADDC A,R7
          n=datamem[PSW] & REGBANK;
          n=datamem[n+7];  //R7
w1:
          CyclCnt+=12;
          a=datamem[AKKU];
          i=(USHORT)a;
          if(datamem[PSW] & CARRY)
            n++;
          i+=n;
          datamem[AKKU]=(UCHAR)i;
          if(i & 0x0100)
          {
            datamem[PSW] |= CARRY;  // carry setzen
          }
          else
          {
            datamem[PSW] &= ~CARRY;
          }
          SetParity((UCHAR)i);
          if(  (a<0x80 && (UCHAR)n<0x80 && (UCHAR)i>0x80)
               ||(a>0x80 && (UCHAR)n>0x80 && (UCHAR)i<0x80))
          datamem[PSW] |= OV;
          else
            datamem[PSW] &= ~OV;

          if((a&0x0F)+(n&0x0F)>0x0F )
            datamem[PSW] |= AC;
          else
            datamem[PSW] &= ~AC;
          break;
      }
      goto ex;
    }
    if(cmd<0x50)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //JC rel
          CyclCnt+=24;
          if(datamem[PSW] & CARRY)
          {
            rel=(SHORT)((CHAR)(codemem[PC]))+1;
            PC=PC+rel;
          }
          else
            PC++;
          break;
        case 0x01: //AJMP page2
          CyclCnt+=24;
          PC=(PC & 0xF800) + (codemem[PC]&0xFF)+0x200;
          break;
        case 0x02: //ORL dadr,A
          CyclCnt+=12;
          n=(UCHAR)codemem[PC++];
          datamem[n] |=datamem[AKKU];
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          break;
        case 0x03: //ORL dadr,#const8
          CyclCnt+=24;
          n=(UCHAR)codemem[PC++];
          a=(UCHAR)codemem[PC++];
          datamem[n] |= a;
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          if(n==AKKU)
            SetParity(datamem[AKKU]);
          break;
        case 0x04: //ORL A,#const8
          CyclCnt+=12;
          a=datamem[AKKU] | ((UCHAR)codemem[PC++]);
          datamem[AKKU]=a;
          SetParity(a);
          break;
        case 0x05: //ORL A,dadr
          CyclCnt+=12;
          a=datamem[AKKU];
          n=(UCHAR)codemem[PC++];
          a |= datamem[n];
          datamem[AKKU]=a;
          SetParity(a);
          break;
        case 0x06: //ORL A,@R0
          n=datamem[PSW] & REGBANK; // adresse von R0
          n=datamem[n]; //R0
          goto w13;
        case 0x07: //ORL A,@R1
          n=(datamem[PSW] & REGBANK) +1; //adresse von R1
          n=datamem[n]; //R1
w13:
          CyclCnt+=12;
          if(n>0x7F)
            n=idatamem[n-0x80];
          else
            n=datamem[n];  //@R0
          a=datamem[AKKU];
          a |= (UCHAR)n;
          datamem[AKKU]=a;
          SetParity(a);
          break;
        case 0x08: //ORL A,R0
          n=(datamem[PSW] & REGBANK); //adresse von R0
          goto w2;
        case 0x09: //ORL A,R1
          n=(datamem[PSW] & REGBANK)+1; //adresse von R1
          goto w2;
        case 0x0A: //ORL A,R2
          n=(datamem[PSW] & REGBANK)+2; //adresse von R2
          goto w2;
        case 0x0B: //ORL A,R3
          n=(datamem[PSW] & REGBANK)+3; //adresse von R3
          goto w2;
        case 0x0C: //ORL A,R4
          n=(datamem[PSW] & REGBANK)+4; //adresse von R4
          goto w2;
        case 0x0D: //ORL A,R5
          n=(datamem[PSW] & REGBANK)+5; //adresse von R5
          goto w2;
        case 0x0E: //ORL A,R6
          n=(datamem[PSW] & REGBANK)+6; //adresse von R6
          goto w2;
        case 0x0F: //ORL A,R7
          n=(datamem[PSW] & REGBANK)+7; //adresse von R7
w2:
          CyclCnt+=12;
          a=datamem[AKKU];
          a |= datamem[n];
          datamem[AKKU]=a;
          SetParity(a);
          break;
      }
      goto ex;
    }
    if(cmd<0x60)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //JNC rel
          CyclCnt+=24;
          if(!(datamem[PSW] & CARRY))
          {
            rel=(SHORT)((CHAR)codemem[PC])+1;
            PC=PC+rel;
          }
          else
            PC++;
          break;
        case 0x01: //ACALL page2
          CyclCnt+=24;
          calladdr=PC-1;
          retAddr=PC+1;
          hPC=PC+1;
          retval=SetSPT(+1);
          i=datamem[SPT];
          if(i>0x7F)
          {
            idatamem[i-0x80]=(UCHAR)(hPC & 0xFF);  //Low ablegen
            i++;
          }
          else
            datamem[i++]=(UCHAR)(hPC & 0xFF);  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            idatamem[i-0x80]=(UCHAR)(hPC>>8);  //High ablegen
          else
            datamem[i]=(UCHAR)(hPC>>8);  //High ablegen
          n=(UCHAR)codemem[PC];
          PC&=0xF800;
          PC|=(USHORT)(0x200 + (n & 0xFF));
          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x02: //ANL dadr,A
          CyclCnt+=12;
          n=(UCHAR)codemem[PC++];
          datamem[n] &=datamem[AKKU];
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          break;
        case 0x03: //ANL dadr,#const8
          CyclCnt+=24;
          n=(UCHAR)codemem[PC++];
          a=(UCHAR)codemem[PC++];
          datamem[n] &= a;
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          else if(n==AKKU)
            SetParity(datamem[AKKU]);
          break;
        case 0x04: //ANL A,#const8
          CyclCnt+=12;
          a=datamem[AKKU] & ((UCHAR)codemem[PC++]);
          datamem[AKKU]=a;
          SetParity(a);
          break;
        case 0x05: //ANL A,dadr
          CyclCnt+=12;
          a=datamem[AKKU];
          n=(UCHAR)codemem[PC++];
          a &= datamem[n];
          datamem[AKKU]=a;
          SetParity(a);
          break;
        case 0x06: //ANL A,@R0
          n=datamem[PSW] & REGBANK; // adresse von R0
          goto w14;
        case 0x07: //ANL A,@R1
          n=(datamem[PSW] & REGBANK) +1; //adresse von R1
w14:
          CyclCnt+=12;
          n=datamem[n]; //R1
          if(n>0x7F)
            n=idatamem[n-0x80];
          else
            n=datamem[n];
          a=datamem[AKKU];
          a &= (UCHAR)n;
          datamem[AKKU]=a;
          SetParity(a);
          break;
        case 0x08: //ANL A,R0
          n=(datamem[PSW] & REGBANK); //adresse von R0
          goto w3;
        case 0x09: //ANL A,R1
          n=(datamem[PSW] & REGBANK)+1; //adresse von R1
          goto w3;
        case 0x0A: //ANL A,R2
          n=(datamem[PSW] & REGBANK)+2; //adresse von R2
          goto w3;
        case 0x0B: //ANL A,R3
          n=(datamem[PSW] & REGBANK)+3; //adresse von R3
          goto w3;
        case 0x0C: //ANL A,R4
          n=(datamem[PSW] & REGBANK)+4; //adresse von R4
          goto w3;
        case 0x0D: //ANL A,R5
          n=(datamem[PSW] & REGBANK)+5; //adresse von R5
          goto w3;
        case 0x0E: //ANL A,R6
          n=(datamem[PSW] & REGBANK)+6; //adresse von R6
          goto w3;
        case 0x0F: //ANL A,R7
          n=(datamem[PSW] & REGBANK)+7; //adresse von R7
w3:
          CyclCnt+=12;
          a=datamem[AKKU];
          a &= datamem[n];
          datamem[AKKU]=a;
          SetParity(a);
          break;
      }
      goto ex;
    }
    if(cmd<0x70)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //JZ rel
          CyclCnt+=24;
          if(datamem[AKKU])
            PC++;
          else
          {
            rel=(SHORT)((CHAR)(codemem[PC]))+1;
            PC=PC+rel;
          }
          break;
        case 0x01: //AJMP page3
          CyclCnt+=24;
          PC=(PC & 0xF800) + (codemem[PC]&0xFF)+0x300;
          break;
        case 0x02: //XRL dadr,A
          CyclCnt+=12;
          n=(UCHAR)codemem[PC++];
          datamem[n] ^=datamem[AKKU];
          if(n==AKKU)
            SetParity(datamem[AKKU]);
          break;
        case 0x03: //XRL dadr,#const8
          CyclCnt+=24;
          n=(UCHAR)codemem[PC++];
          a=(UCHAR)codemem[PC++];
          datamem[n] ^= a;
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          else if(n==AKKU)
            SetParity(datamem[AKKU]);
          break;
        case 0x04: //XRL A,#const8
          CyclCnt+=12;
          a=datamem[AKKU] ^ ((UCHAR)codemem[PC++]);
          datamem[AKKU]=a;
          SetParity(a);
          break;
        case 0x05: //XRL A,dadr
          CyclCnt+=12;
          a=datamem[AKKU];
          n=(UCHAR)codemem[PC++];
          a ^= datamem[n];
          datamem[AKKU]=a;
          SetParity(a);
          break;
        case 0x06: //XRL A,@R0
          n=datamem[PSW] & REGBANK; // adresse von R0
          goto w15;
        case 0x07: //XRL A,@R1
          n=(datamem[PSW] & REGBANK) +1; //adresse von R1
w15:
          CyclCnt+=12;
          a=datamem[AKKU];
          n=datamem[n];  //Ri
          if(n>0x7F)
            n=idatamem[n-0x80];
          else
            n=datamem[n];    //@Ri
          a ^= (UCHAR)n;
          datamem[AKKU]=a;
          SetParity(a);
          break;
        case 0x08: //XRL A,R0
          n=(datamem[PSW] & REGBANK); //adresse von R0
          goto w4;
        case 0x09: //XRL A,R1
          n=(datamem[PSW] & REGBANK)+1; //adresse von R1
          goto w4;
        case 0x0A: //XRL A,R2
          n=(datamem[PSW] & REGBANK)+2; //adresse von R2
          goto w4;
        case 0x0B: //XRL A,R3
          n=(datamem[PSW] & REGBANK)+3; //adresse von R3
          goto w4;
        case 0x0C: //XRL A,R4
          n=(datamem[PSW] & REGBANK)+4; //adresse von R4
          goto w4;
        case 0x0D: //XRL A,R5
          n=(datamem[PSW] & REGBANK)+5; //adresse von R5
          goto w4;
        case 0x0E: //XRL A,R6
          n=(datamem[PSW] & REGBANK)+6; //adresse von R6
          goto w4;
        case 0x0F: //XRL A,R7
          n=(datamem[PSW] & REGBANK)+7; //adresse von R7
w4:
          CyclCnt+=12;
          a=datamem[AKKU];
          a ^= datamem[n];
          datamem[AKKU]=a;
          SetParity(a);
          break;
      }
      goto ex;
    }
    if(cmd<0x80)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //JNZ rel
          CyclCnt+=24;
          if(datamem[AKKU])
          {
            rel=(SHORT)((CHAR)(codemem[PC]))+1;
            PC=PC+rel;
          }
          else
            PC++;
          break;
        case 0x01: //ACALL page3
          CyclCnt+=24;
          calladdr=PC-1;
          retAddr=PC+1;
          hPC=PC+1;
          retval=SetSPT(+1);
          i=datamem[SPT];
          if(i>0x7F)
          {
            idatamem[i-0x80]=(UCHAR)(hPC & 0xFF);  //Low ablegen
            i++;
          }
          else
            datamem[i++]=(UCHAR)(hPC & 0xFF);  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            idatamem[i-0x80]=(UCHAR)(hPC>>8);  //High ablegen
          else
            datamem[i]=(UCHAR)(hPC>>8);  //High ablegen
          n=(UCHAR)codemem[PC];
          PC&=0xF800;
          PC|=(USHORT)(0x300 + (n & 0xFF));
          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x02: //ORL C,badr
          CyclCnt+=24;
          b=GetBitState((UCHAR)codemem[PC++],FALSE);
          if((datamem[PSW] & CARRY) || b)
            datamem[PSW] |=CARRY;
          else
            datamem[PSW] &= ~CARRY;
          break;
        case 0x03: //JMP @A+DPTR
          CyclCnt+=24;
          n=(USHORT)datamem[AKKU];
          i=(USHORT)datamem[DPL] + ((USHORT)(datamem[DPH])<<8);
          PC=n+i;
          break;
        case 0x04: //MOV A,#const8
          CyclCnt+=12;
          datamem[AKKU]=(UCHAR)codemem[PC++];
          a= datamem[AKKU];
          SetParity(a);
          break;
        case 0x05: //MOV dadr,#const8
          CyclCnt+=24;
          n=(UCHAR)codemem[PC++];
          datamem[n]=(UCHAR)codemem[PC++];
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          else if(n==AKKU)
            SetParity(datamem[AKKU]);
          break;
        case 0x06: //MOV @R0,#const8
          n=(datamem[PSW] & REGBANK); // adresse von R0
          goto w16;
        case 0x07: //MOV @R1,#const8
          n=(datamem[PSW] & REGBANK)+1; // adresse von R1
w16:
          CyclCnt+=12;
          n=datamem[n];                 // Ri
          if(n>0x7F)
            idatamem[n-0x80]=(UCHAR)codemem[PC++];
          else
            datamem[n]=(UCHAR)codemem[PC++];
          break;
        case 0x08: //MOV R0,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK);  //adresse von R0
          datamem[n]=(UCHAR)codemem[PC++];
          break;
        case 0x09: //MOV R1,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK)+1;  //adresse von R1
          datamem[n]=(UCHAR)codemem[PC++];
          break;
        case 0x0A: //MOV R2,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK)+2;  //adresse von R2
          datamem[n]=(UCHAR)codemem[PC++];
          break;
        case 0x0B: //MOV R3,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK)+3;  //adresse von R3
          datamem[n]=(UCHAR)codemem[PC++];
          break;
        case 0x0C: //MOV R4,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK)+4;  //adresse von R4
          datamem[n]=(UCHAR)codemem[PC++];
          break;
        case 0x0D: //MOV R5,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK)+5;  //adresse von R5
          datamem[n]=(UCHAR)codemem[PC++];
          break;
        case 0x0E: //MOV R6,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK)+6;  //adresse von R6
          datamem[n]=(UCHAR)codemem[PC++];
          break;
        case 0x0F: //MOV R7,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK)+7;  //adresse von R7
          datamem[n]=(UCHAR)codemem[PC++];
          break;
      }
      goto ex;
    }
    if(cmd<0x90)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //SJMP rel
          CyclCnt+=24;
          rel=(SHORT)((CHAR)(codemem[PC]&0xFF))+1;
          PC=PC+rel;
          break;
        case 0x01: //AJMP page4
          CyclCnt+=24;
          PC=(PC & 0xF800) + (codemem[PC]&0xFF)+0x400;
          break;
        case 0x02: //ANL C,badr
          CyclCnt+=24;
          b=GetBitState((UCHAR)codemem[PC++],FALSE);
          if((datamem[PSW] & CARRY) && b)
            datamem[PSW] |=CARRY;
          else
            datamem[PSW] &= ~CARRY;
          break;
        case 0x03: //MOVC A,@A+PC
          CyclCnt+=24;
          n=(USHORT)datamem[AKKU] + PC;
          datamem[AKKU]=(UCHAR)codemem[n];
          a=datamem[AKKU];
          SetParity(a);
          break;
        case 0x04: //DIV AB
          CyclCnt+=48;
          datamem[PSW] &= ~(CARRY|OV);
          a=datamem[AKKU];
          if(datamem[BREG])
          {
            a /= datamem[BREG];
            datamem[BREG]=datamem[AKKU] % datamem[BREG];
            datamem[AKKU]=a;
          }
          else
          {
            datamem[PSW] |= OV;
          }
          SetParity(a);
          break;
        case 0x05: //MOV dadr,dadr
          CyclCnt+=24;
          n=(UCHAR)codemem[PC++];
          i=(UCHAR)codemem[PC++];
          datamem[i]=datamem[n];
          if(i==S0BUF || i==SCON)
          {
            if(i==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          else if(n==AKKU)
            SetParity(datamem[AKKU]);
          break;
        case 0x06: //MOV dadr,@R0
          n=datamem[PSW] & REGBANK; //adresse R0
          goto w17;
        case 0x07: //MOV dadr,@R1
          n=(datamem[PSW] & REGBANK) +1; //adresse R1
w17:
          CyclCnt+=24;
          n=datamem[n]; //Ri
          if(n>0x7F)
            n=idatamem[n-0x80]; //@Ri
          else
            n=datamem[n]; //@Ri
          i=(UCHAR)codemem[PC++]; //dadr
          datamem[i]=(UCHAR)n;
          if(i==S0BUF || i==SCON)
          {
            if(i==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          else if(n==AKKU)
            SetParity(datamem[AKKU]);
          break;
        case 0x08: //MOV dadr,R0
          n=datamem[PSW] & REGBANK; //adresse R0
          goto w5;
        case 0x09: //MOV dadr,R1
          n=(datamem[PSW] & REGBANK)+1; //adresse R1
          goto w5;
        case 0x0A: //MOV dadr,R2
          n=(datamem[PSW] & REGBANK)+2; //adresse R2
          goto w5;
        case 0x0B: //MOV dadr,R3
          n=(datamem[PSW] & REGBANK)+3; //adresse R3
          goto w5;
        case 0x0C: //MOV dadr,R4
          n=(datamem[PSW] & REGBANK)+4; //adresse R4
          goto w5;
        case 0x0D: //MOV dadr,R5
          n=(datamem[PSW] & REGBANK)+5; //adresse R5
          goto w5;
        case 0x0E: //MOV dadr,R6
          n=(datamem[PSW] & REGBANK)+6; //adresse R6
          goto w5;
        case 0x0F: //MOV dadr,R7
          n=(datamem[PSW] & REGBANK)+7; //adresse R7
w5:
          CyclCnt+=24;
          n=datamem[n]; //Ri
          i=(UCHAR)codemem[PC++]; //dadr
          datamem[i]=(UCHAR)n;
          if(i==S0BUF || i==SCON)
          {
            if(i==S0BUF)
            {
              IsSerChar=TRUE;
            }
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          else if(n==AKKU)
            SetParity(datamem[AKKU]);
          break;
      }
      goto ex;
    }
    if(cmd<0xA0)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //MOV DPTR,#const16
          CyclCnt+=24;
          datamem[DPH]=(UCHAR)codemem[PC++];
          datamem[DPL]=(UCHAR)codemem[PC++];
          break;
        case 0x01: //ACALL page4
          CyclCnt+=24;
          calladdr=PC-1;
          retAddr=PC+1;
          hPC=PC+1;
          retval=SetSPT(+1);
          i=datamem[SPT];
          if(i>0x7F)
          {
            idatamem[i-0x80]=(UCHAR)(hPC & 0xFF);  //Low ablegen
            i++;
          }
          else
            datamem[i++]=(UCHAR)(hPC & 0xFF);  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            idatamem[i-0x80]=(UCHAR)(hPC>>8);  //High ablegen
          else
            datamem[i]=(UCHAR)(hPC>>8);  //High ablegen
          n=(UCHAR)codemem[PC];
          PC&=0xF800;
          PC|=(USHORT)(0x400 + (n & 0xFF));
          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x02: //MOV badr,C
          CyclCnt+=24;
          if(datamem[PSW] & CARRY)
            SetBit((UCHAR)codemem[PC++],TRUE);
          else
            SetBit((UCHAR)codemem[PC++],FALSE);
          break;
        case 0x03: //MOVC A,@A+DPTR
          CyclCnt+=24;
          n=((USHORT)datamem[DPH]<<8)+(USHORT)datamem[DPL];
          i=(USHORT)datamem[AKKU] + n;
          datamem[AKKU]=(UCHAR)codemem[i];
          a=datamem[AKKU];
          SetParity(a);
          break;
        case 0x04: //SUBB A,#const8
          n=(UCHAR)codemem[PC++];
          goto w6;
        case 0x05: //SUBB A,dadr
          n=(USHORT)datamem[(UCHAR)codemem[PC++]];
          goto w6;
        case 0x06: //SUBB A,@R0
          n=datamem[PSW] & REGBANK; //adresse von R0
          goto w18;
        case 0x07: //SUBB A,@R1
          n=(datamem[PSW] & REGBANK)+1;  //adresse von R1
w18:
          n=datamem[n];     //Ri
          if(n>0x7F)
            n=idatamem[n-0x80];
          else
            n=datamem[n];  //@Ri
          goto w6;
        case 0x08: //SUBB A,R0
          n=datamem[PSW] & REGBANK;
          n=datamem[n];    //R0
          goto w6;
        case 0x09: //SUBB A,R1
          n=datamem[PSW] & REGBANK;
          n=datamem[n+1];  //R1
          goto w6;
        case 0x0A: //SUBB A,R2
          n=datamem[PSW] & REGBANK;
          n=datamem[n+2];  //R2
          goto w6;
        case 0x0B: //SUBB A,R3
          n=datamem[PSW] & REGBANK;
          n=datamem[n+3];  //R3
          goto w6;
        case 0x0C: //SUBB A,R4
          n=datamem[PSW] & REGBANK;
          n=datamem[n+4];  //R4
          goto w6;
        case 0x0D: //SUBB A,R5
          n=datamem[PSW] & REGBANK;
          n=datamem[n+5];  //R5
          goto w6;
        case 0x0E: //SUBB A,R6
          n=datamem[PSW] & REGBANK;
          n=datamem[n+6];  //R6
          goto w6;
        case 0x0F: //SUBB A,R7
          n=datamem[PSW] & REGBANK;
          n=datamem[n+7];  //R7
w6:
          CyclCnt+=12;
          a=datamem[AKKU];
          i=(USHORT)a;
          i-=n;
          if(datamem[PSW] & CARRY)
          {
            i--;
            if(((a&0x0F)-1-(n&0x0F)>0x0F))
              datamem[PSW] |= AC;
            else
              datamem[PSW] &= ~AC;
          }
          else
          {
            if(((a&0x0F)-(n&0x0F)>0x0F))
              datamem[PSW] |= AC;
            else
              datamem[PSW] &= ~AC;
          }
          datamem[AKKU]=(UCHAR)i;

          if(i & 0xFF00)
            datamem[PSW] |= CARRY;  // carry setzen
          else
            datamem[PSW] &= ~CARRY;
          SetParity((UCHAR)i);

          if(  ((UCHAR)a<=0x7F && (UCHAR)n>=0x80 && (UCHAR)i>=0x80)
               ||((UCHAR)a>=0x80 && (UCHAR)n<=0x7F && (UCHAR)i<=0x7F) )
          datamem[PSW] |= OV;
          else
            datamem[PSW] &= ~OV;
          break;
      }
      goto ex;
    }
    if(cmd<0xB0)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //ORL C,/badr
          CyclCnt+=24;
          b=GetBitState((UCHAR)codemem[PC++],FALSE);
          if((datamem[PSW] & CARRY) || !b)
            datamem[PSW] |=CARRY;
          else
            datamem[PSW] &= ~CARRY;
          break;
        case 0x01: //AJMP page5
          CyclCnt+=24;
          PC=(PC & 0xF800) + (codemem[PC]&0xFF)+0x500;
          break;
        case 0x02: //MOV C,badr
          CyclCnt+=12;
          if(GetBitState((UCHAR)codemem[PC++],FALSE))
            datamem[PSW] |=CARRY;
          else
            datamem[PSW] &= ~CARRY;
          break;
        case 0x03: //INC DPTR
          CyclCnt+=24;
          n=(USHORT)datamem[DPL]+1;
          datamem[DPL]++;
          if(n > 0xFF)
            datamem[DPH]++;
          break;
        case 0x04: //MUL AB
          CyclCnt+=48;
          datamem[PSW] &= ~(CARRY + OV);
          n=datamem[AKKU] * datamem[BREG];
          datamem[AKKU]= (UCHAR)(n & 0xFF);
          datamem[BREG]=(UCHAR)(n>>8);
          if(datamem[BREG])
            datamem[PSW] |= OV;
          a=datamem[AKKU];
          SetParity(a);
          break;
        case 0x05:  // Breakpoint (wird aber nicht mehr benötigt)
          PC--;
          return(-1); //keine Aufzeichnung eines Tracerecords
          break;
        case 0x06: //MOV @R0,dadr
          n=datamem[PSW] & REGBANK; //adresse von R0
          goto w19;
        case 0x07: //MOV @R1,dadr
          n=(datamem[PSW] & REGBANK)+1; //adresse von R1
w19:
          CyclCnt+=12;
          a=(UCHAR)codemem[PC++];          //dadr
          n=datamem[n];            //Ri
          if(n>0x7F)
            idatamem[n-0x80]=datamem[a];
          else
            datamem[n]=datamem[a];
          break;
        case 0x08: //MOV R0,dadr
          n=(datamem[PSW] & REGBANK); //adresse von R0
          goto w7;
        case 0x09: //MOV R1,dadr
          n=(datamem[PSW] & REGBANK)+1; //adresse von R1
          goto w7;
        case 0x0A: //MOV R2,dadr
          n=(datamem[PSW] & REGBANK)+2; //adresse von R2
          goto w7;
        case 0x0B: //MOV R3,dadr
          n=(datamem[PSW] & REGBANK)+3; //adresse von R3
          goto w7;
        case 0x0C: //MOV R4,dadr
          n=(datamem[PSW] & REGBANK)+4; //adresse von R4
          goto w7;
        case 0x0D: //MOV R5,dadr
          n=(datamem[PSW] & REGBANK)+5; //adresse von R5
          goto w7;
        case 0x0E: //MOV R6,dadr
          n=(datamem[PSW] & REGBANK)+6; //adresse von R6
          goto w7;
        case 0x0F: //MOV R7,dadr
          n=(datamem[PSW] & REGBANK)+7; //adresse von R7
w7:
          CyclCnt+=24;
          a=datamem[(UCHAR)codemem[PC++]];
          datamem[n]=a;
          break;
      }

      goto ex;
    }
    if(cmd<0xC0)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //ANL C,/badr
          CyclCnt+=24;
          b=GetBitState((UCHAR)codemem[PC++],FALSE);
          if((datamem[PSW] & CARRY) && !b)
            datamem[PSW] |=CARRY;
          else
            datamem[PSW] &= ~CARRY;
          break;
        case 0x01: //ACALL page5
          CyclCnt+=24;
          calladdr=PC-1;
          retAddr=PC+1;
          hPC=PC+1;
          retval=SetSPT(+1);
          i=datamem[SPT];
          if(i>0x7F)
          {
            idatamem[i-0x80]=(UCHAR)(hPC & 0xFF);  //Low ablegen
            i++;
          }
          else
            datamem[i++]=(UCHAR)(hPC & 0xFF);  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            idatamem[i-0x80]=(UCHAR)(hPC>>8);  //High ablegen
          else
            datamem[i]=(UCHAR)(hPC>>8);  //High ablegen
          n=(UCHAR)codemem[PC];
          PC&=0xF800;
          PC|=(USHORT)(0x500 + (n & 0xFF));
          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x02: //CPL badr
          CyclCnt+=12;
          a=(UCHAR)codemem[PC++];
          if(GetBitState(a,FALSE))
            SetBit(a,FALSE);
          else
            SetBit(a,TRUE);
          break;
        case 0x03: //CPL C
          CyclCnt+=12;
          datamem[PSW] ^= CARRY; //carry negieren
          break;
        case 0x04: //CJNE A,#const8,rel
          n=(UCHAR)codemem[PC++];  //const8
          i=(USHORT)datamem[AKKU];
          goto w9;
        case 0x05: //CJNE A,dadr,rel
          n=datamem[(UCHAR)codemem[PC++]]; //dadr
          i=(USHORT)datamem[AKKU];
          goto w9;
        case 0x06: //CJNE @R0,#const8,rel
          i=(datamem[PSW] & REGBANK); //adresse von R0
          goto w20;
        case 0x07: //CJNE @R1,#const8,rel
          i=(datamem[PSW] & REGBANK)+1; //adresse von R1
w20:
          i=datamem[i]; //Ri
          n=(UCHAR)codemem[PC++];  //const8
          if(i>0x7F)
            i=idatamem[i-0x80];
          else
            i=datamem[i]; //@Ri
          goto w9;
        case 0x08: //CJNE R0,#const8,rel
          i=(datamem[PSW] & REGBANK); //adresse von R0
          i=datamem[i]; //R0
          n=(UCHAR)codemem[PC++];  //const8
          goto w9;
        case 0x09: //CJNE R1,#const8,rel
          i=(datamem[PSW] & REGBANK)+1; //adresse von R1
          i=datamem[i]; //R1
          n=(UCHAR)codemem[PC++];  //const8
          goto w9;
        case 0x0A: //CJNE R2,#const8,rel
          i=(datamem[PSW] & REGBANK)+2; //adresse von R2
          i=datamem[i]; //R2
          n=(UCHAR)codemem[PC++];  //const8
          goto w9;
        case 0x0B: //CJNE R3,#const8,rel
          i=(datamem[PSW] & REGBANK)+3; //adresse von R3
          i=datamem[i]; //R3
          n=(UCHAR)codemem[PC++];  //const8
          goto w9;
        case 0x0C: //CJNE R4,#const8,rel
          i=(datamem[PSW] & REGBANK)+4; //adresse von R4
          i=datamem[i]; //R4
          n=(UCHAR)codemem[PC++];  //const8
          goto w9;
        case 0x0D: //CJNE R5,#const8,rel
          i=(datamem[PSW] & REGBANK)+5; //adresse von R5
          i=datamem[i]; //R5
          n=(UCHAR)codemem[PC++];  //const8
          goto w9;
        case 0x0E: //CJNE R6,#const8,rel
          i=(datamem[PSW] & REGBANK)+6; //adresse von R6
          i=datamem[i]; //R6
          n=(UCHAR)codemem[PC++];  //const8
          goto w9;
        case 0x0F: //CJNE R7,#const8,rel
          i=(datamem[PSW] & REGBANK)+7; //adresse von R7
          i=datamem[i]; //R7
          n=(UCHAR)codemem[PC++];  //const8
w9:
          CyclCnt+=24;
          rel=(SHORT)((CHAR)codemem[PC++]);  //rel
          if(i != n)
          {
            if(i<n)
              datamem[PSW] |= CARRY;
            else
              datamem[PSW] &= ~CARRY;
            PC +=rel;
          }
          else
          {
            if(i<n)
              datamem[PSW] |= CARRY;
            else
              datamem[PSW] &= ~CARRY;
          }
          break;
      }

      goto ex;
    }
    if(cmd<0xD0)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //PUSH dadr
          CyclCnt+=24;
          retval=SetSPT(+1);
          n=datamem[SPT];
          datamem[SPT]=(UCHAR)n;
          a=datamem[(UCHAR)codemem[PC++]];
          if(n>0x7F)
            idatamem[n-0x80]=a;
          else
            datamem[n]=a;
          callstackvalid--;
          if(callstackvalid==-2)
          {
            retAddr= datamem[SPT-1]+datamem[SPT-2]*256;
            ::AddToCallStackWnd(PC,retAddr);
            callstackvalid=0;
          }
          break;
        case 0x01: //AJMP page6
          CyclCnt+=24;
          PC=(PC & 0xF800) + (codemem[PC]&0xFF)+0x600;
          break;
        case 0x02: //CLR badr
          CyclCnt+=12;
          SetBit((UCHAR)codemem[PC++],FALSE);
          break;
        case 0x03: //CLR C
          CyclCnt+=12;
          datamem[PSW] &= ~CARRY;
          break;
        case 0x04: //SWAP A
          CyclCnt+=12;
          a=datamem[AKKU]>>4;
          i=(datamem[AKKU]<<4) &0xF0;
          a |= (UCHAR)i;
          datamem[AKKU]=a;
          break;
        case 0x05: //XCH A,dadr
          CyclCnt+=12;
          a=datamem[AKKU];
          n=(UCHAR)codemem[PC++];      //dadr
          datamem[AKKU]=datamem[n];
          datamem[n]=a;
          a=datamem[AKKU];
          SetParity(a);
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          break;
        case 0x06: //XCH A,@R0
          i=datamem[PSW] & REGBANK; //adresse von R0
          goto w21;
        case 0x07: //XCH A,@R1
          i=(datamem[PSW] & REGBANK)+1; //adresse von R1
w21:
          CyclCnt+=12;
          a=datamem[AKKU];
          n=datamem[i];             //Ri
          if(n>0x7F)
            n=idatamem[n-0x80];
          else
            n=datamem[n];           //@R1
          a=datamem[AKKU];
          datamem[AKKU]=(UCHAR)n;
          n=datamem[i];             //Ri
          if(n>0x7F)
            idatamem[n-0x80]=a;
          else
            datamem[n]=a;
          SetParity(datamem[AKKU]);
          break;
        case 0x08: //XCH A,R0
          i=(datamem[PSW] & REGBANK); //adresse von R0
          n=datamem[i];              //R0
          goto w8;
        case 0x09: //XCH A,R1
          i=(datamem[PSW] & REGBANK)+1; //adresse von R1
          n=datamem[i];              //R1
          goto w8;
        case 0x0A: //XCH A,R2
          i=(datamem[PSW] & REGBANK)+2; //adresse von R2
          n=datamem[i];              //R2
          goto w8;
        case 0x0B: //XCH A,R3
          i=(datamem[PSW] & REGBANK)+3; //adresse von R3
          n=datamem[i];              //R3
          goto w8;
        case 0x0C: //XCH A7,R4
          i=(datamem[PSW] & REGBANK)+4; //adresse von R4
          n=datamem[i];              //R4
          goto w8;
        case 0x0D: //XCH A,R5
          i=(datamem[PSW] & REGBANK)+5; //adresse von R5
          n=datamem[i];              //R5
          goto w8;
        case 0x0E: //XCH A,R6
          i=(datamem[PSW] & REGBANK)+6; //adresse von R6
          n=datamem[i];              //R6
          goto w8;
        case 0x0F: //XCH A,R7
          i=(datamem[PSW] & REGBANK)+7; //adresse von R7
          n=datamem[i];              //R7
w8:
          CyclCnt+=12;
          a=datamem[AKKU];
          datamem[AKKU]=(UCHAR)n;
          datamem[i]=a;
          SetParity((UCHAR)n);
          break;
      }
      goto ex;
    }
    if(cmd<0xE0)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //POP dadr
          CyclCnt+=24;
          n=(UCHAR)codemem[PC++]; //dadr
          i=datamem[SPT];  //SPT
          if(i>0x7F)
            a=idatamem[i-0x80];    //@SPT
          else
            a=datamem[i];    //@SPT
          datamem[n]=a;
          retval=SetSPT(-1);
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          else if(n==AKKU)
            SetParity(datamem[AKKU]);
          callstackvalid++;
          if(callstackvalid==2)
          {
            ::RemoveFromCallStackWnd(PC);
            callstackvalid=0;
          }
          break;
        case 0x01: //ACALL page6
          CyclCnt+=24;
          calladdr=PC-1;
          retAddr=PC+1;
          hPC=PC+1;
          retval=SetSPT(+1);
          i=datamem[SPT];
          if(i>0x7F)
          {
            idatamem[i-0x80]=(UCHAR)(hPC & 0xFF);  //Low ablegen
            i++;
          }
          else
            datamem[i++]=(UCHAR)(hPC & 0xFF);  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            idatamem[i-0x80]=(UCHAR)(hPC>>8);  //High ablegen
          else
            datamem[i]=(UCHAR)(hPC>>8);  //High ablegen
          n=(UCHAR)codemem[PC];
          PC&=0xF800;
          PC|=(USHORT)(0x600 + (n & 0xFF));
          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x02: //SETB badr
          CyclCnt+=12;
          SetBit((UCHAR)codemem[PC++],TRUE);
          break;
        case 0x03: //SETB C
          CyclCnt+=12;
          datamem[PSW] |= CARRY;
          break;
        case 0x04: //DA
          CyclCnt+=12;
          n=datamem[AKKU];
          if(((n & 0x0F) > 0x09) || (datamem[PSW] & 0x40))  //wenn unteres Nibble > 9 oder Hilfscarry gesetzt
            n+=6;                                            // addiere 6 dazu
          if(n > 0xFF)
            datamem[PSW] |= CARRY;                           //wenn Übertrag Carry setzen (n darf also keine Bytevariable sein!!!

          if(((n & 0x0F0) > 0x90) || (datamem[PSW] & 0x80)) // wenn das obere Nibble jetzt >0x90 oder Carry gesetzt
            n+=0x60;                                // addiere 0x60 dazu
          if(n > 0xF0)                              // wenn das Ergebnis des oberen Nibbles jetzt >0xF0
            datamem[PSW] |= CARRY;                  // setze Carry
          datamem[AKKU]=(UCHAR)n;                   //schreibe den wert in den Akku zurück
          SetParity(datamem[AKKU]);                 // berechne zum Schluss noch das Parity Bit
          break;
        case 0x05: //DJNZ dadr,rel
          CyclCnt+=24;
          n=(UCHAR)codemem[PC++]; //dadr
          rel=(SHORT)((CHAR)codemem[PC++]);
          datamem[n]--;
          if(datamem[n])
            PC+=rel;
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          else if(n==AKKU)
            SetParity(datamem[AKKU]);
          break;
        case 0x06: //XCHD A,@R0
          n=(datamem[PSW] & REGBANK);   //adresse von R0
          goto w10;
        case 0x07: //XCHD A,@R1
          n=(datamem[PSW] & REGBANK)+1; //adresse von R1
w10:
          CyclCnt+=12;
          n=datamem[n];                 // Ri
          a=datamem[AKKU] & 0x0F;       // AKKU unteres Halbbyte
          if(n>0x7F)
          {
            i=idatamem[n-0x80] &0x0F;
            idatamem[n-0x80] &= 0xF0;   //unteres Halbbyte löschen
            idatamem[n-0x80] |=a;
          }
          else
          {
            i=datamem[n] & 0x0F;        // @Ri unteres Halbbyte
            datamem[n] &= 0xF0;         //unteres Halbbyte löschen
            datamem[n] |= a;
          }
          a=datamem[AKKU]&0xF0 | (UCHAR)i;       // AKKU oberes Halbbyte + @Ri unteres HalbByte
          datamem[AKKU] = a;
          SetParity(a);
          break;
        case 0x08: //DJNZ R0,rel
          n=(datamem[PSW] & REGBANK);
          goto w22;
        case 0x09: //DJNZ R1,rel
          n=(datamem[PSW] & REGBANK)+1;
          goto w22;
        case 0x0A: //DJNZ R2,rel
          n=(datamem[PSW] & REGBANK)+2;
          goto w22;
        case 0x0B: //DJNZ R3,rel
          n=(datamem[PSW] & REGBANK)+3;
          goto w22;
        case 0x0C: //DJNZ R4,rel
          n=(datamem[PSW] & REGBANK)+4;
          goto w22;
        case 0x0D: //DJNZ R5,rel
          n=(datamem[PSW] & REGBANK)+5;
          goto w22;
        case 0x0E: //DJNZ R6,rel
          n=(datamem[PSW] & REGBANK)+6;
          goto w22;
        case 0x0F: //DJNZ R7,rel
          n=(datamem[PSW] & REGBANK)+7;
w22:
          CyclCnt+=24;
          rel=(SHORT)((CHAR)codemem[PC++]);
          datamem[n]--;
          if(datamem[n])
            PC+=rel;
          break;
      }

      goto ex;
    }
    if(cmd<0xF0)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //MOVX A,@DPTR
          CyclCnt+=24;
          n=(USHORT)datamem[DPL] + ((USHORT)(datamem[DPH])<<8);
          datamem[AKKU]=(UCHAR)xdatamem[n];
          SetParity(datamem[AKKU]);
          break;
        case 0x01: //AJMP page7
          CyclCnt+=24;
          PC=(PC & 0xF800) + (codemem[PC]&0xFF) +0x700;
          break;
        case 0x02: //MOVX A,@R0
          n=(datamem[PSW] & REGBANK); //adresse von R0
          goto w23;
        case 0x03: //MOVX A,@R1
          n=(datamem[PSW] & REGBANK)+1; //adresse von R1
w23:
          CyclCnt+=24;
          n=datamem[n];  //Ri
          n=(USHORT)n + ((USHORT)datamem[PORT2]<<8);
          datamem[AKKU]=(UCHAR)xdatamem[n];
          SetParity(datamem[AKKU]);
          break;
        case 0x04: //CLR A
          CyclCnt+=12;
          datamem[AKKU]=0;
          datamem[PSW] &= ~PARITY;
          break;
        case 0x05: //MOV A,dadr
          n=datamem[(UCHAR)codemem[PC++]];
          goto w24;
        case 0x06: //MOV A,@R0
          n=(datamem[PSW] & REGBANK);  //adresse von R0
          goto w25;
        case 0x07: //MOV A,@R1
          n=(datamem[PSW] & REGBANK)+1;  //adresse von R1
w25:
          n=datamem[n]; //Ri
          if(n>0x7F)
            n=idatamem[n-0x80];
          else
            n=datamem[n];
          goto w24;
        case 0x08: //MOV A,R0
          n=(datamem[PSW] & REGBANK);  //adresse von R0
          n=datamem[n]; //R0
          goto w24;
        case 0x09: //MOV A,R1
          n=(datamem[PSW] & REGBANK)+1;  //adresse von R1
          n=datamem[n]; //R1
          goto w24;
        case 0x0A: //MOV A,R2
          n=(datamem[PSW] & REGBANK)+2;  //adresse von R2
          n=datamem[n]; //R2
          goto w24;
        case 0x0B: //MOV A,R3
          n=(datamem[PSW] & REGBANK)+3;  //adresse von R3
          n=datamem[n]; //R3
          goto w24;
        case 0x0C: //MOV A,R4
          n=(datamem[PSW] & REGBANK)+4;  //adresse von R4
          n=datamem[n]; //R4
          goto w24;
        case 0x0D: //MOV A,R5
          n=(datamem[PSW] & REGBANK)+5;  //adresse von R5
          n=datamem[n]; //R5
          goto w24;
        case 0x0E: //MOV A,R6
          n=(datamem[PSW] & REGBANK)+6;  //adresse von R6
          n=datamem[n]; //R6
          goto w24;
        case 0x0F: //MOV A,R7
          n=(datamem[PSW] & REGBANK)+7;  //adresse von R7
          n=datamem[n]; //R7
w24:
          CyclCnt+=12;
          datamem[AKKU]=(UCHAR)n;
          SetParity(datamem[AKKU]);
          break;
      }
      goto ex;
    }
    switch(cmd & 0x0F)
    {
      case 0x00: //MOVX @DPTR,A
        CyclCnt+=24;
        n=(USHORT)datamem[DPL] + ((USHORT)(datamem[DPH])<<8);
        xdatamem[n]=datamem[AKKU];
        break;
      case 0x01: //ACALL page7
        CyclCnt+=24;
        calladdr=PC-1;
        retAddr=PC+1;
        hPC=PC+1;
        retval=SetSPT(+1);
        i=datamem[SPT];
        if(i>0x7F)
        {
          idatamem[i-0x80]=(UCHAR)(hPC & 0xFF);  //Low ablegen
          i++;
        }
        else
          datamem[i++]=(UCHAR)(hPC & 0xFF);  //Low ablegen
        retval|=SetSPT(+1);
        if(i>0x7F)
          idatamem[i-0x80]=(UCHAR)(hPC>>8);  //High ablegen
        else
          datamem[i]=(UCHAR)(hPC>>8);  //High ablegen
        n=(UCHAR)codemem[PC];
        PC&=0xF800;
        PC|=(USHORT)(0x700 + (n & 0xFF));
        ::AddToCallStackWnd(calladdr,retAddr);
        break;
      case 0x02: //MOVX @R0,A
        n=(datamem[PSW] & REGBANK); //adresse von R0
        goto w26;
      case 0x03: //MOVX @R1,A
        n=(datamem[PSW] & REGBANK)+1; //adresse von R1
w26:
        CyclCnt+=12;
        n=datamem[n];  //Ri
        i=n+((USHORT)datamem[PORT2]<<8);
        (UCHAR)xdatamem[i]=datamem[AKKU];
        break;
      case 0x04: //CPL A
        CyclCnt+=12;
        datamem[AKKU]=~datamem[AKKU];
        SetParity(datamem[AKKU]);
        break;
      case 0x05: //MOV dadr,A
        n=(UCHAR)codemem[PC++];  //Adresse von dadr
        if(n==S0BUF || n==SCON)
        {
          if(n==S0BUF)
            IsSerChar=TRUE;
          else if(datamem[SCON]&2 )  //muss SCON sein
            GenerateTXInt();
        }
        goto w27;
      case 0x06: //MOV @R0,A
        n=(datamem[PSW] & REGBANK); //Adresse von R0
        goto w28;
      case 0x07: //MOV @R1,A
        n=(datamem[PSW] & REGBANK)+1; //Adresse von R1
w28:
        CyclCnt+=12;
        n=datamem[n]; //Ri
        if(n>0x7F)
          idatamem[n-0x80]=datamem[AKKU];
        else
          datamem[n]=datamem[AKKU];
        break;
      case 0x08: //MOV R0,A
        n=(datamem[PSW] & REGBANK); //Adresse von R0
        goto w27;
      case 0x09: //MOV R1,A
        n=(datamem[PSW] & REGBANK)+1; //Adresse von R1
        goto w27;
      case 0x0A: //MOV R2,A
        n=(datamem[PSW] & REGBANK)+2; //Adresse von R2
        goto w27;
      case 0x0B: //MOV R3,A
        n=(datamem[PSW] & REGBANK)+3; //Adresse von R3
        goto w27;
      case 0x0C: //MOV R4,A
        n=(datamem[PSW] & REGBANK)+4; //Adresse von R4
        goto w27;
      case 0x0D: //MOV R5,A
        n=(datamem[PSW] & REGBANK)+5; //Adresse von R5
        goto w27;
      case 0x0E: //MOV R6,A
        n=(datamem[PSW] & REGBANK)+6; //Adresse von R6
        goto w27;
      case 0x0F: //MOV R7,A
        n=(datamem[PSW] & REGBANK)+7; //Adresse von R7
w27:
        CyclCnt+=12;
        datamem[n]=datamem[AKKU];
        break;
    }
ex:
    if(traceptr) //ASM-Trace schreiben
    {
      if(tracelen==MAXTRACELEN)
      {
        traceptr->Flush();
        traceptr->GetFile()->SeekToBegin();
        tracelen=0;
      }
      WriteCurrentASMTraceRecord(pc);
      tracelen++;
    }

    if(activeMeasure)
    {
      if( codemem[PC] & RUNTIMEMPT ) //der nächste Befehl ist ein Messpunkt
      {
        measurepoint->cycles+= CyclCnt;
        if(CyclCnt <  measurepoint->MinCycles)
          measurepoint->MinCycles=CyclCnt;
        else if(CyclCnt >  measurepoint->MaxCycles)
          measurepoint->MaxCycles=CyclCnt;
        measurepoint->measures++;
      }
    }

    HandleTimers((CyclCnt-lastCycleCnt)/12);
    return 0;
  }
  else  // cmd >0xFF ->breakpoint ,Tracpoint,Messpunkt PC
  {
    if(tracetyp==TRACE_WATCH && (cmd & TRACEPOINT)) //der nächste Befehl ist ein Tracepunkt
    {
      ::WriteCurrentTraceRecord(tracetyp,pc);
    }

    if(activeMeasure && (cmd & BKPT_CODE))  //bei eingeschalteter Messung wird bei jedem
      CyclCnt=0;                          //Breakpoint der Zähler gelöscht
    if(  (cmd & (BKPT_CODE|BKPT_DISABLED)) == BKPT_CODE
         ||(cmd & BKPT_TMP)) //aktiver Breakpunkt
    return -1;
    if( cmd & STIMUPT)
    {
      ::DoNextStimulation();
      cmd &=0x00FF;
      goto exstart;
    }
    else
    {
      cmd &=0x00FF;
      goto exstart;
    }
  }
  return 0;
}

int CProc51::GetCodeMem(USHORT addr, USHORT* val)
{
  //IDS_NOMEMACCESS  "ungültiger Bereich"
  //IDS_NOMEMACCESS  "invalid range "

  if(!IsMemAtAddress(addr,'C'))
  {
    CString msgtxt;
    CString addrtxt;
    msgtxt.LoadString(IDS_NOMEMACCESS);
    msgtxt+=" CODE";
    if(language==_GER)
      addrtxt.Format("Zugriff auf nicht vorhandenen CODE-Speicherbereich:\nAdresse:0x%4.4X \nProgrammzähler: 0x%4.4X",addr,pc);
    else
      addrtxt.Format("Access to unavailable CODE-range:\nAddress:0x%4.4X \nProgram counter: 0x%4.4X",addr,pc);
    ::MessageBox(hMWnd, addrtxt,msgtxt, MB_OK|MB_ICONSTOP);
    retval=-1;
    return retval;
  }
  *val=codemem[addr] &0x00FF;
  if(codemem[addr] & BKPT_READ)
  {
    CAccessDlg accessdlg;
    if(language==_GER)
      accessdlg.memtxt.LoadString(IDS_READACCESS);
    else
      accessdlg.memtxt.LoadString(IDS_READACCESS_E);
    accessdlg.memtxt="CODE-" + accessdlg.memtxt;
    accessdlg.addrtxt.Format("0x%4.4X",addr);
    accessdlg.pctxt.Format("0x%4.4X",pc);
    retval=accessdlg.DoModal();
    return retval;
  }
  return 0;
}

int CProc51::GetXdataMem(USHORT addr, USHORT* val)
{
  if(!IsMemAtAddress(addr,'X'))
  {
    CAccessDlg accessdlg;
    if(language==_GER)
      accessdlg.memtxt.LoadString(IDS_NOMEMACCESS);
    else
      accessdlg.memtxt.LoadString(IDS_NOMEMACCESS_E);
    accessdlg.addrtxt.Format("XDATA 0x%4.4X",addr);
    accessdlg.pctxt.Format("0x%4.4X",pc);
    retval=accessdlg.DoModal();
    return retval;
  }
  *val=xdatamem[addr];
  if(xdatamemfmt[addr] & (BKPT_READ>>8))
  {
    CAccessDlg accessdlg;
    if(language==_GER)
      accessdlg.memtxt.LoadString(IDS_READACCESS);
    else
      accessdlg.memtxt.LoadString(IDS_READACCESS_E);
    accessdlg.memtxt="XDATA-" + accessdlg.memtxt;
    accessdlg.addrtxt.Format("0x%4.4X",addr);
    accessdlg.pctxt.Format("0x%4.4X",pc);
    retval=accessdlg.DoModal();
    return retval;
  }
  return 0;
}

int CProc51::GetIdataMem(USHORT addr, USHORT* val)
{
  if(!IsMemAtAddress(addr+0x80,'I'))
  {
    CAccessDlg accessdlg;
    if(language==_GER)
      accessdlg.memtxt.LoadString(IDS_NOMEMACCESS);
    else
      accessdlg.memtxt.LoadString(IDS_NOMEMACCESS_E);
    accessdlg.addrtxt.Format("IDATA 0x%2.2X",addr+0x80);
    accessdlg.pctxt.Format("0x%4.4X",pc);
    retval=accessdlg.DoModal();
    return retval;
  }
  *val=idatamem[addr];
  if(idatamemfmt[addr] & (BKPT_READ>>8))
  {
    CAccessDlg accessdlg;
    if(language==_GER)
      accessdlg.memtxt.LoadString(IDS_READACCESS);
    else
      accessdlg.memtxt.LoadString(IDS_READACCESS_E);
    accessdlg.memtxt="IDATA-" + accessdlg.memtxt;
    accessdlg.addrtxt.Format("0x%2.2X",addr+0x80);
    accessdlg.pctxt.Format("0x%4.4X",pc);
    retval=accessdlg.DoModal();
    return retval;
  }
  return 0;
}

int CProc51::GetDataMem(USHORT addr, USHORT*val)
{
  *val=datamem[addr];
  if(datamemfmt[addr] & (BKPT_READ>>8))
  {
    CAccessDlg accessdlg;
    accessdlg.memtxt.LoadString(IDS_READACCESS);
    accessdlg.memtxt="DATA-" + accessdlg.memtxt;
    if(addr < 0x80)
      accessdlg.addrtxt.Format("%2.2X",addr);
    else
      accessdlg.addrtxt=SReg510[addr-0x80];
    accessdlg.pctxt.Format("0x%4.4X",pc);
    retval=accessdlg.DoModal();
    return retval;
  }
  return 0;
}

int CProc51::SetIdataMem(USHORT addr, USHORT val)
{
  if(!IsMemAtAddress(addr+0x80,'I'))
  {
    CAccessDlg accessdlg;
    if(language==_GER)
      accessdlg.memtxt.LoadString(IDS_NOMEMACCESS);
    else
      accessdlg.memtxt.LoadString(IDS_NOMEMACCESS_E);
    accessdlg.addrtxt.Format("IDATA 0x%2.2X",addr+0x80);
    accessdlg.pctxt.Format("0x%4.4X",pc);
    retval=accessdlg.DoModal();
    return retval;
  }
  idatamem[addr]=val&0xFF;
  if(idatamemfmt[addr] & (BKPT_WRITE>>8))
  {
    CAccessDlg accessdlg;
    if(language==_GER)
      accessdlg.memtxt.LoadString(IDS_WRITEACCESS);
    else
      accessdlg.memtxt.LoadString(IDS_WRITEACCESS_E);
    accessdlg.memtxt="IDATA-" + accessdlg.memtxt;
    accessdlg.addrtxt.Format("0x%2.2X",addr+0x80);
    accessdlg.pctxt.Format("0x%4.4X",pc);
    retval=accessdlg.DoModal();
    return retval;
  }
  return 0;
}

int CProc51::SetXdataMem(USHORT addr, USHORT val)
{
  if(!IsMemAtAddress(addr,'X'))
  {
    CAccessDlg accessdlg;
    if(language==_GER)
      accessdlg.memtxt.LoadString(IDS_NOMEMACCESS);
    else
      accessdlg.memtxt.LoadString(IDS_NOMEMACCESS_E);
    accessdlg.addrtxt.Format("XDATA 0x%4.4X",addr);
    accessdlg.pctxt.Format("0x%4.4X",pc);
    retval=accessdlg.DoModal();
    return retval;
  }
  xdatamem[addr]=val&0xFF;
  if(xdatamemfmt[addr] & (BKPT_WRITE>>8))
  {
    CAccessDlg accessdlg;
    if(language==_GER)
      accessdlg.memtxt.LoadString(IDS_WRITEACCESS);
    else
      accessdlg.memtxt.LoadString(IDS_WRITEACCESS_E);
    accessdlg.memtxt="XDATA-" + accessdlg.memtxt;
    accessdlg.addrtxt.Format("0x%4.4X",addr);
    accessdlg.pctxt.Format("0x%4.4X",pc);
    retval=accessdlg.DoModal();
    return retval;
  }
  return 0;
}

int CProc51::SetDataMem(USHORT addr, USHORT val)
{
  datamem[addr]=val&0xFF;
  if(addr==AKKU)
    SetParity(datamem[AKKU]);
  if(datamemfmt[addr] & (BKPT_WRITE>>8))
  {
    CAccessDlg accessdlg;
    accessdlg.memtxt.LoadString(IDS_WRITEACCESS);
    accessdlg.memtxt="DATA-" + accessdlg.memtxt;
    accessdlg.addrtxt.Format("0x%2.2X",addr);
    accessdlg.pctxt.Format("0x%4.4X",pc);
    retval=accessdlg.DoModal();
    return retval;
  }
  return 0;
}

//führt ein Kommando aus, mit Überwachung der Speicherzugriffe
int CProc51::ExecNextCmd1()
{

  USHORT cmd;
  USHORT n,i,a,hPC;
  SHORT rel;
  BOOL b;
  ULONG retAddr,calladdr;
  USHORT bval,bval1;

  pc=(USHORT)PC;

iw1:
  if((datamem[0xA8] & 0x80)
     && intReq
     && (actInt^intReq) & (enabledInt=datamem[0xA8]&0x7F | (datamem[0xA8]&0x7F)*256)
     && (((actInt^intReq) & enabledInt)<actInt
     || !actInt))
  {  //Interruptanmeldung die noch nicht bearbeitet wird und höher priorisiert ist

    actInt=0x0001;
    while(!(actInt & intReq))  //den höchst prioren Interrupt raussuchen
      actInt*=2;

    CyclCnt+=36; //das muss noch überprüft werden
    retAddr=PC;
    calladdr=PC;
    retval=SetSPT(+1);
    GetDataMem(SPT,&i); //aktueller Stackzeiger
    if(i>0x7F)
    {
      SetIdataMem(i-0x80,(UCHAR)(PC & 0xFF));  //Low ablegen
      i++;
    }
    else
    {
      SetDataMem(i++,(UCHAR)(PC & 0xFF));  //Low ablegen
    }
    retval|=SetSPT(+1);
    if(i>0x7F)
      SetIdataMem(i-0x80,(UCHAR)(PC>>8));  //High ablegen
    else
      SetDataMem(i,(UCHAR)(PC>>8));  //High ablegen
    switch(actInt)
    {
      case 0x0001:
      case 0x0100: PC=0x0003;  //extInt0
        if(datamem[TCON] & 0x01)
          datamem[TCON] &= ~0x02; //Anmeldebit rücksetzen wenn FlankenInt
        break;
      case 0x0002:
      case 0x0200: PC=0x000B;  //timer0
        datamem[TCON] &= ~0x20;  //Anmeldebit rücksetzen
        break;
      case 0x0004:
      case 0x0400: PC=0x0013;  //extInt1
        if(datamem[TCON] & 0x04)
          datamem[TCON] &= ~0x08; //Anmeldebit rücksetzen wenn FlankenInt
        break;
      case 0x0008:
      case 0x0800: PC=0x001B;  //timer1
        datamem[TCON] &= ~0x80;     //Anmeldebit rücksetzen
        break;
      case 0x0010:
      case 0x1000: PC=0x0023;  //Serial
        break;
      case 0x0020:
      case 0x2000: PC=0x002B;
        break;
      case 0x0040:
      case 0x4000: PC=0x0033;
        break;
      case 0x0080:
      case 0x8000: PC=0x003B;
        break;
    }
    ::AddToCallStackWnd(calladdr,retAddr);
    intReq &= ~actInt; //den aktuellen Interrupt löschen
    actInt=0;
    return retval;
  }
  if(GetCodeMem((USHORT)PC,&cmd))
    return retval;
  cmd=codemem[(USHORT)PC];
  if(!(cmd &0xFF00))
  {
exstart:
    PC++;
    cmd &=0x00FF;
    if(cmd<0x10)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //NOP
          CyclCnt+=12;
          break;
        case 0x01: //AJMP Page0
          CyclCnt+=24;
          GetCodeMem(PC,&bval);
          PC=(PC & 0xF800) + (bval & 0xFF);
          break;
        case 0x02: //LJMP adr16
          CyclCnt+=24;
          GetCodeMem(PC,&bval);
          GetCodeMem(PC+1,&bval1);
          PC=bval*256 + (bval1 & 0xFF);
          break;
        case 0x03: //RR A
          CyclCnt+=12;
          GetDataMem(AKKU,&a);
          if(a &0x01)
            SetDataMem(AKKU,(a>>1)|0x80);
          else
            SetDataMem(AKKU,a>>1);
          break;
        case 0x04: //INC A
          CyclCnt+=12;
          SetDataMem(AKKU,datamem[AKKU]+1);
          break;
        case 0x05: //INC dadr
          CyclCnt+=12;
          GetCodeMem(PC,&bval);
          PC++;
          n=(UCHAR)bval;
          SetDataMem(n,datamem[n]+1);
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          break;
        case 0x06: //INC @R0
          i=datamem[PSW] & REGBANK; //adresse von R0
          goto w11;
        case 0x07: //INC @R1
          i=(datamem[PSW] & REGBANK) +1;
w11:
          CyclCnt+=12;
          n=datamem[i];             //Ri
          if(n>0x7F)
            SetIdataMem(n-0x80,idatamem[n-0x80]+1);
          else
            SetDataMem(n,datamem[n]+1);
          break;
        case 0x08: //INC R0
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          datamem[n]++;
          break;
        case 0x09: //INC R1
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          SetDataMem(n+1,datamem[n+1]+1);
          break;
        case 0x0A: //INC R2
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          SetDataMem(n+2,datamem[n+2]+1);
          break;
        case 0x0B: //INC R3
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          SetDataMem(n+3,datamem[n+3]+1);
          break;
        case 0x0C: //INC R4
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          SetDataMem(n+4,datamem[n+4]+1);
          break;
        case 0x0D: //INC R5
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          SetDataMem(n+5,datamem[n+5]+1);
          break;
        case 0x0E: //INC R6
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          SetDataMem(n+6,datamem[n+6]+1);
          break;
        case 0x0F: //INC R7
          CyclCnt+=12;
          n=datamem[PSW] & REGBANK;
          SetDataMem(n+7,datamem[n+7]+1);
          break;
      }
      goto ex;
    }
    if(cmd<0x20)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //JBC badr,rel
          CyclCnt+=24;
          GetCodeMem(PC++,&bval);
          b=GetBitState((UCHAR)bval,TRUE);
          if(b)
          {
            GetCodeMem(PC,&bval);
            rel=(SHORT)((CHAR)(bval)+1);
            PC=PC+rel;
          }
          else
            PC++;
          break;
        case 0x01: //ACALL page0
          CyclCnt+=24;
          calladdr=PC-1;
          retAddr=PC+1;
          retval=SetSPT(+1);
          i=datamem[SPT];
          if(i>0x7F)
          {
            SetIdataMem(i-0x80,(UCHAR)(retAddr & 0xFF));  //Low ablegen
            i++;
          }
          else
            SetDataMem(i++,(UCHAR)(retAddr & 0xFF));  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            SetIdataMem(i-0x80,(UCHAR)(retAddr>>8));  //High ablegen
          else
            SetDataMem(i,(UCHAR)(retAddr>>8));  //High ablegen
          GetCodeMem(PC,&n);
          PC&=0xF800;
          PC|=(USHORT)(n & 0xFF);
          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x02: //LCALL adr16
          retAddr=PC+2;
          calladdr=PC-1;
          CyclCnt+=24;
          hPC=PC+2;
          retval=SetSPT(+1);
          i=datamem[SPT];  //aktueller Stackzeiger
          if(i>0x7F)
          {
            SetIdataMem(i-0x80,(UCHAR)(hPC & 0xFF));  //Low ablegen
            i++;
          }
          else
            SetDataMem(i++,(UCHAR)(hPC & 0xFF));  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            SetIdataMem(i-0x80,(UCHAR)(hPC>>8));  //High ablegen
          else
            SetDataMem(i,(UCHAR)(hPC>>8));  //High ablegen
          GetCodeMem(PC,&bval);
          GetCodeMem(PC+1,&bval1);
          PC=(USHORT)(bval<<8) | (bval1&0xFF);
          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x03: //RRC A
          CyclCnt+=12;
          a=datamem[AKKU];
          n=datamem[PSW];
          a=a>>1;
          if(n & CARRY)
            a |= 0x80;

          if(datamem[AKKU] & 1)
            datamem[PSW] |= CARRY;   //carry setzen
          else
            datamem[PSW] &=~CARRY;   //carry löschen
          SetDataMem(AKKU,a);
          break;
        case 0x04: //DEC A
          CyclCnt+=12;
          SetDataMem(AKKU,datamem[AKKU]-1);
          break;
        case 0x05: //DEC dadr
          CyclCnt+=24;
          GetCodeMem(PC++,&n);
          SetDataMem(n,datamem[n]-1);
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          break;
        case 0x06: //DEC @R0
          i=datamem[PSW] & REGBANK;
          goto w12;
        case 0x07: //DEC @R1
          i=(datamem[PSW] & REGBANK)+1;
w12:
          CyclCnt+=12;
          n=datamem[i];
          if(n>0x7F)
            SetIdataMem(n-0x80,idatamem[n-0x80]-1);
          else
            SetDataMem(n,datamem[n]-1);
          break;
        case 0x08: //DEC R0
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          SetDataMem(i,datamem[i]-1);
          break;
        case 0x09: //DEC R1
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          SetDataMem(i+1,datamem[i+1]-1);
          break;
        case 0x0A: //DEC R2
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          SetDataMem(i+2,datamem[i+2]-1);
          break;
        case 0x0B: //DEC R3
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          SetDataMem(i+3,datamem[i+3]-1);
          break;
        case 0x0C: //DEC R4
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          SetDataMem(i+4,datamem[i+4]-1);
          break;
        case 0x0D: //DEC R5
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          SetDataMem(i+5,datamem[i+5]-1);
          break;
        case 0x0E: //DEC R6
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          SetDataMem(i+6,datamem[i+6]-1);
          break;
        case 0x0F: //DEC R7
          CyclCnt+=12;
          i=datamem[PSW] & REGBANK;
          SetDataMem(i+7,datamem[i+7]-1);
          break;
      }
      goto ex;
    }
    if(cmd<0x30)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //JB badr,rel
          CyclCnt+=24;
          GetCodeMem(PC++,&bval);
          b=GetBitState((UCHAR)bval,FALSE);
          if(b)
          {
            GetCodeMem(PC,&bval);
            rel=(SHORT)((CHAR)(bval))+1;
            PC=PC+rel;
          }
          else
            PC++;
          break;
        case 0x01: //AJMP page1
          CyclCnt+=24;
          GetCodeMem(PC,&bval);
          PC=(PC & 0xF800) + (bval&0xFF)+0x100;
          break;
        case 0x02: //RET
          CyclCnt+=24;
          //high zurückholen
          n=datamem[SPT];
          if(n<0x80)
          {

            GetDataMem(n,&bval);
            PC=(USHORT)(bval<<8);
          }

          else
          {
            GetIdataMem(n-0x80,&bval);
            PC=(USHORT)(bval<<8);
          }
          retval=SetSPT(-1);
          //low zurückholen
          n=datamem[SPT];
          if(n<0x80)
          {
            GetDataMem(n,&bval);
            PC+=(USHORT)bval;
          }
          else
          {
            GetIdataMem(n-0x80,&bval);
            PC+=(USHORT)bval;
          }
          retval|=SetSPT(-1);
          ::RemoveFromCallStackWnd(PC);
          break;
        case 0x03: //RL A
          CyclCnt+=12;
          a=datamem[AKKU];
          if(a & 0x80)
            SetDataMem(AKKU,(a<<1) | 0x01);
          else
            SetDataMem(AKKU,a<<1);
          break;
        case 0x04: //ADD A,#const8
          GetCodeMem(PC++,&n);
          goto w0;
        case 0x05: //ADD A,dadr
          GetCodeMem(PC++,&bval);
          GetDataMem((UCHAR)bval,&n);
          goto w0;
        case 0x06: //ADD A,@R0
          n=datamem[PSW] & REGBANK;
          n=datamem[n];  //R0
          if(n>0x7F)
            GetIdataMem(n-0x80,&n);
          else
            GetDataMem(n,&n);   //@R0
          goto w0;
        case 0x07: //ADD A,@R1
          n=datamem[PSW] & REGBANK;
          n=datamem[n+1];  //R1
          if(n>0x7F)
            GetIdataMem(n-0x80,&n);
          else
            GetDataMem(n,&n);  //@R1
          goto w0;
        case 0x08: //ADD A,R0
          n=datamem[PSW] & REGBANK;
          GetDataMem(n,&n);    //R0
          goto w0;
        case 0x09: //ADD A,R1
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+1,&n);  //R1
          goto w0;
        case 0x0A: //ADD A,R2
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+2,&n);  //R2
          goto w0;
        case 0x0B: //ADD A,R3
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+3,&n);  //R3
          goto w0;
        case 0x0C: //ADD A,R4
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+4,&n);  //R4
          goto w0;
        case 0x0D: //ADD A,R5
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+5,&n);  //R5
          goto w0;
        case 0x0E: //ADD A,R6
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+6,&n);  //R6
          goto w0;
        case 0x0F: //ADD A,R7
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+7,&n); //R7
w0:
          CyclCnt+=12;
          a=datamem[AKKU];
          i=(USHORT)a;
          i+=n;
          SetDataMem(AKKU,(UCHAR)i);
          if(i & 0x0100)
            datamem[PSW] |= CARRY;  // carry setzen
          else
            datamem[PSW] &= ~CARRY;
          if(  (a<0x80 && (UCHAR)n<0x80 && (UCHAR)i>0x80)
               ||(a>0x80 && (UCHAR)n>0x80 && (UCHAR)i<0x80))
          datamem[PSW] |= OV;
          else
            datamem[PSW] &= ~OV;

          if((a&0x0F)+(n&0x0F)>0x0F )
            datamem[PSW] |= AC;
          else
            datamem[PSW] &= ~AC;
          break;
      }
      goto ex;
    }
    if(cmd<0x40)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //JNB badr,rel
          CyclCnt+=24;
          GetCodeMem(PC++,&bval);
          b=GetBitState((UCHAR)bval,FALSE);
          if(!b)
          {
            GetCodeMem(PC,&bval);
            rel=(SHORT)((CHAR)(bval))+1;
            PC=PC+rel;
          }
          else
            PC++;
          break;
        case 0x01: //ACALL page1
          CyclCnt+=24;
          retAddr=PC+1;
          calladdr=PC-1;
          retval=SetSPT(+1);
          i=datamem[SPT];
          if(i>0x7F)
          {
            SetIdataMem(i-0x80,(UCHAR)(retAddr & 0xFF));  //Low ablegen
            i++;
          }
          else
            SetDataMem(i++,(UCHAR)(retAddr & 0xFF));  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            SetIdataMem(i-0x80,(UCHAR)(retAddr>>8));  //High ablegen
          else
            SetDataMem(i,(UCHAR)(retAddr>>8));  //High ablegen
          GetCodeMem(PC,&n);
          PC&=0xF800;
          PC|=(USHORT)(0x100 + (n & 0xFF));
          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x02: //RETI
          CyclCnt+=24;
          //high zurückholen
          n=datamem[SPT];
          if(n<0x80)
          {
            GetDataMem(n,&bval);
            PC=(USHORT)(bval<<8);
          }
          else
          {

            GetIdataMem(n-0x80,&bval);
            PC=(USHORT)(bval<<8);
          }
          retval=SetSPT(-1);
          //low zurückholen
          n=datamem[SPT];
          if(n<0x80)
          {
            GetDataMem(n,&bval);
            PC+=(USHORT)bval;
          }
          else
          {
            GetIdataMem(n-0x80,&bval);
            PC+=(USHORT)bval;
          }
          retval|=SetSPT(-1);
          ::RemoveFromCallStackWnd(PC);
          if(intReq) //es liegen noch Interrupts an
            goto iw1;
          break;
        case 0x03: //RLC A
          CyclCnt+=12;
          a=datamem[AKKU];
          i=datamem[PSW];
          a=a<<1;
          if(i & CARRY)
            a|= 0x01;
          if(datamem[AKKU] & 0x80)
            datamem[PSW] |= CARRY;
          else
            datamem[PSW] &= ~CARRY;
          SetDataMem(AKKU,a);
          break;
        case 0x04: //ADDC A,#const8
          GetCodeMem(PC++,&n);
          goto w1;
        case 0x05: //ADDC A,dadr
          GetCodeMem(PC++,&bval);
          GetDataMem(bval,&n);
          goto w1;
        case 0x06: //ADDC A,@R0
          n=datamem[PSW] & REGBANK;
          n=datamem[n];  //R0
          if(n>0x7F)
            GetIdataMem(n-0x80,&n);
          else
            GetDataMem(n,&n);  //@R0
          goto w1;
        case 0x07: //ADDC A,@R1
          n=datamem[PSW] & REGBANK;
          n=datamem[n+1];  //R1
          if(n>0x7F)
            GetIdataMem(n-0x80,&n);
          else
            GetDataMem(n,&n); //@R1
          goto w1;
        case 0x08: //ADDC A,R0
          n=datamem[PSW] & REGBANK;
          GetDataMem(n,&n);    //R0
          goto w1;
        case 0x09: //ADDC A,R1
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+1,&n);  //R1
          goto w1;
        case 0x0A: //ADDC A,R2
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+2,&n);  //R2
          goto w1;
        case 0x0B: //ADDC A,R3
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+3,&n);  //R3
          goto w1;
        case 0x0C: //ADDC A,R4
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+4,&n);  //R4
          goto w1;
        case 0x0D: //ADDC A,R5
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+5,&n);  //R5
          goto w1;
        case 0x0E: //ADDC A,R6
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+6,&n);  //R6
          goto w1;
        case 0x0F: //ADDC A,R7
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+7,&n);  //R7
w1:
          CyclCnt+=12;
          a=datamem[AKKU];
          i=(USHORT)a;
          if(datamem[PSW] & CARRY)
            n++;
          i+=n;
          SetDataMem(AKKU,(UCHAR)i);
          if(i & 0x0100)
            datamem[PSW] |= CARRY;  // carry setzen
          else
            datamem[PSW] &= ~CARRY;
          if(  (a<0x80 && (UCHAR)n<0x80 && (UCHAR)i>0x80)
               ||(a>0x80 && (UCHAR)n>0x80 && (UCHAR)i<0x80))
          datamem[PSW] |= OV;
          else
            datamem[PSW] &= ~OV;

          if((a&0x0F)+(n&0x0F)>0x0F )
            datamem[PSW] |= AC;
          else
            datamem[PSW] &= ~AC;
          break;
      }
      goto ex;
    }
    if(cmd<0x50)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //JC rel
          CyclCnt+=24;
          if(datamem[PSW] & CARRY)
          {
            GetCodeMem(PC,&bval);
            rel=(SHORT)((CHAR)(bval))+1;
            PC=PC+rel;
          }
          else
            PC++;
          break;
        case 0x01: //AJMP page2
          CyclCnt+=24;
          GetCodeMem(PC,&bval);
          PC=(PC & 0xF800) + (bval&0xFF)+0x200;
          break;
        case 0x02: //ORL dadr,A
          CyclCnt+=12;
          GetCodeMem(PC++,&n);
          SetDataMem(n,datamem[n]|datamem[AKKU]);
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          break;
        case 0x03: //ORL dadr,#const8
          CyclCnt+=24;
          GetCodeMem(PC++,&n);
          GetCodeMem(PC++,&a);
          SetDataMem(n,datamem[n]|a);
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          break;
        case 0x04: //ORL A,#const8
          CyclCnt+=12;
          GetCodeMem(PC++,&bval);
          a=datamem[AKKU] | ((UCHAR)bval);
          SetDataMem(AKKU,a);
          break;
        case 0x05: //ORL A,dadr
          CyclCnt+=12;
          a=datamem[AKKU];
          GetCodeMem(PC++,&n);
          GetDataMem(n,&bval);
          a |= bval;
          SetDataMem(AKKU,a);
          break;
        case 0x06: //ORL A,@R0
          n=datamem[PSW] & REGBANK; // adresse von R0
          GetDataMem(n,&n); //R0
          goto w13;
        case 0x07: //ORL A,@R1
          n=(datamem[PSW] & REGBANK) +1; //adresse von R1
          GetDataMem(n,&n); //R1
w13:
          CyclCnt+=12;
          if(n>0x7F)
            GetIdataMem(n-0x80,&n);
          else
            GetDataMem(n,&n);  //@R0
          a=datamem[AKKU];
          a |= (UCHAR)n;
          SetDataMem(AKKU,a);
          break;
        case 0x08: //ORL A,R0
          n=(datamem[PSW] & REGBANK); //adresse von R0
          goto w2;
        case 0x09: //ORL A,R1
          n=(datamem[PSW] & REGBANK)+1; //adresse von R1
          goto w2;
        case 0x0A: //ORL A,R2
          n=(datamem[PSW] & REGBANK)+2; //adresse von R2
          goto w2;
        case 0x0B: //ORL A,R3
          n=(datamem[PSW] & REGBANK)+3; //adresse von R3
          goto w2;
        case 0x0C: //ORL A,R4
          n=(datamem[PSW] & REGBANK)+4; //adresse von R4
          goto w2;
        case 0x0D: //ORL A,R5
          n=(datamem[PSW] & REGBANK)+5; //adresse von R5
          goto w2;
        case 0x0E: //ORL A,R6
          n=(datamem[PSW] & REGBANK)+6; //adresse von R6
          goto w2;
        case 0x0F: //ORL A,R7
          n=(datamem[PSW] & REGBANK)+7; //adresse von R7
w2:
          CyclCnt+=12;
          a=datamem[AKKU];
          GetDataMem(n,&bval);
          a |=bval;
          SetDataMem(AKKU,a);
          break;

      }

      goto ex;
    }
    if(cmd<0x60)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //JNC rel
          CyclCnt+=24;
          if(!(datamem[PSW] & CARRY))
          {
            GetCodeMem(PC,&bval);
            rel=(SHORT)((CHAR)bval)+1;
            PC=PC+rel;
          }
          else
            PC++;
          break;
        case 0x01: //ACALL page2
          CyclCnt+=24;
          retAddr=PC+1;
          calladdr=PC-1;
          retval=SetSPT(+1);
          i=datamem[SPT];
          if(i>0x7F)
          {
            SetIdataMem(i-0x80,(UCHAR)(retAddr & 0xFF));  //Low ablegen
            i++;
          }
          else
            SetDataMem(i++,(UCHAR)(retAddr & 0xFF));  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            SetIdataMem(i-0x80,(UCHAR)(retAddr>>8));  //High ablegen
          else
            SetDataMem(i,(UCHAR)(retAddr>>8));  //High ablegen
          GetCodeMem(PC,&n);
          PC&=0xF800;
          PC|=(USHORT)(0x200 + (n & 0xFF));
          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x02: //ANL dadr,A
          CyclCnt+=12;
          GetCodeMem(PC++,&n);
          GetDataMem(AKKU,&bval);
          SetDataMem(n,datamem[n]&bval);
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          break;
        case 0x03: //ANL dadr,#const8
          CyclCnt+=24;
          GetCodeMem(PC++,&n);
          GetCodeMem(PC++,&a);
          SetDataMem(n,datamem[n] & a);
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          break;
        case 0x04: //ANL A,#const8
          CyclCnt+=12;
          GetCodeMem(PC++,&bval);
          a=datamem[AKKU] & ((UCHAR)bval);
          SetDataMem(AKKU,a);
          break;
        case 0x05: //ANL A,dadr
          CyclCnt+=12;
          a=datamem[AKKU];
          GetCodeMem(PC++,&n);
          GetDataMem(n,&bval);
          a = datamem[AKKU] & bval;
          SetDataMem(AKKU,a);
          break;
        case 0x06: //ANL A,@R0
          n=datamem[PSW] & REGBANK; // adresse von R0
          goto w14;
        case 0x07: //ANL A,@R1
          n=(datamem[PSW] & REGBANK) +1; //adresse von R1
w14:
          CyclCnt+=12;
          n=datamem[n]; //Rx
          if(n>0x7F)
            GetIdataMem(n-0x80,&n);
          else
            GetDataMem(n,&n);
          a=datamem[AKKU];
          a &= (UCHAR)n;
          SetDataMem(AKKU,a);
          break;
        case 0x08: //ANL A,R0
          n=(datamem[PSW] & REGBANK); //adresse von R0
          goto w3;
        case 0x09: //ANL A,R1
          n=(datamem[PSW] & REGBANK)+1; //adresse von R1
          goto w3;
        case 0x0A: //ANL A,R2
          n=(datamem[PSW] & REGBANK)+2; //adresse von R2
          goto w3;
        case 0x0B: //ANL A,R3
          n=(datamem[PSW] & REGBANK)+3; //adresse von R3
          goto w3;
        case 0x0C: //ANL A,R4
          n=(datamem[PSW] & REGBANK)+4; //adresse von R4
          goto w3;
        case 0x0D: //ANL A,R5
          n=(datamem[PSW] & REGBANK)+5; //adresse von R5
          goto w3;
        case 0x0E: //ANL A,R6
          n=(datamem[PSW] & REGBANK)+6; //adresse von R6
          goto w3;
        case 0x0F: //ANL A,R7
          n=(datamem[PSW] & REGBANK)+7; //adresse von R7
w3:
          CyclCnt+=12;
          a=datamem[AKKU];
          a &= datamem[n];
          SetDataMem(AKKU,a);
          break;

      }

      goto ex;
    }
    if(cmd<0x70)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //JZ rel
          CyclCnt+=24;
          if(datamem[AKKU])
            PC++;
          else
          {
            GetCodeMem(PC,&bval);
            rel=(SHORT)((CHAR)(bval))+1;
            PC=PC+rel;
          }
          break;
        case 0x01: //AJMP page3
          CyclCnt+=24;
          GetCodeMem(PC,&bval);
          PC=(PC & 0xF800) + (bval&0xFF)+0x300;
          break;
        case 0x02: //XRL dadr,A
          CyclCnt+=12;
          GetCodeMem(PC++,&n);
          GetDataMem(n,&bval);
          SetDataMem(n,bval^datamem[AKKU]);
          break;
        case 0x03: //XRL dadr,#const8
          CyclCnt+=24;
          GetCodeMem(PC++,&n);
          GetCodeMem(PC++,&a);
          SetDataMem(n,datamem[n]^a);
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          break;
        case 0x04: //XRL A,#const8
          CyclCnt+=12;
          GetCodeMem(PC++,&bval);
          a=datamem[AKKU] ^ ((UCHAR)bval);
          SetDataMem(AKKU,a);
          break;
        case 0x05: //XRL A,dadr
          CyclCnt+=12;
          a=datamem[AKKU];
          GetCodeMem(PC++,&n);
          GetDataMem(n,&bval);
          a ^= bval;
          SetDataMem(AKKU,a);
          break;
        case 0x06: //XRL A,@R0
          n=datamem[PSW] & REGBANK; // adresse von R0
          goto w15;
        case 0x07: //XRL A,@R1
          n=(datamem[PSW] & REGBANK) +1; //adresse von R1
w15:
          CyclCnt+=12;
          a=datamem[AKKU];
          n=datamem[n];  //Ri
          if(n>0x7F)
            GetIdataMem(n-0x80,&n);
          else
            GetDataMem(n,&n);    //@Ri
          a ^= (UCHAR)n;
          SetDataMem(AKKU,a);
          break;
        case 0x08: //XRL A,R0
          n=(datamem[PSW] & REGBANK); //adresse von R0
          goto w4;
        case 0x09: //XRL A,R1
          n=(datamem[PSW] & REGBANK)+1; //adresse von R1
          goto w4;
        case 0x0A: //XRL A,R2
          n=(datamem[PSW] & REGBANK)+2; //adresse von R2
          goto w4;
        case 0x0B: //XRL A,R3
          n=(datamem[PSW] & REGBANK)+3; //adresse von R3
          goto w4;
        case 0x0C: //XRL A,R4
          n=(datamem[PSW] & REGBANK)+4; //adresse von R4
          goto w4;
        case 0x0D: //XRL A,R5
          n=(datamem[PSW] & REGBANK)+5; //adresse von R5
          goto w4;
        case 0x0E: //XRL A,R6
          n=(datamem[PSW] & REGBANK)+6; //adresse von R6
          goto w4;
        case 0x0F: //XRL A,R7
          n=(datamem[PSW] & REGBANK)+7; //adresse von R7
w4:
          CyclCnt+=12;
          a=datamem[AKKU];
          GetDataMem(n,&bval);
          a ^= bval;
          SetDataMem(AKKU,a);
          break;

      }

      goto ex;
    }
    if(cmd<0x80)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //JNZ rel
          CyclCnt+=24;
          if(datamem[AKKU])
          {
            GetCodeMem(PC,&bval);
            rel=(SHORT)((CHAR)(bval)+1);
            PC=PC+rel;
          }
          else
            PC++;
          break;
        case 0x01: //ACALL page3
          CyclCnt+=24;
          retAddr=PC+1;
          calladdr=PC-1;
          retval=SetSPT(+1);
          i=datamem[SPT];
          if(i>0x7F)
          {
            SetIdataMem(i-0x80,(UCHAR)(retAddr & 0xFF));  //Low ablegen
            i++;
          }
          else
            SetDataMem(i++,(UCHAR)(retAddr & 0xFF));  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            SetIdataMem(i-0x80,(UCHAR)(retAddr>>8));  //High ablegen
          else
            SetDataMem(i,(UCHAR)(retAddr>>8));  //High ablegen
          GetCodeMem(PC,&n);
          PC&=0xF800;
          PC|=(USHORT)(0x300 + (n & 0xFF));
          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x02: //ORL C,badr
          CyclCnt+=24;
          GetCodeMem(PC++,&bval);
          b=GetBitState((UCHAR)bval,FALSE);
          if((datamem[PSW] & CARRY) || b)
            datamem[PSW] |=CARRY;
          else
            datamem[PSW] &= ~CARRY;
          break;
        case 0x03: //JMP @A+DPTR
          CyclCnt+=24;
          GetDataMem(AKKU,&n);
          GetDataMem(DPL,&bval);
          GetDataMem(DPH,&bval1);
          i=(USHORT)bval + ((USHORT)(bval1<<8));
          PC=(USHORT)n+i;
          break;
        case 0x04: //MOV A,#const8
          CyclCnt+=12;
          GetCodeMem(PC++,&bval);
          SetDataMem(AKKU,bval);
          break;
        case 0x05: //MOV dadr,#const8
          CyclCnt+=24;
          GetCodeMem(PC++,&n);
          GetCodeMem(PC++,&bval);
          datamem[n]=(UCHAR)bval;
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          break;
        case 0x06: //MOV @R0,#const8
          n=(datamem[PSW] & REGBANK); // adresse von R0
          goto w16;
        case 0x07: //MOV @R1,#const8
          n=(datamem[PSW] & REGBANK)+1; // adresse von R1
w16:
          CyclCnt+=12;
          n=datamem[n];                 // Ri
          if(n>0x7F)
          {
            GetCodeMem(PC++,&bval);
            SetIdataMem(n-0x80,(UCHAR)bval);
          }
          else
          {
            GetCodeMem(PC++,&bval);
            SetDataMem(n,bval);
          }
          break;
        case 0x08: //MOV R0,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK);  //adresse von R0
          GetCodeMem(PC++,&bval);
          SetDataMem(n,(UCHAR)bval);
          break;
        case 0x09: //MOV R1,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK)+1;  //adresse von R1
          GetCodeMem(PC++,&bval);
          SetDataMem(n,(UCHAR)bval);
          break;
        case 0x0A: //MOV R2,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK)+2;  //adresse von R2
          GetCodeMem(PC++,&bval);
          SetDataMem(n,(UCHAR)bval);
          break;
        case 0x0B: //MOV R3,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK)+3;  //adresse von R3
          GetCodeMem(PC++,&bval);
          SetDataMem(n,(UCHAR)bval);
          break;
        case 0x0C: //MOV R4,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK)+4;  //adresse von R4
          GetCodeMem(PC++,&bval);
          SetDataMem(n,(UCHAR)bval);
          break;
        case 0x0D: //MOV R5,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK)+5;  //adresse von R5
          GetCodeMem(PC++,&bval);
          SetDataMem(n,(UCHAR)bval);
          break;
        case 0x0E: //MOV R6,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK)+6;  //adresse von R6
          GetCodeMem(PC++,&bval);
          SetDataMem(n,(UCHAR)bval);
          break;
        case 0x0F: //MOV R7,#const8
          CyclCnt+=12;
          n=(datamem[PSW] & REGBANK)+7;  //adresse von R7
          GetCodeMem(PC++,&bval);
          SetDataMem(n,(UCHAR)bval);
          break;
      }
      goto ex;
    }
    if(cmd<0x90)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //SJMP rel
          CyclCnt+=24;
          GetCodeMem(PC,&bval);
          rel=(SHORT)((CHAR)(bval&0xFF))+1;
          PC=PC+rel;
          break;
        case 0x01: //AJMP page4
          CyclCnt+=24;
          GetCodeMem(PC,&bval);
          PC=(PC & 0xF800) + (bval&0xFF)+0x400;
          break;
        case 0x02: //ANL C,badr
          CyclCnt+=24;
          GetCodeMem(PC++,&bval);
          b=GetBitState((UCHAR)bval,FALSE);
          if((datamem[PSW] & CARRY) && b)
            datamem[PSW] |=CARRY;
          else
            datamem[PSW] &= ~CARRY;
          break;
        case 0x03: //MOVC A,@A+PC
          CyclCnt+=24;
          n=(USHORT)datamem[AKKU] + PC;
          GetCodeMem(n,&bval);
          SetDataMem(AKKU,(UCHAR)bval);
          break;
        case 0x04: //DIV AB
          CyclCnt+=48;
          datamem[PSW] &= ~(CARRY|OV);
          a=datamem[AKKU];
          GetDataMem(BREG,&bval);
          if(bval)
          {
            a /= bval;
            SetDataMem(BREG,datamem[AKKU] % bval);
            SetDataMem(AKKU,a);
          }
          else
          {

            datamem[PSW] |= OV;
          }
          break;
        case 0x05: //MOV dadr,dadr
          CyclCnt+=24;
          GetCodeMem(PC++,&n);
          GetCodeMem(PC++,&a);
          GetDataMem(n,&bval);
          SetDataMem(a,bval);
          if(a==S0BUF || a==SCON)
          {
            if(a==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          break;
        case 0x06: //MOV dadr,@R0
          n=datamem[PSW] & REGBANK; //adresse R0
          goto w17;
        case 0x07: //MOV dadr,@R1
          n=(datamem[PSW] & REGBANK) +1; //adresse R1
w17:
          CyclCnt+=24;
          n=datamem[n]; //Ri
          if(n>0x7F)
            GetIdataMem(n-0x80,&n); //@Ri
          else
            GetDataMem(n,&n);   //@Ri
          GetCodeMem(PC++,&i);
          SetDataMem(i,(UCHAR)n);
          if(i==S0BUF || i==SCON)
          {
            if(i==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
        case 0x08: //MOV dadr,R0
          n=datamem[PSW] & REGBANK; //adresse R0
          goto w5;
        case 0x09: //MOV dadr,R1
          n=(datamem[PSW] & REGBANK)+1; //adresse R1
          goto w5;
        case 0x0A: //MOV dadr,R2
          n=(datamem[PSW] & REGBANK)+2; //adresse R2
          goto w5;
        case 0x0B: //MOV dadr,R3
          n=(datamem[PSW] & REGBANK)+3; //adresse R3
          goto w5;
        case 0x0C: //MOV dadr,R4
          n=(datamem[PSW] & REGBANK)+4; //adresse R4
          goto w5;
        case 0x0D: //MOV dadr,R5
          n=(datamem[PSW] & REGBANK)+5; //adresse R5
          goto w5;
        case 0x0E: //MOV dadr,R6
          n=(datamem[PSW] & REGBANK)+6; //adresse R6
          goto w5;
        case 0x0F: //MOV dadr,R7
          n=(datamem[PSW] & REGBANK)+7; //adresse R7
w5:
          CyclCnt+=24;
          GetDataMem(n,&n); //Ri
          GetCodeMem(PC++,&i);
          SetDataMem(i,(UCHAR)n);
          if(i==S0BUF || i==SCON)
          {
            if(i==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          break;
      }
      goto ex;
    }
    if(cmd<0xA0)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //MOV DPTR,#const16
          CyclCnt+=24;
          GetCodeMem(PC++,&bval);
          GetCodeMem(PC++,&bval1);
          SetDataMem(DPH,(UCHAR)bval);
          SetDataMem(DPL,(UCHAR)bval1);
          break;
        case 0x01: //ACALL page4
          CyclCnt+=24;
          retAddr=PC+1;
          calladdr=PC-1;
          retval=SetSPT(+1);
          i=datamem[SPT];
          if(i>0x7F)
          {
            SetIdataMem(i-0x80,(UCHAR)(retAddr & 0xFF));  //Low ablegen
            i++;
          }
          else
            SetDataMem(i++,(UCHAR)(retAddr & 0xFF));  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            SetIdataMem(i-0x80,(UCHAR)(retAddr>>8));  //High ablegen
          else
            SetDataMem(i,(UCHAR)(retAddr>>8));  //High ablegen
          GetCodeMem(PC,&n);
          PC&=0xF800;
          PC|=(USHORT)(0x400 + (n & 0xFF));
          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x02: //MOV badr,C
          CyclCnt+=24;
          GetCodeMem(PC++,&bval);
          if(datamem[PSW] & CARRY)
            SetBit((UCHAR)bval,TRUE);
          else
            SetBit((UCHAR)bval,FALSE);
          break;
        case 0x03: //MOVC A,@A+DPTR
          CyclCnt+=24;
          GetDataMem(DPH,&bval);
          GetDataMem(DPL,&bval1);
          n=((USHORT)bval<<8)+(USHORT)bval1;
          i=(USHORT)datamem[AKKU] + n;
          GetCodeMem(i,&bval);
          SetDataMem(AKKU,(UCHAR)bval);
          break;
        case 0x04: //SUBB A,#const8
          GetCodeMem(PC++,&n);
          goto w6;
        case 0x05: //SUBB A,dadr
          GetCodeMem(PC++,&bval);
          GetDataMem(bval,&n);
          goto w6;
        case 0x06: //SUBB A,@R0
          n=datamem[PSW] & REGBANK; //adresse von R0
          goto w18;
        case 0x07: //SUBB A,@R1
          n=(datamem[PSW] & REGBANK)+1;  //adresse von R1
w18:
          n=datamem[n];     //Ri
          if(n>0x7F)
            GetIdataMem(n-0x80,&n);
          else
            GetDataMem(n,&n);  //@Ri
          goto w6;
        case 0x08: //SUBB A,R0
          n=datamem[PSW] & REGBANK;
          GetDataMem(n,&n);    //R0
          goto w6;
        case 0x09: //SUBB A,R1
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+1,&n);  //R1
          goto w6;
        case 0x0A: //SUBB A,R2
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+2,&n);  //R2
          goto w6;
        case 0x0B: //SUBB A,R3
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+3,&n);  //R3
          goto w6;
        case 0x0C: //SUBB A,R4
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+4,&n);  //R4
          goto w6;
        case 0x0D: //SUBB A,R5
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+5,&n);  //R5
          goto w6;
        case 0x0E: //SUBB A,R6
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+6,&n);  //R6
          goto w6;
        case 0x0F: //SUBB A,R7
          n=datamem[PSW] & REGBANK;
          GetDataMem(n+7,&n);  //R7
w6:
          CyclCnt+=12;
          a=datamem[AKKU];
          i=(USHORT)a;
          i-=n;
          if(datamem[PSW] & CARRY)
          {
            i--;
            if(((a&0x0F)-1-(n&0x0F)>0x0F))
              datamem[PSW] |= AC;
            else
              datamem[PSW] &= ~AC;
          }
          else
          {
            if(((a&0x0F)-(n&0x0F)>0x0F))
              datamem[PSW] |= AC;
            else
              datamem[PSW] &= ~AC;
          }
          SetDataMem(AKKU,(UCHAR)i);
          if(i & 0xFF00)
            datamem[PSW] |= CARRY;  // carry setzen
          else
            datamem[PSW] &= ~CARRY;
          SetParity((UCHAR)i);

          if(  ((UCHAR)a<=0x7F && (UCHAR)n>=0x80 && (UCHAR)i>=0x80)
               ||((UCHAR)a>=0x80 && (UCHAR)n<=0x7F && (UCHAR)i<=0x7F))
          datamem[PSW] |= OV;
          else
            datamem[PSW] &= ~OV;
          break;
      }
      goto ex;
    }
    if(cmd<0xB0)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //ORL C,/badr
          CyclCnt+=24;
          GetCodeMem(PC++,&bval);
          b=GetBitState((UCHAR)bval,FALSE);
          if((datamem[PSW] & CARRY) || !b)
            datamem[PSW] |=CARRY;
          else
            datamem[PSW] &= ~CARRY;
          break;
        case 0x01: //AJMP page5
          CyclCnt+=24;
          GetCodeMem(PC,&bval);
          PC=(PC & 0xF800) + (bval&0xFF) +0x500;
          break;
        case 0x02: //MOV C,badr
          CyclCnt+=12;
          GetCodeMem(PC++,&bval);
          if(GetBitState((UCHAR)bval,FALSE))
            datamem[PSW] |=CARRY;
          else
            datamem[PSW] &= ~CARRY;
          break;
        case 0x03: //INC DPTR
          CyclCnt+=24;
          n=(USHORT)datamem[DPL]+1;
          SetDataMem(DPL,datamem[DPL]+1);
          if(n > 0xFF)
            SetDataMem(DPH,datamem[DPH]+1);
          break;
        case 0x04: //MUL AB
          CyclCnt+=48;
          datamem[PSW] &= ~(CARRY + OV);
          n=datamem[AKKU] * datamem[BREG];
          SetDataMem(AKKU,(UCHAR)(n & 0xFF));
          SetDataMem(BREG,(UCHAR)(n>>8));
          if(datamem[BREG])
            datamem[PSW] |= OV;
          break;
        case 0x05:  // Breakpoint (wird aber nicht mehr benötigt)
          PC--;
          return(-1); //keine Aufzeichnung eines Tracerecords
          break;
        case 0x06: //MOV @R0,dadr
          n=datamem[PSW] & REGBANK; //adresse von R0
          goto w19;
        case 0x07: //MOV @R1,dadr
          n=(datamem[PSW] & REGBANK)+1; //adresse von R1
w19:
          CyclCnt+=12;
          GetCodeMem(PC++,&a);      //dadr
          n=datamem[n];            //Ri
          GetDataMem(a,&bval);
          if(n>0x7F)
            SetIdataMem(n-0x80,bval);
          else
            SetDataMem(n,bval);
          break;
        case 0x08: //MOV R0,dadr
          n=(datamem[PSW] & REGBANK); //adresse von R0
          goto w7;
        case 0x09: //MOV R1,dadr
          n=(datamem[PSW] & REGBANK)+1; //adresse von R1
          goto w7;
        case 0x0A: //MOV R2,dadr
          n=(datamem[PSW] & REGBANK)+2; //adresse von R2
          goto w7;
        case 0x0B: //MOV R3,dadr
          n=(datamem[PSW] & REGBANK)+3; //adresse von R3
          goto w7;
        case 0x0C: //MOV R4,dadr
          n=(datamem[PSW] & REGBANK)+4; //adresse von R4
          goto w7;
        case 0x0D: //MOV R5,dadr
          n=(datamem[PSW] & REGBANK)+5; //adresse von R5
          goto w7;
        case 0x0E: //MOV R6,dadr
          n=(datamem[PSW] & REGBANK)+6; //adresse von R6
          goto w7;
        case 0x0F: //MOV R7,dadr
          n=(datamem[PSW] & REGBANK)+7; //adresse von R7
w7:
          CyclCnt+=24;
          GetCodeMem(PC++,&bval);
          GetDataMem(bval,&a);
          SetDataMem(n,a);
          break;
      }

      goto ex;
    }
    if(cmd<0xC0)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //ANL C,/badr
          CyclCnt+=24;
          GetCodeMem(PC++,&bval);
          b=GetBitState((UCHAR)bval,FALSE);
          if((datamem[PSW] & CARRY) && !b)
            datamem[PSW] |=CARRY;
          else
            datamem[PSW] &= ~CARRY;
          break;
        case 0x01: //ACALL page5
          CyclCnt+=24;
          retAddr=PC+1;
          calladdr=PC-1;
          retval=SetSPT(+1);
          i=datamem[SPT];
          if(i>0x7F)
          {
            SetIdataMem(i-0x80,(UCHAR)(retAddr & 0xFF));  //Low ablegen
            i++;
          }
          else
            SetDataMem(i++,(UCHAR)(retAddr & 0xFF));  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            SetIdataMem(i-0x80,(UCHAR)(retAddr>>8));  //High ablegen
          else
            SetDataMem(i,(UCHAR)(retAddr>>8));  //High ablegen
          GetCodeMem(PC,&n);
          PC&=0xF800;
          PC|=(USHORT)(0x500 + (n & 0xFF));
          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x02: //CPL badr
          CyclCnt+=12;
          GetCodeMem(PC++,&a);
          if(GetBitState(a,FALSE))
            SetBit(a,FALSE);
          else
            SetBit(a,TRUE);
          break;
        case 0x03: //CPL C
          CyclCnt+=12;
          datamem[PSW] ^= CARRY; //carry negieren
          break;
        case 0x04: //CJNE A,#const8,rel
          GetCodeMem(PC++,&n);           //const8
          i=(USHORT)datamem[AKKU];
          goto w9;
        case 0x05: //CJNE A,dadr,rel
          GetCodeMem(PC++,&a);
          n=datamem[(UCHAR)a]; //dadr
          i=(USHORT)datamem[AKKU];
          goto w9;
        case 0x06: //CJNE @R0,#const8,rel
          i=(datamem[PSW] & REGBANK); //adresse von R0
          goto w20;
        case 0x07: //CJNE @R1,#const8,rel
          i=(datamem[PSW] & REGBANK)+1; //adresse von R1
w20:
          i=datamem[i];  //Ri
          GetCodeMem(PC++,&n);  //const8
          if(i>0x7F)
            GetIdataMem(i-0x80,&i);
          else
            GetDataMem(i,&i); //@Ri
          goto w9;
        case 0x08: //CJNE R0,#const8,rel
          i=(datamem[PSW] & REGBANK); //adresse von R0
          GetDataMem(i,&i); //R0
          GetCodeMem(PC++,&n); //const8
          goto w9;
        case 0x09: //CJNE R1,#const8,rel
          i=(datamem[PSW] & REGBANK)+1; //adresse von R1
          GetDataMem(i,&i); //R1
          GetCodeMem(PC++,&n); //const8
          goto w9;
        case 0x0A: //CJNE R2,#const8,rel
          i=(datamem[PSW] & REGBANK)+2; //adresse von R2
          GetDataMem(i,&i); //R2
          GetCodeMem(PC++,&n); //const8
          goto w9;
        case 0x0B: //CJNE R3,#const8,rel
          i=(datamem[PSW] & REGBANK)+3; //adresse von R3
          GetDataMem(i,&i); //R3
          GetCodeMem(PC++,&n); //const8
          goto w9;
        case 0x0C: //CJNE R4,#const8,rel
          i=(datamem[PSW] & REGBANK)+4; //adresse von R4
          GetDataMem(i,&i); //R4
          GetCodeMem(PC++,&n); //const8
          goto w9;
        case 0x0D: //CJNE R5,#const8,rel
          i=(datamem[PSW] & REGBANK)+5; //adresse von R5
          GetDataMem(i,&i); //R5
          GetCodeMem(PC++,&n); //const8
          goto w9;
        case 0x0E: //CJNE R6,#const8,rel
          i=(datamem[PSW] & REGBANK)+6; //adresse von R6
          GetDataMem(i,&i); //R6
          GetCodeMem(PC++,&n); //const8
          goto w9;
        case 0x0F: //CJNE R7,#const8,rel
          i=(datamem[PSW] & REGBANK)+7; //adresse von R7
          GetDataMem(i,&i); //R7
          GetCodeMem(PC++,&n); //const8
w9:
          CyclCnt+=24;
          GetCodeMem(PC++,&a);
          rel=(SHORT)((CHAR)a);  //rel
          if(i != n)
          {
            if(i<n)
              datamem[PSW] |= CARRY;
            else
              datamem[PSW] &= ~CARRY;
            PC +=rel;
          }
          else
          {
            if(i<n)
              datamem[PSW] |= CARRY;
            else
              datamem[PSW] &= ~CARRY;
          }
          break;
      }
      goto ex;
    }
    if(cmd<0xD0)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //PUSH dadr
          CyclCnt+=24;
          retval=SetSPT(+1);
          n=datamem[SPT];
          GetCodeMem(PC++,&bval);
          GetDataMem(bval,&a);
          if(n>0x7F)
            SetIdataMem(n-0x80,a);
          else
            SetDataMem(n,a);
          callstackvalid--;
          if(callstackvalid==-2)
          {
            retAddr= datamem[SPT-1]+datamem[SPT-2]*256;
            ::AddToCallStackWnd(PC,retAddr);
            callstackvalid=0;
          }
          break;
        case 0x01: //AJMP page6
          CyclCnt+=24;
          GetCodeMem(PC,&bval);
          PC=(PC & 0xF800) + (bval&0xFF)+0x600;
          break;
        case 0x02: //CLR badr
          CyclCnt+=12;
          GetCodeMem(PC++,&bval);
          SetBit((UCHAR)bval,FALSE);
          break;
        case 0x03: //CLR C
          CyclCnt+=12;
          datamem[PSW] &= ~CARRY;
          break;
        case 0x04: //SWAP A
          CyclCnt+=12;
          a=datamem[AKKU]>>4;
          i=(datamem[AKKU]<<4) &0xF0;
          a |= (UCHAR)i;
          SetDataMem(AKKU,a);
          break;
        case 0x05: //XCH A,dadr
          CyclCnt+=12;
          a=datamem[AKKU];
          GetCodeMem(PC++,&n); //dadr
          SetDataMem(AKKU,datamem[n]);
          SetDataMem(n,a);
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          break;
        case 0x06: //XCH A,@R0
          i=datamem[PSW] & REGBANK; //adresse von R0
          goto w21;
        case 0x07: //XCH A,@R1
          i=(datamem[PSW] & REGBANK)+1; //adresse von R1
w21:
          CyclCnt+=12;
          a=datamem[AKKU];
          n=datamem[i];             //Ri
          if(n>0x7F)
            GetIdataMem(n-0x80,&n);
          else
            GetDataMem(n,&n);       //@R1
          a=datamem[AKKU];
          SetDataMem(AKKU,(UCHAR)n);
          n=datamem[i];             //Ri
          if(n>0x7F)
            SetIdataMem(n-0x80,a);
          else
            SetDataMem(n,a);
          break;
        case 0x08: //XCH A,R0
          i=(datamem[PSW] & REGBANK); //adresse von R0
          goto w8;
        case 0x09: //XCH A,R1
          i=(datamem[PSW] & REGBANK)+1; //adresse von R1
          goto w8;
        case 0x0A: //XCH A,R2
          i=(datamem[PSW] & REGBANK)+2; //adresse von R2                       //R2
          goto w8;
        case 0x0B: //XCH A,R3
          i=(datamem[PSW] & REGBANK)+3; //adresse von R3
          goto w8;
        case 0x0C: //XCH A7,R4
          i=(datamem[PSW] & REGBANK)+4; //adresse von R4
          goto w8;
        case 0x0D: //XCH A,R5
          i=(datamem[PSW] & REGBANK)+5; //adresse von R5
          goto w8;
        case 0x0E: //XCH A,R6
          i=(datamem[PSW] & REGBANK)+6; //adresse von R6
          goto w8;
        case 0x0F: //XCH A,R7
          i=(datamem[PSW] & REGBANK)+7; //adresse von R7

w8:
          n=datamem[i]; //Rx
          CyclCnt+=12;
          a=datamem[AKKU];
          SetDataMem(AKKU,(UCHAR)n);
          SetDataMem(i,a);
          break;
      }

      goto ex;
    }
    if(cmd<0xE0)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //POP dadr
          CyclCnt+=24;
          GetCodeMem(PC++,&n); //dadr
          i=datamem[SPT];  //SPT
          if(i>0x7F)
            a=idatamem[i-0x80];    //@SPT
          else
            a=datamem[i];    //@SPT
          SetDataMem(n,a);
          retval=SetSPT(-1);
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          callstackvalid++;
          if(callstackvalid==2)
          {
            ::RemoveFromCallStackWnd(PC);
            callstackvalid=0;
          }
          break;
        case 0x01: //ACALL page6
          CyclCnt+=24;
          retAddr=PC+1;
          calladdr=PC-1;
          retval=SetSPT(+1);
          i=datamem[SPT];
          if(i>0x7F)
          {
            SetIdataMem(i-0x80,(UCHAR)(retAddr & 0xFF));  //Low ablegen
            i++;
          }
          else
            SetDataMem(i++,(UCHAR)(retAddr & 0xFF));  //Low ablegen
          retval|=SetSPT(+1);
          if(i>0x7F)
            SetIdataMem(i-0x80,(UCHAR)(retAddr>>8));  //High ablegen
          else
            SetDataMem(i,(UCHAR)(retAddr>>8));  //High ablegen
          GetCodeMem(PC,&n);
          PC&=0xF800;
          PC|=(USHORT)(0x600 + (n & 0xFF));
          ::AddToCallStackWnd(calladdr,retAddr);
          break;
        case 0x02: //SETB badr
          CyclCnt+=12;
          GetCodeMem(PC++,&bval);
          SetBit((UCHAR)bval,TRUE);
          break;
        case 0x03: //SETB C
          CyclCnt+=12;
          datamem[PSW] |= CARRY;
          break;

        case 0x04: //DA
          CyclCnt+=12;
          n=datamem[AKKU];
          if(((n & 0x0F) > 0x09) || (datamem[PSW] & 0x40))  //wenn unteres Nibble > 9 oder Hilfscarry gesetzt
            n+=6;                                            // addiere 6 dazu
          if(n > 0xFF)
            datamem[PSW] |= CARRY;                           //wenn Übertrag Carry setzen (n darf also keine Bytevariable sein!!!
          if(((n & 0x0F0) > 0x90) || (datamem[PSW] & 0x80)) // wenn das obere Nibble jetzt >0x90 oder Carry gesetzt
            n+=0x60;                                // addiere 0x60 dazu
          if(n > 0xF0)                              // wenn das Ergebnis des oberen Nibbles jetzt >0xF0
            datamem[PSW] |= CARRY;                  // setze Carry
          SetDataMem(AKKU,(UCHAR)n);                //schreibe den wert in den Akku zurück
          SetParity(datamem[AKKU]);                 // berechne zum Schluss noch das Parity Bit
          break;
        case 0x05: //DJNZ dadr,rel
          CyclCnt+=24;
          GetCodeMem(PC++,&n); //dadr
          GetCodeMem(PC++,&bval);
          rel=(SHORT)((CHAR)bval);
          SetDataMem(n,datamem[n]-1);
          if(datamem[n])
            PC+=rel;
          if(n==S0BUF || n==SCON)
          {
            if(n==S0BUF)
              IsSerChar=TRUE;
            else if(datamem[SCON]&2 )  //muss SCON sein
              GenerateTXInt();
          }
          break;
        case 0x06: //XCHD A,@R0
          n=(datamem[PSW] & REGBANK);   //adresse von R0
          goto w10;
        case 0x07: //XCHD A,@R1
          n=(datamem[PSW] & REGBANK)+1; //adresse von R1
w10:
          CyclCnt+=12;
          n=datamem[n];                 // Ri
          a=datamem[AKKU] & 0x0F;       // AKKU unteres Halbbyte
          if(n>0x7F)
          {
            i=idatamem[n-0x80] & 0x0F;
            idatamem[n-0x80] &= 0xF0; //unteres Halbbyte löschen
            SetIdataMem(n-0x80,idatamem[n-0x80]|a);
          }
          else
          {
            i=datamem[n] & 0x0F;        // @Ri unteres Halbbyte
            datamem[n] &= 0xF0;         //unteres Halbbyte löschen
            SetDataMem(n,datamem[n] | a);
          }
          a=datamem[AKKU]&0xF0 | (UCHAR)i; // AKKU oberes Halbbyte + @Ri unteres HalbByte
          SetDataMem(AKKU,a);
          break;
        case 0x08: //DJNZ R0,rel
          n=(datamem[PSW] & REGBANK);
          goto w22;
        case 0x09: //DJNZ R1,rel
          n=(datamem[PSW] & REGBANK)+1;
          goto w22;
        case 0x0A: //DJNZ R2,rel
          n=(datamem[PSW] & REGBANK)+2;
          goto w22;
        case 0x0B: //DJNZ R3,rel
          n=(datamem[PSW] & REGBANK)+3;
          goto w22;
        case 0x0C: //DJNZ R4,rel
          n=(datamem[PSW] & REGBANK)+4;
          goto w22;
        case 0x0D: //DJNZ R5,rel
          n=(datamem[PSW] & REGBANK)+5;
          goto w22;
        case 0x0E: //DJNZ R6,rel
          n=(datamem[PSW] & REGBANK)+6;
          goto w22;
        case 0x0F: //DJNZ R7,rel
          n=(datamem[PSW] & REGBANK)+7;
w22:
          CyclCnt+=24;
          GetCodeMem(PC++,&bval);
          rel=(SHORT)((CHAR)bval);
          SetDataMem(n,datamem[n]-1);
          if(datamem[n])
            PC+=rel;
          break;
      }

      goto ex;
    }
    if(cmd<0xF0)
    {
      switch(cmd & 0x0F)
      {
        case 0x00: //MOVX A,@DPTR
          CyclCnt+=24;
          n=(USHORT)datamem[DPL] + ((USHORT)(datamem[DPH])<<8);
          GetXdataMem(n,&bval);
          SetDataMem(AKKU,bval);
          break;
        case 0x01: //AJMP page7
          CyclCnt+=24;
          GetCodeMem(PC,&bval);
          PC=(PC & 0xF800) + (bval&0xFF) +0x700;
          break;
        case 0x02: //MOVX A,@R0
          n=(datamem[PSW] & REGBANK); //adresse von R0
          goto w23;
        case 0x03: //MOVX A,@R1
          n=(datamem[PSW] & REGBANK)+1; //adresse von R1
w23:
          CyclCnt+=24;
          GetDataMem(n,&n);  //Ri
          n=(USHORT)n + ((USHORT)datamem[PORT2]<<8);
          GetXdataMem(n,&bval);
          SetDataMem(AKKU,bval);
          break;
        case 0x04: //CLR A
          CyclCnt+=12;
          SetDataMem(AKKU,0);
          datamem[PSW] &= ~PARITY;
          break;
        case 0x05: //MOV A,dadr
          GetCodeMem(PC++,&bval);
          GetDataMem((UCHAR)bval,&n);
          goto w24;
        case 0x06: //MOV A,@R0
          n=(datamem[PSW] & REGBANK);  //adresse von R0
          goto w25;
        case 0x07: //MOV A,@R1
          n=(datamem[PSW] & REGBANK)+1;  //adresse von R1
w25:
          n=datamem[n];  //Ri
          if(n>0x7F)
            GetIdataMem(n-0x80,&n);
          else
            GetDataMem(n,&n);
          goto w24;
        case 0x08: //MOV A,R0
          n=(datamem[PSW] & REGBANK);  //adresse von R0
          GetDataMem(n,&n); //R0
          goto w24;
        case 0x09: //MOV A,R1
          n=(datamem[PSW] & REGBANK)+1;  //adresse von R1
          GetDataMem(n,&n); //R1
          goto w24;
        case 0x0A: //MOV A,R2
          n=(datamem[PSW] & REGBANK)+2;  //adresse von R2
          GetDataMem(n,&n); //R2
          goto w24;
        case 0x0B: //MOV A,R3
          n=(datamem[PSW] & REGBANK)+3;  //adresse von R3
          GetDataMem(n,&n); //R3
          goto w24;
        case 0x0C: //MOV A,R4
          n=(datamem[PSW] & REGBANK)+4;  //adresse von R4
          GetDataMem(n,&n); //R4
          goto w24;
        case 0x0D: //MOV A,R5
          n=(datamem[PSW] & REGBANK)+5;  //adresse von R5
          GetDataMem(n,&n); //R5
          goto w24;
        case 0x0E: //MOV A,R6
          n=(datamem[PSW] & REGBANK)+6;  //adresse von R6
          GetDataMem(n,&n); //R6
          goto w24;
        case 0x0F: //MOV A,R7
          n=(datamem[PSW] & REGBANK)+7;  //adresse von R7
          GetDataMem(n,&n); //R7
w24:
          CyclCnt+=12;
          SetDataMem(AKKU,(UCHAR)n);
          break;
      }

      goto ex;
    }
    switch(cmd & 0x0F)
    {
      case 0x00: //MOVX @DPTR,A
        CyclCnt+=24;
        n=(USHORT)datamem[DPL] + ((USHORT)(datamem[DPH])<<8);
        SetXdataMem(n,datamem[AKKU]);
        break;
      case 0x01: //ACALL page7
        CyclCnt+=24;
        retAddr=PC+1;
        calladdr=PC-1;
        retval=SetSPT(+1);
        i=datamem[SPT];
        if(i>0x7F)
        {
          SetIdataMem(i-0x80,(UCHAR)(retAddr & 0xFF));  //Low ablegen
          i++;
        }
        else
          SetDataMem(i++,(UCHAR)(retAddr & 0xFF));  //Low ablegen
        retval|=SetSPT(+1);
        if(i>0x7F)
          SetIdataMem(i-0x80,(UCHAR)(retAddr>>8));  //High ablegen
        else
          SetDataMem(i,(UCHAR)(retAddr>>8));  //High ablegen
        GetCodeMem(PC,&n);
        PC&=0xF800;
        PC|=(USHORT)(0x700 + (n & 0xFF));
        ::AddToCallStackWnd(calladdr,retAddr);
        break;
      case 0x02: //MOVX @R0,A
        n=(datamem[PSW] & REGBANK); //adresse von R0
        goto w26;
      case 0x03: //MOVX @R1,A
        n=(datamem[PSW] & REGBANK)+1; //adresse von R1
w26:
        CyclCnt+=12;
        n=datamem[n];  //Ri
        i=n+((USHORT)datamem[PORT2]<<8);
        SetXdataMem(i,datamem[AKKU]);
        break;
      case 0x04: //CPL A
        CyclCnt+=12;
        SetDataMem(AKKU,~datamem[AKKU]);
        break;
      case 0x05: //MOV dadr,A
        GetCodeMem(PC++,&n);  //Adresse von dadr
        if(n==S0BUF || n==SCON)
        {
          if(n==S0BUF)
            IsSerChar=TRUE;
          else if(datamem[SCON]&2 )  //muss SCON sein
            GenerateTXInt();
        }
        goto w27;
      case 0x06: //MOV @R0,A
        n=(datamem[PSW] & REGBANK); //Adresse von R0
        goto w28;
      case 0x07: //MOV @R1,A
        n=(datamem[PSW] & REGBANK)+1; //Adresse von R1
w28:
        CyclCnt+=12;
        GetDataMem(n,&n); //Ri;
        GetDataMem(AKKU,&bval);
        if(n>0x7F)
          SetIdataMem(n-0x80,bval);
        else
          SetDataMem(n,bval);
        break;
      case 0x08: //MOV R0,A
        n=(datamem[PSW] & REGBANK); //Adresse von R0
        goto w27;
      case 0x09: //MOV R1,A
        n=(datamem[PSW] & REGBANK)+1; //Adresse von R1
        goto w27;
      case 0x0A: //MOV R2,A
        n=(datamem[PSW] & REGBANK)+2; //Adresse von R2
        goto w27;
      case 0x0B: //MOV R3,A
        n=(datamem[PSW] & REGBANK)+3; //Adresse von R3
        goto w27;
      case 0x0C: //MOV R4,A
        n=(datamem[PSW] & REGBANK)+4; //Adresse von R4
        goto w27;
      case 0x0D: //MOV R5,A
        n=(datamem[PSW] & REGBANK)+5; //Adresse von R5
        goto w27;
      case 0x0E: //MOV R6,A
        n=(datamem[PSW] & REGBANK)+6; //Adresse von R6
        goto w27;
      case 0x0F: //MOV R7,A
        n=(datamem[PSW] & REGBANK)+7; //Adresse von R7
w27:
        CyclCnt+=12;
        GetDataMem(AKKU,&bval);
        SetDataMem(n,bval);
        break;
    }
ex:
    if(traceptr) //ASM-Trace schreiben
    {
      if(tracelen==MAXTRACELEN)
      {
        traceptr->Flush();
        traceptr->GetFile()->SeekToBegin();
        tracelen=0;
      }
      WriteCurrentASMTraceRecord(pc);
      tracelen++;
    }

    if(activeMeasure)
    {
      if( codemem[PC] & RUNTIMEMPT ) //der nächste Befehl ist ein Messpunkt
      {
        measurepoint->cycles+= CyclCnt;
        if(CyclCnt <  measurepoint->MinCycles)
          measurepoint->MinCycles=CyclCnt;
        else if(CyclCnt >  measurepoint->MaxCycles)
          measurepoint->MaxCycles=CyclCnt;
        measurepoint->measures++;
      }
    }

    return retval;
  }
  else  // cmd >0xFF ->breakpoint ,Tracpoint,Messpunkt PC
  {
    if(tracetyp==TRACE_WATCH && (cmd & TRACEPOINT)) //der nächste Befehl ist ein Tracepunkt
    {
      ::WriteCurrentTraceRecord(tracetyp,pc);
    }

    if(activeMeasure && (cmd & BKPT_CODE))  //bei eingeschalteter Messung wird bei jedem
      CyclCnt=0;                          //Breakpoint der Zähler gelöscht
    if(  (cmd & (BKPT_CODE|BKPT_DISABLED)) == BKPT_CODE
         ||(cmd & BKPT_TMP)) //aktiver Breakpunkt
    return -1;
    if( cmd & STIMUPT)
    {
      ::DoNextStimulation();
      cmd &=0x00FF;
      goto exstart;
    }
    else
    {
      cmd &=0x00FF;
      goto exstart;
    }
  }
  return retval;
}





int CProc51::SetSPT(int diff)
{
  char msg[50];
  datamem[SPT]+=diff;
  if(!datamem[SPT] && TrackStack)
  {
    sprintf(msg,"Stackoverflow at address : 0x%4.4X",PC);
    AfxMessageBox(msg);
    return(-1);
  }
  return(0);
}


BOOL CProc51::SetBreakpoint(ULONG addr,USHORT fmt,int memspec)
{
  POSITION pos;
  bkpt_t* bp;

  if(memspec=='c' || memspec=='C' || !memspec) //Breakpoint auf Codespeicher
  {
    addr&=0xFFFF;
    codemem[addr] &= 0x00FF;
    codemem[addr] |= fmt;      //setze den Breakpoint im Speicher
    if(fmt & (BKPT_TMP|RUNTIMEMPT|TRACEPOINT))
      return TRUE;  //temporäre Breakpoints werden nicht in die Liste aufgenommen
    pos=bkpts.GetHeadPosition();
    while(pos)
    {
      bp= (bkpt_t*)bkpts.GetNext(pos);
      if(bp->addr==addr)
      {
        bp->fmt=fmt;
        return FALSE;  //kein neuer Breakpoint
      }
    }
    bp=new bkpt_t;
    bp->addr=addr;
    bp->fmt=fmt;
    if(fmt & (BKPT_WRITE|BKPT_READ))
      accessbkpts++;
    bp->memspec='c';
    if(fmt & BKPT_CODE)
      bkpts.AddTail((void*)bp);
    else
      bkpts.AddHead((void*)bp);
    return TRUE;
  }
  else if(memspec=='x' || memspec=='X' || memspec==XDATAMEM) //Breakpoint auf Xdata
  {
    addr&=0xFFFF;
    xdatamemfmt[addr] = (fmt & (BKPT_READ|BKPT_WRITE))>>8;
    pos=bkpts.GetHeadPosition();
    while(pos)
    {
      bp= (bkpt_t*)bkpts.GetNext(pos);
      if(bp->addr==addr)
      {
        bp->fmt=fmt& (BKPT_READ|BKPT_WRITE);
        if(!bp->fmt) //jetzt ist nichts mehr übrig also löschen
          RemoveBreakpoint(addr,memspec);
        return FALSE;  //kein neuer Breakpoint
      }
    }
    if( fmt & (BKPT_READ|BKPT_WRITE))
    {
      accessbkpts++;
      bp=new bkpt_t;
      bp->addr=addr;
      bp->fmt=fmt & (BKPT_READ|BKPT_WRITE);
      bp->memspec='x';
      bkpts.AddHead((void*)bp);
      return TRUE;
    }
  }
  if(memspec=='i' || memspec=='I' || memspec==IDATAMEM && addr <0x100 ) //Breakpoint auf Idata
  {
    addr&=0xFF;
    if(addr<0x80)
      datamemfmt[addr] = (fmt & (BKPT_READ|BKPT_WRITE))>>8;
    else
      idatamemfmt[addr-0x80] = (fmt & (BKPT_READ|BKPT_WRITE))>>8;
    pos=bkpts.GetHeadPosition();
    while(pos)
    {
      bp= (bkpt_t*)bkpts.GetNext(pos);
      if(bp->addr==addr)
      {
        bp->fmt=fmt& (BKPT_READ|BKPT_WRITE);
        if(!bp->fmt) //jetzt ist nichts mehr übrig also löschen
          RemoveBreakpoint(addr,memspec);
        return FALSE;  //kein neuer Breakpoint
      }
    }
    if( fmt & (BKPT_READ|BKPT_WRITE))
    {
      accessbkpts++;
      bp=new bkpt_t;
      bp->addr=addr;
      bp->fmt=fmt & (BKPT_READ|BKPT_WRITE);
      bp->memspec='i';
      bkpts.AddHead((void*)bp);
      return TRUE;
    }
  }
  else if(    memspec=='d' || memspec=='D' || memspec==DATAMEM && addr <0x100
              && (fmt& (BKPT_READ|BKPT_WRITE))) //Breakpoint auf Data
  {
    addr&=0xFF;
    datamemfmt[addr] = (fmt & (BKPT_READ|BKPT_WRITE))>>8;
    pos=bkpts.GetHeadPosition();
    while(pos)
    {
      bp= (bkpt_t*)bkpts.GetNext(pos);
      if(bp->addr==addr)
      {
        bp->fmt=fmt & (BKPT_READ|BKPT_WRITE);
        if(!bp->fmt) //jetzt ist nichts mehr übrig also löschen
          RemoveBreakpoint(addr,memspec);
        return FALSE;  //kein neuer Breakpoint
      }
    }
    if( fmt & (BKPT_READ|BKPT_WRITE))
    {
      accessbkpts++;
      bp=new bkpt_t;
      bp->addr=addr;
      bp->fmt=fmt & (BKPT_READ|BKPT_WRITE);
      bp->memspec='d';
      bkpts.AddHead((void*)bp);
      return TRUE;
    }
  }
  return FALSE;
}

BOOL CProc51::RemoveBreakpoint(ULONG addr,int memspec)
{
  POSITION pos;
  bkpt_t* bp;
  int fmt;

  if(memspec=='c' || memspec=='C' || !memspec) //Breakpoint auf Codespeicher
  {
    fmt=codemem[addr]&(BKPT_READ+BKPT_WRITE);
    codemem[addr] &= ~(BKPT_CODE|BKPT_TMP|BKPT_DISABLED|BKPT_READ|BKPT_WRITE);
    pos=bkpts.GetTailPosition();
    while(pos)
    {
      bp=(bkpt_t*)bkpts.GetAt(pos);
      if(bp->addr==addr)
      {
        bkpts.RemoveAt(pos);
        delete bp;
        if(fmt)
          accessbkpts--;
        return TRUE;
      }
      bkpts.GetPrev(pos);
    }
    return FALSE;  //Breakpoint nicht gefunden
  }
  else if(memspec=='x' || memspec=='X' || memspec==XDATAMEM) //Breakpoint auf Xdata
  {
    xdatamemfmt[addr] = 0;
    pos=bkpts.GetHeadPosition();
    while(pos)
    {
      bp=(bkpt_t*)bkpts.GetAt(pos);
      if(bp->addr==addr)
      {
        bkpts.RemoveAt(pos);
        delete bp;
        accessbkpts--;
        return TRUE;
      }
      bkpts.GetNext(pos);
    }
    return FALSE;  //Breakpoint nicht gefunden
  }
  else if(memspec=='i' || memspec=='I' || memspec==IDATAMEM && addr <0x100) //Breakpoint auf Idata
  {
    if(addr<0x80)
      datamemfmt[addr] = 0;
    else
      idatamemfmt[addr-0x80] = 0;
    pos=bkpts.GetHeadPosition();
    while(pos)
    {
      bp=(bkpt_t*)bkpts.GetAt(pos);
      if(bp->addr==addr)
      {
        bkpts.RemoveAt(pos);
        delete bp;
        accessbkpts--;
        return TRUE;
      }
      bkpts.GetNext(pos);
    }
    return FALSE;  //Breakpoint nicht gefunden
  }
  else if(memspec=='d' || memspec=='D' || memspec==DATAMEM && addr <0x100) //Breakpoint auf Data
  {
    datamemfmt[addr] = 0;
    pos=bkpts.GetHeadPosition();
    while(pos)
    {
      bp=(bkpt_t*)bkpts.GetAt(pos);
      if(bp->addr==addr)
      {
        bkpts.RemoveAt(pos);
        delete bp;
        accessbkpts--;
        return TRUE;
      }
      bkpts.GetNext(pos);
    }
    return FALSE;  //Breakpoint nicht gefunden
  }
  return FALSE;
}

ULONG CProc51::IsBreakpointAtAddr(ULONG addr, ULONG fmt,int memspec)
{
  if(memspec=='c' || memspec=='C' || !memspec)
  {
    addr&=0xFFFF;
    if(   (codemem[addr] & (BKPT_CODE|BKPT_TMP)) == BKPT_CODE
          || (codemem[addr] & (BKPT_READ|BKPT_WRITE))  ) //temporäre Breakpoints nicht
    {
      if(!fmt)
        return codemem[addr] & 0xFF00;
      else
        return codemem[addr] & fmt;
    }
  }
  if(memspec=='x' || memspec=='X' || memspec==XDATAMEM)
  {
    addr &=0xFFFF;
    if(!fmt)
      return (xdatamemfmt[addr]<<8) & 0xFF00;
    else
      return (xdatamemfmt[addr]<<8) & fmt;
  }
  if(memspec=='d' || memspec=='D' || memspec==DATAMEM)
  {
    addr&=0xFF;
    if(!fmt)
      return (datamemfmt[addr]<<8) & 0xFF00;
    else
      return (datamemfmt[addr]<<8) & fmt;
  }
  if(memspec=='i' || memspec=='I' || memspec==IDATAMEM)
  {
    addr&=0xFF;
    if(addr >0x80)
    {
      if(!fmt)
        return (idatamemfmt[addr-0x80]<<8) & 0xFF00;
      else
        return (idatamemfmt[addr-0x80]<<8) & fmt;
    }
    else
    {
      if(!fmt)
        return (datamemfmt[addr]<<8) & 0xFF00;
      else
        return (datamemfmt[addr]<<8) & fmt;
    }
  }

  return FALSE; // alle anderen Breakpoints werden nicht unterstützt
}

// Löscht einen temporären Breakpoint wenn vorhanden
BOOL CProc51::ClrTempBkpt(ULONG addr)
{
  if(codemem[addr] & BKPT_TMP)
  {
    codemem[addr]&=~(BKPT_TMP | BKPT_CODE | BKPT_DISABLED);
    return TRUE;
  }
  return FALSE;
}

USHORT CProc51::GetBkptFormat(ULONG addr,int memspec)
{

  switch(memspec)
  {
    case 'c':
    case 'C':
    case 0:
      addr &= 0xFFFF;
      return codemem[addr & 0xFFFF]&0xFF00;
    case 'x':
    case 'X':
    case XDATAMEM:
      addr &= 0xFFFF;
      return (USHORT)(xdatamemfmt[addr]<<8);
    case 'd':
    case 'D':
    case DATAMEM:
      addr &= 0xFF;
      return (USHORT)(datamemfmt[addr]<<8);
    case 'i':
    case 'I':
    case IDATAMEM:
      addr &= 0xFF;
      if(addr <0x80)
        return (USHORT)(datamemfmt[addr]<<8);
      else
        return (USHORT)(idatamemfmt[addr-0x80]<<8);
      break;
  }
  return 0;
}




ULONG CProc51::GetStepOverAddr(ULONG addr)
{
  int cmd;
  char s[100];

  cmd=codemem[addr]&0x00FF;
  switch(cmd)
  {
    case 0x11:    //ACALL page
    case 0x31:
    case 0x51:
    case 0x71:
    case 0x91:
    case 0xB1:
    case 0xD1:
    case 0xF1:
    case 0x12:    // LCALL
      cmd=Reassemble(addr,s);
      return addr+cmd;  //
    default:
      return addr;   //hier ist StepOver gleich StepIn
  }
}

BOOL CProc51::IsReturn(ULONG addr)
{
  if((codemem[addr] & 0xFF )==0x22 || (codemem[addr]& 0xFF )==0x32)
    return TRUE;
  return FALSE;
}

CRegdef* CProc51::GetNextRegister(int index)
{
  return regs.GetReg(index);
}


BOOL CProc51::GetMemFromAddr(ULONG addr,ULONG* valp,ULONG memspec)
{
int bitval;
  switch(memspec & 0xFFFF)
  {
    case 'c':
    case 'C':
    case CODEMEM:
      if(addr<=0xFFFF)
      {
        *valp=(codemem[addr]&0xFF);
        return TRUE;
      }
      break;
    case 'x':
    case 'X':
    case XDATAMEM:
      if(addr<=0xFFFF)
      {
        *valp=((UCHAR)xdatamem[addr]);
        return TRUE;
      }
      break;
    case 'p':
    case 'P':
    case PDATAMEM:
      *valp=(UCHAR)xdatamem[(addr & 0xFF)|((USHORT)datamem[PORT2]<<8)];
      return TRUE;
      break;
    case 'd':
    case 'D':
    case DATAMEM:
      if(addr<=0xFF)
      {
        *valp=(datamem[addr]);
        return TRUE;
      }
      break;
    case 'i':
    case 'I':
    case IDATAMEM:
      if(addr < 0x80)
        *valp=(datamem[addr]);
      else
        *valp=(idatamem[addr-0x80]);
      return TRUE;
      break;
    case BITMEM:
      bitval=1 << (addr%8);
      addr=0x20+addr/8;
      if(addr <=0x2F)
      {
        *valp=datamem[addr] & bitval;
        if(*valp)
          *valp=1;
      }
      return TRUE;
      break;
    default:
      return FALSE ;
  }
  return FALSE;
}

// liefert die maximale Größe des spezifizierten Speicherbereichs
ULONG CProc51::GetMemSize(ULONG memspec)
{
  switch(memspec & 0xFFFF)
  {
    case 'c':
    case 'C':
    case CODEMEM:
      return S16;
      break;
    case 'x':
    case 'X':
    case XDATAMEM:
      return S16;
      break;
    case 'd':
    case 'D':
    case 'p':
    case 'P':
    case DATAMEM:
    case BITMEM:
    case PDATAMEM:
      return S8;
      break;
    case 'i':
    case 'I':
    case IDATAMEM:
      return S8;
      break;
    default:
      return S32 ;
  }
}

// setzt das Byte auf der entsprechenden Adresse und liefert den alten Wert zurück
ULONG CProc51::SetMemAtAddr(ULONG addr,ULONG* pval,ULONG memspec)
{
  ULONG oldval;
  int bitval;

  switch(memspec & 0xFFFF)
  {
    case 'c':
    case 'C':
    case CODEMEM:
      if(addr<=0xFFFF)
      {
        oldval=codemem[addr]&0xFF;
        codemem[addr]=*(UCHAR*)pval & 0xFF;
      }
      break;
    case 'x':
    case 'X':
    case XDATAMEM:
      if(addr<=0xFFFF)
      {
        oldval=(UCHAR)xdatamem[addr];
        (UCHAR)xdatamem[addr]=*(UCHAR*)pval;
      }
      break;
    case 'p':
    case 'P':
    case PDATAMEM:
      addr=((USHORT)datamem[PORT2]<<8)| (addr & 0xFF);
      oldval=(UCHAR)xdatamem[addr];
      (UCHAR)xdatamem[addr]=*(UCHAR*)pval;
      break;
    case 'd':
    case 'D':
    case DATAMEM:
      if(addr<=0xFF)
      {
        oldval=datamem[addr];
        datamem[addr]=*(UCHAR*)pval;
      }
      break;
    case 'i':
    case 'I':
    case IDATAMEM:
      if(addr < 0x80)
      {
        oldval=datamem[addr];
        datamem[addr]=*(UCHAR*)pval;
      }
      else
      {
        oldval=idatamem[addr-0x80];
        idatamem[addr-0x80]=*(UCHAR*)pval;
      }
      break;
    case BITMEM:
    case 'b':
    case 'B':
      if(addr<0x80)
      {
        bitval=1 << (addr%8);
        addr=0x20+addr/8;
      }
      else
      {
        bitval=1 << (addr&0x07);
        addr&=0xFE;
      }
      oldval=datamem[addr];
      if(oldval)
        oldval=1;
      if(*(UCHAR*)pval)
        datamem[addr] |= bitval;
      else
        datamem[addr] &= ~bitval;
      break;
    default:
      return -1;
  }
  return(oldval);
}

//liefert den Inhalt eines Zeigers und den referenzierten
//Speicherbereich zurück
//return=TRUE wenn sich der Inhalt finden lies
//pointmem muss die Werte MS_... haben
//die Adress des Zeigers kann in Abhängigkeit vom Speicherbereich geändert werden
BOOL CProc51::GetPointer(ULONG* pointeraddr,ULONG* pointerval,
                         CTypdesc* pointtyp,
                         USHORT pointmem, ULONG* pointtomem,BOOL orderHL)
{
  BOOL b;
  ULONG v0,v1,v2,v3;
  ULONG memspec;

  *pointtomem=(pointmem << 16);
  memspec=pointmem;
  if(memspec & PDATAMEM)
    *pointeraddr=(*pointeraddr & 0x000000FF) | ((USHORT)datamem[PORT2]<<8);
  switch(pointtyp->typ)
  {
    case  T_POINTDESC:    //allg.Pointer memspec= 1.Byte //nur Keil-> HL immer 
      b=GetMemFromAddr(*pointeraddr,&v0,memspec);
ptr1:
      pointtyp->offset=(USHORT)v0;   //der aktuelle M-Space wird bei
      if(!b)                         //jedem Zugriff neu eingetragen
        return FALSE;
      switch(v0)
      {
        case 0x01:
          *pointtomem|=IDATAMEM;
          GetMemFromAddr(*pointeraddr+2,&v0,memspec);
          *pointerval=v0;
          break;
        case 0x02:
          *pointtomem|=XDATAMEM;
          GetMemFromAddr(*pointeraddr+1,&v0,memspec);
          GetMemFromAddr(*pointeraddr+2,&v1,memspec);
          *pointerval=(v0<<8)+v1;
          break;
        case 0x03:
          *pointtomem|=PDATAMEM;
          GetMemFromAddr(*pointeraddr+2,&v0,memspec); //+((USHORT)datamem[PORT2]<<8)
          *pointerval=v0;
          break;
        case 0x04:
          *pointtomem|=DATAMEM;
          GetMemFromAddr(*pointeraddr+2,&v0,memspec);
          *pointerval=v0;
          break;
        case 0x05:
          *pointtomem|=CODEMEM;
          GetMemFromAddr(*pointeraddr+1,&v0,memspec);
          GetMemFromAddr(*pointeraddr+2,&v1,memspec);
          *pointerval=(v0<<8)+v1;;
          break;
        default:   return FALSE;
      }
      return TRUE;
      break;    
    case T_GENPTRDESC:
      if(pointtyp->elements==1)
      {
        b=GetMemFromAddr(*pointeraddr,&v0,memspec);
        switch((BYTE)v0)
        {
          case 0: v0=4;   //DATA
            break;
          case 1: v0=2;   //XDATA
            break;
          case 0xFF:  v0=5;  //CODE
            break;
          case 0xFE:  v0=3;  //PDATA
            break;
        }

        //der aktuelle M-Space wird bei
        //jedem Zugriff neu eingetragen
        goto ptr1;
      }

      break;
    case T_SPACEDPTR:
      switch(pointtyp->offset)
      {
        case 0x01: *pointtomem|=IDATAMEM;
          break;
        case 0x02: *pointtomem|=XDATAMEM;
          break;
        case 0x03: *pointtomem|=PDATAMEM;
          break;
        case 0x04: *pointtomem|=DATAMEM;
          break;
        case 0x05: *pointtomem|=CODEMEM;
          break;
        default:   return FALSE;
      }
      switch( pointtyp->size)
      {
        case 1:
        case 8:
          b=GetMemFromAddr(*pointeraddr,&v0,memspec);
          *pointerval=v0;
          break;
        case 2:
        case 16:          
          b=GetMemFromAddr(*pointeraddr,&v0,memspec);
          GetMemFromAddr(*pointeraddr+1,&v1,memspec);
          if(orderHL)
            *pointerval=(v0<<8)+v1;
          else
            *pointerval=(v1<<8)+v0; 
          break;

        case 24:
          b=GetMemFromAddr(*pointeraddr,&v0,memspec);
          GetMemFromAddr(*pointeraddr+1,&v1,memspec);
          GetMemFromAddr(*pointeraddr+2,&v2,memspec);
          if(orderHL)
            *pointerval=(v0<<16)+(v1<<8)+v2;
          else
            *pointerval=(v2<<16)+(v1<<8)+v1;
          break;
        case 4:          
          b=GetMemFromAddr(*pointeraddr,&v0,memspec);
          GetMemFromAddr(*pointeraddr+1,&v1,memspec);
          GetMemFromAddr(*pointeraddr+2,&v2,memspec);
          GetMemFromAddr(*pointeraddr+3,&v3,memspec);
          if(orderHL)
            *pointerval=(v0<<24)+(v1<<16)+(v2<<8)+v3;
          else
            *pointerval=(v3<<24)+(v2<<16)+(v1<<8)+v0; 
          break;

      }
      return b;
      break;
  }
  

  return FALSE;
}



void CProc51::ResetCPU()
{
  int i;
  unsigned char *xp;

  PC=0;
  xp=xdatamem;
  for(i=0;i<0x10000;i++)
  {
    *xp++=0;
  }
  for(i=0;i<128;i++)
  {
    datamem[i]=0;            //Direktadressierbaren Speicher löschen
    datamem[i+0x80]=0;
    idatamem[i]=0;           //IdataSpeicher löschen
  }
  datamem[SPT]=0x07;
  datamem[0x80]=0xFF;
  datamem[0x90]=0xFF;
  datamem[0xA0]=0xFF;
  datamem[0xB0]=0xFF;
  CyclCnt=0;
  intReq=0;
  actInt=0;
}

//Testet ob sich im Adressbereich auch tatsächlich Speicher befindet

BOOL CProc51::IsMemAtAddress(ULONG addr, ULONG memspec)
{
  POSITION pos;
  mcfg_t* pm;

  if(defaultMemcfg==0x0007)
    return TRUE;
  pos=memcfgs.GetHeadPosition();
  while(pos)
  {
    pm= (mcfg_t*)memcfgs.GetNext(pos);
    if((ULONG)pm->memspec==memspec)
    {
      if(addr>=pm->startaddr && addr <= pm->endaddr)
        return TRUE;
    }
  }
  return FALSE;
}

int CProc51::CreateMemInRange(ULONG memspec,ULONG startaddr, ULONG endaddr)
{
  mcfg_t* pmcfg;


  switch(memspec)
  {
    case 'x':
    case 'X':
      memspec='X';
      if(startaddr != 0 || endaddr!=0xFFFF)
      {
        if(defaultMemcfg==0x0007)
        {
          accessbkpts++;
          defaultMemcfg|=0x0008;
        }
        defaultMemcfg&=~0x0002;
      }
      else
      {
        if((defaultMemcfg|0x0002) == 0x000F)
        {
          defaultMemcfg&=~0x0008;
          accessbkpts--;
        }
        defaultMemcfg|=0x0002;
      }
      break;
    case 'c':
    case 'C':
    case 0:
      if(startaddr != 0 || endaddr!=0xFFFF)
      {
        if(defaultMemcfg==0x0007)
        {
          accessbkpts++;
          defaultMemcfg|=0x0008;
        }
        defaultMemcfg&=~0x0001;
      }
      else
      {
        if((defaultMemcfg|0x0001) == 0x000F)
        {
          defaultMemcfg&=~0x0008;
          accessbkpts--;
        }
        defaultMemcfg|=0x0001;
      }
      break;
    case 'd':
    case 'D':  //nicht änderbar
      return memcfgs.GetCount();
    case 'i':
    case 'I':
      memspec='I';
      if(startaddr != 0 || endaddr!=0xFF)
      {
        if(defaultMemcfg==0x0007)
        {
          accessbkpts++;
          defaultMemcfg|=0x0008;
        }
        defaultMemcfg&=~0x0004;
      }
      else
      {
        if((defaultMemcfg|0x0004) == 0x000F)
        {
          defaultMemcfg&=~0x0008;
          accessbkpts--;
        }
        defaultMemcfg|=0x0004;
      }
      break;
    case -1:
      SetDefaultMemCfg();
      return memcfgs.GetCount();
    default:
      memspec=-1;
      break;
  }
  if(memspec != -1)
  {
    pmcfg=new mcfg_t;
    pmcfg->startaddr=startaddr;
    pmcfg->endaddr=endaddr;
    pmcfg->memspec=memspec;
    pmcfg->editable=TRUE;
    memcfgs.AddHead(pmcfg);
    PackMemCfg();
    return memcfgs.GetCount();
  }
  return -1;
}

BOOL CProc51::DeleteMem(int blockindex,char memspec)
{
  POSITION pos,pos1;
  mcfg_t* mp;

  if(memspec && blockindex==-1)  //den speicherbereich löschen
  {
c1:
    pos=memcfgs.GetHeadPosition();
    while(pos)
    {
      pos1=pos;
      mp= (mcfg_t*)memcfgs.GetNext(pos);
      if(mp->memspec==memspec || mp->memspec==memspec-0x20)
      {
        delete mp;
        memcfgs.RemoveAt(pos1);
        goto c1;
      }
    }
    return TRUE;
  }
  if(blockindex==-1 && !memspec)
  {
    pos=memcfgs.GetHeadPosition();
    while(pos)
    {
      mp= (mcfg_t*)memcfgs.GetNext(pos);
      delete mp;
    }
    memcfgs.RemoveAll();
    mp=new mcfg_t;
    mp->startaddr=0;
    mp->endaddr=0xFF;
    mp->memspec='D';
    mp->editable=FALSE;
    memcfgs.AddHead(mp);
  }
  else
  {
    pos=memcfgs.FindIndex(blockindex);
    if(pos)
    {
      mp= (mcfg_t*)memcfgs.GetAt(pos);
      delete mp;
      memcfgs.RemoveAt(pos);
      return TRUE;
    }
    return FALSE;
  }
  return FALSE;
}

//liefert die Anzahl der belegten Speicherblöcke und im Zeigerarray
//die Angaben über die Blöcke
// wenn Return=0 sind keine Blöcke eingetragen und änderbar
int CProc51::GetNextMemMapping(mcfg_t* pMemCfg,int blockindex)
{
  POSITION pos;
  mcfg_t* p;

  pos=memcfgs.FindIndex(blockindex);
  if(!pos)
  {
    return -1;
  }
  p=(mcfg_t*)memcfgs.GetAt(pos);
  *pMemCfg=*p;
  return blockindex+1;
}


BOOL CProc51::GetNextBreakpoint(POSITION& pos,bkpt_t* bkpt)
{
  bkpt_t* bp;

  if(pos==(POSITION)-1 )
    return FALSE;
  if(pos==0)
    pos=bkpts.GetHeadPosition();
  if(!pos)
    return FALSE;

  bp=(bkpt_t*)bkpts.GetNext(pos);
  *bkpt=*bp;
  return TRUE;
}


//liefert die Anzahl der gezählten Prozessortakte

ULONG CProc51::GetCyclCnt(BOOL bDelete)
{
  ULONG retval=CyclCnt;
  if(bDelete)
    CyclCnt=0;
  return retval;  //Zyklen *Takte
}

// schaltet den TRACE aus (logfile=0) oder ein
void CProc51::EnableTraceLog(LPCSTR logname,int tracetype,ULONG addr)
{
  if(logname && tracetype==TRACE_ASM)
  {
    tracefile.Open("temptrace.log",CFile::modeCreate | CFile::modeWrite);
    traceptr=new CArchive(&tracefile,CArchive::store);
  }
  else
  {
    delete traceptr;
    if(tracefile.m_hFile != -1)
      tracefile.Close();
    traceptr=0;
  }
  tracelen=0;
  if(tracetype==TRACE_WATCH && logname)
    SetBreakpoint(addr,TRACEPOINT);
  else if(!logname && tracetype==TRACE_WATCH)
    codemem[traceaddr] &= ~(TRACEPOINT);
  if(!logname)
    tracetyp=0;
  else
    tracetyp=tracetype;
  traceaddr=addr;
}


//schreibt einen Tracerecord in ein File in der Form
//PC- PSW- A- R0..R7- B- DPH- DPL- P2- SP
void CProc51::WriteCurrentASMTraceRecord(USHORT pc)
{
  UCHAR r[21];
  int regbank;

  r[0]=TRACE_ASM;
  r[1]=17;
  r[2]=0;
  r[3]=(pc & 0xFF00)/256;
  r[4]=(pc & 0xFF);
  r[5]=datamem[PSW];
  regbank=r[2] & REGBANK;
  r[6]=datamem[AKKU];
  r[7]=datamem[regbank];
  r[8]=datamem[regbank+1];
  r[9]=datamem[regbank+2];
  r[10]=datamem[regbank+3];
  r[11]=datamem[regbank+4];
  r[12]=datamem[regbank+5];
  r[13]=datamem[regbank+6];
  r[14]=datamem[regbank+7];
  r[15]=datamem[BREG];
  r[16]=datamem[DPH];
  r[17]=datamem[DPL];
  r[18]=datamem[PORT2];
  r[19]=datamem[SPT];
  if(traceptr)
    traceptr->Write(r,20);
}


ULONG CProc51::GetRegOffset(int index)
{
  return (datamem[PSW] & REGBANK);
}


void CProc51::SetMeasurePoint(CMeasurePoint* pmpt)
{
  if(pmpt)
  {
    if(measurepoint)
      codemem[measurepoint->addr]&= ~RUNTIMEMPT;
    codemem[pmpt->addr]|=RUNTIMEMPT;
    measurepoint=pmpt;
    activeMeasure=TRUE;
  }
  else //Messpunkt löschen
  {
    if(measurepoint)
      codemem[measurepoint->addr]&= ~RUNTIMEMPT;
    measurepoint=0;
    activeMeasure=FALSE;
  }
}

BOOL CProc51::WriteMemBlock(ULONG addr,UCHAR* bp,USHORT len,ULONG memspec)
{
  USHORT* cp;

  if(addr>0xFFFF || (memspec!='c' && memspec!='C' && memspec)  || (addr+len) > 0xFFFF)
    return FALSE;
  cp=&codemem[addr];
  while(len--)
    *cp++=(USHORT)(*bp++);
  return TRUE;
}

ULONG  CProc51::GetDestinationAddress(ULONG addr,ULONG memspec)
{
  if(memspec =='P' || memspec =='p')
    return addr+((USHORT)datamem[PORT2]<<8);
  return addr;
};

void CProc51::SetDefaultMemCfg()
{
  if(defaultMemcfg&0x0008)
    accessbkpts--;
  defaultMemcfg=0x0007;
  DeleteMem(-1);

  mcfg_t* pm=new mcfg_t;
  pm->memspec='C';
  pm->startaddr=0;
  pm->endaddr=0xFFFF;
  pm->editable=TRUE;
  memcfgs.AddHead(pm);
  pm=new mcfg_t;
  pm->memspec='X';
  pm->startaddr=0;
  pm->endaddr=0xFFFF;
  pm->editable=TRUE;
  memcfgs.AddHead(pm);
  pm=new mcfg_t;
  pm->memspec='I';
  pm->startaddr=0;
  pm->endaddr=0xFF;
  pm->editable=TRUE;
  memcfgs.AddHead(pm);
}

void CProc51::PackMemCfg()
{
  POSITION pos,pos1;
  int n,i,l;
  mcfg_t* m;
  mcfg_t* m1;

nextloop:
  n=memcfgs.GetCount();
  for(i=0;i<n;i++)
  {
    pos=memcfgs.FindIndex(i);
    m=(mcfg_t*)memcfgs.GetNext(pos);
    for(l=i+1;l<n;l++)
    {
      pos1=pos;
      m1=(mcfg_t*)memcfgs.GetNext(pos);
      if(m1->memspec==m->memspec)
      {
        if(   (m1->startaddr >= m->startaddr)
              && (m1->endaddr <= m->endaddr))
        {
          delete m1;
          memcfgs.RemoveAt(pos1);
          goto nextloop;
        }
        else if(   (m1->startaddr <= m->startaddr)
                   && (m1->endaddr <= m->endaddr)
                   && (m1->endaddr >= m->startaddr))
        {
          m->startaddr=m1->startaddr;
          delete m1;
          memcfgs.RemoveAt(pos1);
          goto nextloop;
        }
        else if(   (m1->startaddr >= m->startaddr)
                   && (m1->endaddr >= m->endaddr))
        {
          m->endaddr=m1->endaddr;
          delete m1;
          memcfgs.RemoveAt(pos1);
          goto nextloop;
        }
        else if(   (m1->startaddr <= m->startaddr)
                   && (m1->endaddr >= m->endaddr))
        {
          *m=*m1;
          delete m1;
          memcfgs.RemoveAt(pos1);
          goto nextloop;
        }
      }
    }
  }
}

void CProc51::GenerateTXInt()
{
  if(datamem[IP0] & 0x10 == 0)
    intReq |= 0x1000;
  else
    intReq |= 0x0010;
}

void CProc51::HandleTimers(int ticks)
{
  int prio;
  int ticks1;


  if(datamem[TCON]&0x50) //TR0 oder TR1 aktiv
  {
    ticks1=ticks;
    if(datamem[TCON]&0x10 && ((datamem[TMOD]&0x08)==0 || (datamem[PORT3]&0x02)!=0)) //Timer0 enabled
    {
      switch(datamem[TMOD]&0x07)  //datamem[0x89]&0x07
      {
        case 0x00: //Mode 0 - intern getaktet
          while(ticks--)
          {
            datamem[TL0]++;
            if(datamem[TL0]>0x1F)
            {
              datamem[TL0]=0;
              datamem[TH0]++;
            }
            if((datamem[TL0]|datamem[TH0])==0)  //Überlauf
            {
              prio=datamem[IP0];
              if(prio & 0x02) //hohe Priorität
                intReq |= 0x0002;
              else
                intReq |= 0x0200;
              datamem[TCON]|=0x20;  //Interruptbit TF0 setzen
              datamem[TL0]+=ticks;
              goto handleT1;
            }
          }
          break;
        case 0x04: //Mode0 - extern
          if(extTick0)
          {
            extTick0=FALSE;
            datamem[TL0]++;
            if(datamem[TL0]>0x1F)
            {
              datamem[TL0]=0;
              datamem[TH0]++;
            }
            if((datamem[TL0]|datamem[TH0])==0)  //Überlauf
            {
              prio=datamem[IP0];
              if(prio & 0x02) //hohe Priorität
                intReq |= 0x0002;
              else
                intReq |= 0x0200;
              datamem[TCON]|=0x20;  //Interruptbit TF0 setzen
              goto handleT1;
            }
          }
          break;
        case 0x01: //Mode1 - intern
          while(ticks--)
          {
            datamem[TL0]++;
            if(datamem[TL0]==0)
              datamem[TH0]++;
            if((datamem[TL0]|datamem[TH0])==0)  //Überlauf
            {
              prio=datamem[IP0];
              if(prio & 0x02) //hohe Priorität
                intReq |= 0x0002;
              else
                intReq |= 0x0200;
              datamem[TCON]|=0x20;  //Interruptbit TF0 setzen
              datamem[TL0]+=ticks;
              goto handleT1;
            }
          }

          break;
        case 0x05: //Mode1 - extern
          if(extTick0)
          {
            extTick0=FALSE;
            datamem[TL0]++;
            if(datamem[TL0]==0)
              datamem[TH0]++;
            if((datamem[TL0]|datamem[TH0])==0)  //Überlauf
            {
              prio=datamem[IP0];
              if(prio & 0x02) //hohe Priorität
                intReq |= 0x0002;
              else
                intReq |= 0x0200;
              datamem[TCON]|=0x20;  //Interruptbit TF0 setzen
              goto handleT1;
            }
          }

          break;
        case 0x02: //Mode2 - intern
          while(ticks--)
          {
            datamem[TL0]++;   //datamem[0x8A]
            if(datamem[TL0]==0)
            {
              datamem[TL0]=datamem[TH0];   //TH0= datamem[0x8C]
              prio=datamem[IP0];
              if(prio & 0x02) //hohe Priorität
                intReq |= 0x0002;
              else
                intReq |= 0x0200;
              datamem[TCON]|=0x20;  //Interruptbit TF0
              datamem[TL0]+=ticks;
              goto handleT1;
            }
          }
          break;
        case 0x06: //Mode2 - extern
          if(extTick0)
          {
            extTick0=FALSE;
            datamem[TL0]++;   //datamem[0x8A]
            if(datamem[TL0]==0)
            {
              datamem[TL0]=datamem[TH0];   //TH0= datamem[0x8C]
              prio=datamem[IP0];
              if(prio & 0x02) //hohe Priorität
                intReq |= 0x0002;
              else
                intReq |= 0x0200;
              datamem[TCON]|=0x20;  //Interruptbit TF0
              goto handleT1;
            }
          }
          break;
        case 0x03: //Mode3 - intern
          while(ticks--)
          {
            datamem[TL0]++;   //datamem[0x8A]
            if(datamem[TL0]==0)
            {
              prio=datamem[IP0];
              if(prio & 0x02) //hohe Priorität
                intReq |= 0x0002;
              else
                intReq |= 0x0200;
              datamem[TCON]|=0x20;  //Interruptbit TF0
            }
            if(datamem[TCON]&0x40) //TR1
            {
              datamem[TH0]++;   //datamem[0x8C]
              if(datamem[TH0]==0)
              {
                prio=datamem[IP0];
                if(prio & 0x08) //hohe Priorität
                  intReq |= 0x0008;
                else
                  intReq |= 0x0800;
                datamem[TCON]|=0x80;  //Interruptbit TF1
              }
            }
          }

          break;
        case 0x07: //Mode3 - extern
          break;

      }
    }
handleT1:
    if(datamem[TCON]&0x40 && ((datamem[TMOD]&0x80)==0 || (datamem[PORT3]&0x04)!=0)) //Timer1 enabled
    {
      switch(datamem[TMOD]&0x70)  //datamem[0x89]&0x80     //TCON= datamem[0x88] //PORT3=0xB0
      {
        case 0x00: //Mode 0 - intern getaktet
          while(ticks1--)
          {
            datamem[TL1]++;
            if(datamem[TL1]>0x1F)
            {
              datamem[TL1]=0;
              datamem[TH1]++;
            }
            if((datamem[TL1]|datamem[TH1])==0)  //Überlauf
            {
              prio=datamem[IP0];
              if(prio & 0x08) //hohe Priorität
                intReq |= 0x0008;
              else
                intReq |= 0x0800;
              datamem[TCON]|=0x80;  //Interruptbit TF1 setzen
              datamem[TL1]+=ticks1;
              return;
            }
          }
          break;
        case 0x40: //Mode0 - extern
          if(extTick1)
          {
            extTick1=FALSE;
            datamem[TL1]++;
            if(datamem[TL1]>0x1F)
            {
              datamem[TL1]=0;
              datamem[TH1]++;
            }
            if((datamem[TL1]|datamem[TH1])==0)  //Überlauf
            {
              prio=datamem[IP0];
              if(prio & 0x08) //hohe Priorität
                intReq |= 0x0008;
              else
                intReq |= 0x0800;
              datamem[TCON]|=0x80;  //Interruptbit TF1 setzen
              return;
            }
          }

          break;
        case 0x10: //Mode1 - intern
          while(ticks1--)
          {
            datamem[TL1]++;
            if(datamem[TL1]==0)
              datamem[TH1]++;
            if((datamem[TL1]|datamem[TH1])==0)  //Überlauf
            {
              prio=datamem[IP0];
              if(prio & 0x08) //hohe Priorität
                intReq |= 0x0008;
              else
                intReq |= 0x0800;
              datamem[TCON]|=0x80;  //Interruptbit TF1 setzen
              datamem[TL1]+=ticks1;
              return;
            }
          }

          break;
        case 0x50: //Mode1 - extern
          if(extTick1)
          {
            extTick1=FALSE;
            datamem[TL1]++;
            if(datamem[TL1]==0)
              datamem[TH1]++;
            if((datamem[TL1]|datamem[TH1])==0)  //Überlauf
            {
              prio=datamem[IP0];
              if(prio & 0x08) //hohe Priorität
                intReq |= 0x0008;
              else
                intReq |= 0x0800;
              datamem[TCON]|=0x80;  //Interruptbit TF1 setzen
              return;
            }
          }

          break;
        case 0x20: //Mode2 - intern
          while(ticks1--)
          {
            datamem[TL1]++;
            if(datamem[TL1]==0)
            {
              datamem[TL1]=datamem[TH1];
              prio=datamem[IP0];
              if(prio & 0x08) //hohe Priorität
                intReq |= 0x0008;
              else
                intReq |= 0x0800;
              datamem[TCON]|=0x80;  //Interruptbit TF1
              datamem[TL1]+=ticks1;
              return;
            }
          }
          break;
        case 0x60: //Mode2 - extern
          if(extTick1)
          {
            extTick1=FALSE;
            datamem[TL1]++;   //datamem[0x8A]
            if(datamem[TL1]==0)
            {
              datamem[TL1]=datamem[TH1];   //TH0
              prio=datamem[IP0];
              if(prio & 0x08) //hohe Priorität
                intReq |= 0x0008;
              else
                intReq |= 0x0800;
              datamem[TCON]|=0x80;  //Interruptbit TF1
              return;
            }

          }
          break;
        case 0x30: //Mode3 - intern , Timer1 does nothing
          break;
        case 0x70: //Mode3 - extern
          break;

      }
    }
    if(theApp.m_wndTimers.Created && theApp.m_wndTimers.GetStyle()&WS_VISIBLE)
      theApp.m_wndTimers.UpdateDlgBar();
  }
}
