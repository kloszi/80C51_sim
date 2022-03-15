#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>                                    
#include <time.h>
#include <string.h>                                                       

#include <stdarg.h>
#include <vcl.h>
//#include "Unit3.h"
#define B    0xF0
#pragma warn -aus

enum Flag { P, USER, OV, RS0, RS1, F0, AC, CY_ };


#include "89C51.h"
int ins_print( char *fmt, ... );
char execution_order[50];
/////////////////////////////////////////////
void ADD_A_Rn();
void ADD_A_adres();
void ADD_A_Ri();
void ADD_A_dana();
//------------------------
void ADDC_A_Rn();
void ADDC_A_adres();
void ADDC_A_Ri();
void ADDC_A_dana();
//-----------------------
void SUBB_A_Rn();
void SUBB_A_adres();
void SUBB_A_Ri();
void SUBB_A_dana();
//------------------------
void INC_A();
void INC_Rn();
void INC_adres();
void INC_Ri();
void INC_DPTR();
//-----------------------
void DEC_A();
void DEC_Rn();
void DEC_adres();
void DEC_Ri();
//----------------------
void MUL_AB();
void DIV_AB();
void DA_A();
                              /*Operacje Logiczne*/
//******************************************************
void ANL_A_Rn();
void ANL_A_adres();
void ANL_A_Ri();
void ANL_A_dana();
void ANL_adres_A();
void ANL_adres_dana();
//-----------------------
void ORL_A_Rn();
void ORL_A_adres();
void ORL_A_Ri();
void ORL_A_dana();
void ORL_adres_A();
void ORL_adres_dana();
//----------------------
void XRL_A_Rn();
void XRL_A_adres();
void XRL_A_Ri();
void XRL_A_dana();
void XRL_adres_A();
void XRL_adres_dana();
//----------------------
void CLR_A();
void CPL_A();
void RL_A();
void RLC_A();
void RR_A();
void RRC_A();
void SWAP_A();
                             /*Operacje przemieszczania danych*/
//*****************************************************************
void MOV_A_Rn();
void MOV_A_adres();
void MOV_A_Ri();
void MOV_A_dana();
void MOV_Rn_A();
void MOV_Rn_adres();
void MOV_Rn_dana();
void MOV_adres_A();
void MOV_adres_Rn();
void MOV_adres_adres();
void MOV_adres_Ri();
void MOV_adres_dana();
void MOV_Ri_A();
void MOV_Ri_adres();
void MOV_Ri_dana();
void MOV_DPTR_dana();
//-------------------------
void MOVX_A_Ri();
void MOVX_A_DPTR();
void MOVX_Ri_A();
void MOVX_DPTR_A();
//------------------------
void MOVC_A_A_DPTR();
void MOVC_A_A_PC();
//-----------------------
void XCH_A_Rn();
void XCH_A_adres();
void XCH_A_Ri();
void XCHD_A_Ri();
//-----------------------
void PUSH_adres();
void POP_adres();
                          /*Operacje na bitach*/
void CLR_C();
void CLR_bit();
//----------------
void SETB_C();
void SETB_bit();
void CPL_C();
void CPL_bit();
void ANL_C_bit();
void ANL_C_not_bit();
void ORL_C_bit();
void ORL_C_not_bit();
void MOV_C_bit();
void MOV_bit_C();
//----------------
void JC_ofset();
void JNC_ofset();
void JB_bit_ofset();
void JNB_bit_ofset();
void JBC_bit_ofset();
                        /*   Skoki   */
void ACALL();
void LCALL();
void RET();
void RETI();
void AJMP();
void LJMP();
void SJMP();
void JMP();
void JZ();
void JNZ();
void CJNE_A_adres();
void CJNE_A_dana();
void CJNE_Rn_dana();
void CJNE_Ri_dana();
void DJNZ_Rn_ofset();
void DJNZ_adres_ofset();
void NOP();

void SetBit(char &thisByte, unsigned char thisBit);
void ClearBit(char &thisByte, unsigned char thisBit);
unsigned char GetBit(char thisByte, unsigned char thisBit);
unsigned char GetRegisterBank();
//****************************************
struct mnementry{
                  int  size;       //rozmiar ile zajmuje dana instrukcja
                  int  time_inst;  //ile w cyklach zajmuje dana instrukcja
                  void (*adres_funkcji)();//adres funkcji która obs³uguje dana instrucja
                };
//**************************************
struct label {
   char mcode[10];  //jak poszczególne rejestry siê nazywaj¹
    };
