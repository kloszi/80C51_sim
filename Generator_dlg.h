//---------------------------------------------------------------------------

#ifndef Generator_dlgH
#define Generator_dlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "CSPIN.h"
//---------------------------------------------------------------------------
class TGenerator : public TForm
{
__published:	// IDE-managed Components
    TPageControl *PageControl1;
    TButton *OK;
    TTabSheet *TabSheet1;
    TTabSheet *TabSheet2;
    TLabel *Label1;
    TLabel *Label2;
    TCSpinEdit *CSpinEdit1;
    TCSpinEdit *CSpinEdit2;
    TLabel *Label4;
    TComboBox *ComboBox1;
    TMemo *Memo1;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OKClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ComboBox1Change(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
    long frequency; //czêstotliwoœæ  1-100 000 Hz
    int  fill;      //wspó³czynnik wype³nienia od 1-99%
    __fastcall TGenerator(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TGenerator *Generator;
//---------------------------------------------------------------------------
#endif
