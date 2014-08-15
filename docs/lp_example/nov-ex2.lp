\ Example hard coded

\ K is set to 10000

\ q1_a1 = 1
\ q1_a2 = 0
\ q2_a1 = 0
\ q2_a2 = 1

\ assume:
\ c_e(B) = 1
\ c_n(B) = 1
\ s(a1) = 8
\ s(a2) = 8
\ so 16 c_e(B) + 12 c_n(B) = 16 + 12 = 28

\ s(B) = 1 * (16 + 16) + (1 * 12) = 32 + 12 = 44
\ 1 - ((1 * 16)/44) = 0.64
\ alpha = 0.1

Minimize 
  28 y_p1_q1 + 28 y_p1_q2 + 28 y_p2_q1 + 28 y_p2_q2   
   + 8 z_a1_p1_q1 + 8 z_a2_p1_q1 
   + 8 z_a1_p1_q2 + 8 z_a2_p1_q2 
   + 8 z_a1_p2_q1 + 8 z_a2_p2_q1 
   + 8 z_a1_p2_q2 + 8 z_a2_p2_q2
Subject To 
  x_a1_p1 + x_a1_p2  = 1  
  x_a2_p1 + x_a2_p2  = 1
  1 x_a1_p1 + 0 x_a2_p1 - y_p1_q1 >= 0 
  0 x_a1_p1 + 1 x_a2_p1 - y_p1_q2 >= 0 
  1 x_a1_p2 + 0 x_a2_p2 - y_p2_q1 >= 0 
  0 x_a1_p2 + 1 x_a2_p2 - y_p2_q2 >= 0
  10000 y_p1_q1 - 1 x_a1_p1 - 0 x_a2_p1 >= 0
  10000 y_p1_q2 - 0 x_a1_p1 - 1 x_a2_p1 >= 0 
  10000 y_p2_q1 - 1 x_a1_p2 - 0 x_a2_p2 >= 0
  10000 y_p2_q2 - 0 x_a1_p2 - 1 x_a2_p2 >= 0
  z_a1_p1_q1 - x_a1_p1 - y_p1_q1 >= -1
  z_a1_p1_q2 - x_a1_p1 - y_p1_q2 >= -1
  z_a1_p2_q1 - x_a1_p2 - y_p2_q1 >= -1
  z_a1_p2_q2 - x_a1_p2 - y_p2_q2 >= -1
  z_a2_p1_q1 - x_a2_p1 - y_p1_q1 >= -1
  z_a2_p1_q2 - x_a2_p1 - y_p1_q2 >= -1
  z_a2_p2_q1 - x_a2_p2 - y_p2_q1 >= -1
  z_a2_p2_q2 - x_a2_p2 - y_p2_q2 >= -1
  x_a1_p1 + x_a2_p1 - u_p1 >= 0
  x_a1_p2 + x_a2_p2 - u_p2 >= 0
  10000 u_p1 - x_a1_p1 - x_a2_p1 >= 0
  10000 u_p2 - x_a1_p2 - x_a2_p2 >= 0
  u_p1 + u_p2 <= 1.16
Binary
  x_a1_p1 x_a2_p1 x_a1_p2 x_a2_p2 
  y_p1_q1 y_p1_q2 y_p2_q1 y_p2_q2 
  z_a1_p1_q1 z_a1_p1_q2 
  z_a1_p2_q1 z_a1_p2_q2 
  z_a2_p1_q1 z_a2_p1_q2 
  z_a2_p2_q1 z_a2_p2_q2  
  u_p1 u_p2 
End


