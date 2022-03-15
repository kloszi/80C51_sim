//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TTestThread::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TTestThread::TTestThread()
  : TThread(False)
{
 pause_=false;
}
//---------------------------------------------------------------------------
void __fastcall TTestThread::Execute()
{
  //---- Place thread code here ----
  while (enable ) {
    // do the job
    Form1->simulation();

 // Sleep(1);


  while(pause_&&enable)
    Sleep(10);
  }

}
//---------------------------------------------------------------------------
 