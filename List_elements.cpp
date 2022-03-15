#include "List_elements.h"
#include "Unit1.h"
#include "Bramki.h"
#include "Schematic_paint.h"
#include <typeinfo.h>
#include <algorithm>
#pragma warn -aus

/////////////////////////////////////////////////////////////////////
Lista_elementow::Lista_elementow()
{
list_of_elements=new vector<electronic*>; //tworzymy nowa listê elementów
}
//---------------------------------------------------------------
Lista_elementow::~Lista_elementow()
{
 for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
   delete (*iterator_list_of_elements);    //kasujemy ca³¹ listê
  }
delete list_of_elements;
}
//---------------------------------------------------------------
void Lista_elementow::select(TPoint *Punkt_1,TPoint *Punkt_2)
{
 for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
    //zaznaczenie wszystkich elementow znajdujacych
    //siê wewn¹trz prostok¹ta
    (*iterator_list_of_elements)->select(Punkt_1,Punkt_2);
  }
}
//----------------------------------------------------------------
void Lista_elementow::change_switch(TPoint *Punkt)
{
      vector<electronic*>::iterator iterator_list_of_elements;
  for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
    //zaznaczenie wszystkich elementow znajdujacych
    //siê wewn¹trz prostok¹ta

    if( (*iterator_list_of_elements)->Element==switch_type)
      {
      if( (*iterator_list_of_elements)->is_over(Punkt->x,Punkt->y))
         {
         // ??????????????????????????????????????????????????????????????????????????
         switch_* wskp4 ;
         if( (wskp4 = dynamic_cast<switch_*>(*iterator_list_of_elements))!=0)
            {
             wskp4->switching();
            }
         }
      }
  }
}
//----------------------------------------------------------------
void Lista_elementow::select(TPoint *Punkt)
{
for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
  (*iterator_list_of_elements)->select(Punkt);
  }
}
//----------------------------------------------------------------
void Lista_elementow::select_or_deselect(TPoint *Punkt)
{
//Narazie nie ruszam bo nie wim jakie bêd¹ potrzeby
}
//---------------------------------------------------------------
void Lista_elementow::select_all()
{
for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
  (*iterator_list_of_elements)->select();
  }
}
//--------------------------------------------------------------
void Lista_elementow::deselect_all()
{
for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
  (*iterator_list_of_elements)->deselect();
  }
}
//--------------------------------------------------------------
void Lista_elementow::move(TPoint *Vector)
{
for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
  (*iterator_list_of_elements)->move(Vector);
  if((*iterator_list_of_elements)->is_selected())
     (*iterator_list_of_elements)->draw((*iterator_list_of_elements)->x_position,
                                        (*iterator_list_of_elements)->y_position);

  }
}
//--------------------------------------------------------------
void Lista_elementow::put_gate_to_list(electronic *in_gate)
{
list_of_elements->push_back(in_gate);//odlozenie na liste elementu
}
//-----------------------------------------------------
void Lista_elementow::delete_selected_gate()
{
for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
  if((*iterator_list_of_elements)->is_selected())
    {
     delete (*iterator_list_of_elements) ;
     list_of_elements->erase(iterator_list_of_elements);
     iterator_list_of_elements--;
     if(list_of_elements->size()==0)
        {
        return;
        }
    }
  }
//tutaj trzeba dopisaæ to ¿e je¿eli wire nie ma z drugiej strony portu to usuwamy
for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
      {
       //sprawdzamy czy to jest wire
       if((*iterator_list_of_elements)->Element==WIRE)
            {
             //jeœli jedno z po³¹czeñ wire wskazuje na NULL
             if(((*iterator_list_of_elements)->pointer_conector[0]==NULL)||
                 ((*iterator_list_of_elements)->pointer_conector[1]==NULL))
               {
                delete (*iterator_list_of_elements) ;
                list_of_elements->erase(iterator_list_of_elements);
                iterator_list_of_elements--;
                if(list_of_elements->size()==0)
                    {
                    return;
                    }
               }

            }

      }
}
//--------------------------------------
void Lista_elementow::repaint_all()
{
for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
   (*iterator_list_of_elements)->draw((*iterator_list_of_elements)->x_position,
                                      (*iterator_list_of_elements)->y_position);
  }
}
//-----------------------------------------
int Lista_elementow::get_cursor(TPoint *Punkt)
{
 int return_nr_cursor;
for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
     return_nr_cursor=(*iterator_list_of_elements)->is_over(Punkt->x,Punkt->y);
    if(return_nr_cursor>0)
       return  return_nr_cursor;
  }
