//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "NOT_dlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TNOT_frame *NOT_frame;
//---------------------------------------------------------------------------
__fastcall TNOT_frame::TNOT_frame(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TNOT_frame::FormCreate(TObject *Sender)
{
Memo1->Lines->LoadFromFile(ExtractFileDir(Application->ExeName)+"\\Help\\NOT.txt");

}
//---------------------------------------------------------------------------

