#include "mouse_event.h"

//----------------------------------------------------------------------
//   KONSTRUKTOR
Element_list::Element_list(TPaintBox *plotno)
{
element_stack=new gouped_element(plotno);
create_type_element=arrow; //na radiobutonie jest nacisnieta strzalka
Obrazek=plotno;
}
//--------------------------------------------------------------------
//   DESTRUKTOR

Element_list::~Element_list()
{}
//-----------------------------------------------------------
void Element_list::repaint()
{
element_stack->repaint_all();
}
//----------------------------------------------------------------------
void Element_list::mouse_down(TMouseButton Button, TShiftState Shift, int X, int Y)
{
electronic*        gate;
point_mouse_push_down=Point(X,Y);// myszka zosta³a wciœniêta

if(Button==mbLeft) //jesli w danym momencie
{
last_drawing=point_mouse_push_down;
  if(create_type_element==arrow)
    {
     int cursor=element_stack->get_cursor(&point_mouse_push_down);
//0 - nie jest nad   objaktem
//1 - nad objektem  (bramk¹)   niezaznaczonym
//2 - jestesmy nad   zaznaczonym elementem
//3 - jest nad port  niezaznaczonym
//4 - jest nad port  zaznaczonym
//5 - jest nad wire  niezaznaczonym
//6 - jest nad wire  zaznaczonym
     if(cursor==0)
       {
       element_stack->deselect_all();
       state=selected;  //jesli kliknieto poza elementami
       return;
       }
     if(cursor==1||cursor==5)//jesli jest nad niezaznaczony elementem
       {
       element_stack->deselect_all();
       element_stack->select(&point_mouse_push_down);  //zaznaczenie tego elementu
       state=moved;                     //przesuwanie elementu
       return;
       }
     if(cursor==2||cursor==4||cursor==6)//jesli jest mad zaznaczony elementem
       {
       state=moved; //przesuwanie elementu
       return;
       }
     if(cursor==3)//jesli jest nad niezaznaczony elementem
       {
       element_stack->deselect_all();
       state=conect;
       return;
       }
    }
   else
   {
    element_stack->deselect_all();
    element_stack->new_element(create_type_element,&point_mouse_push_down);  //tworzymy nowa bramke AND
    state=moved;//¿eby mo¿na by³o przesuwaæ element
   }
}
if(Button==mbRight)   //przycisnieto prawy klawisz myszki
    {
    if(create_type_element==arrow)
        {
         element_stack->deselect_all();
         element_stack->set_property(&point_mouse_push_down);}
    if(create_type_element!=arrow)
        create_type_element=arrow ;  //wycofanie sie z tworzenia innych bramek
          //zaznaczenie tego elementu
    }
}
//-----------------------------------------------
void Element_list::mouse_up( TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
TPoint  Punkcik=Point(X,Y);

if(state==selected) //je¿eli zaznaczalimy
    {
    element_stack->select(&Punkcik,&point_mouse_push_down);
    Obrazek->Canvas->Pen->Color=clBlack;
    Obrazek->Canvas->Brush->Style = bsClear;
    Obrazek->Canvas->Pen->Mode    = pmNotXor;
    Obrazek->Canvas->Rectangle(last_drawing.x,last_drawing.y,
                            point_mouse_push_down.x,point_mouse_push_down.y);
    }
if(state==conect)
    {
    int cursor=element_stack->get_cursor(&TPoint(X,Y));
    Obrazek->Canvas->Pen->Color   =clBlack;
    Obrazek->Canvas->Brush->Style =bsClear;
    Obrazek->Canvas->Pen->Mode    =pmNotXor;
    if(cursor==4||cursor==3)
       {
       Obrazek->Canvas->MoveTo(point_mouse_push_down.x ,point_mouse_push_down.y);
       Obrazek->Canvas->LineTo(X,Y) ;
       element_stack->conect(element_stack->get_ID(&Punkcik),element_stack->get_ID(&point_mouse_push_down));
       }
    else //skasowanie tymczasowego po³aczenia
       {
       Obrazek->Canvas->MoveTo(point_mouse_push_down.x ,point_mouse_push_down.y);
       Obrazek->Canvas->LineTo(X,Y) ;
       }
    }
state=none;
element_stack->repaint_all();
}
//-----------------------------------------------------------------------------
void Element_list::pres_key(char Key)
{
if(Key==VK_DELETE)
  element_stack->delete_selected_gate();
if(Key==VK_ESCAPE)
   element_stack->deselect_all();
return;

}
//------------------------------------------------------------------------------
void Element_list::double_click(TPoint *Punkt)
{

element_stack->double_click(Punkt) ;
// state=none;
// element_stack->deselect_all();
}

//-----------------------------------------------------------------------------
//   Zmieniamy rodzaj elementu rysowanego
void Element_list::change_draw_type(elementy_type element)
{
state=none;
element_stack->deselect_all();
create_type_element=element;    //przepisujemy do srodka klasy wybrany element
                                //jaki jest obecnie wybrany
}
//-----------------------------------------------------------------------------
int Element_list::mouse_move(TShiftState Shift,
      int X, int Y)
{
TPoint Vektor;
TPoint mouse_position;

Vektor=Point(X-point_mouse_push_down.x,Y-point_mouse_push_down.y);
Obrazek->Canvas->Pen->Color   =clBlack;
Obrazek->Canvas->Brush->Style =bsClear;
Obrazek->Canvas->Pen->Mode    =pmNotXor;

//jesli przesuwamy elementy o dany wektor
if(state==moved)
    {
    element_stack->move(&Vektor);
    point_mouse_push_down=Point(X,Y);
    element_stack->repaint_all();
    }
//jesli laczymy 2 elementy

if(state==conect)
   {
   Obrazek->Canvas->MoveTo(point_mouse_push_down.x ,point_mouse_push_down.y);
   Obrazek->Canvas->LineTo(last_drawing.x,last_drawing.y) ;

   Obrazek->Canvas->MoveTo(point_mouse_push_down.x ,point_mouse_push_down.y);
   Obrazek->Canvas->LineTo(X,Y) ;
   last_drawing=Point(X,Y);
    element_stack->repaint_all();
   }
//jesli zaznaczamy obszar
if(state==selected)
    {
     Obrazek->Canvas->Rectangle(TRect(last_drawing,point_mouse_push_down));
     Obrazek->Canvas->Rectangle(TRect(point_mouse_push_down,TPoint(X,Y)));
     last_drawing=Point(X,Y);
    }

    mouse_position=Point(X,Y);
return  element_stack->get_cursor(&mouse_position);
 }
//-----------------------------------------------------------------------------
void Element_list::stop_simulate()
{
return element_stack->stop_simulated();
}
//-----------------------------------------------------------------------------
bool Element_list::start_simulate()
{
return element_stack->start_simulated();
}
//-----------------------------------------------------------------------------
void Element_list::save_file(AnsiString output_file)
{
element_stack->save_file(output_file);
}
//-----------------------------------------------------------------------------
void Element_list::open_file(AnsiString input_file)
{
element_stack->open_file(input_file);
}

