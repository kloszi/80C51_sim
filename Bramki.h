#include "Procesor\\89C51.h"
#include "NANA_dlg.h"
#include "8951_simulation_frame.h"
#ifndef Bramki_
#define Bramki_
////////////////////////////////////////////
typedef enum {arrow,AND,NAND,OR,NOT,WIRE,PORT,MCS51_TYPE,
               LED_7_TYPE,LED_1_TYPE,GND_TYPE,VCC_TYPE,Generator_type,
               Oscilloscope_type,tab_LED_type,switch_type,unknow_element} elementy_type;
typedef enum {moved,selected,conect,none} TDoing;
typedef enum {pul_up,pul_down,up,down,unknow} TSignal;


class port;

//0 - nie jest nad   objaktem
//1 - nad objektem  (bramk¹)   niezaznaczonym
//2 - jestesmy nad zaznaczonym elementem
//3 - jest nad port  niezaznaczonym
//4 - jest nad port  zaznaczonym
//5 - jest nad wire  niezaznaczonym
//6 - jest nad wire  zaznaczonym
//----------------------------------------------------------------------------
class  electronic
{
public:
   long          ID;
                 electronic();
   virtual       ~electronic();               //destruktor klasy bazowej
   virtual void  draw(int x,int y){};     //funkcja rysuje dany elementt na podanym miejscu
   virtual bool  set_property(TPoint *click_point){return false;}; //ustawia wlasciwosci danego komonentu
   virtual int   is_over(int x,int y);   //sprawdza czy podany pukt jest
   virtual port* PositionToAddress(int x,int y){return NULL;}
   virtual void  conect_gate_to_port(electronic **tab_conection_port){};
   virtual void  disconect(electronic *conected_element) {};          //roz³ancza elementy
   virtual void  move(TPoint *Vector);  //przesuniêcie o wektor


   virtual void  select(TPoint *click_point);
   virtual void  select(TPoint *first_click,TPoint *second_click); //czy w zaznaczonym prostokacia znajduje sie bramka
           void  select();
           void  deselect();
           bool  is_selected();

   virtual void  simulate(){};
   virtual bool  propagate(TSignal in_Signal){return false;};//"rozprowadza sygna³ po wszystkich ³¹czach
   virtual void  stop_simulation(){};
   virtual bool  start_simulation(){return true;};

   bool          gate_port;  //jesli jest true to znaczy ze dany port nalezy do bramki
   AnsiString    Name;       //nazwa komponentu
   elementy_type Element;    //typ elementu
   electronic    *pointer_conector[4];
   int           x_position;           //pozycja_elementu
   int           y_position;
   TSignal       Signal;
protected:
   bool          selected;          //jesli true to element jest zaznaczony
   int           x_size;            //jego rozmiar
   int           y_size;
   TPaintBox     *plucienko;         //plotno na ktorym malujemy
   bool          point_is_insie_rect(TPoint *rect1,TPoint *rect2,TPoint *point);
};
///////////////////////////////////////////////////////////////////////////////
//    Klasa Portow  (czyli punktow lutowniczych)
///////////////////////////////////////////////////////////////////////////////
class port: public electronic
{
public:
  void   disconect(electronic *conected_element);//od³ancza sie od podanego wire
  void   select(TPoint *click_point){};
  void   select(TPoint *first_click,TPoint *second_click);
         port(TPaintBox *Obrazek,TPoint *Punkt);
        ~port();
  void   draw(int x,int y);    //rysuje objekt o podanych wspolrzednych
  int    is_over(int x,int y);
  port*  PositionToAddress(int x,int y);

