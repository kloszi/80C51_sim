//---------------------------------------------------------------------------

#ifndef Help_dlgH
#define Help_dlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "SHDocVw_OCX.h"
#include <OleServer.hpp>
#include <OleCtrls.hpp>
#include <HTTPApp.hpp>
//---------------------------------------------------------------------------
class THelp_form : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TMemo *Memo1;
    TButton *Button1;
    TLabel *Label2;
    void __fastcall Label1Click(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall THelp_form(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE THelp_form *Help_form;
//---------------------------------------------------------------------------
#endif
