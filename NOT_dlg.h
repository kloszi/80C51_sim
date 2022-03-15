//---------------------------------------------------------------------------

#ifndef NOT_dlgH
#define NOT_dlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TNOT_frame : public TForm
{
__published:	// IDE-managed Components
    TPageControl *PageControl1;
    TTabSheet *TabSheet1;
    TTabSheet *TabSheet2;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label2;
    TImage *Image1;
    TLabel *Label6;
    TLabel *Label1;
    TLabel *Label5;
    TButton *Button1;
    TImage *Image2;
    TLabel *Label7;
    TTabSheet *TabSheet3;
    TMemo *Memo1;
    void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TNOT_frame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TNOT_frame *NOT_frame;
//---------------------------------------------------------------------------
#endif
