//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include "Bramki.h"
#include "mouse_event.h"
#include "Unit2.h"
#include "Procesor\\89C51.h"
#include "info_dlg.h"
#include <AppEvnts.hpp>
#include <Grids.hpp>
#include <vector.h>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TMainMenu *MainMenu1;
    TMenuItem *Plik1;
    TMenuItem *Otwrz1;
    TMenuItem *Zapisz1;
    TMenuItem *Wyjcie1;
    TMenuItem *Help1;
    TMenuItem *About1;
    TStatusBar *StatusBar1;
    TSpeedButton *SpeedButton1;
    TSpeedButton *SpeedButton2;
    TSpeedButton *SpeedButton3;
    TSpeedButton *SpeedButton4;
    TSpeedButton *SpeedButton5;
    TSpeedButton *SpeedButton6;
    TSpeedButton *SpeedButton7;
    TSpeedButton *SpeedButton8;
    TSpeedButton *SpeedButton9;
    TSpeedButton *SpeedButton10;
    TPanel *Panel2;
    TBitBtn *BitBtn1;
    TApplicationEvents *ApplicationEvents1;
    TTimer *Timer1;
    TSpeedButton *SpeedButton11;
    TBitBtn *BitBtn2;
    TLabel *Label5;
    TOpenDialog *OpenDialog1;
    TSaveDialog *SaveDialog1;
    TSpeedButton *SpeedButton12;
    TSpeedButton *SpeedButton13;
    TSpeedButton *SpeedButton15;
        TTrackBar *TrackBar1;
    TLabel *Label1;
    TLabel *Label2;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall SpeedButton9Click(TObject *Sender);
    void __fastcall SpeedButton5Click(TObject *Sender);
    void __fastcall SpeedButton8Click(TObject *Sender);
    void __fastcall SpeedButton4Click(TObject *Sender);
    void __fastcall SpeedButton6Click(TObject *Sender);
    void __fastcall SpeedButton7Click(TObject *Sender);
    void __fastcall BitBtn1Click(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall BitBtn1KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ApplicationEvents1ShortCut(TWMKey &Msg, bool &Handled);
    void __fastcall SpeedButton2Click(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall SpeedButton3Click(TObject *Sender);
    void __fastcall SpeedButton10Click(TObject *Sender);
    void __fastcall SpeedButton11Click(TObject *Sender);
    void __fastcall BitBtn2Click(TObject *Sender);
    void __fastcall Zapisz1Click(TObject *Sender);
    void __fastcall Otwrz1Click(TObject *Sender);
    void __fastcall Wyjcie1Click(TObject *Sender);
    void __fastcall SpeedButton12Click(TObject *Sender);
    void __fastcall About1Click(TObject *Sender);
    void __fastcall SpeedButton13Click(TObject *Sender);
    void __fastcall SpeedButton1Click(TObject *Sender);
private:	// User declarations

public:
  Lista_elementow *WSK_do_simulate;   //adres funkcji non stop wywo³ywanej podczas symulacji
  Element_list    *Lista_elementow;
      __fastcall  TForm1(TComponent* Owner);
void  __fastcall  simulation();
   TTestThread    *test_thread;
   AnsiString     Opening_dir;

   bool         simulating;
   bool         refresh;//zmiennz ustawiana podczas symulacji ze wzgledu na wspó³bierzny charakter symulatora
   long         Locall_time; //oblicza czas

   //adres otwartego katalogu z przyk³adami
//-----------------------------------------------------------
//vector<i8051*> *list_of_procesor;  //lista wskaŸników do elementow
//vector<i8051*>::iterator iterator_list_of_procesor;




};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