  void   move(TPoint *Vector);  //przesuniêcie o wektor
  bool   propagate(TSignal in_Signal);//"rozprowadza sygna³ po wszystkich ³¹czach
private:
} ;
///////////////////////////////////////////////////////////////////////////////
//Klasa reprezentuj¹ca polaczenie pomiedzy poszczegolnymi punktami lutowniczymi
///////////////////////////////////////////////////////////////////////////////
class wire: public electronic
{
public:
  void          disconect(electronic *conected_element);//od³ancza sie od portu
                wire(TPaintBox *TPaintBox);
                ~wire();
  void          draw(int x,int y);      //parametry wejœciowe zbedne
  int           is_over(int x,int y);   //sprawdzamy czy myszka jest nad objektem
  void          conect(electronic* input_conection,electronic* output_conection);
  bool          set_property(TPoint *click_point);
  void          select(TPoint *click_point);
  void          select(TPoint *first_click,TPoint *second_click); //czy w zaznaczonym prostokacia znajduje sie bramka

private:
   void        draw(int x1,int y1,int x2,int y2,TColor Color); //rysuje komponent na danym miejscu u¿ywaj¹c danego koloru
//  --==Zmienne do menu kazdj z bramek==--
   TPoint      prewious_position1;
   TPoint      prewious_position2;
// Wszystko do popup menu//
   TPopupMenu  *wire_up_menu;
   TMenuItem   *Menu;
   void __fastcall  display_wire_property(TObject *Sender);
};
//////////////////////////////////////////////////////////////////////////////
///                  Prze³¹cznik
//////////////////////////////////////////////////////////////////////////////
class  switch_: public electronic
{
public:
                switch_(TPaintBox *Obrazek,TPoint *Punkt);
                ~switch_();
     void       draw(int x,int y);    //rysuje objekt o podanych wspolrzednych
     bool       set_property(TPoint *click_point);
     void       conect_gate_to_port(electronic **tab_conection_port);
     void       simulate();//podczas symulacji wywo³ywana jest ta funkcja
     void       switching(); //zmienia po³ozenie prze³¹cznika
     electronic *in_out_gate_port[3];
private :
   bool         position;
   TPopupMenu   *switch_up_menu;
   TMenuItem    *Menu;
   void draw(int x,int y,TColor Color); //rysuje komponent na danym miejscu u¿ywaj¹c danego koloru
   void __fastcall  display_switch_property(TObject *Sender);
};
//////////////////////////////////////////////////////////////////////////////
//     Brameczka AND
//////////////////////////////////////////////////////////////////////////////
class  AND_gate: public electronic
{
public:
                AND_gate(TPaintBox *Obrazek,TPoint *Punkt);
                ~AND_gate();
     void       draw(int x,int y);    //rysuje objekt o podanych wspolrzednych
     bool       set_property(TPoint *click_point);
     void       conect_gate_to_port(electronic **tab_conection_port);
     void       simulate();//podczas symulacji wywo³ywana jest ta funkcja
     electronic *in_out_gate_port[3];
private :
   TPopupMenu   *AND_up_menu;
   TMenuItem    *Menu;
   void         draw(int x,int y,TColor Color); //rysuje komponent na danym miejscu u¿ywaj¹c danego koloru
   void __fastcall  display_AND_property(TObject *Sender);

};
////////////////////////////////////////////////////////////////////////////////
//      Brameczka NAND
///////////////////////////////////////////////////////////////////////////////
class  NAND_gate: public electronic
{
public:
                NAND_gate(TPaintBox *Obrazek,TPoint *Punkt);
                ~NAND_gate();
     void       draw(int x,int y);    //rysuje objekt o podanych wspolrzednych
     bool       set_property(TPoint *click_point);
     void       conect_gate_to_port(electronic **tab_conection_port);
     void       simulate();//podczas symulacji wywo³ywana jest ta funkcja
     electronic *in_out_gate_port[3];
private :
   TPopupMenu   *NAND_up_menu;
   TMenuItem    *Menu;
   void         draw(int x,int y,TColor Color); //rysuje komponent na danym miejscu u¿ywaj¹c danego koloru
   void __fastcall  display_NAND_property(TObject *Sender);
};
///////////////////////////////////////////////////////////////////////////////
//             Brameczka NOT
///////////////////////////////////////////////////////////////////////////////
class  NOT_gate: public electronic
{
public:
                NOT_gate(TPaintBox *Obrazek,TPoint *Punkt);
                ~NOT_gate();
     void       draw(int x,int y);    //rysuje objekt o podanych wspolrzednych
     bool       set_property(TPoint *click_point);
     void       conect_gate_to_port(electronic **tab_conection_port);
     void       simulate();//podczas symulacji wywo³ywana jest ta funkcja
     electronic *in_out_gate_port[2];
private :
   TPopupMenu   *NOT_up_menu;
   TMenuItem    *Menu;
   void         draw(int x,int y,TColor Color); //rysuje komponent na danym miejscu u¿ywaj¹c danego koloru
   void __fastcall  display_NOT_property(TObject *Sender);
};
///////////////////////////////////////////////////////////////////////////////
//           Brameczka OR
///////////////////////////////////////////////////////////////////////////////
class  OR_gate: public electronic
{
public:
                OR_gate(TPaintBox *Obrazek,TPoint *Punkt);
                ~OR_gate();
     void       draw(int x,int y);    //rysuje objekt o podanych wspolrzednych
     bool       set_property(TPoint *click_point);
     void       conect_gate_to_port(electronic **tab_conection_port);
     void       simulate();//podczas symulacji wywo³ywana jest ta funkcja
     electronic *in_out_gate_port[3];
private :
//  --==Zmienne do menu kazdj z bramek==--
   TPopupMenu   *OR_up_menu;
   TMenuItem    *Menu;
   void         draw(int x,int y,TColor Color); //rysuje komponent na danym miejscu u¿ywaj¹c danego koloru
   void __fastcall  display_OR_property(TObject *Sender);

};
////////////////////////////////////////////////////////////////////////////////
///    Mikroprocesor
///////////////////////////////////////////////////////////////////////////////
class MCS51: public electronic
{
public:
                MCS51(TPaintBox *Obrazek,TPoint *Punkt);
                ~MCS51();
     void       draw(int x,int y);    //rysuje objekt o podanych wspolrzednych
     bool       set_property(TPoint *click_point);
     void       conect_gate_to_port(electronic **tab_conection_port);
     void       simulate();//podczas symulacji wywo³ywana jest ta funkcja
     void       stop_simulation();
     bool       start_simulation();
     i8051      *Proc;
     electronic *in_out_gate_port[40];
private :
     bool       is_loaded; //jesli program jest za³adowany to true
     void       draw(int x,int y,TColor Color); //rysuje komponent na danym miejscu u¿ywaj¹c danego koloru
     int        frequency;  //w KHz
     TPopupMenu *MCS51_menu;
     TMenuItem  *Menu;
     TMenuItem  *Menu1;

