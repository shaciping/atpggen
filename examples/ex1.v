module combo(out1, in1, in2, in3);
   output out1;
   input  in1;
   input  in2;
   input  in3;

   wire   w1;

   and and1(w1, in1, in2);
   or or1(out1, in3, w1);
   
endmodule // combo

