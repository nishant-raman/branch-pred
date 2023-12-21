#./sim bimodal 6 traces/gcc_trace.txt > output/bimodal1_6_gcc.txt
#./sim bimodal 12 traces/gcc_trace.txt > output/bimodal2_12_gcc.txt
#./sim bimodal 4 traces/jpeg_trace.txt > output/bimodal3_4_jpeg.txt
#./sim bimodal 5 traces/perl_trace.txt > output/bimodal4_5_perl.txt
#./sim gshare 9 3 traces/gcc_trace.txt > output/gshare1_9_3_gcc.txt
#./sim gshare 14 8 traces/gcc_trace.txt > output/gshare2_14_8_gcc.txt
#./sim gshare 11 5 traces/jpeg_trace.txt > output/gshare3_11_5_jpeg.txt
#./sim gshare 10 6 traces/perl_trace.txt > output/gshare4_10_6_perl.txt
#./sim hybrid 8 14 10 5 traces/gcc_trace.txt > output/hybrid1_8_14_10_5_gcc.txt
#./sim hybrid 5 10 7 5 traces/jpeg_trace.txt > output/hybrid2_5_10_7_5_jpeg.txt
#
#echo gvimdiff output/bimodal1_6_gcc.txt val/val_bimodal_1.txt
#echo gvimdiff output/bimodal2_12_gcc.txt val/val_bimodal_2.txt
#echo gvimdiff output/bimodal3_4_jpeg.txt val/val_bimodal_3.txt
#echo gvimdiff output/bimodal4_5_perl.txt val/val_bimodal_4.txt
#echo gvimdiff output/gshare1_9_3_gcc.txt val/val_gshare_1.txt
#echo gvimdiff output/gshare2_14_8_gcc.txt val/val_gshare_2.txt
#echo gvimdiff output/gshare3_11_5_jpeg.txt val/val_gshare_3.txt
#echo gvimdiff output/gshare4_10_6_perl.txt val/val_gshare_4.txt
#echo gvimdiff output/hybrid1_8_14_10_5_gcc.txt val/val_hybrid_1.txt
#echo gvimdiff output/hybrid2_5_10_7_5_jpeg.txt val/val_hybrid_2.txt

## bimodal experiment
#M2=(7 8 9 10 11 12 13 14 15 16 17 18 19 20)
#TRACES=(gcc perl jpeg)
#
#for trace in ${TRACES[@]}; do
#	for m2 in ${M2[@]}; do
#		echo "./sim bimodal ${m2} traces/${trace}_trace.txt > output/re_bimodal_${m2}_${trace}.txt"
#		./sim bimodal ${m2} traces/${trace}_trace.txt > output/re_bimodal_${m2}_${trace}.txt
#	done
#done

#gshare experiment
M1=(7 8 9 10 11 12 13 14 15 16 17 18 19 20)
N=(0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)
	
for m in ${M1[@]}; do
	for n in ${N[@]}; do
		if [ $m -ge $n ]
		then
			echo "./sim gshare ${m} ${n} traces/gcc_trace.txt > output/re_gshare_${m}_${n}_gcc.txt"
			./sim gshare ${m} ${n} traces/gcc_trace.txt > output/re_gshare_${m}_${n}_gcc.txt
		fi
	done
done



