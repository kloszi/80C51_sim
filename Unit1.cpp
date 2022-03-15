//---------------------------------------------------------------------------

#include <vcl.h>
#include <math.hpp>
#pragma   hdrstop

#include "Help_dlg.h"
#include "Unit1.h"
#include "Unit2.h"
#include "Schematic_paint.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void  __fastcall TForm1::simulation()
{
static int A=0;
if(refresh)
    {
    Paint_box->Canvas->Lock();// >TryLock();
    Lista_elementow->repaint();
    Paint_box->Canvas->Unlock();
    refresh=false;
    }

Locall_time++;
//Lista_elementow->element_stack->Elementy.simulate() ;  //optymalizacja do szybkoœci wykonywania kodu
//powy¿sza funkcja to samo realizuje poniewaz na pocz¹tku jest jej przypisany adres funkcji
WSK_do_simulate->simulate();
 if(TrackBar1->Position<TrackBar1->Max)
        A++;
        if(A>TrackBar1->Position*5)
           {
           Sleep(1);
           A=0;
           }
return;
}
//----------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
//Lista_elementow=new Element_list(Paint_box->PaintBox1);
//list_of_procesor=new vector<i8051*>;
Locall_time=0;
//0 - nie jest nad   objaktem
//1 - nad objektem   (bramk¹)   niezaznaczonym
//2 - jestesmy nad   zaznaczonym elementem
//3 - jest nad port  niezaznaczonym
//4 - jest nad port  zaznaczonym
//5 - jest nad wire  niezaznaczonym
//6 - jest nad wire  zaznaczonym
Opening_dir=ExtractFileDir(Application->ExeName)+"\\Exemple";
StatusBar1->Panels->Items[1]->Text="  OFF";
simulating=false;

}
//---------------------------------------------------------------------------
void __fastcall TForm1::SpeedButton9Click(TObject *Sender)
{
if(simulating)
    return;
Lista_elementow->change_draw_type(switch_type);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SpeedButton5Click(TObject *Sender)
{
if(simulating)
    return;
Lista_elementow->change_draw_type(MCS51_TYPE);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SpeedButton8Click(TObject *Sender)
{

Lista_elementow->change_draw_type(PORT);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SpeedButton4Click(TObject *Sender)
{

Lista_elementow->change_draw_type(LED_7_TYPE);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SpeedButton6Click(TObject *Sender)
{
if(simulating)
    return;
Lista_elementow->change_draw_type(VCC_TYPE);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SpeedButton7Click(TObject *Sender)
{

Lista_elementow->change_draw_type(GND_TYPE);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BitBtn1Click(TObject *Sender)
{
//wy³¹czenie poprzez przycisk
if(simulating)
    {
    test_thread->enable=false;
    delete test_thread;
    test_thread=NULL;
    BitBtn2->Enabled=false;
    simulating=false;

    Lista_elementow->stop_simulate();
    Lista_elementow->repaint();
    Locall_time=0;

    StatusBar1->Panels->Items[1]->Text="  OFF";

    SpeedButton1->Enabled=true;
    SpeedButton2->Enabled=true;
    SpeedButton3->Enabled=true;
    SpeedButton4->Enabled=true;
    SpeedButton5->Enabled=true;
    SpeedButton6->Enabled=true;
    SpeedButton7->Enabled=true;
    SpeedButton8->Enabled=true;
    SpeedButton9->Enabled=true;
    SpeedButton10->Enabled=true;
    SpeedButton11->Enabled=true;
    SpeedButton12->Enabled=true;
    SpeedButton13->Enabled=true;
    SpeedButton15->Enabled=true;
    MainMenu1->Items[0].Enabled=false;

    }
else
    {
    if(Lista_elementow->start_simulate()==false)
        return;
    WSK_do_simulate=&(Lista_elementow->element_stack->Elementy);

    StatusBar1->Panels->Items[1]->Text="  ON";
    SpeedButton1->Enabled=false;
    SpeedButton2->Enabled=false;
    SpeedButton3->Enabled=false;
    SpeedButton4->Enabled=false;
    SpeedButton5->Enabled=false;
    SpeedButton6->Enabled=false;
    SpeedButton7->Enabled=false;
    SpeedButton8->Enabled=false;
    SpeedButton9->Enabled=false;
    SpeedButton10->Enabled=false;
    SpeedButton11->Enabled=false;
    SpeedButton12->Enabled=false;
    SpeedButton13->Enabled=false;
    SpeedButton15->Enabled=false;

    MainMenu1->Items[0].Enabled=false;
    MainMenu1->Items[0].Visible=false ;

    BitBtn2->Enabled=true;
    simulating=true;
    test_thread=new TTestThread();
    test_thread->enable=true;
    }
  }
//---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{

if(simulating)
    {
    test_thread->pause_=false;
    Sleep(50);

    test_thread->enable=false;
    Sleep(50);
    delete test_thread;
    }
Sleep(10);
delete Lista_elementow;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BitBtn1KeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
Lista_elementow->pres_key(Key) ;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ApplicationEvents1ShortCut(TWMKey &Msg,
      bool &Handled)
{
Lista_elementow->pres_key(Msg.CharCode) ;
Lista_elementow->repaint();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SpeedButton2Click(TObject *Sender)
{

Lista_elementow->change_draw_type(NOT);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
StatusBar1->Panels->Items[2]->Text=FloatToStr(RoundTo(Locall_time/1000000.0,-3))+(AnsiString)" [s]";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SpeedButton3Click(TObject *Sender)
{
Lista_elementow->change_draw_type(OR);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SpeedButton10Click(TObject *Sender)
{
Lista_elementow->change_draw_type(Generator_type);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SpeedButton11Click(TObject *Sender)
{
Lista_elementow->change_draw_type(Oscilloscope_type);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BitBtn2Click(TObject *Sender)
{
if(test_thread==NULL)
    return; //zabespieczenie czy istnieje proces
test_thread->pause_=!test_thread->pause_;
if(test_thread->pause_==true)
    StatusBar1->Panels->Items[1]->Text="  PAUSE";
else
    StatusBar1->Panels->Items[1]->Text="  ON";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Zapisz1Click(TObject *Sender)
{
SaveDialog1->InitialDir=ExtractFileDir(Application->ExeName)+"\\Exemple";
if (SaveDialog1->Execute())
    {
    Lista_elementow->save_file(SaveDialog1->FileName);
    Opening_dir=ExtractFileDir(SaveDialog1->FileName);
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Otwrz1Click(TObject *Sender)
{
OpenDialog1->InitialDir=ExtractFileDir(Application->ExeName)+"\\Exemple";
if (OpenDialog1->Execute())
    {
    Opening_dir=ExtractFileDir(OpenDialog1->FileName);
    Lista_elementow->open_file(OpenDialog1->FileName);
    Lista_elementow->repaint();
    if( FileExists(ExtractFileDir(OpenDialog1->FileName)+"\\info.txt"))
       {
        Info_form->Memo1->Lines->LoadFromFile(ExtractFileDir(OpenDialog1->FileName)+"\\info.txt");
        Info_form->ShowModal();
       }
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Wyjcie1Click(TObject *Sender)
{
Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SpeedButton12Click(TObject *Sender)
{
 Lista_elementow->change_draw_type(LED_1_TYPE);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::About1Click(TObject *Sender)
{
Help_form->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SpeedButton13Click(TObject *Sender)
{
Lista_elementow->change_draw_type(NAND);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SpeedButton1Click(TObject *Sender)
{
 Lista_elementow->change_draw_type(AND);
}
//---------------------------------------------------------------------------






