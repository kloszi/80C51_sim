//---------------------------------------------------------------------------

#ifndef switch_dlgH
#define switch_dlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TLed1_form : public TForm
{
__published:	// IDE-managed Components
        TTabSheet *Info;
        TTabSheet *Help;
        TPageControl *tab;
        TButton *Button1;
        TImage *Image1;
        TMemo *Memo1;
        void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TLed1_form(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TLed1_form *Led1_form;
//---------------------------------------------------------------------------
#endif
