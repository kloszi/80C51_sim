//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "NANA_dlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TNAND_frame *NAND_frame;
//---------------------------------------------------------------------------
__fastcall TNAND_frame::TNAND_frame(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TNAND_frame::FormCreate(TObject *Sender)
{
Memo1->Lines->LoadFromFile(ExtractFileDir(Application->ExeName)+"\\Help\\NAND.txt");
}
//---------------------------------------------------------------------------

