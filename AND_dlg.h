//---------------------------------------------------------------------------

#ifndef AND_dlgH
#define AND_dlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TAND_frame : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label7;
    TLabel *Label8;
    TPageControl *PageControl1;
    TTabSheet *TabSheet2;
    TTabSheet *TabSheet3;
    TImage *Image1;
    TButton *Button1;
    TImage *Image2;
    TLabel *Label9;
    TTabSheet *TabSheet1;
    TMemo *Memo1;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TAND_frame(TComponent* Owner);
long ID;

};
//---------------------------------------------------------------------------
extern PACKAGE TAND_frame *AND_frame;
//---------------------------------------------------------------------------
#endif
