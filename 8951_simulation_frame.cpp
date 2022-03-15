
#include "8951_simulation_frame.h"

TMicrochip_frame::TMicrochip_frame(TPaintBox *Obrazek)
{
Formatka = new TForm(Obrazek);
Formatka->DoubleBuffered=true;
Formatka->Width=400;
Formatka->Height=460;
Formatka->Caption="Procesor";
Formatka->FormStyle=fsStayOnTop;

TrackBar1=new TTrackBar (Formatka);
TrackBar1->Parent=Formatka;
TrackBar1->Left = 140;
TrackBar1->Top = 325;
TrackBar1->Visible=true;
TrackBar1->Max=50;
TrackBar1->Position=25;

Label1=new TLabel(Formatka);
Label1->Parent=Formatka;
Label1->Left = 70;
Label1->Top = 340;
Label1->Width = 50 ;
Label1->Height = 27 ;
Label1->Caption="NOP";
Label1->Visible=true;

Label2=new TLabel(Formatka);
Label2->Parent=Formatka;
Label2->Left =20;
Label2->Top = 340;
//Label2->Width = 50 ;
//Label2->Height = 27 ;
Label2->Caption="0000 h";
Label2->Visible=true;

Label3=new TLabel(Formatka);
Label3->Parent=Formatka;
Label3->Left = 300;
Label3->Top = 340;
//Label3->Width = 50 ;
//Label3->Height = 27 ;
Label3->Caption="NOP";
Label3->Visible=true;


Acc_register=new TLabel(Formatka);
Acc_register->Parent=Formatka;
Acc_register->Left = 0;
Acc_register->Top = 380;
Acc_register->Caption="Acc";
Acc_register->Visible=true;

P0_register=new TLabel(Formatka);
P0_register->Parent=Formatka;
P0_register->Left = 60;
P0_register->Top = 380;
P0_register->Caption="P0";
P0_register->Visible=true;

P1_register=new TLabel(Formatka);
P1_register->Parent=Formatka;
P1_register->Left = 110;
P1_register->Top = 380;
P1_register->Caption="P1";
P1_register->Visible=true;

P2_register=new TLabel(Formatka);
P2_register->Parent=Formatka;
P2_register->Left = 160;
P2_register->Top = 380;
P2_register->Caption="P2";
P2_register->Visible=true;

P3_register=new TLabel(Formatka);
P3_register->Parent=Formatka;
P3_register->Left = 210;
P3_register->Top = 380;
P3_register->Caption="P3";
P3_register->Visible=true;

StringGrid1=new TStringGrid(Formatka);
StringGrid1->Parent=Formatka;
StringGrid1->Left = 1;
StringGrid1->Top = 1;
StringGrid1->Width = 390 ;
StringGrid1->Height = 320 ;

StringGrid1->Ctl3D = False  ;
StringGrid1->DefaultRowHeight =16;
StringGrid1->FixedRows = 0 ;
StringGrid1->ParentCtl3D = False ;
StringGrid1->TabOrder = 2 ;


StringGrid1->RowCount=1;
StringGrid1->ColCount=2;// ColWidths=60;// >ColWidths = [ 64, 86];
StringGrid1->ColWidths[0]=17  ;
StringGrid1->ColWidths[1]=390  ;


StringGrid1->Font->Size=8;
StringGrid1->Font->Name= "Fixedsys";
StringGrid1->OnDrawCell=StringGrid1DrawCell;
//idzie za zaznaczeniem
StringGrid1->Options = StringGrid1->Options << goRowSelect;
StringGrid1->Options = StringGrid1->Options >> goVertLine;
StringGrid1->Options = StringGrid1->Options >> goHorzLine;
StringGrid1->Options = StringGrid1->Options >> goRangeSelect;

on_in_ram=new TButton(in_RAM);
on_in_ram->Parent=Formatka;
on_in_ram->Left = 50;
on_in_ram->Top = 400;
on_in_ram->Width = 90 ;
on_in_ram->Caption="RAM zewnêtrzny";
on_in_ram->Visible=true;
on_in_ram->OnClick=Button1Click;

//------------------------------------------------------------------------------
in_RAM=new TForm(Obrazek);
in_RAM->DoubleBuffered=true;
in_RAM->Width=420;
in_RAM->Height=409;
in_RAM->Caption="Wewnêtrzny RAM";
in_RAM->FormStyle=fsStayOnTop;
in_RAM->Left=400;




StringGrid_in_RAM=new TStringGrid(in_RAM);
StringGrid_in_RAM->Parent=in_RAM;
StringGrid_in_RAM->Left = 1;
StringGrid_in_RAM->Top = 1;
StringGrid_in_RAM->Width = 383 ;
StringGrid_in_RAM->Height = 368 ;
StringGrid_in_RAM->RowCount=17;
StringGrid_in_RAM->ColCount=17;
StringGrid_in_RAM->DefaultRowHeight =20;
StringGrid_in_RAM->DefaultColWidth=20;
StringGrid_in_RAM->Ctl3D=false;

int i=0;
AnsiString B ;
for(i=0;i<16;i++)
    {
    B.printf("%02X",i*16);    //po
    StringGrid_in_RAM->Cells[0][i+1]=B;
    B.printf("x%1Xh",i);
    StringGrid_in_RAM->Cells[i+1][0]=B;
     }

//------------------------------------------------------------------------------
cursor_position=0;
size_of_program=0;
cycles=0;
}
//------------------------------------------------------------------------------
TMicrochip_frame::~TMicrochip_frame()
{
delete Formatka;
}
//------------------------------------------------------------------------------
void TMicrochip_frame::Show()
{
Formatka->Show();
}
//------------------------------------------------------------------------------
void TMicrochip_frame::Hide()
{
Formatka->Hide();
}
//------------------------------------------------------------------------------
void TMicrochip_frame::Add_procesor(i8051  *Proc_WE)
{
int j=0,i=0;
AnsiString B ;
Proc=Proc_WE;
for(i=0;i<16;i++)
    for(j=0;j<16;j++)
        {
        B.printf("%02X",(unsigned char)(Proc->RAM_[i*16+j])); //wyswietlanie poszczególnych komórek
        StringGrid_in_RAM->Cells[j+1][i+1]=B;
        }
}
//------------------------------------------------------------------------------
void __fastcall TMicrochip_frame::Button1Click(TObject *Sender)
{
if (in_RAM->Visible)
    in_RAM->Hide();
else
    in_RAM->Show();
}
//-----------------------------------------------------------------------------
long TMicrochip_frame::HexToDec(char* Hex_number)
{
if(Hex_number[4]!=':')
 return -1;

long Result=0;
int  mnoznik=1;
char Tab_hex_char[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

for(int i=3;i>=0;i--)
    {
    for(int j=0;j<=(sizeof(Tab_hex_char)/sizeof(char))-1;j++)
       {
       if(Hex_number[i]==Tab_hex_char[j])
          Result=Result+(mnoznik*j) ;
       }
    mnoznik=mnoznik*16;
    }
return Result;
}
//------------------------------------------------------------------------------
void TMicrochip_frame::Load_Lst_File(AnsiString filename)
{
char buf[256];
char cut_buf[256];
int  j=0;

ifstream ifs(filename.c_str());

for(long i=0;i<65535;i++)
        {
        tab_PC[i]=-1;
        }

while(!ifs.eof())
    {
    StringGrid1->RowCount=j+1;
	ifs.getline(buf, sizeof(buf));
    tab_PC[j]=HexToDec(buf);
    for(int i=0;i<236;i++)
        {
        cut_buf[i]=buf[i+19];
        }
    cut_buf[36]='\n';
    StringGrid1->Cells[1][j]=cut_buf;
    cut_buf[0]='\n';
    j++;
    }
size_of_program=j;
ifs.close();
return;
}
//-----------------------------------------------------------------------------
void __fastcall TMicrochip_frame::StringGrid1DrawCell(TObject *Sender, int ACol,
      int ARow,const TRect &Rect, TGridDrawState State)
{
//if(ARow ==cursor_position&&ACol==1)
//  {
/*  StringGrid1->Canvas->Brush->Color = clBackground;
  StringGrid1->Canvas->FillRect(Rect);
  StringGrid1->Canvas->TextOut(Rect.Left, Rect.Top, StringGrid1->Cells[ACol][ARow]);   */
//  }
}
//--------------------------------

//------------------------------------------------------------------------------

//----------------------------------------------------------------------------
void TMicrochip_frame::Refresh()
{

if(Formatka->Visible==false)
    return;
//opóŸnienie
long i=0;
if(TrackBar1->Position>0)
   Sleep(TrackBar1->Position*10);
if(execution_order[0]!='\0')
     {
     Label1->Caption=execution_order;
     }
else
    return;//jeœli procesor nie wykona³ ¿adnej instrukcji

Label2->Caption="PC= "+IntToStr(Proc->PC_);
Label3->Caption="Cykle "+IntToStr(cycles++);


for(i=0;i<size_of_program;i++)
    {
    if(Proc->PC_==tab_PC[i])
      break;
    }
if(StringGrid1->RowCount<=i)
    i=StringGrid1->RowCount-1;

cursor_position=i;
StringGrid1->Row=cursor_position;

StringGrid1->Cells[0][cursor_position]=" ";
StringGrid1->Refresh();
//**************************************************
AnsiString tmp;
//Acc_register;
tmp.printf("%02X",(unsigned char)Proc->RAM_[ACC]);
Acc_register->Caption="Acc "+tmp;

tmp.printf("%02X",(unsigned char)Proc->RAM_[P_0]);
P0_register->Caption="P0= "+tmp;

tmp.printf("%02X",(unsigned char)Proc->RAM_[P_1]);
P1_register->Caption="P1= "+tmp;

tmp.printf("%02X",(unsigned char)Proc->RAM_[P_2]);
P2_register->Caption="P2= "+tmp;

tmp.printf("%02X",(unsigned char)Proc->RAM_[P_3]);
P3_register->Caption="P3= "+tmp;

if(on_in_ram->Visible)
    {
 int i=0,j=0;
 AnsiString B ;

    for(i=0;i<16;i++)
        for(j=0;j<16;j++)
            {
            if (Proc->RAM_[i*16+j]!=na_ekranie_wyswietlany_RAM[i*16+j])
                {
                B.printf("%02X",(unsigned char)(Proc->RAM_[i*16+j])); //wyswietlanie poszczególnych komórek
                StringGrid_in_RAM->Cells[j+1][i+1]=B;
                na_ekranie_wyswietlany_RAM[i*16+j]=Proc->RAM_[i*16+j];
                }
            }
    }
}
//------------------------------------------------------------------------------
void TMicrochip_frame::Stop_simulation()
{

}
//------------------------------------------------------------------------------
bool TMicrochip_frame::Start_simulation()
{
return true;
}
