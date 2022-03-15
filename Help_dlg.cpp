//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Help_dlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "SHDocVw_OCX"
#pragma resource "*.dfm"
THelp_form *Help_form;
//---------------------------------------------------------------------------
__fastcall THelp_form::THelp_form(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall THelp_form::Label1Click(TObject *Sender)
{
ShellExecute(NULL,"open","www.kloszi.prv.pl",NULL,NULL,SW_SHOW);     
}
//---------------------------------------------------------------------------
void __fastcall THelp_form::FormShow(TObject *Sender)
{
//CppWebBrowser1->Navigate(WideString("www.kloszi.prv.pl"));
}
//---------------------------------------------------------------------------


