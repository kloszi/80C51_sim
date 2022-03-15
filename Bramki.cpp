#include <vcl.h>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "AND_dlg.h"
#include "Bramki.h"
#include "wire_dlg.h"
#include "MCS_51_dlg.h"
#include "LED7_dlg.h"
#include "Generator_dlg.h"
#include "Unit1.h"
#include "NOT_dlg.h"
#include "OR_dlg.h"
#include "LED1_dlg.h"
#include "switch_dlg.h"
#include "Schematic_paint.h"
//wirtualna klasa ktora reprezentuje ogolnie komponent elektroniczny
int electronic::is_over(int x,int y)   //sprawdza czy podany pukt jest nad objektem
{
if((x>x_position)&&x<(x_position+x_size)&&
   (y>y_position)&&y<(y_position+y_size))
   {
   if (selected==true)
     return 2;
   if (selected==false)
     return 1;
   }
return 0;
}   
//-----------------------------------------------------------------------------
electronic::~electronic()            //odznacz
{
selected=false;
}
//-----------------------------------------------------------------------------
electronic::electronic()
{
gate_port=false;
Name="";
Element=unknow_element;
pointer_conector[0]=NULL;
pointer_conector[1]=NULL;
pointer_conector[2]=NULL;
pointer_conector[3]=NULL;
x_position=0;
y_position=0;
Signal=unknow;
selected=false;
x_size=0;
y_size=0;
plucienko=NULL;
}
//----------------------------------------------------------------------------
void electronic::select()
{
selected=true;
}
//-----------------------------------------------------------------------------
void electronic::deselect()
{
selected=false;
}
//-----------------------------------------------------------------------------
bool electronic::is_selected()
{
return selected;
}
//-------------------------------------------------------------------
void electronic::select(TPoint *click_point)
{
if(x_position<click_point->x&&click_point->x<(x_position+x_size)&&
    y_position<click_point->y&&click_point->y<(y_position+y_size))
    {
    select();
    }
else{
    deselect();
    }
}
//----------------------------------------------------------------------
void electronic::select(TPoint *first_click,TPoint *second_click)
{
TPoint position=Point(x_position,y_position);
if(point_is_insie_rect(first_click,second_click,&position)==true)
   select();
else
   deselect();
}
//-------------------------------------------------------------------------
void electronic::move(TPoint *Vector)
{
if(selected)  //jesli element jest zaznaczony mozna go przesuwac
    draw(x_position+Vector->x,y_position+Vector->y);
}
//-------------------------------------------------------------------------
bool electronic::point_is_insie_rect(TPoint *rect1,TPoint *rect2,TPoint *point)
{
int x1,x2,x3,y1,y2,y3;
x1=rect1->x;
y1=rect1->y;
x2=rect2->x;
y2=rect2->y;
//************************************
if(y1>y2) //zamiana y1 z y2
    {
    y3=y1;
    y1=y2;
    y2=y3;
    }
if(x1>x2) //zamiana x1 z x2
    {
    x3=x1;
    x1=x2;
    x2=x3;
    }
if(x1<point->x&&point->x<x2&&
   y1<point->y&&point->y<y2)
       return true;
   else
       return false;
}
/////////////////////////////////////////////////////////////////////////////
/////                           WIRE
////////////////////////////////////////////////////////////////////////////
wire::wire(TPaintBox *Obrazek)
{
Element=WIRE;
plucienko=Obrazek;

wire_up_menu=new TPopupMenu (plucienko);   //
Menu=new TMenuItem (plucienko);

Menu->Caption="W³aœciwoœci";
Menu->OnClick=display_wire_property;
wire_up_menu->Items->Clear();
wire_up_menu->Items->Add(Menu);

pointer_conector[0]=NULL;
pointer_conector[1]=NULL;
pointer_conector[2]=NULL;   //nadmiarowe wire wykorzystuje tylko 2 pierwsze :)
pointer_conector[3]=NULL;
selected=false;
}
//--------------------------------------------------------------------------
wire::~wire()
{
//zmazanie polaczenia
if(pointer_conector[0]!=NULL)    //od³aczamy wire po jednej stronie
    pointer_conector[0]->disconect(this);
if(pointer_conector[1]!=NULL)   //od³¹czamy wire po drugiej stronie
    pointer_conector[1]->disconect(this);
//zmazujemy fizyznie narysowane po³aczenie
draw(prewious_position1.x,prewious_position1.y,
        prewious_position2.x,prewious_position2.y,clWhite);
}
//------------------------------------------------------------------------------
void wire::disconect(electronic *conected_element)
{
if(pointer_conector[0]==conected_element)
   pointer_conector[0]=NULL;
if(pointer_conector[1]==conected_element)
   pointer_conector[1]=NULL;
}
//------------------------------------------------------------------------------
void wire::draw(int x1,int y1,int x2,int y2,TColor Color)
{
plucienko->Canvas->Pen->Mode=pmCopy;
plucienko->Canvas->Pen->Color=Color;
plucienko->Canvas->MoveTo(x1+3,y1+3);
plucienko->Canvas->LineTo(x1+3,y2+3); //rysujemy najpirw prosta w x
plucienko->Canvas->MoveTo(x1+3,y2+3);
plucienko->Canvas->LineTo(x2+3,y2+3); //rysujemy najpirw prosta w x
}
//--------------------------------------------------------------------------
void wire::draw(int x,int y)
{
int x1,y1,x2,y2;

if(pointer_conector[0]!=NULL)
   {
   x1=pointer_conector[0]->x_position;
   y1=pointer_conector[0]->y_position ;
   }
else
   {
   x1=prewious_position1.x;
   y1=prewious_position1.y;
   }

if(pointer_conector[1]!=NULL)
   {
   x2=pointer_conector[1]->x_position;
   y2=pointer_conector[1]->y_position;
   }
else
   {
   x2=prewious_position2.x;
   y2=prewious_position2.y;
   }

//Nie potrzeba mazaæ jeœli nie zmieniono pozycji
if(prewious_position1.x!=x1||
   prewious_position1.y!=y1||  //sprawdzamy czy któraæ wspó³rzêdna siê zmieni³a
   prewious_position2.x!=x2||  //i czy mamy zmazaæ t¹ liniê
   prewious_position2.y!=y2)
      draw(prewious_position1.x,prewious_position1.y,prewious_position2.x,
           prewious_position2.y,clWhite);

if(selected==true)
   draw(x1,y1,x2,y2,clRed);
else
   draw(x1,y1,x2,y2,clBlack);

prewious_position1=Point(x1,y1);
prewious_position2=Point(x2,y2);
}
//-------------------------------------------------------------------------
//0 - nie jest nad   objaktem
//1 - nad objektem  (bramk¹)   niezaznaczonym
//2 - jestesmy nad zaznaczonym elementem
//3 - jest nad port  niezaznaczonym
//4 - jest nad port  zaznaczonym
//5 - jest nad wire  niezaznaczonym
//6 - jest nad wire  zaznaczonym
//----------------------------------------------------------------------------
//Nie rozumie co tu napisalem ale to dzia³ ;)
int  wire::is_over(int x,int y)
{
int x1,y1,x2,y2,x_tmp,y_tmp;

if(pointer_conector[0]!=NULL)
   {
   x1=pointer_conector[0]->x_position;
   y1=pointer_conector[0]->y_position;
   }
else
   {
   x1=prewious_position1.x;
   y1=prewious_position1.y;
   }

if(pointer_conector[1]!=NULL)
   {
   x2=pointer_conector[1]->x_position;
   y2=pointer_conector[1]->y_position;
   }
else
   {
   x2=prewious_position2.x;
   y2=prewious_position2.y;
   }


 x1=x1+10;
 x2=x2+10;
 if(y2<y&&(y2+7)>y)
   {
    if(x1<x2)
      {
      x1=x1-20;
      x2=x2-20;
      x_tmp=x1;
      x1=x2;
      x2=x_tmp;
      }
    if(x<x1&&x>x2)
       {
       if(selected==false)
          return 5;
       else
          return 6;
         }
    }
 x1=x1-5;
 y1=y1-10;
 y2=y2-10;
 if(x1-7<x&&x1>x)
   {
    if(y1<y2)
      {
      y1=y1+20;
      y2=y2+20;
      y_tmp=y1;
      y1=y2;
      y2=y_tmp;
      }
    if(y<y1&&y>y2)
       {
       if(selected==false)
          return 5;
       else
          return 6;
         }
    }
  return 0;
}
//----------------------------------------------------------------------------
void wire::select(TPoint *click_point)
{
 if(is_over(click_point->x,click_point->y)!=0)
    selected=true;
 else
    selected=false;
}
//----------------------------------------------------------------------------
void wire::select(TPoint *first_click,TPoint *second_click)
{
TPoint position_wire_1,position_wire_2;
if(pointer_conector[0]!=NULL)
    position_wire_1=Point(pointer_conector[0]->x_position,pointer_conector[0]->y_position);
else
    position_wire_1=prewious_position1;

if(pointer_conector[1]!=NULL)
   position_wire_2=Point(pointer_conector[1]->x_position,pointer_conector[1]->y_position);
else
    position_wire_2=prewious_position2;


if(point_is_insie_rect(first_click,second_click,&position_wire_1)&&
   point_is_insie_rect(first_click,second_click,&position_wire_2))
   selected=true;
else
    selected=false;
}
//--------------------------------------------------------------------------
void __fastcall  wire::display_wire_property(TObject *Sender)
{
Wire_form->change_->Checked=false;
Wire_form->ID->Caption=IntToStr(ID);

if(pointer_conector[0]!=NULL)
   Wire_form->Label4->Caption=IntToStr(pointer_conector[0]->ID);
if(pointer_conector[1]!=NULL)
   Wire_form->Label7->Caption=IntToStr(pointer_conector[1]->ID);

Wire_form->ShowModal();

if(Wire_form->change_->Checked==true&&pointer_conector[0]!=NULL&&pointer_conector[1]!=NULL)   //trzeba dopisaæ zmazywanie starej lini
    {
    draw(prewious_position1.x,prewious_position1.y,                           /////TRZEBA POPRAWIÆ
         prewious_position2.x,prewious_position2.y,clWhite);
    pointer_conector[2]=pointer_conector[0];
    pointer_conector[0]=pointer_conector[1];
    pointer_conector[1]=pointer_conector[2];
    draw(pointer_conector[0]->x_position,pointer_conector[0]->y_position,
            pointer_conector[1]->x_position,pointer_conector[1]->y_position,clBlack);
    }
}
//--------------------------------------------------------------------------
bool wire::set_property(TPoint *click_point)
{
GetCursorPos(click_point);
wire_up_menu->Popup((*click_point).x,(*click_point).y);
return true;
}
//--------------------------------------------------------------------------
void wire::conect(electronic* input_conection,electronic* output_conection)
{
pointer_conector[0]=input_conection;
pointer_conector[1]=output_conection;
prewious_position1=Point(pointer_conector[0]->x_position,pointer_conector[0]->y_position);
prewious_position2=Point(pointer_conector[1]->x_position,pointer_conector[1]->y_position);
}
/////////////////////////////////////////////////////////////////////////////
//                          PORT
/////////////////////////////////////////////////////////////////////////////
port::port(TPaintBox *Obrazek,TPoint *Punkt)
{
gate_port=false;   //port nie nalezy do bramki
Element=PORT;      //typ eleementu PORT
selected=false;
Signal=unknow;    //sygna³ jaki jes przechowywany
plucienko=Obrazek;
x_position=Punkt->x;
y_position=Punkt->y;
x_size=8;
y_size=8;
pointer_conector[0]=NULL;
pointer_conector[1]=NULL;
pointer_conector[2]=NULL;
pointer_conector[3]=NULL;
}
//-------------------------------------------------------------------------
port::~port()
{
plucienko->Canvas->Pen->Mode=pmCopy;
plucienko->Canvas->Pen->Color=clWhite;
plucienko->Canvas->Brush->Color=clWhite;
plucienko->Canvas->Ellipse(x_position,y_position,x_position+7,y_position+7);

plucienko->Canvas->Brush->Color=clWhite;
plucienko->Canvas->Brush->Style =bsClear ;
plucienko->Canvas->Pen->Mode=pmCopy; 
for (int i=0;i<4;i++)
    {
    if(pointer_conector[i]!=NULL)   //sprawdzamy czy wogóle por jest pod³¹czony
      pointer_conector[i]->disconect(this);//od³¹czamy poszczególne porty
    }
}
//------------------------------------------------------------------------------
void   port::disconect(electronic *conected_element)
{
for (int i=0;i<4;i++)
    {
    if(pointer_conector[i]==conected_element)   //sprawdzamy czy wogóle por jest pod³¹czony
      pointer_conector[i]=NULL;
    }
}
//------------------------------------------------------------------------------
void   port::select(TPoint *first_click,TPoint *second_click)
{
TPoint position=Point(x_position,y_position);
if(gate_port==false)   //sprawdzamy czy mozna zaznaczyæ
    selected=point_is_insie_rect(first_click,second_click,&position);
}
//--------------------------------------------------------------------------
void port::move(TPoint *Vector)
{
 if(selected && !gate_port)  //jesli element jest zaznaczony mozna go przesuwac
 draw(x_position+Vector->x,y_position+Vector->y);
}
/////////////////////////////////////////
//Sprawdzamy czy kursor jest na tym komponencie
//0 - nie jest nad   objaktem
//1 - nad objektem   (bramk¹)   niezaznaczonym
//2 - jestesmy nad   zaznaczonym elementem
//3 - jest nad port  niezaznaczonym
//4 - jest nad port  zaznaczonym
//5 - jest nad wire  niezaznaczonym
//6 - jest nad wire  zaznaczonym
//----------------------------------------------------------------------------
int  port::is_over(int x,int y)
{
int x_wzg=x-x_position; //wzgledna pozycja kursora
int y_wzg=y-y_position; //czyli nad ktora czescia wire znajduje sie kursor
//jak port jest przy bramce to mo¿eny tylko ³¹czyæ z tym portem
if(gate_port==false)
    {
    if(x_wzg>-1&&x_wzg<8&&y_wzg>-2&&y_wzg<8)
        {
        if(selected==true)
            return 4;
        if(selected==false)
            return 3;
        }
    }
else
    {
    if(x_wzg>-1&&x_wzg<8 && y_wzg>-1&&y_wzg<8)
        return 3;
    }
return 0;
}
//----------------------------------------------------------------------------
port*  port::PositionToAddress(int x,int y)
{
//sprawdzam czy jest nad portem jak tak to zwracam adres tego objektu
int cur=is_over(x,y);
if(cur==3||cur==4)
    return this;
return NULL;
}
//----------------------------------------------------------------------------
void port::draw(int x,int y)
{
plucienko->Canvas->Pen->Mode=pmCopy;


if(x!=x_position||y!=y_position)
    {
    plucienko->Canvas->Pen->Color=clWhite; //zmazujemy
    plucienko->Canvas->Brush->Color=clWhite;
    plucienko->Canvas->Ellipse(x_position,y_position,x_position+7,y_position+7);
    }

if(selected==false)
    {
    plucienko->Canvas->Pen->Color=clBlack;
    plucienko->Canvas->Brush->Color=clBlack;
    }
else
    {
    plucienko->Canvas->Pen->Color=clRed;
    plucienko->Canvas->Brush->Color=clRed;
    }
plucienko->Canvas->Ellipse(x,y,x+7,y+7);
x_position=x;
y_position=y;

plucienko->Canvas->Brush->Color=clWhite;
plucienko->Canvas->Brush->Style =bsClear ;
plucienko->Canvas->Pen->Mode=pmCopy;
}
//---------------------------------------------------------------------------
bool   port::propagate(TSignal in_Signal)
{
//zabespieczenie przed ponownym zapisaniem wartosci
                                              
if(in_Signal==Signal)
   return true; //nie trzeba propagowaæ dalej takiego sygna³u
Signal=in_Signal;
for (int i=0;i<4;i++)
{
    if(pointer_conector[i]!=NULL)
    {
     if(pointer_conector[i]->pointer_conector[0]!=this&&
        pointer_conector[i]->pointer_conector[0]!=NULL)//sprawdzamy czy ten koniec nie jest podpiêty do siebie samego
        {
        pointer_conector[i]->pointer_conector[0]->propagate(in_Signal);
        }
     if(pointer_conector[i]->pointer_conector[1]!=this&&
        pointer_conector[i]->pointer_conector[1]!=NULL)
        {
        pointer_conector[i]->pointer_conector[1]->propagate(in_Signal);
        }
    }
}
return true;
}
////////////////////////////////////////////////////////////////////////////////
//                    Prze³¹cznik
////////////////////////////////////////////////////////////////////////////////
switch_::switch_(TPaintBox *Obrazek,TPoint *Punkt)
{
Element=switch_type;
plucienko=Obrazek;

switch_up_menu=new TPopupMenu (plucienko);   //
Menu=new TMenuItem (plucienko);

Menu->Caption="W³aœciwoœci przelacznika";
Menu->OnClick=display_switch_property;
switch_up_menu->Items->Clear();
switch_up_menu->Items->Add(Menu);

in_out_gate_port[0]=NULL; //brameczka nie pod³¹czona do portów
in_out_gate_port[1]=NULL;
in_out_gate_port[2]=NULL;

x_position=Punkt->x;
y_position=Punkt->y;
x_size=22;
y_size=40;

position=false;
}
///////////////////////////////////////////////////////////////////////
switch_::~switch_()
{
//kasujemy bramkê i portom pozwalamy sie przesuwaæ
if(in_out_gate_port[0]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[0]->gate_port=false;
if(in_out_gate_port[1]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[1]->gate_port=false;
if(in_out_gate_port[2]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[2]->gate_port=false;

draw(x_position,y_position,clWhite);
}
//------------------------------------------------------------------------------
void switch_::draw(int x,int y)
{
 if(in_out_gate_port[0]==NULL||
    in_out_gate_port[1]==NULL||
    in_out_gate_port[2]==NULL)
    return;  //nadmiaroe zabepieczenie je¿eli jedno z wejœ nie jest podpiête

 in_out_gate_port[0]->draw(x,y-3);
 in_out_gate_port[1]->draw(x,y+38);
 in_out_gate_port[2]->draw(x+24,y+19);

if(x!=x_position||y!=y_position) //jeœli komponent znajduje sie na tym samym miejscu nie trzeba go mazaæ
   draw(x_position,y_position,clWhite);

if(selected)
   draw(x,y,clRed);
else
   draw(x,y,clBlack);

x_position=x;
y_position=y;
}
//------------------------------------------------------------------------------
void switch_::switching()
{
draw(x_position,y_position,clWhite);

position=!position;
draw(x_position,y_position);
}
//------------------------------------------------------------------------------
bool switch_::set_property(TPoint *click_point)
{
GetCursorPos(click_point);
switch_up_menu->Popup((*click_point).x,(*click_point).y);
return true;
}
//------------------------------------------------------------------------------
void switch_::conect_gate_to_port(electronic **tab_conection_port)
{
 in_out_gate_port[0]=tab_conection_port[0];
 in_out_gate_port[1]=tab_conection_port[1];
 in_out_gate_port[2]=tab_conection_port[2];
 in_out_gate_port[0]->gate_port=true;
 in_out_gate_port[1]->gate_port=true;
 in_out_gate_port[2]->gate_port=true;
}
//------------------------------------------------------------------------------
void switch_::simulate()
{
if(position==true)
 in_out_gate_port[2]->propagate(in_out_gate_port[0]->Signal);
else
 in_out_gate_port[2]->propagate(in_out_gate_port[1]->Signal);
}
//------------------------------------------------------------------------------
void __fastcall  switch_::display_switch_property(TObject *Sender)
{
Swith_form->Label5->Caption=IntToStr(ID);
Swith_form->Label8->Caption=IntToStr(in_out_gate_port[0]->ID);
Swith_form->Label6->Caption=IntToStr(in_out_gate_port[1]->ID);
Swith_form->Label7->Caption=IntToStr(in_out_gate_port[2]->ID);
Swith_form->ShowModal();
}
//------------------------------------------------------------------------------
void switch_::draw(int x,int y,TColor Color)
{

plucienko->Canvas->Pen->Mode=pmCopy;
plucienko->Canvas->Pen->Color=Color;
plucienko->Canvas->MoveTo(x+3,y);
plucienko->Canvas->LineTo(x+3,y+10);

plucienko->Canvas->Ellipse(x,y+10,x+6,y+16);
plucienko->Canvas->Ellipse(x,y+28,x+6,y+34);

plucienko->Canvas->MoveTo(x+3,y+34);
plucienko->Canvas->LineTo(x+3,y+44);

plucienko->Canvas->Ellipse(x+13,y+19,x+19,y+25);

plucienko->Canvas->MoveTo(x+19,y+22);
plucienko->Canvas->LineTo(x+29,y+22);

plucienko->Canvas->MoveTo(x+13,y+22);
if(position==true)
    plucienko->Canvas->LineTo(x-4,y+13);
else
    plucienko->Canvas->LineTo(x-4,y+29);
}
/////////////////////////////////////////////////////////////////////////////
//                          Brameczka AND
/////////////////////////////////////////////////////////////////////////////
void AND_gate::draw(int x,int y,TColor Color)
{

plucienko->Canvas->Pen->Mode=pmCopy;
plucienko->Canvas->Pen->Color=Color;

plucienko->Canvas->MoveTo(x+30,y);
plucienko->Canvas->LineTo(x+10,y);
plucienko->Canvas->LineTo(x+10,y+24);
plucienko->Canvas->LineTo(x+30,y+24);

plucienko->Canvas->MoveTo(x,y+5);  //1 wejscie
plucienko->Canvas->LineTo(x+10,y+5);

plucienko->Canvas->MoveTo(x,y+19); //2 wejscie
plucienko->Canvas->LineTo(x+10,y+19);

plucienko->Canvas->Arc(x+18,y,x+42,y+24,x+30,y+24,x+30,y);

plucienko->Canvas->MoveTo(x+42,y+12); //2 wejscie
plucienko->Canvas->LineTo(x+52,y+12);

}
//------------------------------------------------------------------------------
void AND_gate::draw(int x,int y)
{
 if(in_out_gate_port[0]==NULL||
    in_out_gate_port[1]==NULL||
    in_out_gate_port[2]==NULL)
    return;  //nadmiaroe zabepieczenie

 in_out_gate_port[0]->draw(x-7,y+2);
 in_out_gate_port[1]->draw(x-7,y+16);
 in_out_gate_port[2]->draw(x+52,y+9);

if(x!=x_position||y!=y_position) //jeœli komponent znajduje sie na tym samym miejscu nie trzeba go mazaæ
   draw(x_position,y_position,clWhite);

if(selected)
   draw(x,y,clRed);
else
   draw(x,y,clBlack);

x_position=x;
y_position=y;
}
//-----------------------------------------------------------------------------
/// CZyszczenie za soba obrazu
AND_gate::~AND_gate()
{
//kasujemy bramkê i portom pozwalamy sie przesuwaæ
if(in_out_gate_port[0]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[0]->gate_port=false;
if(in_out_gate_port[1]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[1]->gate_port=false;
if(in_out_gate_port[2]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[2]->gate_port=false;

draw(x_position,y_position,clWhite);
}
//----------------------------------------------------------------------------
//konstruktor brameczki
AND_gate::AND_gate(TPaintBox *Obrazek,TPoint *Punkt)
{
Element=AND;
plucienko=Obrazek;

AND_up_menu=new TPopupMenu (plucienko);   //
Menu=new TMenuItem (plucienko);

Menu->Caption="W³aœciwoœci AND";
Menu->OnClick=display_AND_property;
AND_up_menu->Items->Clear();
AND_up_menu->Items->Add(Menu);

in_out_gate_port[0]=NULL; //brameczka nie pod³¹czona do portów
in_out_gate_port[1]=NULL;
in_out_gate_port[2]=NULL;

x_position=Punkt->x;
y_position=Punkt->y;
x_size=50;
y_size=20;
}
//----------------------------------------------------------------------------
void AND_gate::conect_gate_to_port(electronic **tab_conection_port)
{
 in_out_gate_port[0]=tab_conection_port[0];
 in_out_gate_port[1]=tab_conection_port[1];
 in_out_gate_port[2]=tab_conection_port[2];
 in_out_gate_port[0]->gate_port=true;
 in_out_gate_port[1]->gate_port=true;
 in_out_gate_port[2]->gate_port=true;
}
//----------------------------------------------------------------------------
void __fastcall AND_gate::display_AND_property(TObject *Sender)
{
AND_frame->ID=ID;
AND_frame->Label6->Caption=IntToStr(in_out_gate_port[0]->ID);
AND_frame->Label7->Caption=IntToStr(in_out_gate_port[1]->ID);
AND_frame->Label8->Caption=IntToStr(in_out_gate_port[2]->ID);
AND_frame->ShowModal();
}
//----------------------------------------------------------------------------
bool AND_gate::set_property(TPoint *click_point)
{
//Ustawienie w³aœciwoœci komponenty
GetCursorPos(click_point);
AND_up_menu->Popup((*click_point).x,(*click_point).y);
return true;
}
//-----------------------------------------------------------------------------
void AND_gate::simulate()
{
if((in_out_gate_port[0]->Signal==up||in_out_gate_port[0]->Signal==pul_up)&&
    (in_out_gate_port[1]->Signal==up||in_out_gate_port[1]->Signal==pul_up))
  in_out_gate_port[2]->propagate(up);
else
  in_out_gate_port[2]->propagate(down);
}
////////////////////////////////////////////////////////////////////////////////
//      Brameczka NAND
////////////////////////////////////////////////////////////////////////////////
void NAND_gate::draw(int x,int y,TColor Color)
{

plucienko->Canvas->Pen->Mode=pmCopy;
plucienko->Canvas->Pen->Color=Color;

plucienko->Canvas->MoveTo(x+30,y);
plucienko->Canvas->LineTo(x+10,y);
plucienko->Canvas->LineTo(x+10,y+24);
plucienko->Canvas->LineTo(x+30,y+24);


plucienko->Canvas->MoveTo(x,y+5);  //1 wejscie
plucienko->Canvas->LineTo(x+10,y+5);

plucienko->Canvas->MoveTo(x,y+19); //2 wejscie
plucienko->Canvas->LineTo(x+10,y+19);

plucienko->Canvas->Arc(x+18,y,x+42,y+24,x+30,y+24,x+30,y);

plucienko->Canvas->Ellipse(x+42,y+9,x+48,y+15);
plucienko->Canvas->MoveTo(x+48,y+12); //2 wejscie
plucienko->Canvas->LineTo(x+52,y+12);

}
//------------------------------------------------------------------------------
void NAND_gate::draw(int x,int y)
{
 if(in_out_gate_port[0]==NULL||
    in_out_gate_port[1]==NULL||
    in_out_gate_port[2]==NULL)
    return;  //nadmiaroe zabepieczenie

 in_out_gate_port[0]->draw(x-7,y+2);
 in_out_gate_port[1]->draw(x-7,y+16);
 in_out_gate_port[2]->draw(x+52,y+9);

if(x!=x_position||y!=y_position) //jeœli komponent znajduje sie na tym samym miejscu nie trzeba go mazaæ
   draw(x_position,y_position,clWhite);

if(selected)
   draw(x,y,clRed);
else
   draw(x,y,clBlack);

x_position=x;
y_position=y;
}
//-----------------------------------------------------------------------------
/// CZyszczenie za soba obrazu
NAND_gate::~NAND_gate()
{
//kasujemy bramkê i portom pozwalamy sie przesuwaæ
if(in_out_gate_port[0]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[0]->gate_port=false;
if(in_out_gate_port[1]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[1]->gate_port=false;
if(in_out_gate_port[2]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[2]->gate_port=false;

draw(x_position,y_position,clWhite);
}
//----------------------------------------------------------------------------
//konstruktor brameczki
NAND_gate::NAND_gate(TPaintBox *Obrazek,TPoint *Punkt)
{
Element=NAND;
plucienko=Obrazek;

NAND_up_menu=new TPopupMenu (plucienko);   //
Menu=new TMenuItem (plucienko);

Menu->Caption="W³aœciwoœci NAND";
Menu->OnClick=display_NAND_property;
NAND_up_menu->Items->Clear();
NAND_up_menu->Items->Add(Menu);

in_out_gate_port[0]=NULL; //brameczka nie pod³¹czona do portów
in_out_gate_port[1]=NULL;
in_out_gate_port[2]=NULL;

x_position=Punkt->x;
y_position=Punkt->y;
x_size=50;
y_size=20;
}
//----------------------------------------------------------------------------
void NAND_gate::conect_gate_to_port(electronic **tab_conection_port)
{
 in_out_gate_port[0]=tab_conection_port[0];
 in_out_gate_port[1]=tab_conection_port[1];
 in_out_gate_port[2]=tab_conection_port[2];
 in_out_gate_port[0]->gate_port=true;
 in_out_gate_port[1]->gate_port=true;
 in_out_gate_port[2]->gate_port=true;
}
//----------------------------------------------------------------------------
void __fastcall NAND_gate::display_NAND_property(TObject *Sender)
{
NAND_frame->Label2->Caption=IntToStr(ID);
NAND_frame->Label5->Caption=IntToStr(in_out_gate_port[0]->ID);
NAND_frame->Label6->Caption=IntToStr(in_out_gate_port[1]->ID);
NAND_frame->Label8->Caption=IntToStr(in_out_gate_port[2]->ID);
NAND_frame->ShowModal();
}
//----------------------------------------------------------------------------
bool NAND_gate::set_property(TPoint *click_point)
{
//Ustawienie w³aœciwoœci komponenty
GetCursorPos(click_point);
NAND_up_menu->Popup((*click_point).x,(*click_point).y);
return true;
}
//-----------------------------------------------------------------------------
void NAND_gate::simulate()
{
if((in_out_gate_port[0]->Signal==up||in_out_gate_port[0]->Signal==pul_up)&&
    (in_out_gate_port[1]->Signal==up||in_out_gate_port[1]->Signal==pul_up))
  in_out_gate_port[2]->propagate(down);
else
  in_out_gate_port[2]->propagate(up);
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void NOT_gate::draw(int x,int y,TColor Color)
{
plucienko->Canvas->Pen->Mode=pmCopy;
plucienko->Canvas->Pen->Color=Color;

plucienko->Canvas->MoveTo(x,y);
plucienko->Canvas->LineTo(x,y+24);
plucienko->Canvas->LineTo(x+20,y+12);
plucienko->Canvas->LineTo(x,y);
plucienko->Canvas->Ellipse(x+20,y+9,x+26,y+15);
plucienko->Canvas->MoveTo(x+27,y+12);
plucienko->Canvas->LineTo(x+30,y+12);

plucienko->Canvas->MoveTo(x,y+12);
plucienko->Canvas->LineTo(x-4,y+12);
}


//------------------------------------------------------------------------------
void NOT_gate::draw(int x,int y)
{
if(in_out_gate_port[0]==NULL||
    in_out_gate_port[1]==NULL)
    return;

in_out_gate_port[0]->draw(x-10,y+10);
in_out_gate_port[1]->draw(x+30,y+10);

draw(x_position,y_position,clWhite);

if(selected)
   draw(x,y,clRed);
else
   draw(x,y,clBlack);

x_position=x;
y_position=y;
}
//-----------------------------------------------------------------------------
/// Czyszczenie za soba obrazu
NOT_gate::~NOT_gate()
{
//kasujemy bramkê i portom pozwalamy sie przesuwaæ
if(in_out_gate_port[0]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[0]->gate_port=false;
if(in_out_gate_port[1]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[1]->gate_port=false;


draw(x_position,y_position,clWhite);
}
//----------------------------------------------------------------------------
//konstruktor brameczki
NOT_gate::NOT_gate(TPaintBox *Obrazek,TPoint *Punkt)
{
Element=NOT;
plucienko=Obrazek;

NOT_up_menu=new TPopupMenu (plucienko);   //
Menu=new TMenuItem (plucienko);

Menu->Caption="W³aœciwoœci";
Menu->OnClick=display_NOT_property;
NOT_up_menu->Items->Clear();
NOT_up_menu->Items->Add(Menu);

x_position=Punkt->x;
y_position=Punkt->y;
x_size=30;
y_size=30;
}
//----------------------------------------------------------------------------
void NOT_gate::conect_gate_to_port(electronic **tab_conection_port)
{
 in_out_gate_port[0]=tab_conection_port[0];
 in_out_gate_port[1]=tab_conection_port[1];

 in_out_gate_port[0]->gate_port=true;
 in_out_gate_port[1]->gate_port=true;

}
//----------------------------------------------------------------------------
void __fastcall NOT_gate::display_NOT_property(TObject *Sender)
{
NOT_frame->Label5->Caption=IntToStr(ID);
NOT_frame->Label6->Caption=IntToStr(in_out_gate_port[0]->ID);
NOT_frame->Label1->Caption=IntToStr(in_out_gate_port[1]->ID);
NOT_frame->ShowModal();    
}
//----------------------------------------------------------------------------
bool NOT_gate::set_property(TPoint *click_point)
{
//Ustawienie w³aœciwoœci komponenty
GetCursorPos(click_point);
NOT_up_menu->Popup((*click_point).x,(*click_point).y);
return true;   
}
//-----------------------------------------------------------------------------
void NOT_gate::simulate()
{
//port *in_out_gate_port[3]
if(in_out_gate_port[0]->Signal==up||in_out_gate_port[0]->Signal==pul_up)
  in_out_gate_port[1]->propagate(down);
else
  in_out_gate_port[1]->propagate(up);
}
///////////////////////////////////////////////////////////////////////////////
//        Brameczka OR
///////////////////////////////////////////////////////////////////////////////

void OR_gate::draw(int x,int y,TColor Color)
{
plucienko->Canvas->Pen->Mode=pmCopy;
plucienko->Canvas->Pen->Color=Color;

plucienko->Canvas->Arc(x,y-40,x+60,y+40,x+30,y+40,x+56,y+20);

plucienko->Canvas->Arc(x,y,x+60,y+80,x+56,y+20,x+30,y);

plucienko->Canvas->MoveTo(x+10,y);
plucienko->Canvas->LineTo(x+30,y);

plucienko->Canvas->MoveTo(x+10,y+40);
plucienko->Canvas->LineTo(x+30,y+40);

plucienko->Canvas->Arc(x-35,y-15,x+15,y+50,x+10,y+40,x+10,y);

plucienko->Canvas->MoveTo(x+2,y+8);
plucienko->Canvas->LineTo(x+13,y+8);

plucienko->Canvas->MoveTo(x+2,y+28);
plucienko->Canvas->LineTo(x+13,y+28);

plucienko->Canvas->MoveTo(x+2,y+28);
plucienko->Canvas->LineTo(x+13,y+28);

plucienko->Canvas->MoveTo(x+55,y+20);
plucienko->Canvas->LineTo(x+65,y+20);

//   plucienko->Canvas->TextOutA(drawing_point[17].x,drawing_point[17].y,Name.c_str())  ;
}
//------------------------------------------------------------------------------
void OR_gate::draw(int x,int y)
{
 if(in_out_gate_port[0]==NULL||
    in_out_gate_port[1]==NULL||
    in_out_gate_port[2]==NULL)
    return;  //nadmiaroe zabepieczenie

 in_out_gate_port[0]->draw(x-5,y+5);
 in_out_gate_port[1]->draw(x-5,y+25);
 in_out_gate_port[2]->draw(x+65,y+17);

draw(x_position,y_position,clWhite);

if(selected)
   draw(x,y,clRed);
else
   draw(x,y,clBlack);

x_position=x;
y_position=y;
}
//-----------------------------------------------------------------------------
/// CZyszczenie za soba obrazu
OR_gate::~OR_gate()
{
if(in_out_gate_port[0]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[0]->gate_port=false;
if(in_out_gate_port[1]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[1]->gate_port=false;
if(in_out_gate_port[2]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[2]->gate_port=false;
    
draw(x_position,y_position,clWhite);  //trzeba dopisac po¿adny destruktor
}
//----------------------------------------------------------------------------
//konstruktor brameczki
OR_gate::OR_gate(TPaintBox *Obrazek,TPoint *Punkt)
{
Element=OR;
plucienko=Obrazek;

OR_up_menu=new TPopupMenu (plucienko);   //
Menu=new TMenuItem (plucienko);

Menu->Caption="W³aœciwoœci";
Menu->OnClick=display_OR_property;
OR_up_menu->Items->Clear();
OR_up_menu->Items->Add(Menu);

x_position=Punkt->x;
y_position=Punkt->y;
x_size=60;
y_size=40;
}
//----------------------------------------------------------------------------
void OR_gate::conect_gate_to_port(electronic **tab_conection_port)
{
 in_out_gate_port[0]=tab_conection_port[0];
 in_out_gate_port[1]=tab_conection_port[1];
 in_out_gate_port[2]=tab_conection_port[2];
 in_out_gate_port[0]->gate_port=true;
 in_out_gate_port[1]->gate_port=true;
 in_out_gate_port[2]->gate_port=true;
}
//----------------------------------------------------------------------------
void __fastcall OR_gate::display_OR_property(TObject *Sender)
{
OR_frame->Label1->Caption=IntToStr(ID);
OR_frame->Label6->Caption=IntToStr(in_out_gate_port[0]->ID);
OR_frame->Label7->Caption=IntToStr(in_out_gate_port[1]->ID);
OR_frame->Label8->Caption=IntToStr(in_out_gate_port[2]->ID);
OR_frame->ShowModal();
}
//----------------------------------------------------------------------------
bool OR_gate::set_property(TPoint *click_point)
{
//Ustawienie w³aœciwoœci komponenty
GetCursorPos(click_point);
OR_up_menu->Popup((*click_point).x,(*click_point).y);
return true;
}
//-----------------------------------------------------------------------------
void OR_gate::simulate()
{
if((in_out_gate_port[0]->Signal==up||in_out_gate_port[0]->Signal==pul_up)||
    (in_out_gate_port[1]->Signal==up||in_out_gate_port[1]->Signal==pul_up))
  in_out_gate_port[2]->propagate(up);
else
  in_out_gate_port[2]->propagate(down);
}
///////////////////////////////////////////////////////////////////
//                          8051
///////////////////////////////////////////////////////////////////
MCS51::MCS51(TPaintBox *Obrazek,TPoint *Punkt)
{
Element=MCS51_TYPE;
plucienko=Obrazek;

MCS51_menu=new TPopupMenu(plucienko);   //
Menu=new TMenuItem(plucienko);
Menu1=new TMenuItem(plucienko);

Menu->Caption="W³aœciwoœci";
Menu->OnClick=display_MCS51_property;

Menu1->Caption="Dezasembler";
Menu1->OnClick=display_dezasembler;


MCS51_menu->Items->Clear();
MCS51_menu->Items->Add(Menu);
MCS51_menu->Items->Add(Menu1);

//frequency=11059;//  do zastosowania w nastêpnych wersjach ;)
x_position=Punkt->x;
y_position=Punkt->y;
x_size=80;
y_size=250;
Proc=new i8051();

is_loaded=false;

chip=new TMicrochip_frame(Obrazek);
chip->Add_procesor(Proc);

for(int i=0;i<40;i++)
   in_out_gate_port[i]=NULL;
}
//-----------------------------------------------------------------------------
bool MCS51::set_property(TPoint *click_point)
{
GetCursorPos(click_point);
MCS51_menu->Popup((*click_point).x,(*click_point).y);

return true;
}
//----------------------------------------------------------------------------
MCS51::~MCS51()
{
for(int i=0;i<40;i++)
    {
    if(in_out_gate_port[i]!=NULL) //nadmiarowe zabespieczenie
        in_out_gate_port[i]->gate_port=false;
    }
draw(x_position,y_position,clWhite);
}
//----------------------------------------------------------------------------
 //rysuje komponent na danym miejscu u¿ywaj¹c danego koloru

void MCS51::draw(int x,int y,TColor Color)
{
plucienko->Canvas->Pen->Color=Color;
plucienko->Canvas->Pen->Mode=pmCopy;
plucienko->Canvas->Font->Color=Color;

plucienko->Canvas->Rectangle(x,y,x+80,y+260);
plucienko->Canvas->Rectangle(x-70,y+230,x-20,y+250);
plucienko->Canvas->MoveTo(x-20,y+240);
plucienko->Canvas->LineTo(x-5,y+240);

plucienko->Canvas->TextOutA( x-65,y+235,"12MHz");//FloatToStr(frequency/1000.0));
plucienko->Canvas->TextOutA(x+2,y+245,"20")  ;
plucienko->Canvas->TextOutA(x+63,y+245,"21")  ;
plucienko->Canvas->TextOutA(x+63,y+2,"40")  ;
plucienko->Canvas->TextOutA(x+2,y+2,"1")  ;
plucienko->Canvas->TextOutA(x+25,y+120,"89C51");
}
//---------------------------------------------------------------------------
 void MCS51::draw(int x,int y)
{

if(in_out_gate_port[5]==NULL)
  return;

if(x!=x_position||y!=y_position)
   {
   plucienko->Canvas->Pen->Mode=pmCopy;
   draw(x_position,y_position,clWhite);
   }


plucienko->Canvas->Font->Color = clBlack;
if(selected)
    draw(x,y,clRed);
else
    draw(x,y,clBlack);

x_position=x;
y_position=y;

for(int i=0;i<20;i++)
    in_out_gate_port[i]->draw(x-7,y+13*i+5);
for(int i=20;i<40;i++)
    in_out_gate_port[i]->draw(x+80,y+13*i-255);

}

//---------------------------------------------------------------------------
void MCS51::conect_gate_to_port(electronic **tab_conection_port)
{
int i;
for(i=0;i<40;i++)
    {
    in_out_gate_port[i]=tab_conection_port[i];
    in_out_gate_port[i]->gate_port=true;
    }
}
//----------------------------------------------------------------------------
void MCS51::stop_simulation()
{
Proc->PC_=0;
}
//----------------------------------------------------------------------------
bool MCS51::start_simulation()
{
if(!is_loaded)  //przepisaæ do start simulation
  {
  ShowMessage("Za³aduj program do procka");
  return false;
  }
chip->Start_simulation();
Proc->PC_=0;
return true;
}
//----------------------------------------------------------------------------
void __fastcall  MCS51::display_dezasembler(TObject *Sender)
{
chip->Show();
}
//-------------------------------------------------------------------------
void __fastcall  MCS51::display_MCS51_property(TObject *Sender)
{

if(MCS51_frame->ShowModal()==mrOk)
    {

    if(MCS51_frame->CurrentFile!="")
         is_loaded=Proc->LoadHexToRom(MCS51_frame->CurrentFile.c_str()) ;   //za³adowanie pliku do procka
    ShowMessage("Za³adowano: "+ MCS51_frame->CurrentFile);

    if(is_loaded&&MCS51_frame->CurrentFile!="")
        {
     int dlugosc=MCS51_frame->CurrentFile.Length();
     //ma³a podmiana rozszerzenia z asm na hex
     if(MCS51_frame->OpenDialog2->FileName[dlugosc-2]=='h')
        MCS51_frame->OpenDialog2->FileName.c_str()[dlugosc-3]='l';
     if(MCS51_frame->OpenDialog2->FileName[dlugosc-1]=='e')
        MCS51_frame->OpenDialog2->FileName.c_str()[dlugosc-2]='s';
     if(MCS51_frame->OpenDialog2->FileName[dlugosc]=='x')
         MCS51_frame->OpenDialog2->FileName.c_str()[dlugosc-1]='t';
    chip->Load_Lst_File(MCS51_frame->OpenDialog2->FileName) ;
        }
    }
}
//----------------------------------------------------------------------------
void MCS51::simulate()
{
Proc->Step();
chip->Refresh();
for (int i=0;i<50;i++)
    {
    execution_order[i]='\0';
    }

unsigned char mask=1;

in_out_gate_port[20]->propagate(up);  //napiêcie zasilania procesora
in_out_gate_port[19]->propagate(down); //masa

//  P0
for(int i=0;i<8;i++)
    {
    //sprawdzamy czy przyadkiem nie wpisa³a jakaœ bramka stanu wysokiego lub niskiego
    //bo jesli wpisa³a to traktujemy to jako wejœcie
    if((in_out_gate_port[21+i]->Signal==up)||(in_out_gate_port[21+i]->Signal==down))
        {
         if(in_out_gate_port[21+i]->Signal==up)
            Proc->RAM_[P_0]=(Proc->RAM_[P_0])|(mask); //ustawiamy odpowiedni bit
         else
            Proc->RAM_[P_0]=(Proc->RAM_[P_0])&(~mask);
        }
    else //w przeciwnym razie to s¹ wyjscia
        {
        if((Proc->RAM_[P_0])&(mask))
            in_out_gate_port[21+i]->propagate(pul_up);
        else
            in_out_gate_port[21+i]->propagate(pul_down);
        }
    mask= mask<<1;
    }
//  P1
mask=1;
for(int i=0;i<8;i++)
    {
     if((in_out_gate_port[i]->Signal==up)||(in_out_gate_port[i]->Signal==down))
        {
        if(in_out_gate_port[i]->Signal==up)
           Proc->RAM_[P_1]=Proc->RAM_[P_1]|(mask);
        else
           Proc->RAM_[P_1]=Proc->RAM_[P_1]&(~mask);
        }
     else
        {
        if((Proc->RAM_[P_1])&(mask))
            in_out_gate_port[i]->propagate(pul_up);
        else
            in_out_gate_port[i]->propagate(pul_down);
        }
    mask= mask<<1;
    }
// P2
mask=1;
for(int i=0;i<8;i++)
    {
    if((in_out_gate_port[39-i]->Signal==up)||(in_out_gate_port[39-i]->Signal==down))
        {
        if(in_out_gate_port[39-i]->Signal==up)
            Proc->RAM_[P_2]=Proc->RAM_[P_2]|(mask);
        else
            Proc->RAM_[P_2]=Proc->RAM_[P_2]&(~mask);
        }
    else
        {
        if((Proc->RAM_[P_2])&(mask))
            in_out_gate_port[39-i]->propagate(pul_up);
        else
            in_out_gate_port[39-i]->propagate(pul_down);
        }
    mask= mask<<1;
    }
//  P3
mask=1;
for(int i=0;i<8;i++)
    {
    if((in_out_gate_port[9+i]->Signal==up)||(in_out_gate_port[9+i]->Signal==down))
        {
        if(in_out_gate_port[9+i]->Signal==up)
          Proc->RAM_[P_3]=Proc->RAM_[P_3]|(mask);
        else
          Proc->RAM_[P_3]=Proc->RAM_[P_3]&(~mask);
        }
    else
        {
        if((Proc->RAM_[P_3])&(mask))
            in_out_gate_port[9+i]->propagate(pul_up);
        else
            in_out_gate_port[9+i]->propagate(pul_down);
        }
    mask= mask<<1;
    }
}
//////////////////////////////////////////////////////////////////////////////
///            1 Diodka
//////////////////////////////////////////////////////////////////////////////

LED_1::LED_1(TPaintBox *Obrazek,TPoint *Punkt)
{
Element=LED_1_TYPE;
plucienko=Obrazek;

LED1_menu=new TPopupMenu (plucienko);   //
Menu=new TMenuItem (plucienko);

Menu->Caption="W³aœciwoœci";
Menu->OnClick=display_LED1_property;
LED1_menu->Items->Clear();
LED1_menu->Items->Add(Menu);

Timer1 = new TTimer(NULL);
Timer1->Interval=30;
Timer1->Enabled=false;
Timer1->OnTimer=refresh;
                         //-----------------------------------------------------
x_position=Punkt->x;
y_position=Punkt->y;
x_size=20;
y_size=20;

in_out_gate_port[0]=NULL;
in_out_gate_port[1]=NULL;

HI_sihnall_red=1;
LO_signall_red=1;
HI_sihnall_green=1;
LO_signall_green=1;
}
//------------------------------------------------------------------------------
LED_1::~LED_1()
{
for(int i=0;i<2;i++)
    {
    if(in_out_gate_port[i]!=NULL) //nadmiarowe zabespieczenie
        in_out_gate_port[i]->gate_port=false;
    }
draw(x_position,y_position,clWhite);
}
//-----------------------------------------------------------------------------
void LED_1::draw(int x,int y)
{
if(in_out_gate_port[0]==NULL)//sprawdzamy czy porty sa podpiête pod bramke
  return;

draw(x_position,y_position,clWhite);
//plucienko->Canvas->Font->Color = clBlack;
if(selected)
    draw(x,y,clRed);
else
    draw(x,y,clBlack);

x_position=x;
y_position=y;

in_out_gate_port[0]->draw(x+4,y-7);
in_out_gate_port[1]->draw(x+4,y+16);
}
//-----------------------------------------------------------------------------
bool LED_1::set_property(TPoint *click_point)
{
GetCursorPos(click_point);
LED1_menu->Popup((*click_point).x,(*click_point).y);
return true;
}
//-----------------------------------------------------------------------------
void LED_1::conect_gate_to_port(electronic **tab_conection_port)
{
int i;

in_out_gate_port[0]=tab_conection_port[0];
in_out_gate_port[0]->gate_port=true;

in_out_gate_port[1]=tab_conection_port[1];
in_out_gate_port[1]->gate_port=true;
}
//-----------------------------------------------------------------------------
void LED_1::simulate()
{
Timer1->Enabled=true;
if(in_out_gate_port[0]->Signal==up&&in_out_gate_port[1]->Signal==down)
    HI_sihnall_red++;
else
    LO_signall_red++;

if(in_out_gate_port[0]->Signal==down&&in_out_gate_port[1]->Signal==up)
    HI_sihnall_green++;
else
    LO_signall_green++;
}
//------------------------------------------------------------------------------
void LED_1::stop_simulation()
{
Timer1->Enabled=false;
plucienko->Canvas->Pen->Color=clWhite;
plucienko->Canvas->Pen->Mode=pmCopy;
plucienko->Canvas->Pen->Width=4;

plucienko->Canvas->MoveTo(x_position+5,y_position+3);
plucienko->Canvas->LineTo(x_position+5,y_position+12);

plucienko->Canvas->MoveTo(x_position+10,y_position+3);
plucienko->Canvas->LineTo(x_position+10,y_position+12);


plucienko->Canvas->Pen->Width=1;

LO_signall_red=0;
HI_sihnall_red=0;
LO_signall_green=0;
HI_sihnall_green=0;
}
//------------------------------------------------------------------------------
void LED_1::draw(int x,int y,TColor Color)
{
plucienko->Canvas->Pen->Color=Color;
plucienko->Canvas->Pen->Mode=pmCopy;
plucienko->Canvas->Font->Color=Color;
plucienko->Canvas->Rectangle(x,y,x+15,y+15);
}
//------------------------------------------------------------------------------
void __fastcall  LED_1::display_LED1_property(TObject *Sender)
{
//NULL;
/*
LED7_form->Label10->Caption=IntToStr(ID);
LED7_form->Label2->Caption=IntToStr(in_out_gate_port[0]->ID);          ?????????????????????
LED7_form->Label3->Caption=IntToStr(in_out_gate_port[1]->ID);
LED7_form->Label4->Caption=IntToStr(in_out_gate_port[2]->ID);
LED7_form->Label5->Caption=IntToStr(in_out_gate_port[3]->ID);
LED7_form->Label6->Caption=IntToStr(in_out_gate_port[4]->ID);
LED7_form->Label7->Caption=IntToStr(in_out_gate_port[5]->ID);
LED7_form->Label8->Caption=IntToStr(in_out_gate_port[6]->ID);
LED7_form->Label9->Caption=IntToStr(in_out_gate_port[7]->ID);
*/
Led1_form->ShowModal();

}
//------------------------------------------------------------------------------
void __fastcall  LED_1::refresh(TObject *Sender)
{
Graphics::TColor Kolory[2];
Timer1->Enabled=false; //¿eby przypadkiem nie odpali³ tej funkcji jeszcze raz

Kolory[0]=static_cast <TColor> (RGB (
                        255,
                        ((float)LO_signall_red/(float)(LO_signall_red+(float)HI_sihnall_red))*255,
                        ((float)LO_signall_red/(float)(LO_signall_red+(float)HI_sihnall_red))*255));

Kolory[1]=static_cast <TColor> (RGB (
                        ((float)LO_signall_green/(float)(LO_signall_green+(float)HI_sihnall_green))*255,
                        255,
                        ((float)LO_signall_green/(float)(LO_signall_green+(float)HI_sihnall_green))*255));


plucienko->Canvas->Pen->Mode=pmCopy;
plucienko->Canvas->Pen->Width=4;

plucienko->Canvas->Pen->Color=Kolory[0]	;
plucienko->Canvas->MoveTo(x_position+5,y_position+3);
plucienko->Canvas->LineTo(x_position+5,y_position+12);

plucienko->Canvas->Pen->Color=Kolory[1];
plucienko->Canvas->MoveTo(x_position+10,y_position+3);
plucienko->Canvas->LineTo(x_position+10,y_position+12);

plucienko->Canvas->Pen->Width=1;

LO_signall_red=0;
HI_sihnall_red=0;
LO_signall_green=0;
HI_sihnall_green=0;
}
//////////////////////////////////////////////////////////////////////////////
//              Wyœwietlacz 7-dmio segmêtowy
/////////////////////////////////////////////////////////////////////////////
LED_7::LED_7(TPaintBox *Obrazek,TPoint *Punkt)
{
Element=LED_7_TYPE;
plucienko=Obrazek;

LED7_menu=new TPopupMenu (plucienko);   //
Menu=new TMenuItem (plucienko);

Menu->Caption="W³aœciwoœci";
Menu->OnClick=display_LED7_property;
LED7_menu->Items->Clear();
LED7_menu->Items->Add(Menu);


Timer1 = new TTimer(NULL);
Timer1->Interval=30;
Timer1->Enabled=false;
Timer1->OnTimer=refresh;
                         //-------------------------------------------------------
x_position=Punkt->x;
y_position=Punkt->y;
x_size=40;
y_size=60;
for(int i=0;i<9;i++)
   in_out_gate_port[i]=NULL;
for(int i=0;i<8;i++)
    {
     HI_sihnall[i]=0;
     LO_signall[i]=0;
     }
 }
//----------------------------------------------------------------------------
LED_7::~LED_7()
{
for(int i=0;i<9;i++)
    {
    if(in_out_gate_port[i]!=NULL) //nadmiarowe zabespieczenie
        in_out_gate_port[i]->gate_port=false;
    }
draw(x_position,y_position,clWhite);
}
//----------------------------------------------------------------------------
void LED_7::draw(int x,int y)
{

if(in_out_gate_port[5]==NULL)//sprawdzamy czy porty sa podpiête pod bramke
  return;

draw(x_position,y_position,clWhite);
//plucienko->Canvas->Font->Color = clBlack;
if(selected)
    draw(x,y,clRed);
else
    draw(x,y,clBlack);

x_position=x;
y_position=y;

for(int i=0;i<8;i++)
    in_out_gate_port[i]->draw(x-17+10*i,y+60);
in_out_gate_port[8]->draw(x+17,y-7);
}
//----------------------------------------------------------------------------
bool LED_7::set_property(TPoint *click_point)
{
//Ustawienie w³aœciwoœci komponenty
GetCursorPos(click_point);
LED7_menu->Popup((*click_point).x,(*click_point).y);
return true;
}
//-----------------------------------------------------------------------------
void LED_7::conect_gate_to_port(electronic **tab_conection_port)
{

 for(int i=0;i<9;i++)
 {
 in_out_gate_port[i]=tab_conection_port[i];
 in_out_gate_port[i]->gate_port=true;
 }
}
//---------------------------------------------------------------------------
void LED_7::draw(int x,int y,TColor Color)
{
plucienko->Canvas->Pen->Color=Color;
plucienko->Canvas->Pen->Mode=pmCopy;
plucienko->Canvas->Font->Color=Color;
plucienko->Canvas->Rectangle(x,y,x+40,y+60);
}
//------------------------------------------------------------------------------
void LED_7::simulate()
{

Timer1->Enabled=true;
for (int i=0;i<8;i++)
    {
   if(in_out_gate_port[8]->Signal==down&&in_out_gate_port[i]->Signal==up)
     HI_sihnall[i]++;
   else
     LO_signall[i]++;
     }
}
//------------------------------------------------------------------------------

void __fastcall  LED_7::refresh(TObject *Sender)
{
if(Form1->refresh)
   return;
plucienko->Canvas->Lock();
Graphics::TColor Kolory[9];
Timer1->Enabled=false; //¿eby przypadkiem nie odpali³ tej funkcji jeszcze raz

for (int i=0;i<8;i++)
   {
/*    Kolory[i]=static_cast <TColor> (RGB (255,
                        ((float)LO_signall[i]/(float)(LO_signall[i]+(float)HI_sihnall[i]))*255,
                        ((float)LO_signall[i]/(float)(LO_signall[i]+(float)HI_sihnall[i]))*255));
  */
   Kolory[i]=(Graphics::TColor)RGB (255,
                        (LO_signall[i]/(float)(LO_signall[i]+HI_sihnall[i])*255),
                        (LO_signall[i]/(float)(LO_signall[i]+HI_sihnall[i])*255));
   }
//ShowMessage(AnsiString("LO=")+AnsiString(IntToStr(LO_signall[3])) +AnsiString("HI=")+AnsiString(IntToStr(HI_sihnall[3])) );
plucienko->Canvas->Pen->Mode=pmCopy;
plucienko->Canvas->Pen->Width=3;

plucienko->Canvas->Pen->Color=Kolory[0]	;
plucienko->Canvas->MoveTo(x_position+10,y_position+7);
plucienko->Canvas->LineTo(x_position+25,y_position+7);

plucienko->Canvas->Pen->Color=Kolory[1]	;
plucienko->Canvas->MoveTo(x_position+7,y_position+9);
plucienko->Canvas->LineTo(x_position+7,y_position+25);

plucienko->Canvas->Pen->Color=Kolory[2];
plucienko->Canvas->MoveTo(x_position+10,y_position+27);
plucienko->Canvas->LineTo(x_position+25,y_position+27);

plucienko->Canvas->Pen->Color=Kolory[3]	;
plucienko->Canvas->MoveTo(x_position+7,y_position+29);
plucienko->Canvas->LineTo(x_position+7,y_position+45);

plucienko->Canvas->Pen->Color=Kolory[4];
plucienko->Canvas->MoveTo(x_position+10,y_position+47);
plucienko->Canvas->LineTo(x_position+25,y_position+47);

plucienko->Canvas->Pen->Color=Kolory[5];
plucienko->Canvas->MoveTo(x_position+28,y_position+29);
plucienko->Canvas->LineTo(x_position+28,y_position+45);

plucienko->Canvas->Pen->Color=Kolory[6];
plucienko->Canvas->MoveTo(x_position+28,y_position+9);
plucienko->Canvas->LineTo(x_position+28,y_position+25);

plucienko->Canvas->Pen->Color=Kolory[7];
plucienko->Canvas->Ellipse(x_position+32,y_position+48,x_position+36,y_position+52);

plucienko->Canvas->Pen->Width=1;
//wyzerowanie PWM-ów
for(int i=0;i<8;i++)
    {
     HI_sihnall[i]=0;
     LO_signall[i]=0;
     }
plucienko->Canvas->Unlock();
}
//-----------------------------------------------------------------------------
void __fastcall  LED_7::display_LED7_property(TObject *Sender)
{
LED7_form->Label10->Caption=IntToStr(ID);
LED7_form->Label2->Caption=IntToStr(in_out_gate_port[0]->ID);
LED7_form->Label3->Caption=IntToStr(in_out_gate_port[1]->ID);
LED7_form->Label4->Caption=IntToStr(in_out_gate_port[2]->ID);
LED7_form->Label5->Caption=IntToStr(in_out_gate_port[3]->ID);
LED7_form->Label6->Caption=IntToStr(in_out_gate_port[4]->ID);
LED7_form->Label7->Caption=IntToStr(in_out_gate_port[5]->ID);
LED7_form->Label8->Caption=IntToStr(in_out_gate_port[6]->ID);
LED7_form->Label9->Caption=IntToStr(in_out_gate_port[7]->ID);
LED7_form->ShowModal();
}
//------------------------------------------------------------------------------
void LED_7::stop_simulation()
{
Timer1->Enabled=false;
plucienko->Canvas->Pen->Color=clWhite;
plucienko->Canvas->Pen->Mode=pmWhite;
plucienko->Canvas->Font->Color=clWhite;
plucienko->Canvas->Brush->Style=bsSolid	;
plucienko->Canvas->Brush->Color=clWhite;
plucienko->Canvas->Rectangle(x_position,y_position,x_position+40,y_position+60);
for(int i=0;i<8;i++)
    {
     HI_sihnall[i]=0;
     LO_signall[i]=0;
     }
}

//------------------------------------------------------------------------------
//          VCC-Napiêcie zasilania
//-----------------------------------------------------------------------------
VCC::VCC(TPaintBox *Obrazek,TPoint *Punkt)
{
Element=VCC_TYPE;
plucienko=Obrazek;
x_position=Punkt->x;
y_position=Punkt->y;
x_size=20;
y_size=10;
in_out_gate_port[0]=NULL;
}
//------------------------------------------------------------------------------
VCC::~VCC()
{
if(in_out_gate_port[0]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[0]->gate_port=false;

draw(x_position,y_position,clWhite);
}
//------------------------------------------------------------------------------
void VCC::draw(int x,int y)
{
if(in_out_gate_port[0]==NULL)
  return;

plucienko->Canvas->Pen->Mode=pmCopy;
draw(x_position,y_position,clWhite);
plucienko->Canvas->Font->Color = clBlack;

if(selected)
    draw(x,y,clRed);
else
    draw(x,y,clBlack);

x_position=x;
y_position=y;

in_out_gate_port[0]->draw(x+10,y+10);
}
//------------------------------------------------------------------------------
bool VCC::set_property(TPoint *click_point)
{
return false;
}
//------------------------------------------------------------------------------
void VCC::conect_gate_to_port(electronic **tab_conection_port)
{
in_out_gate_port[0]=tab_conection_port[0];
in_out_gate_port[0]->gate_port=true;
}
//------------------------------------------------------------------------------
void VCC::simulate()
{
in_out_gate_port[0]->propagate(up);
}
//------------------------------------------------------------------------------
void VCC::draw(int x,int y,TColor Color)
{
plucienko->Canvas->Pen->Color=Color;
plucienko->Canvas->Font->Color=Color;
plucienko->Canvas->TextOutA( x,y,"+VCC");
}
//-----------------------------------------------------------------------------
//          GND-Napiêcie zasilania
//-----------------------------------------------------------------------------
GND::GND(TPaintBox *Obrazek,TPoint *Punkt)
{
Element=GND_TYPE;
plucienko=Obrazek;
x_position=Punkt->x;
y_position=Punkt->y;
x_size=20;
y_size=10;
in_out_gate_port[0]=NULL;
}
//------------------------------------------------------------------------------
GND::~GND()
{
if(in_out_gate_port[0]!=NULL) //nadmiarowe zabespieczenie
    in_out_gate_port[0]->gate_port=false;

draw(x_position,y_position,clWhite);
}
//------------------------------------------------------------------------------
void GND::draw(int x,int y)
{

if(in_out_gate_port[0]==NULL)
  return;

plucienko->Canvas->Pen->Mode=pmCopy;
draw(x_position,y_position,clWhite);
plucienko->Canvas->Font->Color = clBlack;

if(selected)
    draw(x,y,clRed);
else
    draw(x,y,clBlack);

x_position=x;
y_position=y;

in_out_gate_port[0]->draw(x+10,y+10);
}
//------------------------------------------------------------------------------
bool GND::set_property(TPoint *click_point)
{
return false;
}
//------------------------------------------------------------------------------
void GND::conect_gate_to_port(electronic **tab_conection_port)
{
in_out_gate_port[0]=tab_conection_port[0];
in_out_gate_port[0]->gate_port=true;
}
//------------------------------------------------------------------------------
void GND::simulate()
{
in_out_gate_port[0]->propagate(down);
}
//------------------------------------------------------------------------------
void GND::draw(int x,int y,TColor Color)
{
plucienko->Canvas->Pen->Color=Color;
plucienko->Canvas->Pen->Mode=pmCopy;

plucienko->Canvas->Font->Color=Color;
plucienko->Canvas->TextOutA( x,y,"GND");
}
////////////////////////////////////////////////////////////////////////////////
//          Generator
///////////////////////////////////////////////////////////////////////////////
Generator::Generator(TPaintBox *Obrazek,TPoint *Punkt)
{
Element=Generator_type;
plucienko=Obrazek;
x_position=Punkt->x;
y_position=Punkt->y;
x_size=30;
y_size=30;
in_out_gate_port[0]=NULL;

Generator_menu=new TPopupMenu (plucienko);   //
Menu=new TMenuItem (plucienko);

Menu->Caption="W³aœciwoœci";
Menu->OnClick=display_Generator_property;
Generator_menu->Items->Clear();
Generator_menu->Items->Add(Menu);
frequency=100;
fill=50;
counter=0;
}
//------------------------------------------------------------------------------
Generator::~Generator()
{               //Dopisaæ po¿adny destruktor
draw(x_position,y_position,clWhite);
if(in_out_gate_port[0]!=NULL)
   in_out_gate_port[0]->gate_port=false;
}
//------------------------------------------------------------------------------
void Generator::draw(int x,int y)
{

if(in_out_gate_port[0]==NULL)
  return;

plucienko->Canvas->Pen->Mode=pmCopy;
draw(x_position,y_position,clWhite);
plucienko->Canvas->Font->Color = clBlack;

if(selected)
    draw(x,y,clRed);
else
    draw(x,y,clBlack);

x_position=x;
y_position=y;

in_out_gate_port[0]->draw(x+13,y+30);
}
//------------------------------------------------------------------------------
bool Generator::set_property(TPoint *click_point)
{
GetCursorPos(click_point);
Generator_menu->Popup((*click_point).x,(*click_point).y);
return true;
}
//------------------------------------------------------------------------------
void Generator::conect_gate_to_port(electronic **tab_conection_port)
{
in_out_gate_port[0]=tab_conection_port[0];
in_out_gate_port[0]->gate_port=true;
}
//------------------------------------------------------------------------------
//Sprawdziæ bug pod wzgledem wystepowania generatora przy duzym i ma³ym wype³nieniu
void Generator::simulate()
{
counter++;
if((int)(10000/frequency)*(fill)<counter)
    {
    in_out_gate_port[0]->propagate(down);
    }
else
    {
    in_out_gate_port[0]->propagate(up);
    }
if(counter>1000000/frequency)
    {
    in_out_gate_port[0]->propagate(down);
    counter=0;
    }
}
//------------------------------------------------------------------------------
void Generator::draw(int x,int y,TColor Color)
{
plucienko->Canvas->Pen->Color=Color;
plucienko->Canvas->Pen->Mode=pmCopy;
plucienko->Canvas->Font->Color=Color;
plucienko->Canvas->Rectangle(x,y,x+30,y+30);
plucienko->Canvas->TextOutA( x+5,y+2,"Gen");
}
//------------------------------------------------------------------------------
void __fastcall  Generator::display_Generator_property(TObject *Sender)
{
Generator->frequency=frequency;
Generator->fill=fill;

Generator->ShowModal();
frequency=Generator->frequency;
fill=Generator->fill;
}
///////////////////////////////////////////////////////////////////////////////
//                    Oscilloscop
////////////////////////////////////////////////////////////////////////////////
Oscilloskop::Oscilloskop(TPaintBox *Obrazek,TPoint *Punkt)
{
Element=Oscilloscope_type;  //typ elementu
plucienko=Obrazek;
x_position=Punkt->x;
y_position=Punkt->y;
x_size=30;
y_size=30;
in_out_gate_port[0]=NULL;
in_out_gate_port[1]=NULL;

Oscilloskop_menu=new TPopupMenu (plucienko);   //
Menu=new TMenuItem (plucienko);

Menu->Caption="W³aœciwoœci";
Menu->OnClick=display_Oscilloskop_property;
Oscilloskop_menu->Items->Clear();
Oscilloskop_menu->Items->Add(Menu);

podzielnik=0;
max_counter=1;
x_pos_draw=0;

Formatka = new TForm(plucienko); //Form2->Create(this);
Formatka->DoubleBuffered=true;
Formatka->Width=800;
Formatka->Height=105;
Formatka->Color=clWhite;
Formatka->Caption="OSCYLOSKOP";
Formatka->FormStyle=fsStayOnTop;

Sterow_podzi=new TUpDown(Formatka);

Sterow_podzi->Width=16;
Sterow_podzi->Height=25;
Sterow_podzi->Left=20;
Sterow_podzi->Top=45;
Sterow_podzi->Visible=true;
Sterow_podzi->Parent=Formatka;
Sterow_podzi->OnClick=UpDown_change;
Sterow_podzi->Min=0;
Sterow_podzi->Max=21;
Sterow_podzi->Position=0;

Label_podzielnik=new TLabel(Formatka);
Label_podzielnik->Parent=Formatka;
Label_podzielnik->Left=50;
Label_podzielnik->Top=50;
Label_podzielnik->Caption="1us/dz" ;

Formatka->Show();
}
//------------------------------------------------------------------------------
Oscilloskop::~Oscilloskop()
{               //Dopisaæ po¿adny destruktor
draw(x_position,y_position,clWhite);
if(in_out_gate_port[0]!=NULL)
   in_out_gate_port[0]->gate_port=false;
if(in_out_gate_port[1]!=NULL)
   in_out_gate_port[1]->gate_port=false;
delete Formatka;
}
//------------------------------------------------------------------------------
void Oscilloskop::draw(int x,int y)
{

if(in_out_gate_port[0]==NULL||in_out_gate_port[1]==NULL)
  return;

plucienko->Canvas->Pen->Mode=pmCopy;
draw(x_position,y_position,clWhite);
plucienko->Canvas->Font->Color = clBlack;

if(selected)
    draw(x,y,clRed);
else
    draw(x,y,clBlack);

x_position=x;
y_position=y;
in_out_gate_port[0]->draw(x+30,y+10);
in_out_gate_port[1]->draw(x+30,y+20);
}
//------------------------------------------------------------------------------
bool Oscilloskop::set_property(TPoint *click_point)
{
GetCursorPos(click_point);
Oscilloskop_menu->Popup((*click_point).x,(*click_point).y);
return true;
}
//------------------------------------------------------------------------------
void Oscilloskop::conect_gate_to_port(electronic **tab_conection_port)
{
in_out_gate_port[0]=tab_conection_port[0];
in_out_gate_port[1]=tab_conection_port[1];
in_out_gate_port[0]->gate_port=true;
in_out_gate_port[1]->gate_port=true;

}
//------------------------------------------------------------------------------
//Sprawdziæ bug pod wzgledem wystepowania generatora przy duzym i ma³ym wype³nieniu
void Oscilloskop::simulate()
{
podzielnik++;

if(Input_A!=in_out_gate_port[0]->Signal)
   state_was_changeA=true;
if(Input_B!=in_out_gate_port[1]->Signal)
   state_was_changeB=true;



if(podzielnik>=max_counter&& Formatka->Visible)
    {
    Formatka->Canvas->Lock();
    podzielnik=0;
    x_pos_draw++;
//------------------------------------------------------------
//jesli sygna³ jest wysoki to rysujemy u góry

    if(in_out_gate_port[0]->Signal==up||in_out_gate_port[0]->Signal==pul_up)
        Formatka->Canvas->Pixels [x_pos_draw][5] = clBlack;
     //jesli sygnal jest niski
    if(in_out_gate_port[0]->Signal==down||in_out_gate_port[0]->Signal==pul_down)
        Formatka->Canvas->Pixels [x_pos_draw][15] = clBlack;;

    if(state_was_changeA)
        {
        Formatka->Canvas->Pen->Color =clBlack;
        Formatka->Canvas->MoveTo(x_pos_draw,5);
        Formatka->Canvas->LineTo(x_pos_draw,15);
        Input_A=in_out_gate_port[0]->Signal;
        state_was_changeA=false;
        }
//--------------------------------------------------------
//drugie wejscie
    if(in_out_gate_port[1]->Signal==up||in_out_gate_port[1]->Signal==pul_up)
        Formatka->Canvas->Pixels [x_pos_draw][25] = clBlack;

    if(in_out_gate_port[1]->Signal==down||in_out_gate_port[1]->Signal==pul_down)
        Formatka->Canvas->Pixels [x_pos_draw][35] = clBlack;;

    if(state_was_changeB)
        {
        Formatka->Canvas->Pen->Color =clBlack;
        Formatka->Canvas->MoveTo(x_pos_draw,25);
        Formatka->Canvas->LineTo(x_pos_draw,35);
        Input_B=in_out_gate_port[1]->Signal;
        state_was_changeB=false;
        }

   if(x_pos_draw>790)
        {
        stop_simulation();
        }
Formatka->Canvas->Unlock();
    }
}
//------------------------------------------------------------------------------
void Oscilloskop::draw(int x,int y,TColor Color)
{
plucienko->Canvas->Pen->Color=Color;
plucienko->Canvas->Font->Color=Color;
plucienko->Canvas->Rectangle(x,y,x+30,y+30);
plucienko->Canvas->TextOutA( x+5,y+3,"Osc");
}
//------------------------------------------------------------------------------
void __fastcall  Oscilloskop::display_Oscilloskop_property(TObject *Sender)
{
Formatka->Show();
}
//------------------------------------------------------------------------------
void Oscilloskop::stop_simulation()
{
podzielnik=0;
x_pos_draw=0;
x_pos_draw=1;

Formatka->Canvas->Pen->Color = clWhite;
Formatka->Canvas->Pen->Width = 20;

Formatka->Canvas->MoveTo(1,10);
Formatka->Canvas->LineTo(799,10) ;
Formatka->Canvas->MoveTo(1,30);
Formatka->Canvas->LineTo(799,30) ;

Formatka->Canvas->Pen->Width=1;
Formatka->Canvas->Pen->Color = clSilver;
Formatka->Canvas->Pen->Style = psDot;
for (int counter=1;counter<80;counter++)
    {
    Formatka->Canvas->MoveTo(counter*10,1);
    Formatka->Canvas->LineTo(counter*10,40);
    }
Formatka->Canvas->Pen->Style =psSolid;
}
//------------------------------------------------------------------------------
bool Oscilloskop::start_simulation()
{
Formatka->Canvas->Pen->Color = clSilver;
Formatka->Canvas->Pen->Style = psDot;
for (int counter=1;counter<80;counter++)
     {
     Formatka->Canvas->MoveTo(counter*10,1);
     Formatka->Canvas->LineTo(counter*10,40);
     }
Formatka->Canvas->Pen->Style =psSolid;
return true;
}
//------------------------------------------------------------------------------
void __fastcall  Oscilloskop::UpDown_change(TObject *Sender, TUDBtnType Button)
{
struct row_post_czas
    {
    char Label_podzielnika[10];
    long podzielnik;
    };
//struct
struct row_post_czas Tab_podstaw_czas[]=
        {{"10us/dz",1},{"20us/dz",2},{"50us/dz",5},{"0.1ms/dz",10},{"0.2ms/dz",20},{"0.5ms/dz",50},
        {"1ms/dz",100},{"2ms/dz",200},{"5ms/dz",500},{"10ms/dz",1000},{"20ms/dz",2000},
        {"50ms/dz",5000},{"0.1s/dz",10000},{"0.2s/dz",20000},{"0.5/dz",50000},
        {"1s/dz",100000},{"2s/dz",200000},{"5s/dz",500000},{"10s/dz",1000000},
        {"20s/dz",2000000},{"50s/dz",5000000},{"100s/dz",10000000}} ;

stop_simulation();
x_pos_draw=0;
podzielnik=0;
Formatka->Canvas->Pen->Width=1;
Label_podzielnik->Caption=Tab_podstaw_czas[Sterow_podzi->Position].Label_podzielnika;
max_counter=Tab_podstaw_czas[Sterow_podzi->Position].podzielnik;
}