return 0;
}
///////////////////////////////////////////////////////////////////////////////
//ustawienie w³aœciwoœci danego komponentu
//zwraca 0 jeœli nie natrafi³o na komponent
//          1 jeœli ustawi³o w³aœciwosci komponentu

bool Lista_elementow::set_property(TPoint *Punkt)
{
 for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
  if((*iterator_list_of_elements)->is_over(Punkt->x,Punkt->y))
        {
        if((*iterator_list_of_elements)->set_property(Punkt))
        return true;
        }
  }
return false;  
}
///////////////////////////////////////////////////////////////////////////////
electronic*  Lista_elementow::get_adress_by_position(TPoint *Punkt)
{
 int return_nr_cursor;
for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
     return_nr_cursor=(*iterator_list_of_elements)->is_over(Punkt->x,Punkt->y);
    if(return_nr_cursor>0)
       return  (*iterator_list_of_elements);
  }
return NULL;
}
//////////////////////////////////////////////////////////////////////////////
electronic*  Lista_elementow::get_adress_by_ID(long ID)
{
  int return_nr_cursor;
for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
    if((*iterator_list_of_elements)->ID==ID )
     return  (*iterator_list_of_elements);
  }
return 0;
}
//----------------------------------------------------------------------------
void Lista_elementow::simulate()
{
vector<electronic*>::iterator iterator_list_of_elements;
for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
  if((*iterator_list_of_elements)->Element!=WIRE&&(*iterator_list_of_elements)->Element!=PORT)
    {
    (*iterator_list_of_elements)->simulate();
    }
  else
    return ;
  }
}
//-----------------------------------------------------------------------------
void Lista_elementow::stop_simulated()
{
for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
  (*iterator_list_of_elements)->Signal=unknow;
  (*iterator_list_of_elements)->stop_simulation();
  }
return;
}
//-----------------------------------------------------------------------------
//zrobione sortowanie elementów
// do optymalizacji prêdkosci wykonywania kodu

bool compare(electronic* in_element)
{
if(in_element->Element!=WIRE&&in_element->Element!=PORT)
    return true;
else
    return false;
}
//
bool Lista_elementow::start_simulated()
{
//dopisac czy wszystkie wire maja porty po obu stronach
for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
  (*iterator_list_of_elements)->Signal=unknow;
  if((*iterator_list_of_elements)->start_simulation()==false)
    return false;
  }
//sortowanie dla optymalizacji prêdkosci wykonywania kodu
partition(list_of_elements->begin(),list_of_elements->end(),compare);//

return true;
}

//----------------------------------------------------------------------------
void Lista_elementow::save_file(AnsiString output_file)
{
  //Dopisaæ ¿e je¿eli port nale¿y do bramki to ustawiæ gate port
 TIniFile *PlikIni = new TIniFile(output_file);

  DeleteFile(output_file);

for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
{
//dla wszystkich elementów oprócz wire

if((*iterator_list_of_elements)->Element==WIRE)
    {
   PlikIni->WriteString(AnsiString((*iterator_list_of_elements)->ID),
                         "Name",typeid(**iterator_list_of_elements).name());
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "0 ",(*iterator_list_of_elements)->pointer_conector[0]->ID);
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "1 ",(*iterator_list_of_elements)->pointer_conector[1]->ID);
    }

if((*iterator_list_of_elements)->Element!=WIRE)
    {
    PlikIni->WriteString(AnsiString((*iterator_list_of_elements)->ID),
                         "Name",typeid(**iterator_list_of_elements).name());
    PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "X ",(*iterator_list_of_elements)->x_position);
    PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "Y ",(*iterator_list_of_elements)->y_position);
    }
