//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Generator_dlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma resource "*.dfm"
TGenerator *Generator;
//---------------------------------------------------------------------------
__fastcall TGenerator::TGenerator(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TGenerator::FormShow(TObject *Sender)
{
if(frequency>999)
    {
    ComboBox1->ItemIndex=1;    //w kHz
    CSpinEdit1->Value=frequency/1000;
    }
else
    {
    ComboBox1->ItemIndex=0;
    CSpinEdit1->Value=frequency;
    }
CSpinEdit2->Value=fill;
}
//---------------------------------------------------------------------------

void __fastcall TGenerator::OKClick(TObject *Sender)
{
fill=CSpinEdit2->Value;

if(ComboBox1->ItemIndex==1)
    {
    frequency=CSpinEdit1->Value*1000;
    }
else
    {
    frequency=CSpinEdit1->Value;
    }
}
//---------------------------------------------------------------------------

void __fastcall TGenerator::FormClose(TObject *Sender,
      TCloseAction &Action)
{
fill=CSpinEdit2->Value;

if(ComboBox1->ItemIndex==1)
    {
    frequency=CSpinEdit1->Value*1000;
    }
else
    {
    frequency=CSpinEdit1->Value;
    }
}
//---------------------------------------------------------------------------

void __fastcall TGenerator::ComboBox1Change(TObject *Sender)
{
 fill=CSpinEdit2->Value;

if(ComboBox1->ItemIndex==1)
    {
    frequency=CSpinEdit1->Value*1000;
    }
else
    {
    frequency=CSpinEdit1->Value;
    }
}
//---------------------------------------------------------------------------

void __fastcall TGenerator::FormCreate(TObject *Sender)
{
Memo1->Lines->LoadFromFile(ExtractFileDir(Application->ExeName)+"\\Help\\generator.txt");

}
//---------------------------------------------------------------------------

