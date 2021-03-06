opt = -O3
### -O2 -O5 -Os                                                                                                                   
pkgs = -L/opt/linux/centos/7.x/x86_64/pkgs
itpp=${pkgs}/itpp/4.3.1/lib/ -litpp
# INC_DIR=weilei_lib
# CXX = g++ -Wall -g -m64 -std=c++0x ${opt} ${itpp}
CXX = g++ -Wall -g -m64 -std=c++11 -pthread ${opt} ${itpp}
CXX = g++ -Wall -g -m64 -std=c++11 -fopenmp ${opt} ${itpp}
CMD= -lm -lgmpxx -lgmp

INC_DIR=~/working/weilei_lib
# INC_DIR=../../weilei_lib
files=$(INC_DIR)/mm_read.c $(INC_DIR)/mm_read.h $(INC_DIR)/mmio.c $(INC_DIR)/mmio.h $(INC_DIR)/mm_write.c $(INC_DIR)/mm_write.h $(INC_DIR)/lib.cpp $(INC_DIR)/lib.h $(INC_DIR)/dist.c $(INC_DIR)/dist.h $(INC_DIR)/concatenation_lib.c $(INC_DIR)/concatenation_lib.h $(INC_DIR)/bp.c $(INC_DIR)/bp.h $(INC_DIR)/bp_decoder.h $(INC_DIR)/my_lib.h makefile

# command=$(CXX) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(word 8, $^) $(word 10, $^) $(CMD)

# command=$(CXX) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(word 8,  $(CMD)

command=$(CXX) -I $(INC_DIR) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(word 8, $^) $(word 10, $^) $(word 12, $^) $(word 14, $^) $(word 15, $^) $(CMD) ; echo finish making






all: parserTest.out ldpcTest.out test.out alist_test.out


partial_sum.out:partial_sum.c $(files)
	$(command)
pattern_observer.out:pattern_observer.c $(files)
	$(command)
bp_decoding4.out:bp_decoding4.c $(files)
	$(command)
my_bp1.out:my_bp1.c $(files)
	$(command)
my_bp2.out:my_bp2.c $(files)
	$(command)
my_bp3.out:my_bp3.c $(files)
	$(command)
my_bp4.out:my_bp4.cpp $(files)
	$(command)
my_bp5.out:my_bp5.cpp $(files)
	$(command)
#bp_decoding3.out:bp_decoding3.c mm_read.c mm_read.h mmio.c mmio.h mm_write.c mm_write.h lib.cpp lib.h my_lib.h makefile
bp_decoding3.out:bp_decoding3.c $(files)
	$(command)
data_collect_perm_check.out:data_collect_perm_check.c $(files)
	$(command)
convert_data.out:convert_data.c $(files)
	$(command)
#test.out:test.c
#	g++ -std=c++11 -pthread -o test.out test.c
test.out:test.c $(files)
	$(command)
#	./test.out
#	$(CXX) $(START) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(END)
error_analysis.out:error_analysis.c mm_read.c mm_read.h mmio.c mmio.h mm_write.c mm_write.h my_lib.h
	$(CXX) $(START) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(END)
rand_decode_perm_check.out:rand_decode_perm_check.c mm_read.c mm_read.h mmio.c mmio.h mm_write.c mm_write.h lib.cpp lib.h my_lib.h
	$(CXX) $(START) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(word 8, $^) $(END)
rand_decode_perm.out:rand_decode_perm.c mm_read.c mm_read.h mmio.c mmio.h mm_write.c mm_write.h lib.cpp lib.h my_lib.h
	$(CXX) $(START) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(word 8, $^) $(END)
rand_decode3.out:rand_decode3.c mm_read.c mm_read.h mmio.c mmio.h mm_write.c mm_write.h lib.cpp lib.h my_lib.h
	$(CXX) $(START) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(word 8, $^) $(END)
rand_decode.out:rand_decode.c mm_read.c mm_read.h mmio.c mmio.h mm_write.c mm_write.h lib.cpp lib.h my_lib.h
	$(CXX) $(START) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(word 8, $^) $(END)
gauge_to_stabilizer.out:gauge_to_stabilizer.c mm_read.c mm_read.h mmio.c mmio.h mm_write.c mm_write.h my_lib.h
	$(CXX) $(START) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(END)
decoding.out:decoding.c mm_read.c mm_read.h mmio.c mmio.h mm_write.c mm_write.h my_lib.h
	$(CXX) $(START) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(END)
#test.out:test.c mm_read.c mm_read.h mmio.c mmio.h mm_write.c mm_write.h lib.cpp lib.h my_lib.h
#	$(CXX) $(START) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(word 8, $^) $(END)
code_generator.out:code_generator.c mmio.c mmio.h mm_write.c mm_write.h
	$(CXX) $(START) -o $@ $< $(word 2, $^) $(word 4, $^) $(END)
clean:
	echo this command clean all temperary files in this folder.
	rm  *~
	rm \#*

