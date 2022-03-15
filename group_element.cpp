#include "group_element.h"


gouped_element::gouped_element(TPaintBox *Obrazek)
{
ID_counter=1;
Canwas=Obrazek;
}
//----------------------------------------------------------------------------
gouped_element::~gouped_element()
{
}
//----------------------------------------------------------------------------
void gouped_element::select(TPoint *Punkt_1,TPoint *Punkt_2)
{
Elementy.select(Punkt_1,Punkt_2);
}
//----------------------------------------------------------------------------
void gouped_element::select(TPoint *Punkt)
{
Elementy.select(Punkt);
}
//----------------------------------------------------------------------------
void gouped_element::select_or_deselect(TPoint *Punkt)
{
Elementy.select_or_deselect(Punkt);
}
//-----------------------------------------------------------------------------
void gouped_element::select_all()
{
Elementy.select_all();
}
//----------------------------------------------------------------------------
void gouped_element::deselect_all()
{
Elementy.deselect_all();
}
//----------------------------------------------------------------------------
void gouped_element::move(TPoint *Vector)
{
Elementy.move(Vector);
}
//-------------------------------------------------------------------------
void gouped_element::delete_selected_gate()
{
Elementy.delete_selected_gate();
}
//----------------------------------------------------------------------------
void gouped_element::repaint_all()
{
Elementy.repaint_all();
}
//----------------------------------------------------------------------------
int  gouped_element::get_cursor(TPoint *Punkt)
{
  int curs;
  curs=Elementy.get_cursor(Punkt);

return curs ;
}
//----------------------------------------------------------------------------
bool gouped_element::set_property(TPoint *Punkt)
{
Elementy.set_property(Punkt);
return true;
}
//---------------------------------------------------------------------------
void gouped_element::double_click(TPoint *Punkt)
{
Elementy.change_switch(Punkt);
}
//---------------------------------------------------------------------------
long gouped_element::get_ID(TPoint *Punkt)
{
return (Elementy.get_adress_by_position(Punkt))->ID ;
}
//---------------------------------------------------------------------------
void gouped_element::new_element(elementy_type element,TPoint *Punkt)//utworzenie nowego elementu
{
 electronic*        gate;
 int i;
// ID_counter++;//przyznaje poszczególnym elementom rozne nr ID
 if(ID_counter==0) //rozdano ju¿ wszystkie ID
    {
     Application->MessageBox("Za duzo elementów","Ostrze¿enie",
                            MB_OK | MB_ICONINFORMATION);

     }
 if(element==PORT)
   {
   gate=new port(Canwas,Punkt);
   gate->ID=ID_counter;
   ID_counter++;
   Elementy.put_gate_to_list(gate);
   }
 if(element==MCS51_TYPE)
   {
   ID_counter++;
   MCS51 *mikroprocesorek;
   electronic *tab_gate_port[40];
   mikroprocesorek=new MCS51(Canwas,Punkt);
   mikroprocesorek->Proc->Name=IntToStr(ID_counter);
   gate=mikroprocesorek;
   gate->ID=ID_counter;
   ID_counter++;
   for(i=0;i<40;i++)
        {
        tab_gate_port[i]=new port(Canwas,Punkt);
        tab_gate_port[i]->ID=ID_counter;
        ID_counter++;
        Elementy.put_gate_to_list(tab_gate_port[i]);
        }
   gate->conect_gate_to_port(tab_gate_port) ;
   Elementy.put_gate_to_list(gate);
   }
 if(element==LED_7_TYPE)
   {
   electronic *tab_gate_port[9];
   gate=new LED_7(Canwas,Punkt);
   gate->ID=ID_counter;
   ID_counter++;
   for(i=0;i<9;i++)
        {
        tab_gate_port[i]=new port(Canwas,Punkt);
        tab_gate_port[i]->ID=ID_counter;
        ID_counter++;
        Elementy.put_gate_to_list(tab_gate_port[i]);
        }
   gate->conect_gate_to_port(tab_gate_port) ;
   Elementy.put_gate_to_list(gate);
   }
 if(element==LED_1_TYPE)
   {
   electronic *tab_gate_port[2];
   gate=new LED_1(Canwas,Punkt);
   gate->ID=ID_counter;
   ID_counter++;
   tab_gate_port[0]=new port(Canwas,Punkt);
   tab_gate_port[0]->ID=ID_counter;          //nadajemy poszczególnym elementom
   ID_counter++;
   Elementy.put_gate_to_list(tab_gate_port[0]);
   tab_gate_port[1]=new port(Canwas,Punkt);
   tab_gate_port[1]->ID=ID_counter;
   ID_counter++;
   Elementy.put_gate_to_list(tab_gate_port[1]);
   gate->conect_gate_to_port(tab_gate_port) ;
   Elementy.put_gate_to_list(gate);
   }
 if(element==GND_TYPE)
   {
   electronic *tab_gate_port[1];
   gate=new GND(Canwas,Punkt);
   gate->ID=ID_counter;
   ID_counter++;
   tab_gate_port[0]=new port(Canwas,Punkt);
   tab_gate_port[0]->ID=ID_counter;
   ID_counter++;
   Elementy.put_gate_to_list(tab_gate_port[0]);
   gate->conect_gate_to_port(tab_gate_port) ;
   Elementy.put_gate_to_list(gate);
   }
 if(element==VCC_TYPE)
   {
   electronic *tab_gate_port[1];
   gate=new VCC(Canwas,Punkt);
   gate->ID=ID_counter;
   ID_counter++;
   tab_gate_port[0]=new port(Canwas,Punkt);
   tab_gate_port[0]->ID=ID_counter;
   ID_counter++;
   Elementy.put_gate_to_list(tab_gate_port[0]);
   gate->conect_gate_to_port(tab_gate_port) ;
   Elementy.put_gate_to_list(gate);
   }

 if(element==Generator_type)
   {
   electronic *tab_gate_port[1];
   gate=new Generator(Canwas,Punkt);
   gate->ID=ID_counter;
   ID_counter++;
   tab_gate_port[0]=new port(Canwas,Punkt);
   tab_gate_port[0]->ID=ID_counter;
   ID_counter++;
   Elementy.put_gate_to_list(tab_gate_port[0]);
   gate->conect_gate_to_port(tab_gate_port) ;
   Elementy.put_gate_to_list(gate);
   }
 if(element==Oscilloscope_type)
   {
   electronic *tab_gate_port[2];
   gate=new Oscilloskop(Canwas,Punkt);
   gate->ID=ID_counter;
   ID_counter++;
   tab_gate_port[0]=new port(Canwas,Punkt);
   tab_gate_port[0]->ID=ID_counter;
   Elementy.put_gate_to_list(tab_gate_port[0]);
   ID_counter++;
   tab_gate_port[1]=new port(Canwas,Punkt);
   tab_gate_port[1]->ID=ID_counter;
   Elementy.put_gate_to_list(tab_gate_port[1]);
   ID_counter++;
   gate->conect_gate_to_port(tab_gate_port) ;
   Elementy.put_gate_to_list(gate);
   }

 if(element==NOT)
   {
   electronic *tab_gate_port[2];
   gate=new NOT_gate(Canwas,Punkt);
   gate->ID=ID_counter;
   ID_counter++;
   tab_gate_port[0]=new port(Canwas,Punkt);
   tab_gate_port[0]->ID=ID_counter;          //nadajemy poszczególnym elementom
   ID_counter++;
   Elementy.put_gate_to_list(tab_gate_port[0]);
   tab_gate_port[1]=new port(Canwas,Punkt);
   tab_gate_port[1]->ID=ID_counter;
   ID_counter++;
   Elementy.put_gate_to_list(tab_gate_port[1]);
   gate->conect_gate_to_port(tab_gate_port) ;
   Elementy.put_gate_to_list(gate);
   }
//////////////////////
 if(element==switch_type)
   {
   electronic *tab_gate_port[3];
   gate=new switch_(Canwas,Punkt);
   gate->ID=ID_counter;
   ID_counter++;
   for(int i=0;i<3;i++)
        {
       tab_gate_port[i]=new port(Canwas,Punkt);
       tab_gate_port[i]->ID=ID_counter;          //nadajemy poszczególnym elementom
       ID_counter++;
       Elementy.put_gate_to_list(tab_gate_port[i]);
        }
   gate->conect_gate_to_port(tab_gate_port) ;
   Elementy.put_gate_to_list(gate);
   }
////////////////////

 if(element==AND)
   {
   electronic *tab_gate_port[3];
   gate=new AND_gate(Canwas,Punkt);
   gate->ID=ID_counter;
   ID_counter++;
   for(int i=0;i<3;i++)
        {
       tab_gate_port[i]=new port(Canwas,Punkt);
       tab_gate_port[i]->ID=ID_counter;          //nadajemy poszczególnym elementom
       ID_counter++;
       Elementy.put_gate_to_list(tab_gate_port[i]);
        }
   gate->conect_gate_to_port(tab_gate_port) ;
   Elementy.put_gate_to_list(gate);
   }
// NAND
 if(element==NAND)
   {
   electronic *tab_gate_port[3];
   gate=new NAND_gate(Canwas,Punkt);
   gate->ID=ID_counter;
   ID_counter++;
   for(int i=0;i<3;i++)
        {
       tab_gate_port[i]=new port(Canwas,Punkt);
       tab_gate_port[i]->ID=ID_counter;          //nadajemy poszczególnym elementom
       ID_counter++;
       Elementy.put_gate_to_list(tab_gate_port[i]);
        }
   gate->conect_gate_to_port(tab_gate_port) ;
   Elementy.put_gate_to_list(gate);
   }


 if(element==OR)
   {
   electronic *tab_gate_port[3];

   gate=new OR_gate(Canwas,Punkt);
   gate->ID=ID_counter;
   ID_counter++;
   for(int i=0;i<3;i++)
        {
       tab_gate_port[i]=new port(Canwas,Punkt);
       tab_gate_port[i]->ID=ID_counter;          //nadajemy poszczególnym elementom
       ID_counter++;
       Elementy.put_gate_to_list(tab_gate_port[i]);
        }
   gate->conect_gate_to_port(tab_gate_port) ;
   Elementy.put_gate_to_list(gate);
   }
}
//----------------------------------------------------------------------------
bool gouped_element::conect(long ID_1,long ID_2)
{
//adresy dwoch portow do po³¹czenia
if(ID_1==ID_2)
    return false;
electronic* first_port;
electronic* second_port;
electronic* conector_wire;

first_port=Elementy.get_adress_by_ID(ID_1);
second_port=Elementy.get_adress_by_ID(ID_2);
 if(first_port->Element!=PORT ||
    second_port->Element!=PORT)
    return false;

conector_wire=new wire(Canwas);
conector_wire->pointer_conector[0]=first_port;
conector_wire->pointer_conector[1]=second_port;
conector_wire->ID=ID_counter;
//sprawdzamy czy do obu portów da si podpi¹æ przynajmniej 1 wire
if((first_port->pointer_conector[0]==NULL||
    first_port->pointer_conector[1]==NULL||
    first_port->pointer_conector[2]==NULL||
    first_port->pointer_conector[3]==NULL)&&
  (second_port->pointer_conector[0]==NULL||
   second_port->pointer_conector[1]==NULL||
   second_port->pointer_conector[2]==NULL||
   second_port->pointer_conector[3]==NULL
    ))
{
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
else
 {
 delete conector_wire;
 return false;
 }
ID_counter++;
Elementy.put_gate_to_list(conector_wire);
return true;
}
//-----------------------------------------------------------------------------
void gouped_element::stop_simulated()
{
Elementy.stop_simulated();
return;
}
//-----------------------------------------------------------------------------
bool gouped_element::start_simulated()
{
return Elementy.start_simulated();

}
//-----------------------------------------------------------------------------
void gouped_element::save_file(AnsiString output_file)
{
Elementy.save_file(output_file) ;
}
//-----------------------------------------------------------------------------
void gouped_element::open_file(AnsiString input_file)
{
ID_counter=Elementy.open_file( input_file);
ID_counter++;
}
