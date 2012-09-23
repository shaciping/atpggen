module combo(out1, out2, out3, out4, in1, in2, in3);
   output out1;
   output out2;
   output out3;
   output out4;
   input  in1;
   input  in2;
   input  in3;
   

   wire   w1, w2;

   and and1(w1, in1, in2);
   not not1(out1, w1);

   or or1(w2, in1, in2);
   not not2(out2, w2);

   or or2(out3, out2, in3);
   and and2(out4, in3, w2);


endmodule // combo