/////////////////////////////////////////////////////////////////
struct mnementry mnemtbl[256] = {
{1,1,NOP},            {1,2,AJMP},            {3,2,LJMP},         {1,1,RR_A },         /* 00 - 03 */
{1,1,INC_A},          {2,1,INC_adres},       {1,1,INC_Ri},       {1,1,INC_Ri},        /* 04 - 07 */
{1,1,INC_Rn},         {1,1,INC_Rn},          {1,1,INC_Rn},       {1,1,INC_Rn},        /* 08 - 0b */
{1,1,INC_Rn},         {1,1,INC_Rn},          {1,1,INC_Rn},       {1,1,INC_Rn},        /* 0c - 0f */
{2,2,JBC_bit_ofset},  {2,2,ACALL},           {3,2,LCALL},        {1,1,RRC_A},         /* 10 - 13 */
{1,1,DEC_A},          {2,1,DEC_adres},       {1,1,DEC_Ri},       {1,1,DEC_Ri},        /* 14 - 17 */
{1,1,DEC_Rn},         {1,1,DEC_Rn},          {1,1,DEC_Rn},       {1,1,DEC_Rn},        /* 18 - 1b */
{1,1,DEC_Rn},         {1,1,DEC_Rn},          {1,1,DEC_Rn},       {1,1,DEC_Rn},        /* 1c - 1f */
{3,2,JB_bit_ofset},   {2,2,AJMP},            {1,2,RET},          {1,1,RL_A},          /* 20 - 23 */
{2,1,ADD_A_dana},     {2,1,ADD_A_adres},     {1,1,ADD_A_Ri},     {1,1,ADD_A_Ri},      /* 24 - 27 */
{1,1,ADD_A_Rn},       {1,1,ADD_A_Rn},        {1,1,ADD_A_Rn},     {1,1,ADD_A_Rn},      /* 28 - 2b */
{1,1,ADD_A_Rn},       {1,1,ADD_A_Rn},        {1,1,ADD_A_Rn},     {1,1,ADD_A_Rn},      /* 2c - 2f */
{3,3,JNB_bit_ofset},  {2,2,ACALL},           {1,2,RETI},         {1,1,RLC_A},         /* 30 - 33 */
{2,1,ADDC_A_dana},    {2,1,ADDC_A_adres},    {1,1,ADDC_A_Ri},    {1,1,ADDC_A_Ri},     /* 34 - 37 */
{1,1,ADDC_A_Rn},      {1,1,ADDC_A_Rn},       {1,1,ADDC_A_Rn},    {1,1,ADDC_A_Rn},     /* 38 - 3b */
{1,1,ADDC_A_Rn},      {1,1,ADDC_A_Rn},       {1,1,ADDC_A_Rn},    {1,1,ADDC_A_Rn},     /* 3c - 3f */
{2,2,JC_ofset},       {2,2,AJMP},            {2,1,ORL_adres_A},  {3,2,ORL_adres_dana},/* 40 - 43 */
{2,1,ORL_A_dana},     {2,1,ORL_A_adres},     {1,1,ORL_A_Ri},     {1,1,ORL_A_Ri},      /* 44 - 47 */
{1,1,ORL_A_Rn},       {1,1,ORL_A_Rn},        {1,1,ORL_A_Rn},     {1,1,ORL_A_Rn},      /* 48 - 4b */
{1,1,ORL_A_Rn},       {1,1,ORL_A_Rn},        {1,1,ORL_A_Rn},     {1,1,ORL_A_Rn},      /* 4c - 4f */
{2,2,JNC_ofset},      {2,2,ACALL},           {2,1,ANL_adres_A},  {3,2,ANL_adres_dana},/* 50 - 53 */
{2,1,ANL_A_dana},     {2,1,ANL_A_adres},     {1,1,ANL_A_Ri},     {1,1,ANL_A_Ri},      /* 54 - 57 */
{1,1,ANL_A_Rn},       {1,1,ANL_A_Rn},        {1,1,ANL_A_Rn},     {1,1,ANL_A_Rn},      /* 58 - 5b */
{1,1,ANL_A_Rn},       {1,1,ANL_A_Rn},        {1,1,ANL_A_Rn},     {1,1,ANL_A_Rn},      /* 5c - 5f */
{2,2,JZ},             {2,2,AJMP},            {2,1,XRL_adres_A},  {3,2,XRL_adres_dana},/* 60 - 63 */
{2,1,XRL_A_dana},     {2,1,XRL_A_adres},     {1,1,XRL_A_Ri},     {1,1,XRL_A_Ri},      /* 64 - 67 */
{1,1,XRL_A_Rn},       {1,1,XRL_A_Rn},        {1,1,XRL_A_Rn},     {1,1,XRL_A_Rn},      /* 68 - 6b */
{1,1,XRL_A_Rn},       {1,1,XRL_A_Rn},        {1,1,XRL_A_Rn},     {1,1,XRL_A_Rn},      /* 6c - 6f */
{2,2,JNZ},            {2,2,ACALL},           {2,2,ORL_C_bit},    {1,2,JMP},           /* 70 - 73 */
{2,1,MOV_A_dana},     {3,2,MOV_adres_dana},  {2,1,MOV_Ri_dana},  {2,1,MOV_Ri_dana},   /* 74 - 77 */
{2,1,MOV_Rn_dana},    {2,1,MOV_Rn_dana},     {2,1,MOV_Rn_dana},  {2,1,MOV_Rn_dana},   /* 78 - 7b */
{2,1,MOV_Rn_dana},    {2,1,MOV_Rn_dana},     {2,1,MOV_Rn_dana},  {2,1,MOV_Rn_dana},   /* 7c - 7f */
{2,2,SJMP},           {2,2,AJMP},            {2,2,ANL_C_bit},    {1,2,MOVC_A_A_PC},   /* 80 - 83 */
{1,4,DIV_AB},         {3,2,MOV_adres_adres}, {2,2,MOV_A_Ri},     {2,2,MOV_A_Ri},      /* 84 - 87 */
{2,2,MOV_adres_Rn},   {2,2,MOV_adres_Rn},    {2,2,MOV_adres_Rn}, {2,2,MOV_adres_Rn},  /* 88 - 8b */
{2,2,MOV_adres_Rn},   {2,2,MOV_adres_Rn},    {2,2,MOV_adres_Rn}, {2,2,MOV_adres_Rn},  /* 8c - 8f */
{3,2,MOV_DPTR_dana},  {2,2,ACALL},           {2,2,MOV_bit_C},    {1,2,MOVC_A_A_DPTR}, /* 90 - 93 */
{2,1,SUBB_A_dana},    {2,1,SUBB_A_adres},    {1,1,SUBB_A_Ri},    {1,1,SUBB_A_Ri},     /* 94 - 97 */
{1,1,SUBB_A_Rn},      {1,1,SUBB_A_Rn},       {1,1,SUBB_A_Rn},    {1,1,SUBB_A_Rn},     /* 98 - 9b */
{1,1,SUBB_A_Rn},      {1,1,SUBB_A_Rn},       {1,1,SUBB_A_Rn},    {1,1,SUBB_A_Rn},     /* 9c - 9f */
{2,2,ORL_C_not_bit},  {2,2,AJMP},            {2,1,MOV_C_bit},    {1,2,INC_DPTR},      /* a0 - a3 */
{1,4,MUL_AB},         {1,1,NULL},            {2,2,MOV_Ri_adres}, {2,2,MOV_Ri_adres},  /* a4 - a7 */
{2,2,MOV_Rn_adres},   {2,2,MOV_Rn_adres},    {2,2,MOV_Rn_adres}, {2,2,MOV_Rn_adres},  /* a8 - ab */
{2,2,MOV_Rn_adres},   {2,2,MOV_Rn_adres},    {2,2,MOV_Rn_adres}, {2,2,MOV_Rn_adres},  /* ac - af */
{2,2,ANL_C_not_bit},  {2,2,ACALL},           {1,1,CPL_bit},      {1,1,CPL_C},         /* b0 - b3 */
{3,2,CJNE_A_dana},    {3,2,CJNE_A_adres},    {3,2,CJNE_Ri_dana}, {3,2,CJNE_Ri_dana},  /* b4 - b7 */
{3,2,CJNE_Rn_dana},   {3,2,CJNE_Rn_dana},    {3,2,CJNE_Rn_dana}, {3,2,CJNE_Rn_dana},  /* b8 - bb */
{3,2,CJNE_Rn_dana},   {3,2,CJNE_Rn_dana},    {3,2,CJNE_Rn_dana}, {3,2,CJNE_Rn_dana},  /* bc - bf */
{2,2,PUSH_adres},     {2,2,AJMP},            {1,1,CLR_bit},      {1,1,CLR_C},         /* c0 - c3 */
{1,1,SWAP_A},         {2,1,XCH_A_adres},     {1,1,XCH_A_Ri},     {1,1,XCH_A_Ri},      /* c4 - c7 */
{2,1,XCH_A_Rn},       {2,1,XCH_A_Rn},        {2,1,XCH_A_Rn},     {2,1,XCH_A_Rn},      /* c8 - cb */
{2,1,XCH_A_Rn},       {2,1,XCH_A_Rn},        {2,1,XCH_A_Rn},     {2,1,XCH_A_Rn},      /* cc - cf */
{2,2,POP_adres},      {2,2,ACALL},           {2,1,SETB_bit},     {1,1,SETB_C},        /* d0 - d3 */
{1,1,DA_A},           {3,2,DJNZ_adres_ofset},{1,1,XCHD_A_Ri},    {1,1,XCHD_A_Ri},     /* d4 - d7 */
{2,2,DJNZ_Rn_ofset},  {2,2,DJNZ_Rn_ofset},   {2,2,DJNZ_Rn_ofset},{2,2,DJNZ_Rn_ofset}, /* d8 - db */
{2,2,DJNZ_Rn_ofset},  {2,2,DJNZ_Rn_ofset},   {2,2,DJNZ_Rn_ofset},{2,2,DJNZ_Rn_ofset}, /* dc - df */
{1,2,MOVX_A_DPTR},    {2,2,AJMP},            {1,2,MOVX_A_Ri},    {1,2,MOVX_A_Ri},     /* e0 - e3 */
{1,1,CLR_A},          {2,1,MOV_A_adres},     {1,1,MOV_A_Ri},     {1,1,MOV_A_Ri},      /* e4 - e7 */
{1,1,MOV_A_Rn},       {1,1,MOV_A_Rn},        {1,1,MOV_A_Rn},     {1,1,MOV_A_Rn},      /* e8 - eb */
{1,1,MOV_A_Rn},       {1,1,MOV_A_Rn},        {1,1,MOV_A_Rn},     {1,1,MOV_A_Rn},      /* ec - ef */
{1,2,MOVX_DPTR_A},    {2,2,ACALL},           {1,2,MOVX_Ri_A},    {1,2,MOVX_Ri_A},     /* f0 - f3 */
{1,1,CPL_A},          {2,1,MOV_adres_A},     {2,2,MOV_Ri_A},     {2,2,MOV_Ri_A},      /* f4 - f7 */
{1,1,MOV_Rn_A},       {1,1,MOV_Rn_A},        {1,1,MOV_Rn_A},     {1,1,MOV_Rn_A},      /* f8 - fb */
{1,1,MOV_Rn_A},       {1,1,MOV_Rn_A},        {1,1,MOV_Rn_A},     {1,1,MOV_Rn_A}       /* fc - ff */
} ;
//nazwy poszczególnych bajtów
 const struct label sfr[128] = {

    {"p0"},     {"sp"},     {"dpl"},    {"dph"},    /* 80 - 83 */
    {"adat"},   {"85h"},    {"86h"},    {"pcon"},   /* 84 - 87 */
    {"tcon"},   {"tmod"},   {"tl0"},    {"tl1"},    /* 88 - 8b */
    {"th0"},    {"th1"},    {"pwcm"},   {"pwmp"},   /* 8a - 8f */
    {"p1"},     {"91h"},    {"92h"},    {"93h"},    /* 90 - 93 */
    {"94h"},    {"95h"},    {"96h"},    {"97h"},    /* 94 - 97 */
    {"scon"},   {"sbuf"},   {"9ah"},    {"9bh"},    /* 98 - 9b */
    {"9ch"},    {"9dh"},    {"9eh"},    {"9fh"},    /* 9c - 9f */
    {"p2"},     {"0a1h"},   {"0a2h"},   {"0a3h"},   /* a0 - a3 */
    {"0a4h"},   {"0a5h"},   {"0a6h"},   {"0a7h"},   /* a4 - a7 */
    {"ie"},     {"cml0"},   {"cml1"},   {"cml2"},   /* a8 - ab */
    {"ctl0"},   {"ctl1"},   {"ctl2"},   {"ctl3"},   /* ac - af */
    {"p3"},     {"0b1h"},   {"0b2h"},   {"0b3h"},   /* b0 - b3 */
    {"0b4h"},   {"0b5h"},   {"0b6h"},   {"0b7h"},   /* b4 - b7 */
    {"ip"},     {"0b9h"},   {"0bah"},   {"0bbh"},   /* b8 - bb */
    {"0bch"},   {"0bdh"},   {"0beh"},   {"0bfh"},   /* bc - bf */
    {"p4"},     {"0c1h"},   {"0c2h"},   {"0c3h"},   /* c0 - c3 */
    {"p5"},     {"adcon"},  {"adch"},   {"0c7h"},   /* c4 - c7 */
    {"t2con"},  {"cmh0"},   {"rcap2l"}, {"rcap2h"}, /* c8 - cb */
    {"tl2"},    {"th2"},    {"cth2"},   {"cth3"},   /* cc - cf */
    {"psw"},    {"0d1h"},   {"0d2h"},   {"0d3h"},   /* d0 - d3 */
    {"0d4h"},   {"0d5h"},   {"0d6h"},   {"0d7h"},   /* d4 - d7 */
    {"i2cfg"},  {"s1sta"},  {"s1dat"},  {"s1adr"},  /* d8 - db */
    {"0dch"},   {"0ddh"},   {"0deh"},   {"0dfh"},   /* dc - df */
    {"acc"},    {"0e1h"},   {"0e2h"},   {"0e3h"},   /* e0 - e3 */
    {"0e4h"},   {"0e5h"},   {"0e6h"},   {"0e7h"},   /* e4 - e7 */
    {"csr"},    {"0e9h"},   {"tm2con"}, {"ctcon"},  /* e8 - eb */
    {"tml2"},   {"tmh2"},   {"ste"},    {"rte"},    /* ec - ef */
    {"b"},      {"0f1h"},   {"0f2h"},   {"0f3h"},   /* f0 - f3 */
    {"0f4h"},   {"0f5h"},   {"0f6h"},   {"0f7h"},   /* f4 - f7 */
    {"i2sta"},  {"0f9h"},   {"0fah"},   {"0fbh"},   /* f8 - fb */
    {"pwm0"},   {"pwm1"},   {"pwena"},  {"t3"}      /* fc - ff */
} ;

/* obszar adresowany bitowo */
 const struct label sfrbits[256] = {
    {"20h.0"},  {"20h.1"},  {"20h.2"},  {"20h.3"}, /* 00 - 03 */
    {"20h.4"},  {"20h.5"},  {"20h.6"},  {"20h.7"}, /* 04 - 07 */
    {"21h.0"},  {"21h.1"},  {"21h.2"},  {"21h.3"}, /* 08 - 8b */
    {"21h.4"},  {"21h.5"},  {"21h.6"},  {"21h.7"}, /* 0c - 0f */
    {"22h.0"},  {"22h.1"},  {"22h.2"},  {"22h.3"}, /* 10 - 13 */
    {"22h.4"},  {"22h.5"},  {"22h.6"},  {"22h.7"}, /* 14 - 17 */
    {"23h.0"},  {"23h.1"},  {"23h.2"},  {"23h.3"}, /* 18 - 1b */
    {"23h.4"},  {"23h.5"},  {"23h.6"},  {"23h.7"}, /* 1c - 1f */
    {"24h.0"},  {"24h.1"},  {"24h.2"},  {"24h.3"}, /* 20 - 23 */
    {"24h.4"},  {"24h.5"},  {"24h.6"},  {"24h.7"}, /* 24 - 27 */
    {"25h.0"},  {"25h.1"},  {"25h.2"},  {"25h.3"}, /* 28 - 2b */
    {"25h.4"},  {"25h.5"},  {"25h.6"},  {"25h.7"}, /* 2c - 2f */
    {"26h.0"},  {"26h.1"},  {"26h.2"},  {"26h.3"}, /* 30 - 33 */
    {"26h.4"},  {"26h.5"},  {"26h.6"},  {"26h.7"}, /* 34 - 37 */
    {"27h.0"},  {"27h.1"},  {"27h.2"},  {"27h.3"}, /* 38 - 3b */
    {"27h.4"},  {"27h.5"},  {"27h.6"},  {"27h.7"}, /* 3c - 3f */
    {"28h.0"},  {"28h.1"},  {"28h.2"},  {"28h.3"}, /* 40 - 43 */
    {"28h.4"},  {"28h.5"},  {"28h.6"},  {"28h.7"}, /* 44 - 47 */
    {"29h.0"},  {"29h.1"},  {"29h.2"},  {"29h.3"}, /* 48 - 4b */
    {"29h.4"},  {"29h.5"},  {"29h.6"},  {"29h.7"}, /* 4c - 4f */
    {"2ah.0"},  {"2ah.1"},  {"2ah.2"},  {"2ah.3"}, /* 50 - 53 */
    {"2ah.4"},  {"2ah.5"},  {"2ah.6"},  {"2ah.7"}, /* 54 - 57 */
    {"2bh.0"},  {"2bh.1"},  {"2bh.2"},  {"2bh.3"}, /* 58 - 5b */
    {"2bh.4"},  {"2bh.5"},  {"2bh.6"},  {"2bh.7"}, /* 5c - 5f */
    {"2ch.0"},  {"2ch.1"},  {"2ch.2"},  {"2ch.3"}, /* 60 - 63 */
    {"2ch.4"},  {"2ch.5"},  {"2ch.6"},  {"2ch.7"}, /* 64 - 67 */
    {"2dh.0"},  {"2dh.1"},  {"2dh.2"},  {"2dh.3"}, /* 68 - 6b */
    {"2dh.4"},  {"2dh.5"},  {"2dh.6"},  {"2dh.7"}, /* 6c - 6f */
    {"2eh.0"},  {"2eh.1"},  {"2eh.2"},  {"2eh.3"}, /* 70 - 73 */
    {"2eh.4"},  {"2eh.5"},  {"2eh.6"},  {"2eh.7"}, /* 74 - 77 */
    {"2fh.0"},  {"2fh.1"},  {"2fh.2"},  {"2fh.3"}, /* 78 - 7b */
    {"2fh.4"},  {"2fh.5"},  {"2fh.6"},  {"2fh.7"}, /* 7c - 7f */
    {"p0.0"},   {"p0.1"},   {"p0.2"},   {"p0.3"},   /* 80 - 83 */
    {"p0.4"},   {"p0.5"},   {"p0.6"},   {"p0.7"},   /* 84 - 87 */
    {"it0"},    {"ie0"},    {"it1"},    {"ie1"},    /* 88 - 8b */
    {"tr0"},    {"tf0"},    {"tr1"},    {"tf1"},    /* 8c - 8f */
    {"p1.0"},   {"p1.1"},   {"p1.2"},   {"p1.3"},   /* 90 - 93 */
    {"p1.4"},   {"p1.5"},   {"p1.6"},   {"p1.7"},   /* 94 - 97 */
    {"ri"},     {"ti"},     {"rb8"},    {"tb8"},    /* 98 - 9b */
    {"ren"},    {"sm2"},    {"sm1"},    {"sm0"},    /* 9c - 9f */
    {"p2.0"},   {"p2.1"},   {"p2.2"},   {"p2.3"},   /* a0 - a3 */
    {"p2.4"},   {"p2.5"},   {"p2.6"},   {"p2.7"},   /* a4 - a7 */
    {"ex0"},    {"et0"},    {"ex1"},    {"et1"},    /* a8 - ab */
    {"es"},     {"ie.5"},   {"ie.6"},   {"ea"},     /* ac - af */
    {"rxd"},    {"txd"},    {"int0"},   {"int1"},   /* b0 - b3 */
    {"t0"},     {"t1"},     {"wr"},     {"rd"},     /* b4 - b7 */
    {"px0"},    {"pt0"},    {"px1"},    {"pt1"},    /* b8 - bb */
    {"ps"},     {"ip.5"},   {"ip.6"},   {"ip.7"},   /* bc - bf */
    {"0c0h.0"}, {"0c0h.1"}, {"0c0h.2"}, {"0c0h.3"}, /* c0 - c3 */
    {"0c0h.4"}, {"0c0h.5"}, {"0c0h.6"}, {"0c0h.7"}, /* c4 - c7 */
    {"cprl2"},  {"ct2"},    {"tr2"},    {"exen2"},  /* c8 - cb */
    {"tclk"},   {"rclk"},   {"exf2"},   {"tf2"},    /* cc - cf */
    {"p"},      {"psw.1"},  {"ov"},     {"rs0"},    /* d0 - d3 */
    {"rs1"},    {"f0"},     {"ac"},     {"cy"},     /* d4 - d7 */
    {"ct0"},    {"ct1"},    {"i2cfg.2"},{"i2cfg.3"},/* d8 - db */
    {"tirun"},  {"clrti"},  {"mastrq"}, {"slaven"}, /* dc - df */
    {"acc.0"},  {"acc.1"},  {"acc.2"},  {"acc.3"},  /* e0 - e3 */
    {"acc.4"},  {"acc.5"},  {"acc.6"},  {"acc.7"},  /* e4 - e7 */
    {"ibf"},    {"obf"},    {"idsm"},   {"obfc"},   /* e8 - eb */
    {"ma0"},    {"ma1"},    {"mb0"},    {"mb1"},    /* ec - ef */
    {"b.0"},    {"b.1"},    {"b.2"},    {"b.3"},    /* f0 - f3 */
    {"b.4"},    {"b.5"},    {"b.6"},    {"b.7"},    /* f4 - f7 */
    {"xstp"},   {"xstr"},   {"makstp"}, {"makstr"}, /* f8 - fb */
    {"xactv"},  {"xdata"},  {"idle"},   {"i2sta.7"} /* fc - ff */
} ;

