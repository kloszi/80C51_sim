//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LED7_dlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLED7_form *LED7_form;
//---------------------------------------------------------------------------
__fastcall TLED7_form::TLED7_form(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TLED7_form::FormCreate(TObject *Sender)
{
Memo1->Lines->LoadFromFile(ExtractFileDir(Application->ExeName)+"\\Help\\LED7.txt");

}
//---------------------------------------------------------------------------