////////////////////////////////////////////////////////////////////
if((*iterator_list_of_elements)->Element==switch_type)
    {
    switch_* wskp4 ;
 if( (wskp4 = dynamic_cast<switch_*>(*iterator_list_of_elements))!=0)
        {
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "0 ",wskp4->in_out_gate_port[0]->ID);
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "1 ",wskp4->in_out_gate_port[1]->ID);
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "2 ",wskp4->in_out_gate_port[2]->ID);
         }
    }
///////////////////////////////////////////////////////////////////
if((*iterator_list_of_elements)->Element==AND)
    {
    AND_gate* wskp4 ;
 if( (wskp4 = dynamic_cast<AND_gate*>(*iterator_list_of_elements))!=0)
        {
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "0 ",wskp4->in_out_gate_port[0]->ID);
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "1 ",wskp4->in_out_gate_port[1]->ID);
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "2 ",wskp4->in_out_gate_port[2]->ID);
         }
    }
// Bramka NAND
if((*iterator_list_of_elements)->Element==NAND)
    {
    NAND_gate* wskp4 ;
 if( (wskp4 = dynamic_cast<NAND_gate*>(*iterator_list_of_elements))!=0)
        {
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "0 ",wskp4->in_out_gate_port[0]->ID);
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "1 ",wskp4->in_out_gate_port[1]->ID);
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "2 ",wskp4->in_out_gate_port[2]->ID);
         }
    }
///Bamka NOT
if((*iterator_list_of_elements)->Element==NOT)
    {
    NOT_gate* wskp4 ;
 if( (wskp4 = dynamic_cast<NOT_gate*>(*iterator_list_of_elements))!=0)
        {
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "0 ",wskp4->in_out_gate_port[0]->ID);
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "1 ",wskp4->in_out_gate_port[1]->ID);
         }
    }
//// Bramka OR
if((*iterator_list_of_elements)->Element==OR)
    {
    OR_gate* wskp4 ;
 if( (wskp4 = dynamic_cast<OR_gate*>(*iterator_list_of_elements))!=0)
        {
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "0 ",wskp4->in_out_gate_port[0]->ID);
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "1 ",wskp4->in_out_gate_port[1]->ID);
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "2 ",wskp4->in_out_gate_port[2]->ID);
         }
    }
//   MCS51
if((*iterator_list_of_elements)->Element==MCS51_TYPE)
    {
    MCS51* wskp4 ;
 if( (wskp4 = dynamic_cast<MCS51*>(*iterator_list_of_elements))!=0)
        {
         for(int i=0;i<40;i++)
             PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         AnsiString(i),wskp4->in_out_gate_port[i]->ID);
         }
    }
//wyœwietlacz LED
if((*iterator_list_of_elements)->Element==LED_7_TYPE)
    {
    LED_7* wskp4 ;
 if( (wskp4 = dynamic_cast<LED_7*>(*iterator_list_of_elements))!=0)
        {
         for(int i=0;i<9;i++)
             PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         AnsiString(i),wskp4->in_out_gate_port[i]->ID);
         }
    }
// Czerwona i zielona diodka LED
if((*iterator_list_of_elements)->Element==LED_1_TYPE)
    {
    LED_1* wskp4 ;
 if( (wskp4 = dynamic_cast<LED_1*>(*iterator_list_of_elements))!=0)
        {
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "0 ",wskp4->in_out_gate_port[0]->ID);
   PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                         "1 ",wskp4->in_out_gate_port[1]->ID);
         }
    }
// Napiêcie zasilania VCC
if((*iterator_list_of_elements)->Element==VCC_TYPE)
    {
    VCC* wskp4 ;
 if( (wskp4 = dynamic_cast<VCC*>(*iterator_list_of_elements))!=0)
        {
        PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                        "0",wskp4->in_out_gate_port[0]->ID);
         }
    }
//Masa
 if((*iterator_list_of_elements)->Element==GND_TYPE)
    {
    GND* wskp4 ;
 if( (wskp4 = dynamic_cast<GND*>(*iterator_list_of_elements))!=0)
        {
         PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                        "0",wskp4->in_out_gate_port[0]->ID);
         }
    }
//   Generator_type

