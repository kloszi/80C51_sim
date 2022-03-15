//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LED1_dlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSwith_form *Swith_form;
//---------------------------------------------------------------------------
__fastcall TSwith_form::TSwith_form(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSwith_form::FormCreate(TObject *Sender)
{
Memo1->Lines->LoadFromFile(ExtractFileDir(Application->ExeName)+"\\Help\\SWITCH.txt");    
}
//---------------------------------------------------------------------------