     TMicrochip_frame  *chip;  //do podgl¹dania kodu
     void __fastcall  display_MCS51_property(TObject *Sender);
     void __fastcall  display_dezasembler(TObject *Sender);
};
//---------------------------------------------------------------------------
class LED_1:public electronic
{
public:
                LED_1(TPaintBox *Obrazek,TPoint *Punkt);
                ~LED_1();
    void        draw(int x,int y);    //rysuje objekt o podanych wspolrzednych
    bool        set_property(TPoint *click_point);
    void        conect_gate_to_port(electronic **tab_conection_port);
    void        simulate();//podczas symulacji wywo³ywana jest ta funkcja
    void        stop_simulation();
    electronic  *in_out_gate_port[2];
private :
    void        draw(int x,int y,TColor Color); //rysuje komponent na danym miejscu u¿ywaj¹c danego koloru
    TPopupMenu  *LED1_menu;
    TMenuItem   *Menu;
//--------------------------------------------------------
    long        HI_sihnall_red;  //dla PWM
    long        LO_signall_red;  //dla PWM
    long        HI_sihnall_green;  //dla PWM
    long        LO_signall_green;  //dla PWM

    TTimer      *Timer1;
    void __fastcall  display_LED1_property(TObject *Sender);
    void __fastcall  refresh(TObject *Sender);
};
//-----------------------------------------------------------------------------
class LED_7: public electronic
{
public:
                LED_7(TPaintBox *Obrazek,TPoint *Punkt);
                ~LED_7();
     void       draw(int x,int y);    //rysuje objekt o podanych wspolrzednych
     bool       set_property(TPoint *click_point);
     void       conect_gate_to_port(electronic **tab_conection_port);
     void       simulate();//podczas symulacji wywo³ywana jest ta funkcja
     void       stop_simulation();
     electronic *in_out_gate_port[9];
private :
     void       draw(int x,int y,TColor Color); //rysuje komponent na danym miejscu u¿ywaj¹c danego koloru
     TPopupMenu *LED7_menu;
     TMenuItem  *Menu;
//--------------------------------------------------------
    long        HI_sihnall[8];  //dla PWM
    long        LO_signall[8];  //dla PWM
    TTimer      *Timer1;
    void __fastcall  refresh(TObject *Sender);
    void __fastcall  display_LED7_property(TObject *Sender);
};
//---------------------------------------------------------------------------
class VCC: public electronic
{
public:
                VCC(TPaintBox *Obrazek,TPoint *Punkt);
                ~VCC();
     void       draw(int x,int y);    //rysuje objekt o podanych wspolrzednych
     bool       set_property(TPoint *click_point);
     void       conect_gate_to_port(electronic **tab_conection_port);
     void       simulate();//podczas symulacji wywo³ywana jest ta funkcja
     electronic *in_out_gate_port[1];
private :
     void       draw(int x,int y,TColor Color); //rysuje komponent na danym miejscu u¿ywaj¹c danego koloru
};
//-----------------------------------------------------------------------------
class GND: public electronic
{
public:
                GND(TPaintBox *Obrazek,TPoint *Punkt);
                ~GND();
     void       draw(int x,int y);    //rysuje objekt o podanych wspolrzednych
     bool       set_property(TPoint *click_point);
     void       conect_gate_to_port(electronic **tab_conection_port);
     void       simulate();//podczas symulacji wywo³ywana jest ta funkcja
     electronic *in_out_gate_port[1];
private :
     void       draw(int x,int y,TColor Color); //rysuje komponent na danym miejscu u¿ywaj¹c danego koloru
};
////////////////////////////////////////////////////////////////////////////////
//       GENERATOR
////////////////////////////////////////////////////////////////////////////////
class Generator: public electronic
{
public:
                Generator(TPaintBox *Obrazek,TPoint *Punkt);
                ~Generator();
     void       draw(int x,int y);    //rysuje objekt o podanych wspolrzednych
     bool       set_property(TPoint *click_point);
     void       conect_gate_to_port(electronic **tab_conection_port);
     void       simulate();//podczas symulacji wywo³ywana jest ta funkcja
     electronic *in_out_gate_port[1];
     long       frequency; //w Hz
     int        fill;      //wype³nienie % (1-99)
private :
     long       counter;
     void       draw(int x,int y,TColor Color); //rysuje komponent na danym miejscu u¿ywaj¹c danego koloru
   TPopupMenu   *Generator_menu;
   TMenuItem    *Menu;
   void __fastcall  display_Generator_property(TObject *Sender);
};
///////////////////////////////////////////////////////////////////////////////
//                  Oscyloskop
////////////////////////////////////////////////////////////////////////////////
class Oscilloskop: public electronic
{
public:
                Oscilloskop(TPaintBox *Obrazek,TPoint *Punkt);
                ~Oscilloskop();
     void       draw(int x,int y);    //rysuje objekt o podanych wspolrzednych
     bool       set_property(TPoint *click_point);
     void       conect_gate_to_port(electronic **tab_conection_port);
     void       simulate();//podczas symulacji wywo³ywana jest ta funkcja
     void       stop_simulation();
     bool       start_simulation();
     electronic *in_out_gate_port[2];
     TUpDown    *Sterow_podzi; //public poniewa¿ podczas zachowywania do pliku musimy ppobraæ wartoœæ
     void __fastcall  UpDown_change(TObject *Sender, TUDBtnType Button);
private :
   bool         state_was_changeA,state_was_changeB;
   int          x_pos_draw;//pozycja na oscyloskopie
   long         podzielnik;   //wartoœæ która po ka¿dym wywo³anu funkcji zwiêksza swoja watoœc do
   long         max_counter; //max counter

   TSignal      Input_A,Input_B;
   TForm        *Formatka; //formatka na której bedzie rysowany wykres
   TLabel       *Label_podzielnik;

   void         draw(int x,int y,TColor Color); //rysuje komponent na danym miejscu u¿ywaj¹c danego koloru

   TPopupMenu   *Oscilloskop_menu;
   TMenuItem    *Menu;
   void __fastcall  display_Oscilloskop_property(TObject *Sender);
};

#endif

