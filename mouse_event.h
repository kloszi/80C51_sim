
#include <vector.h>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>

#include "Bramki.h"
#include "List_elements.h"
#include "group_element.h"

#ifndef List_of_elements_
#define List_of_elements_

class  Element_list{
public:
          Element_list(TPaintBox *plotno);
         ~Element_list();
     void mouse_down(TMouseButton Button,TShiftState Shift,int X,int Y);
     void mouse_up(TMouseButton Button,TShiftState Shift,int X,int Y);
     int  mouse_move(TShiftState Shift,int X, int Y);
     void pres_key(char Key);
     void double_click(TPoint *Punkt); //funkcja wywo³ywana podczas symulacji
                          //do zmiany pozycji prze³¹cznika
     void change_draw_type(elementy_type element);
     void repaint();


     bool start_simulate();
     void stop_simulate();

     void save_file(AnsiString output_file);
     void open_file(AnsiString input_file);
     gouped_element     *element_stack;        //stos z elementami
private:
     TPaintBox          *Obrazek;              //obszar na którym  rysujemy
     TPoint             last_drawing;
     TPoint             point_mouse_push_down; //gdzie myszka zosta³a wciœniêta
     TDoing             state;                 //co w danej chwili robi
     elementy_type      create_type_element;   //co jest w danej chwili wybrane do malowania
     } ;

               

  #endif