///////////////////// - = Udostempniane dane = - /////////////////////

char *ROM;   //delaracja zewnetrznej pamiêci programu
char *RAM;     //deklaracja wewnêtrznej pamiêci danych
char *XRAM;  //deklaracja zewnêtrznej pamiêci danych
unsigned short PC;

//////////////////////////////////////////////////////////////////////
                           /*Operacje arytmetyczne*/
//***************************************************
void  ADD_A_Rn(){
                    bool carry3 = false;
                    bool carry6 = false;
                    bool carry7 = false;
                    short tempAdd;
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];

                    ins_print("ADD  A,R%X ",(int)(IR & 0x07));

		    regNum = IR & 0x07;

		    tempAdd = (RAM[ACC] & 0x0F) +
			(RAM[GetRegisterBank()+regNum] & 0x0F);
		    if( (tempAdd & 0x0010) == 0x0010 ) carry3 = true;
		    tempAdd += ((RAM[ACC] & 0x70) +
			(RAM[GetRegisterBank()+regNum] & 0x70));
		    if( (tempAdd & 0x0080) == 0x0080 ) carry6 = true;
		    tempAdd += ((RAM[ACC] & 0x80) +
			(RAM[GetRegisterBank()+regNum] & 0x80));
		    if( (tempAdd & 0x0100) == 0x0100 ) carry7 = true;
		    RAM[ACC] = tempAdd;
		    if( carry3 ) SetBit(RAM[PSW], AC);
		    else ClearBit(RAM[PSW], AC);
		    if( carry7 ) SetBit(RAM[PSW], CY_);
		    else ClearBit(RAM[PSW], CY_);
		    if( (carry6 && !carry7) || (!carry6 && carry7) )
			SetBit(RAM[PSW], OV);
		    else ClearBit(RAM[PSW], OV);
              }
//************************************************************
void ADD_A_adres(){
                    bool carry3= false;
                    bool carry6= false;
                    bool carry7= false;
                    unsigned char  IR=ROM[++PC];
                    short tempAdd;
                    ins_print("ADD  A,%X ",(unsigned int)(IR));

		    tempAdd = (RAM[ACC] & 0x0F) + ( RAM[IR] & 0x0F );
                    if( (tempAdd & 0x0010) == 0x0010 ) carry3 = true;
                      tempAdd += ((RAM[ACC] & 0x70) + (RAM[IR] & 0x70));
                    if( (tempAdd & 0x0080) == 0x0080 ) carry6 = true;
		    tempAdd += ((RAM[ACC] & 0x80) + (RAM[IR] & 0x80));
                    if( (tempAdd & 0x0100) == 0x0100 ) carry7 = true;
		              RAM[ACC] = tempAdd;
		    if( carry3 ) SetBit(RAM[PSW], AC);
		          else ClearBit(RAM[PSW], AC);
		    if( carry7 ) SetBit(RAM[PSW], CY_);
		          else ClearBit(RAM[PSW], CY_);
		    if( (carry6 && !carry7) || (!carry6 && carry7) )
			SetBit(RAM[PSW], OV);
		          else ClearBit(RAM[PSW], OV);
}
//***********************************************
void  ADD_A_Ri(){
                    bool carry3 = false;
                    bool carry6 = false;
                    bool carry7 = false;
                    short tempAdd;
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("ADD  A,@R%X ",((int)(IR & 0x01)));
		    regNum = IR & 0x01;
		    tempAdd = (RAM[ACC] & 0x0F) +
			(RAM[RAM[GetRegisterBank()+regNum]] & 0x0F);
		    if( (tempAdd & 0x0010) == 0x0010 ) carry3 = true;
		    tempAdd += ((RAM[ACC] & 0x70) +
			(RAM[RAM[GetRegisterBank()+regNum]] & 0x70));
		    if( (tempAdd & 0x0080) == 0x0080 ) carry6 = true;
		    tempAdd += ((RAM[ACC] & 0x80) +
			(RAM[RAM[GetRegisterBank()+regNum]] & 0x80));
		    if( (tempAdd & 0x0100) == 0x0100 ) carry7 = true;
		    RAM[ACC] = tempAdd;
		    if( carry3 ) SetBit(RAM[PSW], AC);
		    else ClearBit(RAM[PSW], AC);
		    if( carry7 ) SetBit(RAM[PSW], CY_);
		    else ClearBit(RAM[PSW], CY_);
		    if( (carry6 && !carry7) || (!carry6 && carry7) )
			SetBit(RAM[PSW], OV);
		    else ClearBit(RAM[PSW], OV);
                }
//*****************************************************
void  ADD_A_dana(){
                    bool carry3 = false;
                    bool carry6 = false;
                    bool carry7 = false;
                    short tempAdd;
                    unsigned char  IR=ROM[++PC];
                    ins_print("ADD  A,#%X ",(int)(IR));

                     tempAdd = (RAM[ACC] & 0x0F) + ((char)IR & 0x0F);
		    if( (tempAdd & 0x0010) == 0x0010 ) carry3 = true;
		    tempAdd += ((RAM[ACC] & 0x70) + ((char)IR & 0x70));
		    if( (tempAdd & 0x0080) == 0x0080 ) carry6 = true;
		    tempAdd += ((RAM[ACC] & 0x80) + ((char)IR & 0x80));
		    if( (tempAdd & 0x0100) == 0x0100 ) carry7 = true;
		    RAM[ACC] = tempAdd;
		    if( carry3 ) SetBit(RAM[PSW], AC);
		    else ClearBit(RAM[PSW], AC);
		    if( carry7 ) SetBit(RAM[PSW], CY_);
		    else ClearBit(RAM[PSW], CY_);
		    if( (carry6 && !carry7) || (!carry6 && carry7) )
			SetBit(RAM[PSW], OV);
		    else ClearBit(RAM[PSW], OV);
                  }
//*******************************************************
void  ADDC_A_Rn(){
                    bool carry3 = false;
                    bool carry6 = false;
                    bool carry7 = false;
                    short tempAdd;
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("ADDC  A,@R%X ",(int)(IR & 0x07));

		    regNum = IR & 0x07;
		    tempAdd = (RAM[ACC] & 0x0F) +
			(RAM[GetRegisterBank()+regNum] & 0x0F) +
			(char)GetBit(RAM[PSW], CY_);
		    if( (tempAdd & 0x0010) == 0x0010 ) carry3 = true;
		    tempAdd += (RAM[ACC] & 0x70) +
			(RAM[GetRegisterBank()+regNum] & 0x70);
		    if( (tempAdd & 0x0080) == 0x0080 ) carry6 = true;
		    tempAdd += (RAM[ACC] & 0x80) +
			(RAM[GetRegisterBank()+regNum] & 0x80);
		    if( (tempAdd & 0x0100) == 0x0100 ) carry7 = true;
		    RAM[ACC] = (unsigned char)(tempAdd & 0x00FF);
		    if( carry3 ) SetBit(RAM[PSW], AC);
		    else ClearBit(RAM[PSW], AC);
		    if( carry7 ) SetBit(RAM[PSW], CY_);
		    else ClearBit(RAM[PSW], CY_);
		    if( (carry6 && !carry7) || (!carry6 && carry7) )
			SetBit(RAM[PSW], OV);
		    else ClearBit(RAM[PSW], OV);
}
//*************************************************
void  ADDC_A_adres(){
                    bool carry3 = false;
                    bool carry6 = false;
                    bool carry7 = false;
                    short tempAdd;
                    unsigned char  IR=ROM[++PC];
                    ins_print("ADDC  A,%X",(int)(IR));

		    tempAdd = (RAM[ACC] & 0x0F) +
			(RAM[IR] & 0x0F) +
			(char)GetBit(RAM[PSW], CY_);
		    if( (tempAdd & 0x0010) == 0x0010 ) carry3 = true;
		    tempAdd += (RAM[ACC] & 0x70) +
			(RAM[IR] & 0x70);
		    if( (tempAdd & 0x0080) == 0x0080 ) carry6 = true;
		    tempAdd += (RAM[ACC] & 0x80) +
			(RAM[IR ] & 0x80);
		    if( (tempAdd & 0x0100) == 0x0100 ) carry7 = true;
		    RAM[ACC] = (unsigned char)(tempAdd & 0x00FF);
		    if( carry3 ) SetBit(RAM[PSW], AC);
		    else ClearBit(RAM[PSW], AC);
		    if( carry7 ) SetBit(RAM[PSW], CY_);
		    else ClearBit(RAM[PSW], CY_);
		    if( (carry6 && !carry7) || (!carry6 && carry7) )
			SetBit(RAM[PSW], OV);
		    else ClearBit(RAM[PSW], OV);
	            }
