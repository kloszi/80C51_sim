//---------------------------------------------------------------------------

#ifndef NANA_dlgH
#define NANA_dlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TNAND_frame : public TForm
{
__published:	// IDE-managed Components
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
    TImage *Image1;
    TImage *Image2;
    TButton *Button1;
    TTabSheet *TabSheet3;
    TMemo *Memo1;
    void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TNAND_frame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TNAND_frame *NAND_frame;
//---------------------------------------------------------------------------
#endif
