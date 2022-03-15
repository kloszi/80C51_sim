//---------------------------------------------------------------------------

#ifndef OR_dlgH
#define OR_dlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TOR_frame : public TForm
{
__published:	// IDE-managed Components
    TPageControl *PageControl1;
    TTabSheet *TabSheet1;
    TTabSheet *TabSheet2;
    TImage *Image1;
    TLabel *Label3;
    TLabel *Label6;
    TLabel *Label4;
    TLabel *Label7;
    TLabel *Label5;
    TLabel *Label8;
    TLabel *Label2;
    TLabel *Label1;
    TImage *Image2;
    TTabSheet *TabSheet3;
    TMemo *Memo1;
    TButton *Button1;
private:	// User declarations
public:		// User declarations
    __fastcall TOR_frame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TOR_frame *OR_frame;
//---------------------------------------------------------------------------
#endif
