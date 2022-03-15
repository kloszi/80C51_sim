
#ifndef funkcje
#define funkcje

#define B_   0xF0
#define ACC  0xE0
#define PSW  0xD0
#define SP   0x81
#define P_0   0x80
#define P_1   0x90
#define P_2   0xA0
#define P_3   0xB0
#define BREG  0xF0
#define SPT   0x81
#define DPL   0x82
#define DPH   0x83
#define PSW   0xD0
#define PORT2 0xA0
#define PORT3 0xB0
#define IE0   0xA8
#define IP0   0xB8
#define TCON  0x88
#define TMOD  0x89
#define TL0   0x8A
#define TH0   0x8C
#define TL1   0x8B
#define TH1   0x8D
#define SCON  0x98
#define S0BUF 0x99


#define RomSize  2024
#define RamSize  256
#define XRamSize 0

class  i8051
{
public:
    i8051();
    void Step() ;
    void disassembler();
    bool LoadHexToRom (char *filename);
    bool LoadHexToRam (char *filename);

    char ROM_[RomSize];   //delaracja zewnetrznej pamiêci programu
    char RAM_[RamSize];     //deklaracja wewnêtrznej pamiêci danych
    char XRAM_[XRamSize];  //deklaracja zewnêtrznej pamiêci danych
    unsigned short PC_;  //wska¿nik rozkazów w pamiêci ROM
    AnsiString Name;
private:
    bool Hex2Short(const char* buf, unsigned &val);
    bool Load(const char* buf, unsigned char* rom, unsigned& prgSize);
    char cycle;
    void HandleTimers(int ticks);
    char prio; //priorytet obecnie obs³ugiwanego przerwnia
} ;


//extern  i8051 procek;         //deklaracja powyzszej klasy
   extern  char execution_order[50];


#endif