if((*iterator_list_of_elements)->Element==Generator_type)
    {
    Generator* wskp4 ;
 if( (wskp4 = dynamic_cast<Generator*>(*iterator_list_of_elements))!=0)
        {
         PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                        "0",wskp4->in_out_gate_port[0]->ID);
         PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                       "fill",wskp4->fill);
         PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                       "frequency",wskp4->frequency);
         }
    }
 //Oscilloskop
if((*iterator_list_of_elements)->Element==Oscilloscope_type)
    {
    Oscilloskop* wskp4 ;
    if( (wskp4 = dynamic_cast<Oscilloskop*>(*iterator_list_of_elements))!=0)
        {
         PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                     "0",wskp4->in_out_gate_port[0]->ID);
         PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                        "1",wskp4->in_out_gate_port[1]->ID);
         PlikIni->WriteInteger(AnsiString((*iterator_list_of_elements)->ID),
                        "div",wskp4->Sterow_podzi->Position);
        }
    }
}
 delete PlikIni;
}
///////////////////////////////////////////////////////////////////////////////
////      ODczytywanie pliku
int Lista_elementow::open_file(AnsiString input_file)
{
int  ID_GET=0;
int  MAX_ID=0;
electronic  *port_tmp;
TPoint       Tmp_point=Point(0,0);
TStringList *txt  = new TStringList;
TIniFile *PlikIni = new TIniFile(input_file);

delete_all();
PlikIni->ReadSections(txt);
 //wczytanie porow
for(int i=0;i<txt->Count;i++)
    {
    ID_GET=StrToInt(txt->Strings[i]);
    if((PlikIni->ReadString(IntToStr(ID_GET),"Name","B³¹d"))=="port")
        {
        Tmp_point=Point(PlikIni->ReadInteger(IntToStr(ID_GET),"X",-1),
                        PlikIni->ReadInteger(IntToStr(ID_GET),"Y",-1));
        port_tmp=new port(Paint_box->PaintBox1,&Tmp_point);
        port_tmp->ID=ID_GET;
        put_gate_to_list(port_tmp);
        }
    }

//   Wire
for(int i=0;i<txt->Count;i++)
    {
    ID_GET=StrToInt(txt->Strings[i]);
    if((PlikIni->ReadString(IntToStr(ID_GET),"Name","B³¹d"))=="wire")
        {
        electronic* first_port=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"0",-1));
        electronic* second_port=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"1",-1));

        electronic* conector_wire=new wire(Paint_box->PaintBox1);
        conector_wire->pointer_conector[0]=first_port;
        conector_wire->pointer_conector[1]=second_port;
        conector_wire->ID=ID_GET;
        put_gate_to_list(conector_wire);
/////////////////////////////////////////////////////////////////////////////////
//zrobiæ to trzeba za pomoc¹ switch ????(teraz bym polemizowal
////////////////////////////////////////////////////////////////////////////////
    if(first_port->pointer_conector[0]==NULL)
        {
        first_port->pointer_conector[0]=conector_wire;
        goto SECOND;
        }
    if(first_port->pointer_conector[1]==NULL)
        {
        first_port->pointer_conector[1]=conector_wire;
        goto SECOND;
        }
    if(first_port->pointer_conector[2]==NULL)
        {
        first_port->pointer_conector[2]=conector_wire;
        goto SECOND;
        }
    if(first_port->pointer_conector[3]==NULL)
        {
        first_port->pointer_conector[3]=conector_wire;
        goto SECOND;
        }
SECOND:
    if(second_port->pointer_conector[0]==NULL)
        {
        second_port->pointer_conector[0]=conector_wire;
        goto END_Conection;
        }
    if(second_port->pointer_conector[1]==NULL)
        {
        second_port->pointer_conector[1]=conector_wire;
        goto END_Conection;
        }
    if(second_port->pointer_conector[2]==NULL)
        {
        second_port->pointer_conector[2]=conector_wire;
        goto END_Conection;
        }
    if(second_port->pointer_conector[3]==NULL)
        {
        second_port->pointer_conector[3]=conector_wire;
        goto END_Conection;
        }
END_Conection:
        }
    }
