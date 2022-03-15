//---------------------------------------------------------------------------

#ifndef LED1_dlgH
#define LED1_dlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TSwith_form : public TForm
{
__published:	// IDE-managed Components
        TButton *Button1;
        TPageControl *PageControl1;
        TTabSheet *TabSheet1;
        TTabSheet *TabSheet2;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TRadioButton *RadioButton1;
        TRadioButton *RadioButton2;
        TImage *Image1;
        TImage *Image2;
        TImage *Image3;
        TLabel *Label9;
    TMemo *Memo1;
    void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TSwith_form(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSwith_form *Swith_form;
//---------------------------------------------------------------------------
#endif
