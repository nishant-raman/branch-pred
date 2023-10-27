 ./sim bimodal 6 traces/gcc_trace.txt > output/bimodal1_6_gcc.txt
 ./sim bimodal 12 traces/gcc_trace.txt > output/bimodal2_12_gcc.txt
 ./sim bimodal 4 traces/jpeg_trace.txt > output/bimodal3_4_jpeg.txt
 ./sim bimodal 5 traces/perl_trace.txt > output/bimodal4_5_perl.txt
 ./sim gshare 9 3 traces/gcc_trace.txt > output/gshare1_9_3_gcc.txt
 ./sim gshare 14 8 traces/gcc_trace.txt > output/gshare2_14_8_gcc.txt
 ./sim gshare 11 5 traces/jpeg_trace.txt > output/gshare3_11_5_jpeg.txt
 ./sim gshare 10 6 traces/perl_trace.txt > output/gshare4_10_6_perl.txt

echo gvimdiff output/bimodal1_6_gcc.txt val/val_bimodal_1.txt
echo gvimdiff output/bimodal2_12_gcc.txt val/val_bimodal_2.txt
echo gvimdiff output/bimodal3_4_jpeg.txt val/val_bimodal_3.txt
echo gvimdiff output/bimodal4_5_perl.txt val/val_bimodal_4.txt
echo gvimdiff output/gshare1_9_3_gcc.txt val/val_gshare_1.txt
echo gvimdiff output/gshare2_14_8_gcc.txt val/val_gshare_2.txt
echo gvimdiff output/gshare3_11_5_jpeg.txt val/val_gshare_3.txt
echo gvimdiff output/gshare4_10_6_perl.txt val/val_gshare_4.txt
