//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "MCS_51_dlg.h"
#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMCS51_frame *MCS51_frame;
//---------------------------------------------------------------------------
__fastcall TMCS51_frame::TMCS51_frame(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TMCS51_frame::Button1Click(TObject *Sender)
{
// OpenDialog1->InitialDir=ExtractFileDir(Application->ExeName)+"\\Hex";
 OpenDialog1->InitialDir=Form1->Opening_dir;
 if (OpenDialog1->Execute()){
    CurrentFile = OpenDialog1->FileName;
    Edit1->Text=CurrentFile;
  }
}
//---------------------------------------------------------------------------



void __fastcall TMCS51_frame::Button3Click(TObject *Sender)
{

 char  shortPath[255];
 OpenDialog2->InitialDir=Form1->Opening_dir;// ExtractFileDir(Application->ExeName);

  if (OpenDialog2->Execute())
    {

    Edit2->Text=OpenDialog2->FileName; //wyswietlanie scierzki do otwartego pliku asm
//*********************************************************

    SECURITY_ATTRIBUTES sa;
    HANDLE hRead,hWrite;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&hRead,&hWrite,&sa,0))
    {
      ShowMessage("Error On CreatePipe()");
      return;
    }
   // GetShortPathName
     GetShortPathName(OpenDialog2->FileName.c_str(), shortPath, sizeof(shortPath)) ;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si);
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    if (!CreateProcess(NULL,("as31.exe -l "+(AnsiString)shortPath).c_str() ,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))
    {
       ShowMessage("Error on CreateProcess()");
        return;
    }
    CloseHandle(hWrite);

    char buffer[4096] = {0};
    DWORD bytesRead;
    for(;;)
      {
        if (ReadFile(hRead,buffer,4095,&bytesRead,NULL) == NULL)
            break;
        Memo1->Lines->Add(buffer);
      }
    //za³adowanie hex_a
     int dlugosc=OpenDialog2->FileName.Length();

     //ma³a podmiana rozszerzenia z asm na hex
     if(OpenDialog2->FileName[dlugosc-2]=='a')
        OpenDialog2->FileName.c_str()[dlugosc-3]='h';
     if(OpenDialog2->FileName[dlugosc-1]=='s')
        OpenDialog2->FileName.c_str()[dlugosc-2]='e';
     if(OpenDialog2->FileName[dlugosc]=='m')
         OpenDialog2->FileName.c_str()[dlugosc-1]='x';

     if(FileExists(OpenDialog2->FileName))  //sprawdzamy czy siê skompilowa³o
     {
     // DWORD dwSize  Size(OpenDialog2->FileName)
     //FileSizeByName GetFileSize  FindFirstFile   OpenFile
     long size;
     FILE *f;

     f = fopen(OpenDialog2->FileName.c_str(), "rb");
     fseek(f, 0, SEEK_END);
     size = ftell(f);//pobieramy rozmiar pliku
     fclose(f);

     if(size>16)
        {
        CurrentFile=OpenDialog2->FileName;
        Edit1->Text=CurrentFile;
        }
     }
     //***********************************************************

  }
}
//---------------------------------------------------------------------------

void __fastcall TMCS51_frame::FormCreate(TObject *Sender)
{
Memo1->Clear();
Memo2->Lines->LoadFromFile(ExtractFileDir(Application->ExeName)+"\\Help\\89C51.txt");
}
//---------------------------------------------------------------------------

void __fastcall TMCS51_frame::FormShow(TObject *Sender)
{
Edit1->Text="";
Edit2->Text="";
Memo1->Clear();
CurrentFile="";
}
//---------------------------------------------------------------------------