///////////////////////////////////////////////////////////////////////////////
// Odczytywanie komponentów
//////////////////////////////////////////////////////////////////////////////
for(int i=0;i<txt->Count;i++)
    {
    ID_GET=StrToInt(txt->Strings[i]);
      Tmp_point=Point(PlikIni->ReadInteger(IntToStr(ID_GET),"X",-1),
                        PlikIni->ReadInteger(IntToStr(ID_GET),"Y",-1));
    if((PlikIni->ReadString(IntToStr(ID_GET),"Name","B³¹d"))=="AND_gate")
        {
        AND_gate *And=new AND_gate(Paint_box->PaintBox1,&Tmp_point);
        And->in_out_gate_port[0]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"0",-1));
        And->in_out_gate_port[1]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"1",-1));
        And->in_out_gate_port[2]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"2",-1));
        And->ID=ID_GET;
        And->in_out_gate_port[0]->gate_port=true;
        And->in_out_gate_port[1]->gate_port=true;
        And->in_out_gate_port[2]->gate_port=true;

        put_gate_to_list(And);
        }
/////////////////////////////////////////////////////////////////////////////////
if((PlikIni->ReadString(IntToStr(ID_GET),"Name","B³¹d"))=="NAND_gate")
        {
        NAND_gate *NAnd=new NAND_gate(Paint_box->PaintBox1,&Tmp_point);
        NAnd->in_out_gate_port[0]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"0",-1));
        NAnd->in_out_gate_port[1]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"1",-1));
        NAnd->in_out_gate_port[2]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"2",-1));
        NAnd->ID=ID_GET;
        NAnd->in_out_gate_port[0]->gate_port=true;
        NAnd->in_out_gate_port[1]->gate_port=true;
        NAnd->in_out_gate_port[2]->gate_port=true;

        put_gate_to_list(NAnd);
        }
/////////////////////////////////////////////////////////////////////////////////
if((PlikIni->ReadString(IntToStr(ID_GET),"Name","B³¹d"))=="switch_")
        {
        switch_ *switch_com=new switch_(Paint_box->PaintBox1,&Tmp_point);
        switch_com->in_out_gate_port[0]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"0",-1));
        switch_com->in_out_gate_port[1]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"1",-1));
        switch_com->in_out_gate_port[2]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"2",-1));
        switch_com->ID=ID_GET;
        switch_com->in_out_gate_port[0]->gate_port=true;
        switch_com->in_out_gate_port[1]->gate_port=true;
        switch_com->in_out_gate_port[2]->gate_port=true;

        put_gate_to_list(switch_com);
        }
////////////////////////////////////////////////////////////////////////////////
    if((PlikIni->ReadString(IntToStr(ID_GET),"Name","B³¹d"))=="NOT_gate")
        {
        NOT_gate *Not=new NOT_gate(Paint_box->PaintBox1,&Tmp_point);
        Not->in_out_gate_port[0]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"0",-1));
        Not->in_out_gate_port[1]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"1",-1));
        Not->in_out_gate_port[0]->gate_port=true;
        Not->in_out_gate_port[1]->gate_port=true;
        Not->ID=ID_GET;
        put_gate_to_list(Not);
        }
     if((PlikIni->ReadString(IntToStr(ID_GET),"Name","B³¹d"))=="OR_gate")
        {
        OR_gate *OR=new OR_gate(Paint_box->PaintBox1,&Tmp_point);
        OR->in_out_gate_port[0]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"0",-1));
        OR->in_out_gate_port[1]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"1",-1));
        OR->in_out_gate_port[2]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"2",-1));
        OR->in_out_gate_port[0]->gate_port=true;
        OR->in_out_gate_port[1]->gate_port=true;
        OR->in_out_gate_port[2]->gate_port=true;

        OR->ID=ID_GET;
        put_gate_to_list(OR);
        }
     if((PlikIni->ReadString(IntToStr(ID_GET),"Name","B³¹d"))=="MCS51")
        {
        MCS51 *Microproc=new  MCS51(Paint_box->PaintBox1,&Tmp_point);
        for(int i=0;i<40;i++)
            {
            Microproc->in_out_gate_port[i]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),IntToStr(i),-1));
            Microproc->in_out_gate_port[i]->gate_port=true;
            }
        Microproc->ID=ID_GET;
        put_gate_to_list(Microproc);
        }