//********************************************************
void  ADDC_A_Ri(){
                    bool carry3 = false;
                    bool carry6 = false;
                    bool carry7 = false;
                    short tempAdd;
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("ADDC  A,@R%X ",(int)(IR & 0x01));

		    regNum = IR & 0x01;
		    tempAdd = (RAM[ACC] & 0x0F) +
			(RAM[RAM[GetRegisterBank()+regNum]] & 0x0F) +
			(char)GetBit(RAM[PSW], CY_);
		    if( (tempAdd & 0x0010) == 0x0010 ) carry3 = true;
		    tempAdd += (RAM[ACC] & 0x70) +
			(RAM[RAM[GetRegisterBank()+regNum]] & 0x70);
		    if( (tempAdd & 0x0080) == 0x0080 ) carry6 = true;
		    tempAdd += (RAM[ACC] & 0x80) +
			(RAM[RAM[GetRegisterBank()+regNum]] & 0x80);
		    if( (tempAdd & 0x0100) == 0x0100 ) carry7 = true;
		    RAM[ACC] = (unsigned char)(tempAdd & 0x00FF);
		    if( carry3 ) SetBit(RAM[PSW], AC);
		    else ClearBit(RAM[PSW], AC);
		    if( carry7 ) SetBit(RAM[PSW], CY_);
		    else ClearBit(RAM[PSW], CY_);
		    if( (carry6 && !carry7) || (!carry6 && carry7) )
			SetBit(RAM[PSW], OV);
		    else ClearBit(RAM[PSW], OV);
}
//************************************************************
void  ADDC_A_dana(){
                    bool carry3 = false;
                    bool carry6 = false;
                    bool carry7 = false;
                    short tempAdd;
                    unsigned char  IR=ROM[++PC];
                    ins_print("ADDC  A,#%X",(int)(IR));

	            tempAdd = (RAM[ACC] & 0x0F) +
			(IR & 0x0F) + GetBit(RAM[PSW], CY_);
		    if( (tempAdd & 0x0010) == 0x0010 ) carry3 = true;
		    tempAdd += ((RAM[ACC] & 0x70) + ((char)IR & 0x70));
		    if( (tempAdd & 0x0080) == 0x0080 ) carry6 = true;
		    tempAdd += ((RAM[ACC] & 0x80) + ((char)IR & 0x80));
		    if( (tempAdd & 0x0100) == 0x0100 ) carry7 = true;
		    RAM[ACC] = (unsigned char)(tempAdd & 0x00FF);
		    if( carry3 ) SetBit(RAM[PSW], AC);
		    else ClearBit(RAM[PSW], AC);
		    if( carry7 ) SetBit(RAM[PSW], CY_);
		    else ClearBit(RAM[PSW], CY_);
		    if( (carry6 && !carry7) || (!carry6 && carry7) )
			SetBit(RAM[PSW], OV);
		    else ClearBit(RAM[PSW], OV);
		    }
//**********************************************
void  SUBB_A_Rn(){
                    unsigned char regNum;
                    bool borrow3 = false;
                    bool borrow6 = false;
                    bool borrow7 = false;
                    unsigned char  IR=ROM[PC];
                    ins_print("SUBB  A,R%X",(int)(IR & 0x07));

		    if( (unsigned char)(RAM[ACC] & 0x0F) <
			(unsigned char)((RAM[GetRegisterBank()+regNum] & 0x0F) +
					(char)GetBit(RAM[PSW], CY_)) ) {
			borrow3 = true;
		    }
		    if( (unsigned char)(RAM[ACC] & 0x7F) <
			(unsigned char)((RAM[GetRegisterBank()+regNum] & 0x7F) +
					(char)GetBit(RAM[PSW], CY_)) ) {
			borrow6 = true;
		    }
		    if( (unsigned short)(unsigned char)RAM[ACC] <
			((unsigned short)(unsigned char)RAM[GetRegisterBank()+regNum] +
			 (unsigned short)GetBit(RAM[PSW], CY_)) ) {
			borrow7 = true;
		    }
	            RAM[ACC] = (unsigned short)(unsigned char)RAM[ACC] -
			((unsigned short)(unsigned char)RAM[GetRegisterBank()+regNum] +
			 (unsigned short)GetBit(RAM[PSW], CY_));
		    if( borrow3 ) SetBit(RAM[PSW], AC);
		    else ClearBit(RAM[PSW], AC);
		    if( borrow7 ) SetBit(RAM[PSW], CY_);
		    else ClearBit(RAM[PSW], CY_);
		    if( (borrow6 && !borrow7) || (!borrow6 && borrow7) )
			SetBit(RAM[PSW], OV);
		    else ClearBit(RAM[PSW], OV);
}
//***********************************************************
void  SUBB_A_adres(){
                    bool borrow3 = false;
                    bool borrow6 = false;
                    bool borrow7 = false;
                    unsigned char  IR=ROM[++PC];
                    ins_print("SUBB  A,%X",IR);

                    if( (unsigned char)(RAM[ACC] & 0x0F) <
			(unsigned char)((RAM[IR ]  & 0x0F) +
                          (char)GetBit(RAM[PSW], CY_)) ) {
			borrow3 = true;
		    }
		    if( (unsigned char)(RAM[ACC] & 0x7F) <
			(unsigned char)((RAM[IR ] & 0x7F) +
			 (char)GetBit(RAM[PSW], CY_)) ) {
			borrow6 = true;
		    }
		    if( (unsigned short)(unsigned char)RAM[ACC] <
			((unsigned short)(unsigned char)RAM[IR ] +
			 (unsigned short)GetBit(RAM[PSW], CY_)) ) {
			borrow7 = true;
		    }
		    RAM[ACC] = (unsigned short)(unsigned char)RAM[ACC] -
			((unsigned short)RAM[IR ] +
			 (unsigned short)GetBit(RAM[PSW], CY_));
		    if( borrow3 ) SetBit(RAM[PSW], AC);
		    else ClearBit(RAM[PSW], AC);
		    if( borrow7 ) SetBit(RAM[PSW], CY_);
		    else ClearBit(RAM[PSW], CY_);
		    if( (borrow6 && !borrow7) || (!borrow6 && borrow7) )
			SetBit(RAM[PSW], OV);
		    else ClearBit(RAM[PSW], OV);
		   }
//******************************************************
void  SUBB_A_Ri(){
                    unsigned char regNum;
                    bool borrow3 = false;
                    bool borrow6 = false;
                    bool borrow7 = false;
                    unsigned char  IR=ROM[PC];
                    ins_print("SUBB  A,@R%X ",(int)(IR & 0x01));

		    if( (unsigned char)(RAM[ACC] & 0x0F) <
			(unsigned char)((RAM[RAM[GetRegisterBank()+regNum]] & 0x0F) +
			 (char)GetBit(RAM[PSW], CY_)) ) {
			borrow3 = true;
		    }
		    if( (unsigned char)(RAM[ACC] & 0x7F) <
			(unsigned char)((RAM[RAM[GetRegisterBank()+regNum]] & 0x7F) +
			 (char)GetBit(RAM[PSW], CY_)) ) {
			borrow6 = true;
		    }
		    if( (unsigned short)(unsigned char)RAM[ACC] <
			((unsigned short)(unsigned char)RAM[RAM[GetRegisterBank()+regNum]] +
			 (unsigned short)GetBit(RAM[PSW], CY_)) ) {
			borrow7 = true;
		    }
		    RAM[ACC] = (unsigned short)(unsigned char)RAM[ACC] -
			((unsigned short)(unsigned char)RAM[RAM[GetRegisterBank()+regNum]] +
			 (unsigned short)GetBit(RAM[PSW], CY_));
		    if( borrow3 ) SetBit(RAM[PSW], AC);
		    else ClearBit(RAM[PSW], AC);
		    if( borrow7 ) SetBit(RAM[PSW], CY_);
		    else ClearBit(RAM[PSW], CY_);
		    if( (borrow6 && !borrow7) || (!borrow6 && borrow7) )
			SetBit(RAM[PSW], OV);
		    else ClearBit(RAM[PSW], OV);
		  }
//*********************************************************
void  SUBB_A_dana(){
                    bool borrow3 = false;
                    bool borrow6 = false;
                    bool borrow7 = false;
                    unsigned char  IR=ROM[++PC];
                    ins_print("SUBB  A,#%X",IR);


		    if( (unsigned char)(RAM[ACC] & 0x0F) <
			(unsigned char)((IR & 0x0F) + (char)GetBit(RAM[PSW], CY_)) ) {
			borrow3 = true;
		    }
		    if( (unsigned char)(RAM[ACC] & 0x7F) <
			(unsigned char)((IR & 0x7F) + (char)GetBit(RAM[PSW], CY_)) ) {
			borrow6 = true;
		    }
		    if( (unsigned short)(unsigned char)RAM[ACC] <
			((unsigned short)IR + (unsigned short)GetBit(RAM[PSW], CY_)) ) {
			borrow7 = true;
		    }
		    RAM[ACC] = (unsigned short)(unsigned char)RAM[ACC] -
			((unsigned short)IR + (unsigned short)GetBit(RAM[PSW], CY_));
		    if( borrow3 ) SetBit(RAM[PSW], AC);
		    else ClearBit(RAM[PSW], AC);
		    if( borrow7 ) SetBit(RAM[PSW], CY_);
		    else ClearBit(RAM[PSW], CY_);
		    if( (borrow6 && !borrow7) || (!borrow6 && borrow7) )
			SetBit(RAM[PSW], OV);
		    else ClearBit(RAM[PSW], OV);
		   }
//*****************************************
void  INC_A(){
	            ins_print ("INC  A");
		    RAM[ACC]++;
}
//****************************************
void  INC_Rn(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("INC  R%X",(int)(IR & 0x07));
		    regNum = IR & 0x07;
		    RAM[GetRegisterBank()+regNum]++;
}
//*************************************
void  INC_adres(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("INC  %X",IR);
		    RAM[IR ]++;
}
//*******************************************
void  INC_Ri(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("INC  @R%X",(int)(IR & 0x01));
		    regNum = IR & 0x01;
		    RAM[RAM[GetRegisterBank()+regNum]]++;
}
//***************************************
void  INC_DPTR(){
                    unsigned short tempDPTR;
	            ins_print("INC  DPTR");
		    ((unsigned char*)&tempDPTR)[1] = RAM[DPH];
		    ((unsigned char*)&tempDPTR)[0] = RAM[DPL];
		    tempDPTR++;
		    RAM[DPH] = ((unsigned char*)&tempDPTR)[1];
		    RAM[DPL] = ((unsigned char*)&tempDPTR)[0];
}
//*******************************************
void  DEC_A(){
                    ins_print("DEC  A");
                    RAM[ACC]--;
}
//********************************************
void  DEC_Rn(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("DEC  R%X",(unsigned int)(IR & 0x07));
	            regNum = IR & 0x07;
		    RAM[GetRegisterBank()+regNum]--;
}
//*********************************************
void  DEC_adres(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("DEC  %X",(unsigned int)(IR));
		    RAM[IR]--;
                  }
//**********************************************
void  DEC_Ri(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("DEC  @R%X",(int)(IR & 0x01));
		    RAM[RAM[GetRegisterBank()+regNum]]--;
}
//************************************************
void  MUL_AB(){
                    unsigned short tempProduct = 0;
                    ins_print("MUL  AB");
		    tempProduct = (unsigned char)RAM[ACC] *
			(unsigned char)RAM[B];
		    if( tempProduct > 255 ) {
			SetBit(RAM[PSW], OV);
		    }
		    else {
			ClearBit(RAM[PSW], OV);
		    }
		    ClearBit(RAM[PSW], CY_);
		    RAM[ACC] = ((unsigned char*)&tempProduct)[0];
		    RAM[B] = ((unsigned char*)&tempProduct)[1];

}
//**********************************************
void  DIV_AB(){
                    unsigned char tempACC;
                    ins_print("DIV  AB");
		    if( RAM[B] == 0x00 ) {
			SetBit(RAM[PSW], OV);
		    }
		    else {
			ClearBit(RAM[PSW], OV);
			tempACC = RAM[ACC];
			RAM[ACC] = tempACC/RAM[B];
			RAM[B] = tempACC%RAM[B];
		    }
		    ClearBit(RAM[PSW], CY_);
                  }
