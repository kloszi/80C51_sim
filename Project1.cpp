//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("Unit1.cpp", Form1);
USEFORM("AND_dlg.cpp", AND_frame);
USEFORM("LED7_dlg.cpp", LED7_form);
USEFORM("MCS_51_dlg.cpp", MCS51_frame);
USEFORM("Wire_dlg.cpp", Wire_form);
USEFORM("Generator_dlg.cpp", Generator);
USEFORM("NOT_dlg.cpp", NOT_frame);
USEFORM("OR_dlg.cpp", OR_frame);
USEFORM("Help_dlg.cpp", Help_form);
USEFORM("Schematic_paint.cpp", Paint_box);
USEFORM("NANA_dlg.cpp", NAND_frame);
USEFORM("LED1_dlg.cpp", Swith_form);
USEFORM("switch_dlg.cpp", Led1_form);
USEFORM("info_dlg.cpp", Info_form);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();

         Application->CreateForm(__classid(TForm1), &Form1);
                 Application->CreateForm(__classid(TAND_frame), &AND_frame);
                 Application->CreateForm(__classid(TLED7_form), &LED7_form);
                 Application->CreateForm(__classid(TMCS51_frame), &MCS51_frame);
                 Application->CreateForm(__classid(TWire_form), &Wire_form);
                 Application->CreateForm(__classid(TGenerator), &Generator);
                 Application->CreateForm(__classid(TNOT_frame), &NOT_frame);
                 Application->CreateForm(__classid(TOR_frame), &OR_frame);
                 Application->CreateForm(__classid(THelp_form), &Help_form);
                 Application->CreateForm(__classid(TPaint_box), &Paint_box);
                 Application->CreateForm(__classid(TNAND_frame), &NAND_frame);
                 Application->CreateForm(__classid(TSwith_form), &Swith_form);
                 Application->CreateForm(__classid(TLed1_form), &Led1_form);
                 Application->CreateForm(__classid(TInfo_form), &Info_form);
                 Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    catch (...)
    {
         try
         {
             throw Exception("");
         }
         catch (Exception &exception)
         {
             Application->ShowException(&exception);
         }
    }
    return 0;
}
//---------------------------------------------------------------------------
