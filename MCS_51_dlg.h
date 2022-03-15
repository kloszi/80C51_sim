//---------------------------------------------------------------------------

#ifndef MCS_51_dlgH
#define MCS_51_dlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TMCS51_frame : public TForm
{
__published:	// IDE-managed Components
    TPageControl *Propeis;
    TTabSheet *TabSheet1;
    TTabSheet *TabSheet2;
    TImage *Image1;
    TButton *Button1;
    TEdit *Edit1;
    TComboBox *ComboBox1;
    TLabel *Label1;
    TOpenDialog *OpenDialog1;
    TButton *Button2;
    TLabel *Label2;
    TGroupBox *GroupBox1;
    TGroupBox *GroupBox2;
    TButton *Button3;
    TEdit *Edit2;
    TOpenDialog *OpenDialog2;
    TMemo *Memo1;
    TTabSheet *TabSheet3;
    TMemo *Memo2;
    TButton *Button4;
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall Button3Click(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TMCS51_frame(TComponent* Owner);
     AnsiString CurrentFile;
};
//---------------------------------------------------------------------------
extern PACKAGE TMCS51_frame *MCS51_frame;
//---------------------------------------------------------------------------
#endif
