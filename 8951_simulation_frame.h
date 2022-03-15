//---------------------------------------------------------------------------
#ifndef _simulation_frame_
#define _simulation_frame_
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <vcl.h>
#include <Grids.hpp>
#include <fstream.h> 

#include "Procesor\\89C51.h"

class TMicrochip_frame
{

public:		// User declarations
   TMicrochip_frame(TPaintBox *Obrazek);  //¿eby mieæ rodzica
   ~TMicrochip_frame();
   void Show();
   void Hide();
   void Add_procesor(i8051  *Proc_WE);
   void Load_Lst_File(AnsiString filename);
   void Refresh();
   void Stop_simulation(); //trzeba sprawdziæ czy procesor OK
   bool Start_simulation(); //trzeba sprawdziæ czy procesor OK

   long HexToDec(char* Hex_number);
   TTrackBar    *TrackBar1;
   TLabel       *Label1;
   TLabel       *Label2;
   TLabel       *Label3;

   TLabel       *Acc_register;
   TLabel       *P0_register;
   TLabel       *P1_register;
   TLabel       *P2_register;
   TLabel       *P3_register;
   TForm        *Formatka; //formatka na której bedzie dezasembler
   TStringGrid  *StringGrid1;
   TStringGrid  *StringGrid2;
   TButton      *on_in_ram;

   TForm        *in_RAM;   //formatka wewnetrznego RAM-u
   TStringGrid  *StringGrid_in_RAM;

   char na_ekranie_wyswietlany_RAM[RamSize];

   i8051        *Proc;
   long          tab_PC[65535];
   int           cursor_position;
   long          old_PC;
   long          size_of_program;///wielkoœc programu wyrazona w linijkach kodu
   long          cycles;
   private:
   void __fastcall StringGrid1DrawCell(TObject *Sender, int ACol, int ARow, const TRect &Rect, TGridDrawState State);

   void __fastcall Button1Click(TObject *Sender);

};




#endif
 