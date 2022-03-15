//---------------------------------------------------------------------------

#ifndef Schematic_paintH
#define Schematic_paintH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TPaint_box : public TForm
{
__published:	// IDE-managed Components
    TPaintBox *PaintBox1;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall PaintBox1MouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall PaintBox1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall PaintBox1Paint(TObject *Sender);
    void __fastcall PaintBox1MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall PaintBox1DblClick(TObject *Sender);
private:	// User declarations
public:		// User declarations

    int X_;//zmienne wykorzystywane podczas dwukrotnego klikniêcia na prze³acznik
    int Y_;
//    bool is_painting; //zmienna wykorzystywnia do blokowania podczas rysowania
    __fastcall TPaint_box(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TPaint_box *Paint_box;
//---------------------------------------------------------------------------
#endif
