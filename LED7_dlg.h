//---------------------------------------------------------------------------

#ifndef LED7_dlgH
#define LED7_dlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TLED7_form : public TForm
{
__published:	// IDE-managed Components
    TPageControl *PageControl1;
    TTabSheet *TabSheet1;
    TTabSheet *TabSheet2;
    TImage *Image1;
    TButton *Button1;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label7;
    TLabel *Label8;
    TLabel *Label9;
    TLabel *Label10;
    TTabSheet *TabSheet3;
    TImage *Image2;
    TTabSheet *TabSheet4;
    TMemo *Memo1;
    void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TLED7_form(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TLED7_form *LED7_form;
//---------------------------------------------------------------------------
#endif
