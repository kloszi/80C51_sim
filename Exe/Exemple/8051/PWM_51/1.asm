;	Symulator MCS_51
	
stop:
  mov P0,#00000000b
  mov P0,#00000000b

  mov P0,#00000001b
  mov P0,#00000001b

  mov P0,#00000011b
  mov P0,#00000011b

  mov P0,#00000111b
  mov P0,#00000111b

  mov P0,#00001111b        			
  mov P0,#00001111b        			

   sjmp	stop		;petla nieskonczona

.END
