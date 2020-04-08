

#include "bp_decoder.h"
#include <iostream>

int main(){
  BP_Decoder bp_decoder;
  bp_decoder.decode_mode = 1;
  std::cout<<  bp_decoder.decode_mode << std::endl;
  bp_decoder.set_decode_mode(2);
  std::cout<<  bp_decoder.decode_mode << std::endl;
  return 0;


}