//wyœwietlacz 7dmio segmêtowy
     if((PlikIni->ReadString(IntToStr(ID_GET),"Name","B³¹d"))=="LED_7")
        {
        LED_7 *LEd_s=new  LED_7(Paint_box->PaintBox1,&Tmp_point);
        for(int i=0;i<9;i++)
            {
            LEd_s->in_out_gate_port[i]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),IntToStr(i),-1));
            LEd_s->in_out_gate_port[i]->gate_port=true;
            }
        LEd_s->ID=ID_GET;
        put_gate_to_list(LEd_s);
        }
//diodka czerwoina i zielona
    if((PlikIni->ReadString(IntToStr(ID_GET),"Name","B³¹d"))=="LED_1")
        {
        LED_1 *LED=new LED_1(Paint_box->PaintBox1,&Tmp_point);
        LED->in_out_gate_port[0]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"0",-1));
        LED->in_out_gate_port[1]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"1",-1));
        LED->in_out_gate_port[0]->gate_port=true;
        LED->in_out_gate_port[1]->gate_port=true;
        LED->ID=ID_GET;
        put_gate_to_list(LED);
        }
// Vcc
     if((PlikIni->ReadString(IntToStr(ID_GET),"Name","B³¹d"))=="VCC")
        {
        VCC *VCC_obj=new  VCC(Paint_box->PaintBox1,&Tmp_point);
        VCC_obj->in_out_gate_port[0]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"0",-1));
        VCC_obj->in_out_gate_port[0]->gate_port=true;
        VCC_obj->ID=ID_GET;
        put_gate_to_list(VCC_obj);
        }
     if((PlikIni->ReadString(IntToStr(ID_GET),"Name","B³¹d"))=="GND")
        {
        GND *GND_obj=new  GND(Paint_box->PaintBox1,&Tmp_point);
        GND_obj->in_out_gate_port[0]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"0",-1));
        GND_obj->in_out_gate_port[0]->gate_port=true;
        GND_obj->ID=ID_GET;
        put_gate_to_list(GND_obj);
        }
     if((PlikIni->ReadString(IntToStr(ID_GET),"Name","B³¹d"))=="Generator")
        {
        Generator *Generator_obj=new  Generator(Paint_box->PaintBox1,&Tmp_point);
        Generator_obj->in_out_gate_port[0]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"0",-1));
        Generator_obj->in_out_gate_port[0]->gate_port=true;
        Generator_obj->ID=ID_GET;
        Generator_obj->frequency=PlikIni->ReadInteger(IntToStr(ID_GET),"frequency",-1);
        Generator_obj->fill=PlikIni->ReadInteger(IntToStr(ID_GET),"fill",-1);
        put_gate_to_list(Generator_obj);
        }

     if((PlikIni->ReadString(IntToStr(ID_GET),"Name","B³¹d"))=="Oscilloskop")
        {
        TUDBtnType TMP;
        Oscilloskop *Oscilloskop_obj=new  Oscilloskop(Paint_box->PaintBox1,&Tmp_point);
        Oscilloskop_obj->in_out_gate_port[0]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"0",-1));
        Oscilloskop_obj->in_out_gate_port[1]=get_adress_by_ID(PlikIni->ReadInteger(IntToStr(ID_GET),"1",-1));
        Oscilloskop_obj->in_out_gate_port[0]->gate_port=true;
        Oscilloskop_obj->in_out_gate_port[1]->gate_port=true;
        Oscilloskop_obj->ID=ID_GET;
        Oscilloskop_obj->Sterow_podzi->Position=PlikIni->ReadInteger(IntToStr(ID_GET),"div",-1);
         Oscilloskop_obj->UpDown_change(NULL,TMP);
        put_gate_to_list(Oscilloskop_obj);
        }
     if(MAX_ID<ID_GET)
        MAX_ID=ID_GET;
    }
return  MAX_ID;  //potrzebne zwracanie do dalszej edycji
}
///////////////////////////////////////////////////////////
void Lista_elementow::delete_all()
{
 for(iterator_list_of_elements=list_of_elements->begin();
     iterator_list_of_elements!= list_of_elements->end();
      iterator_list_of_elements++)
  {
   delete (*iterator_list_of_elements);    //kasujemy ca³¹ listê
  }
 list_of_elements->clear();
}


