//---------------------------------------------------------------------------

#ifndef info_dlgH
#define info_dlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TInfo_form : public TForm
{
__published:	// IDE-managed Components
    TButton *Button1;
    TMemo *Memo1;
private:	// User declarations
public:		// User declarations
    __fastcall TInfo_form(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TInfo_form *Info_form;
//---------------------------------------------------------------------------
#endif
