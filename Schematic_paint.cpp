//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "Unit1.h"
#include "Schematic_paint.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPaint_box *Paint_box;
//---------------------------------------------------------------------------
__fastcall TPaint_box::TPaint_box(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TPaint_box::FormCreate(TObject *Sender)
{

Screen->Cursors[1] = LoadCursor (HInstance,"ID_moved") ;   //kursor przesuwania
Screen->Cursors[2] = LoadCursor(HInstance,"ID_conect") ; //kursor po³aczenia
Screen->Cursors[3] = LoadCursor(HInstance,"ID_cursor");    //domyslny kursor
Form1->Lista_elementow=new Element_list(PaintBox1);
}
//---------------------------------------------------------------------------

void __fastcall TPaint_box::PaintBox1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
if(Form1->simulating)
   {
   X_=X;
   Y_=Y;
   return;
   }
if(Button==mbRight)
   Form1->SpeedButton9->Down=true;
Form1->Lista_elementow->mouse_down(Button,Shift,X,Y) ;
}
//---------------------------------------------------------------------------

void __fastcall TPaint_box::PaintBox1MouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
if(Form1->simulating)  //zmienic do jednej funkcjii i wszystkie butomy Buton1->Enable=False;
    return;


Form1->Lista_elementow->mouse_up(Button,Shift,X,Y);
//wyciœniêcie wszystkich speedbuttonów

if(Form1->SpeedButton9->Down==false)
   {
   Form1->Lista_elementow->change_draw_type(arrow);
   Form1->SpeedButton9->Down=true;
   }
Form1->Lista_elementow->repaint();   
}
//---------------------------------------------------------------------------

void __fastcall TPaint_box::PaintBox1Paint(TObject *Sender)
{
if(Form1->simulating)
    return;
Form1->Lista_elementow->repaint();
}
//---------------------------------------------------------------------------

void __fastcall TPaint_box::PaintBox1MouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
int cur;
if(Form1->simulating)
    return;
cur=Form1->Lista_elementow->mouse_move(Shift,X,Y);
if(cur==3||cur==5)
{Paint_box->PaintBox1->Cursor=(TCursor)(2);
    return;
}
if(cur==4||cur==6)
{Paint_box->PaintBox1->Cursor=(TCursor)(1);
return;}

if(cur==2||cur==1)
    {
    //cur=Lista_elementow->mouse_move(Shift,X,Y);
    Paint_box->PaintBox1->Cursor=(TCursor)(1);
    return;
    }
Paint_box->PaintBox1->Cursor=(TCursor)(3);
}
//---------------------------------------------------------------------------

void __fastcall TPaint_box::FormPaint(TObject *Sender)
{
if(Form1->simulating)
   {
   Form1->refresh=true;
   return;
   }
else
Form1->Lista_elementow->repaint();
}
//---------------------------------------------------------------------------


void __fastcall TPaint_box::PaintBox1DblClick(TObject *Sender)
{
if(Form1->simulating)  //podczas symumulcji mozemy tylko zmieniæ stan prze³¹cznika
   {
   TPoint mouse_down(X_,Y_);
   Form1->Lista_elementow->double_click(&mouse_down);//klikniêcia na prze³¹cznik
   Form1->refresh=true;
   return;
   }   
}
//---------------------------------------------------------------------------

