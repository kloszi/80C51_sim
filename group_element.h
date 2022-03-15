#include <ExtCtrls.hpp>
#include <vector.h>
#include "Bramki.h"
#include "List_elements.h"

#include "Unit1.h"

#ifndef group_element_
#define group_element_
///////////////////////////////////////////////
/// Klasa skladajaca sie z listy elementow i polaczen pomiedzy elementami

class gouped_element
{
public:
      gouped_element(TPaintBox *Obrazek);
      ~gouped_element();


     void select(TPoint *Punkt_1,TPoint *Punkt_2); //zaznaczanie w kwadracie
     void select(TPoint *Punkt);       //zaznaczanie elementu z Shiftem
     void select_or_deselect(TPoint *Punkt);  //klikanie lewym klawiszem na poszczegolne elementy
     void select_all();          //zaznaczenie wszystkich objektow
     void deselect_all();
     void delete_selected_gate();    //skasowanie zaznaczonych elementów

     void double_click(TPoint *Punkt); //zmiana  pozycji prze³¹cznika

     void move(TPoint *Vector);  //przerznoszenie poszczegolnych elementow o wektor
     void repaint_all();            //przerysowanie wszystkich brameczek od nowa
     int  get_cursor(TPoint *Punkt); //pobranie wygladu kursora
     bool set_property(TPoint *Punkt);
     void new_element(elementy_type element,TPoint *Punkt);//utworzenie nowego elementu
     bool conect(long ID_1,long ID_2);
     long get_ID(TPoint *Punkt);
//------------------------------------------------------------------------------
     void stop_simulated();
     bool start_simulated();

     void save_file(AnsiString output_file);
     void open_file(AnsiString input_file);
     Lista_elementow Elementy; //public dla optymalizacji wykonywania funkcji simulate
private:
    long            ID_counter;

    TPaintBox       *Canwas;    //tam gdzie rysowane s¹ zgrupowane elementy

};





#endif