//****************************************
void  DA_A(){
                    short tempAdd;
                    ins_print("DA  A");

                    if( (RAM[ACC] & 0x0F) > 9 ||
			GetBit(RAM[PSW], AC) == 0x01 ) {
			tempAdd = RAM[ACC] + 0x06;
			RAM[ACC] = (char)tempAdd;
			if( ((unsigned char*)&tempAdd)[1] != 0 ) {
			    SetBit(RAM[PSW], CY_);
			}
		    }
		    if( ((RAM[ACC] & 0xF0) >> 4) > 9 ||
			GetBit(RAM[PSW], CY_) == 0x01 ) {
			tempAdd = RAM[ACC] + 0x60;
			RAM[ACC] = (char)tempAdd;
			if( ((unsigned char*)&tempAdd)[1] != 0 ) {
			    SetBit(RAM[PSW], CY_);
			}
		    }
           }
//******************************************************
                              /*Operacje Logiczne*/
//******************************************************
void  ANL_A_Rn(){		//(A) <- (A) & (Rn)
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("ANL  A,R%X",(int)(IR & 0x07));
		    regNum = IR & 0x07;
		    RAM[ACC] &= RAM[GetRegisterBank()+regNum];
                }
//***********************************************************
void  ANL_A_adres(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("ANL  A,%X",( int)(IR));
		    RAM[ACC] &= RAM[IR < 128 ? IR : (IR+128)];
                  }
//*********************************************************
void  ANL_A_Ri(){
                    unsigned char  regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("ANL  A,@R%X",(int)(IR & 0x01));
                    regNum = IR & 0x01;
		    RAM[ACC] &= RAM[RAM[GetRegisterBank()+regNum]];
		    }
//**********************************************************
void  ANL_A_dana(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("ANL  A,#%X",(int)(IR));
		    RAM[ACC] &= (char)IR;
}
//*********************************************************
void  ANL_adres_A(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("ANL  %X,A",(int)(IR));
		    RAM[IR] &= RAM[ACC];
}
//**********************************************************
void  ANL_adres_dana(){
                    unsigned char  IR;
                    unsigned char directAddr = 0;
                    directAddr = ROM[++PC];
                    IR = ROM[++PC];
                    ins_print("ANL  %X,#%X",(int)(directAddr),(int)(IR) );
		    RAM[ directAddr ] &=    (char)IR;
      }
//-----------------------          *****************************-
void  ORL_A_Rn(){
                    unsigned char     regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("ORL  A,R%X",(int)(IR & 0x07));
		    regNum = IR & 0x07;
		    RAM[ACC] |= RAM[GetRegisterBank()+regNum];
}
//********************************************************
void  ORL_A_adres(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("ORL  A,%X",(int)(IR));
     	            RAM[ACC] |= RAM[IR];
		   }
//********************************************************
void  ORL_A_Ri(){
                    unsigned char      regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("ORL  A,@R%X",(int)(IR & 0x01));
                    regNum = IR & 0x01;
		    RAM[ACC] |= RAM[RAM[GetRegisterBank()+regNum]];
		    }
//*******************************************************
void  ORL_A_dana(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("ORL  A,#%X",(int)(IR));
		    RAM[ACC] |= (char)IR;
}
//******************************************************
void  ORL_adres_A(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("ORL  %X,A",(int)(IR));
	            RAM[IR < 128 ? IR : (IR+128)] |= RAM[ACC];
}
//****************************************************
void  ORL_adres_dana(){
                    unsigned char directAddr = 0;
                    unsigned char  IR=ROM[PC];
                    directAddr = ROM[++PC];
		    IR = ROM[++PC];
                    ins_print("ORL  %X,#%X",directAddr,IR);
                    RAM[directAddr ] |= (char)IR;
}
//**********************************************
void  XRL_A_Rn(){
                    unsigned char  IR=ROM[PC];
                    unsigned char regNum;
                    ins_print("XRL  A,R%X",(int)(IR & 0x07));
		    regNum = IR & 0x07;
		    RAM[ACC] ^= RAM[GetRegisterBank()+regNum];
}
//*****************************************
void  XRL_A_adres(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("XRL  A,%X",(int)(IR));
	            RAM[ACC] ^= RAM[IR < 128 ? IR : (IR+128)];
}
//****************************************
void  XRL_A_Ri(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("XRL  A,@R%X",(int)(IR & 0x01));
                    regNum = IR & 0x01;
		    RAM[ACC] ^= RAM[RAM[GetRegisterBank()+regNum]];
}
//*********************************
void  XRL_A_dana(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("XRL  A,#%X",(int)(IR));
	            RAM[ACC] ^= (char)IR;
}
//****************************************************
void  XRL_adres_A(){
                    unsigned char  IR=ROM[PC];
                    ins_print("XRL  %X,A",(int)(IR));
	            RAM[IR ] ^= RAM[ACC];
}
//**************************************************
void  XRL_adres_dana(){
                    unsigned char directAddr = 0;
                    unsigned char  IR=ROM[PC];
                    directAddr = ROM[++PC];
		    IR = ROM[++PC];
                    ins_print("XRL  %X,#%X",directAddr,IR);
	            RAM[directAddr]^= (char)IR;
}
//***************************************************
void  CLR_A(){
                    ins_print("CLR  A");
                    RAM[ACC] = 0x00;
}
//***************************************************
void  CPL_A(){
                    ins_print("CPL  A");
	            RAM[ACC] = ~RAM[ACC];
}
//**************************************************
void  RL_A(){
                    unsigned char rotateBit;
                    ins_print("RL  A");
                    rotateBit = GetBit(RAM[ACC], 7);
		    RAM[ACC] = (unsigned char)RAM[ACC] << 1;
		    if( rotateBit == 0x01 ) {
			SetBit(RAM[ACC], 0);
		    }
		    else {
			ClearBit(RAM[ACC], 0);
		    }
}
//***************************************************
void  RLC_A(){
                    unsigned char rotateBit;
                    ins_print("RLC  A");
                    rotateBit = GetBit(RAM[ACC], 7);
		    RAM[ACC] = (unsigned char)RAM[ACC] << 1;
		    if( GetBit(RAM[PSW], CY_) == 0x01 ) {
			SetBit(RAM[ACC], 0);
		    }
		    else {
			ClearBit(RAM[ACC], 0);
		    }
		    if( rotateBit == 0x01 ) {
			SetBit(RAM[PSW], CY_);
		    }
		    else {
			ClearBit(RAM[PSW], CY_);
		    }
}
//****************************************************
void  RR_A(){
                    unsigned char rotateBit;
                    ins_print("RR  A");
                    rotateBit = GetBit(RAM[ACC], 0);
		    RAM[ACC] = (unsigned char)RAM[ACC] >> 1;
		    if( rotateBit == 0x01 ) {
			SetBit(RAM[ACC], 7);
		    }
		    else {
			ClearBit(RAM[ACC], 7);
		    }
}
//***************************************************
void  RRC_A(){
                    unsigned char rotateBit;
                    ins_print("RRC  A");
                    rotateBit = GetBit(RAM[ACC], 0);
		    RAM[ACC] = (unsigned char)RAM[ACC] >> 1;
		    if( GetBit(RAM[PSW], CY_) == 0x01 ) {
			SetBit(RAM[ACC], 7);
		    }
		    else {
			ClearBit(RAM[ACC], 7);
		    }
		    if( rotateBit == 0x01 ) {
			SetBit(RAM[PSW], CY_);
		    }
		    else {
			ClearBit(RAM[PSW], CY_);
		    }
}
//**********************************************************
void  SWAP_A(){
                    unsigned char lowerNibble;
                    ins_print("SWAP  A");
                    lowerNibble = RAM[ACC] & 0x0F;
		    RAM[ACC] = (RAM[ACC] >> 4) & 0x0F;
		    RAM[ACC] |= ((lowerNibble << 4) & 0xF0);
}
                             /*Operacje przemieszczania danych*/
//*****************************************************************
void  MOV_A_Rn(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("MOV  A,R%X",(int)(IR & 0x07));
	            regNum = IR & 0x07;
		    RAM[ACC] = RAM[GetRegisterBank()+regNum];
}
//*******************************************************
void  MOV_A_adres(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("MOV  A,%X",(int)(IR));
	            RAM[ACC] = RAM[IR];
}
//*****************************************************
void  MOV_A_Ri(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("MOV  A,@R%X",(int)(IR & 0x01));
	            regNum = IR & 0x01;
		    RAM[ACC] = RAM[RAM[GetRegisterBank()+regNum]];
}
//*****************************************************
void  MOV_A_dana(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("MOV  A,#%X",(int)(IR));
                    RAM[ACC] = (char)IR;
}
//*********************************************************
void  MOV_Rn_A(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("MOV  R%X,A",(int)(IR & 0x07));
		    regNum = IR & 0x07;
		    RAM[GetRegisterBank()+regNum] = RAM[ACC];
}
//********************************************************
void  MOV_Rn_adres(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
		    regNum = IR & 0x07;
		    IR = ROM[++PC];
                    ins_print("MOV  R%X,%X",(int)(regNum),(int)(IR) );
           	    RAM[GetRegisterBank()+regNum]= RAM[IR];
}
//*****************************************************
void  MOV_Rn_dana(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
	            regNum = IR & 0x07;
		    IR = ROM[++PC];
                    ins_print("MOV  R%X,#%X",(int)(regNum),(int)(IR));
		    RAM[GetRegisterBank()+regNum] = (char)IR;
}
//*****************************************************
void  MOV_adres_A(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("MOV  %X,A",(int)(IR));
		    RAM[IR] = RAM[ACC];
}
//****************************************************
void  MOV_adres_Rn(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    regNum = IR & 0x07;
		    IR = ROM[++PC];
                    ins_print("MOV  %X,R%X",(int)(IR),(int)(regNum));
		    RAM[IR] = RAM[GetRegisterBank()+regNum];
}
//****************************************************
void  MOV_adres_adres(){
                    unsigned char directAddr = 0;
                    unsigned char  IR=ROM[PC];
		    directAddr = ROM[++PC];
		    IR = ROM[++PC];
                    ins_print("MOV  %X,%X",IR,directAddr);
		    RAM[IR] = RAM[ directAddr ];
}
//******************************************************
void  MOV_adres_Ri(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    regNum = IR & 0x01;
		    IR = ROM[++PC];
                    ins_print("MOV  %X,@R%X",IR,regNum);
                    RAM[IR] =RAM[RAM[GetRegisterBank()+regNum]];
}
//******************************************************
void  MOV_adres_dana(){
                    unsigned char directAddr = 0;
                    unsigned char  IR=ROM[PC];
                    directAddr = ROM[++PC];
		            IR = ROM[++PC];
                    ins_print("MOV  %X,#%X",(int)(directAddr),(int)(IR));
		    RAM[directAddr] = (char)IR;
}
//*****************************************************
void  MOV_Ri_A(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    ins_print("MOV  @R%X,A",(int)(IR & 0x01));
		    regNum = IR & 0x01;
		    RAM[RAM[GetRegisterBank()+regNum]] = RAM[ACC];
}
//****************************************************
void  MOV_Ri_adres(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    regNum = IR & 0x01;
		    IR = ROM[++PC];
                    ins_print("MOV  @R%X,%X",(int)(regNum),(int)(IR));
		    RAM[RAM[GetRegisterBank()+regNum]] = RAM[IR];
}
//*****************************************************
void  MOV_Ri_dana(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
		    regNum = IR & 0x01;
		    IR = ROM[++PC];;
                    ins_print("MOV  @R%X,#%X",(int)(regNum),(int)(IR));
		    RAM[RAM[GetRegisterBank()+regNum]] = (char)IR;
}
//*******************************************************
void  MOV_DPTR_dana(){

		    RAM[DPH] = ROM[++PC];
		    RAM[DPL] = ROM[++PC];
                    ins_print("MOV  DPTR,# %X %X",(unsigned char)(RAM[DPH]),(unsigned char) (RAM[DPL]));
                    }
