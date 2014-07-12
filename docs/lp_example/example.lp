\ Example hard coded

\ This doesn't quite work:
\ 

\ K is set to 1000000
\ ? is set to 8

Minimize
   8 v_a1_p1_q1 + 8 v_a2_p1_q1 
   + 8 v_a1_p1_q2 + 8 v_a2_p1_q2 
   + 8 v_a1_p2_q1 + 8 v_a2_p2_q1 
   + 8 v_a1_p2_q2 + 8 v_a2_p2_q2 
   - 1000000 v_p1_q1
   - 1000000 v_p1_q2
   - 1000000 v_p2_q1
   - 1000000 v_p2_q2
Subject To 
  v_a1_q1 = 1  
  v_a2_q1 = 0
  v_a1_q2 = 0
  v_a2_q2 = 1
  v_a1_p1 + v_a1_p2  = 1  
  v_a2_p1 + v_a2_p2  = 1
  v_a1_p1_q1 - (v_a1_p1 + v_p1_q1) >= -1
  v_a1_p1_q2 - (v_a1_p1 + v_p1_q2) >= -1
  v_a1_p2_q1 - (v_a1_p2 + v_p2_q1) >=  -1
  v_a1_p2_q2 - (v_a1_p2 + v_p2_q2) >=  -1
  v_a2_p1_q1 - (v_a2_p1 + v_p1_q1) >= -1
  v_a2_p1_q2 - (v_a2_p1 + v_p1_q2) >= -1
  v_a2_p2_q1 - (v_a2_p2 + v_p2_q1) >= -1
  v_a2_p2_q2 - (v_a2_p2 + v_p2_q2) >= -1
  1 v_a1_p1 + 1 v_a2_q1 - v_p1_q1 >= 0  

Binary
  v_a1_p1 v_a2_p1 v_a1_p2 v_a2_p2 v_p1_q1 v_p1_q2 v_p2_q1 v_p2_q2 
  v_a1_p1_q1 v_a1_p1_q2 v_a1_p2_q1 v_a1_p2_q2
  v_a2_p1_q1 v_a2_p1_q2 v_a2_p2_q1 v_a2_p2_q2
End


