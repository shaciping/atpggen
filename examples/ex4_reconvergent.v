module combo(out1, in1, in2);
   output out1;
   input  in1;
   input  in2;

   wire   w1, w2, w3;

   nand nand1 (w1, in1, in2);
   nand nand2 (w2, in1, w1);
   nand nand3 (w3, w1, in2);
   nand nand4 (out1, w2, w3);

endmodule // combo

