;	Symulator MCS_51

  .org	0 
.EQU	DL1,	78h	
.EQU	DL2,	79h	   
.EQU  counter1,  77h
.EQU  counter2,  76h




        mov    p1,#00000011b ;wygaszenie obu wyswietlaczy        
	mov    Dl1,#0
	mov    Dl2,#0
	mov    DPTR,#TAB
        mov    counter1,#0 
	mov    counter2,#0
	clr A
	
stop:
	mov  A,counter1
	movc A,@A+DPTR
	mov  DL1,A      ;za³adowanie 1 cyfry

	mov  A,counter2
	movc A,@A+DPTR
	mov  DL2,A      ;za³adowanie 2 cyfry

        lcall  DELAY 
        
        inc  counter1
	mov  A,counter1
	cjne A,#9,NEXT_count
	mov  counter1,#0
	
	inc  counter2
	mov  A,counter2
	cjne A,#9,NEXT_count
	mov  counter2,#0
	
NEXT_count:
	
          			
 
 
       
   sjmp	stop		;petla nieskonczona


DELAY:
	mov    Acc,#0xffh
DDD2:
 
        mov    p1,#00000000b ;zgaszenie wyswietlaczy
        mov    P0,DL1;2 ;wystawienie wartosci
        mov    p1,#00000010b
	nop
	nop
	nop
	nop
	nop
	nop


        mov    p1,#00000000b ;zgaszenie wyswietlaczy
	mov    P0,DL2;2 ;wystawienie wartosci
        mov    p1,#00000001b
	nop
	nop
	nop
	nop
	nop
	nop
	
        dec Acc
        jnz DDD2



	ret
TAB:
 .DB	10000100b,10011111b,10000010b,10001010b,10011001b,11001000b
 .DB	11000000b,10011110b,10000000b,10001000b	

  

	.END