//******************************************
void  MOVX_A_Ri(){
                    unsigned char      regNum;
                    unsigned char  IR=ROM[PC];
                    regNum = IR & 0x01;
                    ins_print("MOVX  A,@R%X",(int)(regNum));
		    RAM[ACC] = XRAM[RAM[GetRegisterBank()+regNum]];
                }
//******************************************
void  MOVX_A_DPTR(){
                    unsigned short tempDPTR;
	            ((unsigned char*)&tempDPTR)[1] = RAM[DPH];
	            ((unsigned char*)&tempDPTR)[0] = RAM[DPL];
                    ins_print("MOVX  A,@DPTR");
	            RAM[ACC] = XRAM[tempDPTR];
}
//*****************************************
void  MOVX_Ri_A(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    regNum = IR & 0x01;
                    ins_print("MOVX  @R%X,A",(int)(regNum));
		    XRAM[RAM[GetRegisterBank()+regNum]] = RAM[ACC];
}
//****************************************
void  MOVX_DPTR_A(){
                    unsigned short tempDPTR;
	            ((unsigned char*)&tempDPTR)[1] = RAM[DPH];
		    ((unsigned char*)&tempDPTR)[0] = RAM[DPL];
                    ins_print("MOVX  @DPTR,A");
		    XRAM[tempDPTR] = RAM[ACC];
}
//*****************************************
void  MOVC_A_A_DPTR(){
                    unsigned short tempDPTR;
		    ((unsigned char*)&tempDPTR)[1] = RAM[DPH];
		    ((unsigned char*)&tempDPTR)[0] = RAM[DPL];
                    ins_print("MOVC  A,@A+DPTR");
		    RAM[ACC] = ROM[(unsigned short)(unsigned char)RAM[ACC]+tempDPTR];
}
//*******************************************
void  MOVC_A_A_PC(){
                    ins_print("MOVC  A,@A+PC");
                    RAM[ACC] = ROM[(unsigned char)RAM[ACC]+PC];
}
//***************************************
void  XCH_A_Rn(){
                    unsigned char regNum;
                    char temp;
                    unsigned char  IR=ROM[PC];
                    regNum = IR & 0x07;
                    ins_print("XCH  A,R%X",(int)(regNum));
		    temp = RAM[ACC];
		    RAM[ACC] = RAM[GetRegisterBank()+regNum];
		    RAM[GetRegisterBank()+regNum] = temp;
}
//************************************
void  XCH_A_adres(){
                    char temp;
                    unsigned char  IR=ROM[++PC];
                    ins_print("XCH  A,%X",(int)(IR));
                    temp = RAM[ACC];
		    RAM[ACC] = RAM[IR ];
		    RAM[IR ] = temp;
}
//*******************************************
void  XCH_A_Ri(){
                    char temp;
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    regNum = IR & 0x01;
                    ins_print("XCH  A,@R%X",(int)(regNum));
		    temp = RAM[ACC];
		    RAM[ACC] = RAM[RAM[GetRegisterBank()+regNum]];
		    RAM[RAM[GetRegisterBank()+regNum]] = temp;
}
//*******************************************
void  XCHD_A_Ri(){
                    char temp;
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    regNum = IR & 0x01;
                    ins_print("XCHD  A,@R%X",(int)(regNum));
		    temp = RAM[ACC] & 0x0F;
		    RAM[ACC] = (RAM[ACC] & 0xF0) |
			(RAM[RAM[GetRegisterBank()+regNum]] & 0x0F);
		    RAM[RAM[GetRegisterBank()+regNum]] =
			(RAM[RAM[GetRegisterBank()+regNum]] & 0xF0 ) | temp;
}
//*********************************************************
void  PUSH_adres(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("PUSH  %X",(int)(IR));
                    RAM[SP] = (unsigned char)RAM[SP] + 1;
		    RAM[(unsigned char)RAM[SP]] = RAM[IR];
}
//*******************************************************
void  POP_adres(){
                    char popData;
                    unsigned char  IR=ROM[++PC];
                    ins_print("POP  %X",(int)(IR));
		    popData = RAM[(unsigned char)RAM[SP]];
		    RAM[SP] = (unsigned char)RAM[SP] - 1;
		    RAM[IR] = popData;
}
//***************************************************
                                 /*Operacje na bitach*/
void  CLR_C(){
		    ins_print("CLR  C");
		    ClearBit(RAM[PSW], CY_);
}
//***************************************
void  CLR_bit(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("CLR  %s",sfrbits[IR].mcode);
		    ClearBit(RAM[((IR & 0xF8) < 128) ?
				(((IR & 0xF8)>>3)+32) :
				(128 + (IR & 0xF8))], (IR & 0x07));
		    }
//**********************************************
void  SETB_C(){
                    ins_print("SETB  C");
                    SetBit(RAM[PSW], CY_);
}
//***********************************************
void  SETB_bit(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("SETB  %s",sfrbits[IR].mcode);
		    SetBit(RAM[((IR & 0xF8) < 128) ?
			      (((IR & 0xF8)>>3)+32) :
			      (128 + (IR & 0xF8))], (IR & 0x07));
}
//***************************************
void  CPL_C(){
                    ins_print("CPL  C");
                    if( GetBit(RAM[PSW], CY_) == 0x01 ) {
			ClearBit(RAM[PSW], CY_);
		    }
		    else {
			SetBit(RAM[PSW], CY_);
		    }
}
//****************************************
void  CPL_bit(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("CPL  %s",sfrbits[IR].mcode);
		    if( GetBit(RAM[((IR & 0xF8) < 128) ?
				  (((IR & 0xF8)>>3)+32) :
				  (128 + (IR & 0xF8))], (IR & 0x07)) == 0x01 ) {
			ClearBit(RAM[((IR & 0xF8) < 128) ?
				    (((IR & 0xF8)>>3)+32) :
				    (128 + (IR & 0xF8))], (IR & 0x07));
		    }
		    else {
			SetBit(RAM[((IR & 0xF8) < 128) ?
				  (((IR & 0xF8)>>3)+32) :
				  (128 + (IR & 0xF8))], (IR & 0x07));
		    }
}
//*******************************************
void  ANL_C_bit(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("ANL  C,%s",sfrbits[IR].mcode);
		    if( !GetBit(RAM[((IR & 0xF8) < 128) ?
				    (((IR & 0xF8)>>3)+32) :
				    (128 + (IR & 0xF8))], (IR & 0x07)) ) {
			ClearBit(RAM[PSW], CY_);
		    }
		  }
//*******************************************
void  ANL_C_not_bit(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("ANL  C,/ %s",sfrbits[IR].mcode);
		    if( GetBit(RAM[((IR & 0xF8) < 128) ?
				    (((IR & 0xF8)>>3)+32) :
				    (128 + (IR & 0xF8))], (IR & 0x07)) ) {
			ClearBit(RAM[PSW], CY_);
		    }
}
//************************************
void  ORL_C_bit(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("ORL  C,%s",sfrbits[IR].mcode);
                    if( GetBit(RAM[((IR & 0xF8) < 128) ?
				    (((IR & 0xF8)>>3)+32) :
				    (128 + (IR & 0xF8))], (IR & 0x07)) ) {
			SetBit(RAM[PSW], CY_);
		    }
}
//******************************************
void  ORL_C_not_bit(){
                    unsigned char  IR=ROM[++PC];
		    ins_print("ORL  C,/%s",sfrbits[IR].mcode);
		    if( !GetBit(RAM[((IR & 0xF8) < 128) ?
				    (((IR & 0xF8)>>3)+32) :
				    (128 + (IR & 0xF8))], (IR & 0x07)) ) {
			SetBit(RAM[PSW], CY_);
		    }
}
//*********************************************
void  MOV_C_bit(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("MOV  C,%s",sfrbits[IR].mcode);
		    if( GetBit(RAM[((IR & 0xF8) < 128) ?
				  (((IR & 0xF8)>>3)+32) :
				  (128 + (IR & 0xF8))], (IR & 0x07)) == 0x01 ) {
			SetBit(RAM[PSW], CY_);
		    }
		    else {
			ClearBit(RAM[PSW], CY_);
		    }
}
//**********************************************
void  MOV_bit_C(){
                    unsigned char  IR=ROM[++PC];

                    ins_print("MOV  %s,C",sfrbits[IR].mcode);
		    if( GetBit(RAM[PSW], CY_) == 0x01 ) {
			SetBit(RAM[((IR & 0xF8) < 128) ?
				  (((IR & 0xF8)>>3)+32) :
				  (128 + (IR & 0xF8))], (IR & 0x07));
		    }
		    else {
			ClearBit(RAM[((IR & 0xF8) < 128) ?
				    (((IR & 0xF8)>>3)+32) :
				    (128 + (IR & 0xF8))], (IR & 0x07));
		    }
}
//*****************************************
void  JC_ofset(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("JC  %X",(int) (PC + (char)IR));
		    if( GetBit(RAM[PSW], CY_) == 0x01 )
			PC += (char)IR;

}
//****************************
void  JNC_ofset(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("JNC  %X",(int) (PC + (char)IR));
	            if( GetBit(RAM[PSW], CY_) == 0x00 )
			PC += (char)IR;
		 }
//******************************
void  JB_bit_ofset(){
                    unsigned char  IR=ROM[++PC];
		    ins_print("JB  %s,%X",sfrbits[IR].mcode,
                                  (int)(PC+(char)(ROM[PC+1])));
		    if( GetBit(RAM[((IR & 0xF8) < 128) ?
				  (((IR & 0xF8)>>3)+32) :
				  (128 + (IR & 0xF8))], (IR & 0x07)) == 0x01 ) {
			IR = ROM[++PC];
			PC += (char)IR;
		    }
                    }
//***********************
void  JNB_bit_ofset(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("JNB  %s,%X",sfrbits[IR].mcode,
                                  (int)(PC+(char)(ROM[PC+1])));
		    if( GetBit(RAM[((IR & 0xF8) < 128) ?
				  (((IR & 0xF8)>>3)+32) :
				  (128 + (IR & 0xF8))], (IR & 0x07)) != 0x01 ) {
			IR = ROM[++PC];
			PC += (char)IR;
		    }

}
//********************************************
void  JBC_bit_ofset(){
                    unsigned char  IR=ROM[++PC];
                     ins_print("JBC  %s,%X",sfrbits[IR].mcode,
                                  (int)(PC+(char)(ROM[PC+1])));
		    if( GetBit(RAM[((IR & 0xF8) < 128) ?
				  (((IR & 0xF8)>>3)+32) :
				  (128 + (IR & 0xF8))], (IR & 0x07)) == 0x01 ) {
			ClearBit(RAM[((IR & 0xF8) < 128) ?
				    (((IR & 0xF8)>>3)+32) :
				    (128 + (IR & 0xF8))], (IR & 0x07));
		     	IR = ROM[++PC];
			PC += (char)IR;
		    }
}
//*******************************************************
                        /*   Skoki   */
