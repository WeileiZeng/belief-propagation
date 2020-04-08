//this is the class for bp_decoder

#include <string>
//#include <itbase.h>

class BP_Decoder{
 public:
  int decode_mode;
  std::string decode_mode_str;
  //  itpp::GF2mat H;//parity check matrix
  void set_decode_mode(int new_decode_mode);
};

void BP_Decoder::set_decode_mode(int new_decode_mode){
  decode_mode = new_decode_mode;
  return;

};
