Minimize
  28 y_p1_q1 + 28 y_p1_q2 + 28 y_p2_q1 + 28 y_p2_q2 + 16 z_a1_p1_q1
   + 16 z_a1_p1_q2 + 16 z_a1_p2_q1 + 16 z_a1_p2_q2 + 16 z_a2_p1_q1
   + 16 z_a2_p1_q2 + 16 z_a2_p2_q1 + 16 z_a2_p2_q2
Subject To
 R0: x_a1_p1 + x_a1_p2 >= 1
 R1: x_a2_p1 + x_a2_p2 >= 1
 R2: z_a1_p1_q1 + z_a1_p2_q1 >= 1
 R3: z_a1_p1_q2 + z_a1_p2_q2 >= 0
 R4: z_a2_p1_q1 + z_a2_p2_q1 >= 0
 R5: z_a2_p1_q2 + z_a2_p2_q2 >= 1
 R6: x_a1_p1 - z_a1_p1_q1 >= 0
 R7: x_a1_p1 - z_a1_p1_q2 >= 0
 R8: x_a1_p2 - z_a1_p2_q1 >= 0
 R9: x_a1_p2 - z_a1_p2_q2 >= 0
 R10: x_a2_p1 - z_a2_p1_q1 >= 0
 R11: x_a2_p1 - z_a2_p1_q2 >= 0
 R12: x_a2_p2 - z_a2_p2_q1 >= 0
 R13: x_a2_p2 - z_a2_p2_q2 >= 0
 R14: - y_p1_q1 + z_a1_p1_q1 + z_a2_p1_q1 >= 0
 R15: - y_p1_q2 + z_a1_p1_q2 + z_a2_p1_q2 >= 0
 R16: - y_p2_q1 + z_a1_p2_q1 + z_a2_p2_q1 >= 0
 R17: - y_p2_q2 + z_a1_p2_q2 + z_a2_p2_q2 >= 0
 R18: 1000 y_p1_q1 - z_a1_p1_q1 - z_a2_p1_q1 >= 0
 R19: 1000 y_p1_q2 - z_a1_p1_q2 - z_a2_p1_q2 >= 0
 R20: 1000 y_p2_q1 - z_a1_p2_q1 - z_a2_p2_q1 >= 0
 R21: 1000 y_p2_q2 - z_a1_p2_q2 - z_a2_p2_q2 >= 0
 R22: - x_a1_p1 - y_p1_q1 + z_a1_p1_q1 >= -1
 R23: - x_a1_p1 - y_p1_q2 + z_a1_p1_q2 >= -1
 R24: - x_a1_p2 - y_p2_q1 + z_a1_p2_q1 >= -1
 R25: - x_a1_p2 - y_p2_q2 + z_a1_p2_q2 >= -1
 R26: - x_a2_p1 - y_p1_q1 + z_a2_p1_q1 >= -1
 R27: - x_a2_p1 - y_p1_q2 + z_a2_p1_q2 >= -1
 R28: - x_a2_p2 - y_p2_q1 + z_a2_p2_q1 >= -1
 R29: - x_a2_p2 - y_p2_q2 + z_a2_p2_q2 >= -1
 R30: x_a1_p1 + x_a2_p1 - u_p1 >= 0
 R31: x_a1_p2 + x_a2_p2 - u_p2 >= 0
 R32: - x_a1_p1 - x_a2_p1 + 1000 u_p1 >= 0
 R33: - x_a1_p2 - x_a2_p2 + 1000 u_p2 >= 0
 R34: - x_a1_p2 - u_p2 <= 3.78
Bounds
Binaries
 x_a1_p1 x_a1_p2 x_a2_p1 x_a2_p2 y_p1_q1 y_p1_q2 y_p2_q1 y_p2_q2 z_a1_p1_q1
 z_a1_p1_q2 z_a1_p2_q1 z_a1_p2_q2 z_a2_p1_q1 z_a2_p1_q2 z_a2_p2_q1
 z_a2_p2_q2 u_p1 u_p2
End
