//---------------------------------------------------------------------------

#ifndef Wire_dlgH
#define Wire_dlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TWire_form : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TLabel *ID;
    TCheckBox *change_;
    TButton *Button1;
    TGroupBox *GroupBox1;
    TGroupBox *GroupBox2;
    TGroupBox *GroupBox3;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label6;
    TLabel *Label7;
    TImage *Image1;
    TPageControl *PageControl1;
    TTabSheet *TabSheet1;
    TTabSheet *TabSheet2;
private:	// User declarations
public:		// User declarations
    __fastcall TWire_form(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TWire_form *Wire_form;
//---------------------------------------------------------------------------
#endif
