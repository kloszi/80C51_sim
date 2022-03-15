//---------------------------------------------------------------------------
#include "Unit1.h"
#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "Unit1.h"
//---------------------------------------------------------------------------
class TTestThread : public TThread
{            
private:

protected:
  void __fastcall Execute();
public:
  __fastcall TTestThread::TTestThread();
  bool enable;
  bool pause_;
  };
//---------------------------------------------------------------------------

#endif
 