//*******************************************************
void  ACALL(){
            unsigned char  IR=ROM[PC];
            unsigned short jumpAddr=0;
	    ((unsigned char*)&jumpAddr)[0] = ROM[++PC];
		    ((unsigned char*)&jumpAddr)[1] = (PC & 0xF800) | ((IR & 0xE0) >> 5);
                    ins_print("ACALL  %X",jumpAddr);
		    RAM[SP] = (unsigned char)RAM[SP] + 1;
		    RAM[(unsigned char)RAM[SP]]  = ((unsigned char*)&PC)[0];
		    RAM[SP] = (unsigned char)RAM[SP] + 1;
		    RAM[(unsigned char)RAM[SP]]  = ((unsigned char*)&PC)[1];
		    PC = jumpAddr;
}
//***************************************************
void  LCALL(){
                    unsigned short jumpAddr;
		    ((unsigned char*)&jumpAddr)[1] = ROM[++PC];
		    ((unsigned char*)&jumpAddr)[0] = ROM[++PC];
                    ins_print("LCALL  %X",jumpAddr);
                    RAM[SP] = (unsigned char)RAM[SP] + 1;
		    RAM[(unsigned char)RAM[SP]]  = ((unsigned char*)&PC)[0];
		    RAM[SP] = (unsigned char)RAM[SP] + 1;
		    RAM[(unsigned char)RAM[SP]]  = ((unsigned char*)&PC)[1];
		    PC = jumpAddr-1;                                                    //sprawdziæ
}
//**********************************************
void  RET(){
		    ((unsigned char*)&PC)[1] = RAM[(unsigned char)RAM[SP]];
		    RAM[SP] = (unsigned char)RAM[SP] - 1;
		    ((unsigned char*)&PC)[0] = RAM[(unsigned char)RAM[SP]];
		    RAM[SP] = (unsigned char)RAM[SP] - 1;
		    ins_print("RET");
}
//**************************************************
void  RETI(){
                    ((unsigned char*)&PC)[1] = RAM[(unsigned char)RAM[SP]];
		    RAM[SP] = (unsigned char)RAM[SP] - 1;
		    ((unsigned char*)&PC)[0] = RAM[(unsigned char)RAM[SP]];
		    RAM[SP] = (unsigned char)RAM[SP] - 1;
		    ins_print("RETI");
}
//*******************************************************
void  AJMP(){
                  unsigned char  IR=ROM[PC];
                  unsigned short jumpAddr;
		  ((unsigned char*)&jumpAddr)[0] = ROM[++PC];
		  ((unsigned char*)&jumpAddr)[1] = (PC & 0xF800) | ((IR & 0xE0) >> 5);
                   ins_print("AJMP  %X",jumpAddr);
		    PC = jumpAddr;
		    }
//*********************************************
void  LJMP(){
                  unsigned short jumpAddr;
	          ((unsigned char*)&jumpAddr)[1] = ROM[++PC];
		  ((unsigned char*)&jumpAddr)[0] = ROM[++PC];
                  ins_print("LJMP  %X",jumpAddr);
                  PC = jumpAddr-1;
}
//*******************************************
void  SJMP(){
                  unsigned char  IR=ROM[++PC];
                  ins_print("SJMP  %X",PC +  ((char)(IR)+1));
                  PC += (char)IR;
}
//**********************************************
void  JMP(){
                  unsigned short tempDPTR;
	          ((unsigned char*)&tempDPTR)[1] = RAM[DPH];
		  ((unsigned char*)&tempDPTR)[0] = RAM[DPL];
                  tempDPTR += (unsigned char)RAM[ACC];
                  ins_print("JMP  %X",tempDPTR);
		  PC = (unsigned short)tempDPTR;
}
//**************************************************
void  JZ(){
                  unsigned char  IR=ROM[++PC];
                  ins_print("JZ  %X",PC+(char)IR);
		  if( RAM[ACC] == 0x00 )
			PC += (char)IR;
}
//**************************************************
void  JNZ(){
                    unsigned char  IR=ROM[++PC];
                    ins_print("JNZ  %X",PC+(char)IR);
	            if( RAM[ACC] != 0x00 )
	               PC += (char)IR;
}
//********************************************************
void  CJNE_A_adres(){
                    unsigned char directAddr = 0;
                    unsigned char  IR=ROM[++PC];
                    directAddr = IR;
                    ins_print("CJME  A,%X,%4d",(int)(directAddr),
                                     (PC+1) + (char)(ROM[PC+1]));
		    if( RAM[ACC] != RAM[IR ] ) {
			IR = ROM[++PC];
			PC += (char)IR;
		    }
		    else {
			++PC;
		    }
                    if( (unsigned char)RAM[ACC] <
			(unsigned char)RAM[directAddr < 128 ? directAddr:
					  (directAddr+128)] ) {
			SetBit(RAM[PSW], CY_);
		    }
		    else {
			ClearBit(RAM[PSW], CY_);
		    }
}
//*******************************************************
void  CJNE_A_dana(){
                    unsigned char directAddr = 0;
                    unsigned char  IR=ROM[++PC];
                    ins_print("CJME  A,%X,%4d",(int)(directAddr),
                                     (PC+1) + (char)(ROM[PC+1]));
		    directAddr = IR;
		    if( RAM[ACC] != (char)IR ) {
			IR = ROM[++PC];
			PC += (char)IR;
		    }
		    else {
			++PC;
		    }
		    if( (unsigned char)RAM[ACC] < (unsigned char)directAddr ) {
			SetBit(RAM[PSW], CY_);
		    }
		    else {
			ClearBit(RAM[PSW], CY_);
		    }
}
//**********************************************
void  CJNE_Rn_dana(){
                    unsigned char  directAddr = 0;
                    unsigned char  regNum;
                    unsigned char  IR=ROM[PC];
		    regNum = IR & 0x07;
		    IR = ROM[++PC];
		    directAddr = IR;
                  //  (PC+(char)ROM[PC+1]+2)
                    if( RAM[GetRegisterBank()+regNum] != (char)IR ) {
			IR = ROM[++PC];
			PC += (char)IR;
		    }
		    else {
			++PC;
		         }
                    ins_print("CJNE  R%X,#%X,%X",(int)(regNum),(int)(directAddr),
                               (int)((char)(ROM[PC])+PC) ) ;
		    if( (unsigned char)RAM[GetRegisterBank()+regNum] <
			(unsigned char)directAddr ) {
			SetBit(RAM[PSW], CY_);
		    }
		    else {
			ClearBit(RAM[PSW], CY_);
		    }
}
//**********************************************
void  CJNE_Ri_dana(){
                    unsigned char directAddr = 0;
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
		    regNum = IR & 0x01;
		    IR = ROM[++PC];
		    directAddr = IR;
		    if( RAM[RAM[GetRegisterBank()+regNum]] != (char)IR ) {
			IR = ROM[++PC];
			PC += (char)IR;
		    }
		    else {
			++PC;
		    }
                    ins_print("CJNE  @R%X,#%X,%X",(int)(regNum),(int)(directAddr),
                               (int)((char)(ROM[PC])+PC) ) ;
		    if( (unsigned char)RAM[RAM[GetRegisterBank()+regNum]] <
			(unsigned char)directAddr ) {
			SetBit(RAM[PSW], CY_);
		    }
		    else {
			ClearBit(RAM[PSW], CY_);
		    }
}
//*********************************************
void  DJNZ_Rn_ofset(){
                    unsigned char regNum;
                    unsigned char  IR=ROM[PC];
                    
		    regNum = IR & 0x07;
		    RAM[GetRegisterBank()+regNum] =
			(unsigned char)RAM[GetRegisterBank()+regNum] - 1;
		    if( RAM[GetRegisterBank()+regNum] != 0x00 ) {
			IR = ROM[++PC];
			PC += (char)IR;
		    }
		    else {
			++PC;
		    }
                    ins_print("DJNZ  RN ofset :) ");
                    }
