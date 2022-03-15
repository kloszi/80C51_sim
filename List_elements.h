#include <ExtCtrls.hpp>
#include <vector.h>
#include <inifiles.hpp>
#include "Unit1.h"
#include "Bramki.h"

#ifndef List_elements_
#define List_elements_




///////////////////////////////////////////////////////////////////////
//    Klasa przetrzymuj¹ca wszystkie eleenty
/////////////////////////////////////////////////////////////////////
class Lista_elementow
{
public:
          Lista_elementow();   //konstruktor
         ~Lista_elementow();   //destruktor
     void select(TPoint *Punkt_1,TPoint *Punkt_2); //zaznaczanie w kwadracie
     void select(TPoint *Punkt);       //zaznaczanie elementu z Shiftem
     void select_or_deselect(TPoint *Punkt);  //klikanie lewym klawiszem na poszczegolne elementy
     void select_all();          //zaznaczenie wszystkich objektow
     void deselect_all();
     void move(TPoint *Vector);  //przerznoszenie poszczegolnych elementow o wektor
     void put_gate_to_list(electronic *in_gate); //odlozenie elementu na liste
     void delete_selected_gate();    //skasowanie zaznaczonych elementów
     void repaint_all();
     int  get_cursor(TPoint *Punkt); //pobranie wygladu kursora
     void save_file(AnsiString output_file);
     int  open_file(AnsiString input_file);  //zwraca waroœæ max ID
     void delete_all();
     void change_switch(TPoint *Punkt);
     //0 obszar na ktorym nie ma elementow
     //1 obszar nad elementem niezaznaczonym
     //2 obszar nad elementem zaznaczonym
     //3 obszar nad jego wejsciem i wyjsciem
     bool         set_property(TPoint *Punkt);
     electronic*  get_adress_by_position(TPoint *Punkt);
     electronic*  get_adress_by_ID(long ID);
     //ustawienie w³aœnciwoœci danego komponentu
     //zwraca 0 jeœli nie natrafi³o na komponent
     //       1 jeœli ustawi³o w³aœciwosci komponentu
     void simulate();
     void stop_simulated();
     bool start_simulated();
///-------- Stos przetrzymujacy wszystkie elementy
     vector<electronic*> *list_of_elements;  //lista wskaŸników do elementow
     vector<electronic*>::iterator iterator_list_of_elements;
} ;

  #endif
