;	Symulator MCS_51
  .org	0 
.EQU	DL1,	78h	
.EQU  counter1,  77h
.EQU  port,      76h

      mov    Dl1,#0
      mov    counter1,#0 
      mov    port,#0
      mov    DPTR,#TAB

stop:
      mov    port,P1
      mov    A,counter1
      movc   A,@A+DPTR
      mov    P0,A;2 ;wystawienie wartosci
      inc    counter1
      mov    A,counter1
      cjne   A,#10,NEXT_count
      xrl    P2,#0xff	
      mov    counter1,#0
NEXT_count:
	
   mov  A,P1
   xrl  A,port
   jnz  stop	      			
 
   sjmp	NEXT_count		;petla nieskonczona

TAB:
 .DB	10000100b,10011111b,10100010b,10001010b,10011001b,11001000b
 .DB	11000000b,10011110b,10000000b,10001000b	

  

	.END