//*****************************************
void  DJNZ_adres_ofset(){
                    unsigned char  tmp=ROM[++PC];
                    unsigned short tmp_PC=PC;
                    unsigned char  IR=ROM[++PC];

		    RAM[IR] =	(unsigned char)RAM[IR] - 1;
                    ++PC;
		    if( RAM[IR] != 0x00 ) {
			IR = ROM[PC];
			PC += (char)IR;
		         }
		    ins_print("DJNZ  %02X,%04X",tmp,(tmp_PC += (char)IR));
}
//*********************************************
void  NOP(){
                    ins_print("NOP ");
		    // no code
}
//------------------------------------------------------------------------------
void i8051::HandleTimers(int ticks)
{
  int prio;
  int ticks1;
  int intReq;
//  int  exTick0;
//  int  exTick1;
  int extTick0;
  int extTick1;

  if(RAM_[TCON]&0x50) //TR0 oder TR1 aktywne
  {
    ticks1=ticks;
    if(RAM_[TCON]&0x10 && ((RAM_[TMOD]&0x08)==0 || (RAM_[P_3]&0x02)!=0)) //Timer0 enabled
    {
      switch(RAM_[TMOD]&0x07)  //RAM_[0x89]&0x07
      {
        case 0x00: //Mode 0 - intern getaktet
          while(ticks--)
          {
            RAM_[TL0]++;
            if(RAM_[TL0]>0x1F)
            {
              RAM_[TL0]=0;
              RAM_[TH0]++;
            }
            if((RAM_[TL0]|RAM_[TH0])==0)  //Überlauf
            {
              prio=RAM_[IP0];
              if(prio & 0x02) //hohe Priorität
                intReq |= 0x0002;
              else
                intReq |= 0x0200;
              RAM_[TCON]|=0x20;  //Interruptbit TF0 setzen
              RAM_[TL0]+=ticks;
              goto handleT1;
            }
          }
          break;
        case 0x04: //Mode0 - extern
          if(extTick0)
          {
            extTick0=FALSE;
            RAM_[TL0]++;
            if(RAM_[TL0]>0x1F)
            {
              RAM_[TL0]=0;
              RAM_[TH0]++;
            }
            if((RAM_[TL0]|RAM_[TH0])==0)  //Überlauf
            {
              prio=RAM_[IP0];
              if(prio & 0x02) //hohe Priorität
                intReq |= 0x0002;
              else
                intReq |= 0x0200;
              RAM_[TCON]|=0x20;  //Interruptbit TF0 setzen
              goto handleT1;
            }
          }
          break;
        case 0x01: //Mode1 - intern
          while(ticks--)
          {
            RAM_[TL0]++;
            if(RAM_[TL0]==0)
              RAM_[TH0]++;
            if((RAM_[TL0]|RAM_[TH0])==0)  //Überlauf
            {
              prio=RAM_[IP0];
              if(prio & 0x02) //hohe Priorität
                intReq |= 0x0002;
              else
                intReq |= 0x0200;
              RAM_[TCON]|=0x20;  //Interruptbit TF0 setzen
              RAM_[TL0]+=ticks;
              goto handleT1;
            }
          }

          break;
        case 0x05: //Mode1 - extern
          if(extTick0)
          {
            extTick0=FALSE;
            RAM_[TL0]++;
            if(RAM_[TL0]==0)
              RAM_[TH0]++;
            if((RAM_[TL0]|RAM_[TH0])==0)  //Überlauf
            {
              prio=RAM_[IP0];
              if(prio & 0x02) //hohe Priorität
                intReq |= 0x0002;
              else
                intReq |= 0x0200;
              RAM_[TCON]|=0x20;  //Interruptbit TF0 setzen
              goto handleT1;
            }
          }

          break;
        case 0x02: //Mode2 - intern
          while(ticks--)
          {
            RAM_[TL0]++;   //RAM_[0x8A]
            if(RAM_[TL0]==0)
            {
              RAM_[TL0]=RAM_[TH0];   //TH0= RAM_[0x8C]
              prio=RAM_[IP0];
              if(prio & 0x02) //hohe Priorität
                intReq |= 0x0002;
              else
                intReq |= 0x0200;
              RAM_[TCON]|=0x20;  //Interruptbit TF0
              RAM_[TL0]+=ticks;
              goto handleT1;
            }
          }
          break;
        case 0x06: //Mode2 - extern
          if(extTick0)
          {
            extTick0=FALSE;
            RAM_[TL0]++;   //RAM_[0x8A]
            if(RAM_[TL0]==0)
            {
              RAM_[TL0]=RAM_[TH0];   //TH0= RAM_[0x8C]
              prio=RAM_[IP0];
              if(prio & 0x02) //hohe Priorität
                intReq |= 0x0002;
              else
                intReq |= 0x0200;
              RAM_[TCON]|=0x20;  //Interruptbit TF0
              goto handleT1;
            }
          }
          break;
        case 0x03: //Mode3 - intern
          while(ticks--)
          {
            RAM_[TL0]++;   //RAM_[0x8A]
            if(RAM_[TL0]==0)
            {
              prio=RAM_[IP0];
              if(prio & 0x02) //hohe Priorität
                intReq |= 0x0002;
              else
                intReq |= 0x0200;
              RAM_[TCON]|=0x20;  //Interruptbit TF0
            }
            if(RAM_[TCON]&0x40) //TR1
            {
              RAM_[TH0]++;   //RAM_[0x8C]
              if(RAM_[TH0]==0)
              {
                prio=RAM_[IP0];
                if(prio & 0x08) //hohe Priorität
                  intReq |= 0x0008;
                else
                  intReq |= 0x0800;
                RAM_[TCON]|=0x80;  //Interruptbit TF1
              }
            }
          }

          break;
        case 0x07: //Mode3 - extern
          break;

      }
    }
handleT1:
    if(RAM_[TCON]&0x40 && ((RAM_[TMOD]&0x80)==0 || (RAM_[PORT3]&0x04)!=0)) //Timer1 enabled
    {
      switch(RAM_[TMOD]&0x70)  //RAM_[0x89]&0x80     //TCON= RAM_[0x88] //PORT3=0xB0
      {
        case 0x00: //Mode 0 - intern getaktet
          while(ticks1--)
          {
            RAM_[TL1]++;
            if(RAM_[TL1]>0x1F)
            {
              RAM_[TL1]=0;
              RAM_[TH1]++;
            }
            if((RAM_[TL1]|RAM_[TH1])==0)  //Überlauf
            {
              prio=RAM_[IP0];
              if(prio & 0x08) //hohe Priorität
                intReq |= 0x0008;
              else
                intReq |= 0x0800;
              RAM_[TCON]|=0x80;  //Interruptbit TF1 setzen
              RAM_[TL1]+=ticks1;
              return;
            }
          }
          break;
        case 0x40: //Mode0 - extern
          if(extTick1)
          {
            extTick1=FALSE;
            RAM_[TL1]++;
            if(RAM_[TL1]>0x1F)
            {
              RAM_[TL1]=0;
              RAM_[TH1]++;
            }
            if((RAM_[TL1]|RAM_[TH1])==0)  //Überlauf
            {
              prio=RAM_[IP0];
              if(prio & 0x08) //hohe Priorität
                intReq |= 0x0008;
              else
                intReq |= 0x0800;
              RAM_[TCON]|=0x80;  //Interruptbit TF1 setzen
              return;
            }
          }

          break;
        case 0x10: //Mode1 - intern
          while(ticks1--)
          {
            RAM_[TL1]++;
            if(RAM_[TL1]==0)
              RAM_[TH1]++;
            if((RAM_[TL1]|RAM_[TH1])==0)  //Überlauf
            {
              prio=RAM_[IP0];
              if(prio & 0x08) //hohe Priorität
                intReq |= 0x0008;
              else
                intReq |= 0x0800;
              RAM_[TCON]|=0x80;  //Interruptbit TF1 setzen
              RAM_[TL1]+=ticks1;
              return;
            }
          }

          break;
        case 0x50: //Mode1 - extern
          if(extTick1)
          {
            extTick1=FALSE;
            RAM_[TL1]++;
            if(RAM_[TL1]==0)
              RAM_[TH1]++;
            if((RAM_[TL1]|RAM_[TH1])==0)  //Überlauf
            {
              prio=RAM_[IP0];
              if(prio & 0x08) //hohe Priorität
                intReq |= 0x0008;
              else
                intReq |= 0x0800;
              RAM_[TCON]|=0x80;  //Interruptbit TF1 setzen
              return;
            }
          }

          break;
        case 0x20: //Mode2 - intern
          while(ticks1--)
          {
            RAM_[TL1]++;
            if(RAM_[TL1]==0)
            {
              RAM_[TL1]=RAM_[TH1];
              prio=RAM_[IP0];
              if(prio & 0x08) //hohe Priorität
                intReq |= 0x0008;
              else
                intReq |= 0x0800;
              RAM_[TCON]|=0x80;  //Interruptbit TF1
              RAM_[TL1]+=ticks1;
              return;
            }
          }
          break;
        case 0x60: //Mode2 - extern
          if(extTick1)
          {
            extTick1=FALSE;
            RAM_[TL1]++;   //RAM_[0x8A]
            if(RAM_[TL1]==0)
            {
              RAM_[TL1]=RAM_[TH1];   //TH0
              prio=RAM_[IP0];
              if(prio & 0x08) //hohe Priorität
                intReq |= 0x0008;
              else
                intReq |= 0x0800;
              RAM_[TCON]|=0x80;  //Interruptbit TF1
              return;
            }

          }
          break;
        case 0x30: //Mode3 - intern , Timer1 does nothing
          break;
        case 0x70: //Mode3 - extern
          break;

      }
    }

  }
}



//*********************************************
// Set single bit of the given byte at the given location.
void   SetBit(char &thisByte, unsigned char thisBit)
{
    thisByte |= (0x01 << thisBit);
}
//-----------------------------------------------------------------------------
// Clear single bit of the given byte at the given location.
void  ClearBit(char &thisByte, unsigned char thisBit)
{
    thisByte &= ~(0x01 << thisBit);
}
//-----------------------------------------------------------------------------
// Return value of bit as 0 or 1.

unsigned char GetBit(char thisByte, unsigned char thisBit)
{
    return( (thisByte & (0x01 << thisBit)) >> thisBit);
}
//-----------------------------------------------------------------------------
// Returns the integer value for the current register bank.
unsigned char GetRegisterBank()
{
    return 8 * ((RAM[PSW] & 0x18) >> 3);
}
//----------------------------------------------------------------------------
//          Printing the instruction to Forms
//----------------------------------------------------------------------------
int ins_print( char *fmt, ... )
{
  va_list  argptr;			/* Argument list pointer	*/
//  char str[140];			/* Buffer to build sting into	*/
  int cnt;				/* Result of SPRINTF for return */
  va_start( argptr, fmt );		/* Initialize va_ functions	*/
  cnt = vsprintf(execution_order, fmt, argptr );	/* prints string to buffer	*/
 //         cout<<"PC= "<<PC<<"    "<<execution_order<<endl;

 // Form3->Memo1->Lines->Add(execution_order);
  va_end( argptr );			/* Close va_ functions		*/

  return( cnt );			/* Return the conversion count	*/
}
////////////////////////////////////////////////////////////////////////////////
bool
i8051::Hex2Short(const char* buf, unsigned &val)
{
    int i;

    if( sscanf(buf, "%x", &i) != 1 ) {
        cerr << "Error: hex file error." << endl;
        return false;
    }
    val = i;
    return true;
}
//-------------------------------------------------------
bool
i8051::Load(const char* buf, unsigned char* rom, unsigned& prgSize)
{
    unsigned temp;
    unsigned len, base, type;
    unsigned char checksum = 0;
    char hex[16];

    if( buf[0] != ':' ) {
       ShowMessage("Error: hex file error.");
    //    cerr << "Error: hex file error." << endl;
        return false;
    }

    hex[0] = buf[1];
    hex[1] = buf[2];
    hex[2] = 0;
    if( !Hex2Short(hex, len) ) return false;

    hex[0] = buf[3];
    hex[1] = buf[4];
    hex[2] = buf[5];
    hex[3] = buf[6];
    hex[4] = 0;
    if( !Hex2Short(hex, base) ) return false;

    hex[0] = buf[7];
    hex[1] = buf[8];
    hex[2] = 0;
    if( !Hex2Short(hex, type) ) return false;

    if( type == 1 ) return true;

    if ( (base+len) > (prgSize) ) {
        (prgSize) = base + len + 2;
    }

    if( (prgSize) >= 65536 || (base+len) >= 65536 ) {
        printf("program too large\n");
        exit(1);
    }

    for(unsigned i=0; i<len; i++) {
        hex[0] = buf[ 9 + i * 2];
        hex[1] = buf[10 + i * 2];
	if( !Hex2Short(hex, temp) ) return false;
        rom[base + i] = (unsigned char)temp;
    }

    //-----------------------------------------------------------

    for(unsigned i=0; buf[i * 2 + 1] && buf[i * 2 + 2]; i++) {

        hex[0] = buf[i * 2 + 1];
        hex[1] = buf[i * 2 + 2];
	unsigned temp;
	if( !Hex2Short(hex, temp) ) return false;
        checksum += (unsigned char)temp;
    }

    if( checksum != 0 ) {
            ShowMessage("Error: checksum failed.");

      //  cerr << "Error: checksum failed." << endl;
        return false;
    }
    return false;
}

//-----------------------------------------------------------------------------

bool
i8051::LoadHexToRom(char *filename)
{
    unsigned prgSize = 0;
    char buf[256];
    char path[255];

    ifstream ifs(filename);
    memset(ROM_, 0, sizeof(char)*RomSize);

    if( ifs.bad() )
        {
	//cerr << "Error: invalid HEX file." << endl;
        ShowMessage("Error: invalid HEX file.");
	    return false;
        }
    while( !ifs.eof() )
        {
	    ifs.getline(buf, sizeof(buf));
	    if( Load(buf, (unsigned char*)ROM_, prgSize) ) break;
        }
    ifs.close();
    return true;
}
//******************************************************************************

bool i8051::LoadHexToRam(char *filename)
{
    unsigned prgSize = 0;
    char buf[256];

    ifstream ifs(filename);

    memset(XRAM_, 0, sizeof(char)*RamSize);

    if( ifs.bad() )
        {
	    //cerr << "Error: invalid HEX file." << endl;
        ShowMessage("Error: invalid HEX file.");
	    return false;
        }
    while( !ifs.eof() )
        {
	    ifs.getline(buf, sizeof(buf));
	    if( Load(buf, (unsigned char*)XRAM_, prgSize) ) break;
        }
    ifs.close();
    return true;
}
///////////////////////////////////////////////////////////////////////////////
void i8051::Step()
{
if(cycle<=0)
    {
    PC=PC_;
    ROM=ROM_;
    RAM=RAM_;
    XRAM=XRAM_;
   //AnsiString BB;
   //BB.printf("PC %X",PC);
   //Form3->Memo1->Lines->Add(BB);

    cycle=mnemtbl[(unsigned char)(ROM[PC_])].time_inst; //odczytanie ile cykli na dana instukcje przypada
    mnemtbl[(unsigned char)(ROM[PC_])].adres_funkcji() ;
    ++PC;
    PC_=PC;
    cycle--;
    }
else
    cycle--;
}
//******************************************************************************
void i8051::disassembler()
    {
    unsigned short PC_tmp=0;
    ROM=ROM_;
    RAM=RAM_;
    XRAM=XRAM_;
    PC_tmp=PC;
    mnemtbl[(unsigned char)(ROM[PC])].adres_funkcji() ;
    PC=PC_tmp+mnemtbl[(unsigned char)(ROM[PC_tmp])].size;
    }
//******************************************************************************

i8051::i8051()
{
for (int i=0;i<RamSize;i++)
      RAM_[i]=0;

PC_=0;
PC=PC_;
cycle=0;
ROM=ROM_;
RAM=RAM_;
XRAM=XRAM_;
Name="";
RAM[P_0]=0xff;
RAM[P_1]=0xff;
RAM[P_2]=0xff;
RAM[P_3]=0xff;
}


