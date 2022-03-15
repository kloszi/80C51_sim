//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "switch_dlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLed1_form *Led1_form;
//---------------------------------------------------------------------------
__fastcall TLed1_form::TLed1_form(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TLed1_form::FormCreate(TObject *Sender)
{
Memo1->Lines->LoadFromFile(ExtractFileDir(Application->ExeName)+"\\Help\\LED1.txt");
}
//---------------------------------------------------------------------------

