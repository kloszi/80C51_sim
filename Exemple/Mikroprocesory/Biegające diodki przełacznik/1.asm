; Symulator MCS_51

  mov P0,#0x0h
  mov P1,#0x0h
  mov P2,#0x1h

	
stop:
  mov A,P0
  xrl A,#0x1h
  rr  A
  mov P0,A
  
  mov A,P2
  rl  A
  mov P2,A		  
   
  mov A,P1
  jz  stop
stop1:
  mov A,P1
  jz  stop
  sjmp	stop1	

  
  .END
