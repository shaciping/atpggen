module combo(out1, out2, in1, in2);
   output out1;
   output out2;
   input  in1;
   input  in2;
   

   wire   w1, w2;

   and and1(w1, in1, in2);
   not not1(out1, w1);

   or or1(w2, in1, in2);
   not not2(out2, w2);
   
endmodule // combo

