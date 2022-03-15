//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AND_dlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAND_frame *AND_frame;
//---------------------------------------------------------------------------
__fastcall TAND_frame::TAND_frame(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TAND_frame::FormShow(TObject *Sender)
{
 Label1->Caption=IntToStr(ID);   
}
//---------------------------------------------------------------------------



void __fastcall TAND_frame::FormCreate(TObject *Sender)
{
Memo1->Lines->LoadFromFile(ExtractFileDir(Application->ExeName)+"\\Help\\AND.txt");    
}
//---------------------------------------------------------------------------

