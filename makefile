#CXX=g++ -g -Wall  #for debug mode
CXX=g++ -O3 -Wall
### -O2 -O5 -Os
#g++ `pkg-config --cflags itpp` -o hello.out hello.cpp `pkg-config --libs itpp`



START=`pkg-config --cflags itpp`
END=`pkg-config --libs itpp`
#files=mm_read.c mm_read.h mmio.c mmio.h mm_write.c mm_write.h lib.cpp lib.h my_lib.h makefile
#command=$(CXX) $(START) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(word 8, $^) $(END)


#update:
INC_DIR=~/working/weilei_lib
files=$(INC_DIR)/mm_read.c $(INC_DIR)/mm_read.h $(INC_DIR)/mmio.c $(INC_DIR)/mmio.h $(INC_DIR)/mm_write.c $(INC_DIR)/mm_write.h $(INC_DIR)/lib.cpp $(INC_DIR)/lib.h $(INC_DIR)/dist.c $(INC_DIR)/dist.h $(INC_DIR)/concatenation_lib.c $(INC_DIR)/concatenation_lib.h $(INC_DIR)/my_lib.h makefile
#command=$(CXX) -I../../weilei_lib $(START) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(word 8, $^) $(word 10, $^) $(word 12, $^) $(END)

command=$(CXX) -std=c++11 -pthread -I $(INC_DIR) $(START) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(word 8, $^) $(word 10, $^) $(word 12, $^) $(END) ; echo finish making



all: parserTest.out ldpcTest.out test.out alist_test.out
###include all headfiles into my_lib.h
#pattern_observer.out:pattern_observer.c mm_read.c mm_read.h mmio.c mmio.h mm_write.c mm_write.h lib.cpp lib.h my_lib.h makefile

partial_sum.out:partial_sum.c $(files)
	$(command)
pattern_observer.out:pattern_observer.c $(files)
	$(command)
bp_decoding4.out:bp_decoding4.c $(files)
	$(command)
my_bp1.out:my_bp1.c $(files)
	$(command)
#bp_decoding3.out:bp_decoding3.c mm_read.c mm_read.h mmio.c mmio.h mm_write.c mm_write.h lib.cpp lib.h my_lib.h makefile
bp_decoding3.out:bp_decoding3.c $(files)
	$(command)
data_collect_perm_check.out:data_collect_perm_check.c $(files)
	$(command)
convert_data.out:convert_data.c $(files)
	$(command)
test.out:test.c
	g++ -std=c++11 -pthread -o test.out test.c
#test.out:test.c $(files)
#	$(command)
	./test.out
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

