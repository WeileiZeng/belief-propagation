//Weilei  Apr 6, 2020
// copied from my_bp1.c
//#include <future>
#include <string>
#include <iostream>
#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include <sstream>
#include <stdio.h>
#include "my_lib.h"
#include <math.h>
#include "bp_decoder.h"
using namespace std;
using namespace itpp;



// Read the code from files and do BP decoding
//input:source file for stabilzier matrix; error propability p ;

int decode( BP_Decoder,const  GF2mat G, const GF2mat H, const double p, mat * data, const int col_index, const int row_index, const int cycles, const int feedback, const double time_out, const int num_data_points, const int cores);
  
int main(int argc, char **argv){
  Parser parser;
  parser.init(argc,argv);
  //p.set_silentmode(true);

  string filename_G, filename_H;
  //parser.get(filename_G,"filename_G");
  Real_Timer timer;   double remained_time; //remained time for each size
  int cores=16; parser.get(cores, "cores");
  string filename_data;
  filename_data="gnuplot/result/my-bp5-test.gnudat"; 
  parser.get(filename_data,"filename_data");
  
  int feedback=5; parser.get(feedback,"feedback");
    int cycles = 1000;//70 sec for 2,000,000
    double time_out=20;//time out in seconds for each data points (p and size)
  int num_data_points = 100;//data entry for each data points
  parser.get(num_data_points, "num_data_points");
  //change parameter p, code size
  double p;
  //parser.get(p,"p");
  //  int sizes[]= {13,11,9,7,5};
  int sizes[]= {13,9,5};
  int size_of_sizes = sizeof(sizes)/sizeof(*sizes);
  string stabilizer_folder="data/toric/stabilizer";
  double ip_begin=-0.7;
  double ip_end=-1.8;    
  int data_rows = (ip_begin-ip_end)/0.1;
  mat data(data_rows,5 * size_of_sizes );   //return result in a mat, 5 columns for each size. format defines in header  
  data.zeros();

  int exit_iteration=9; parser.get(exit_iteration,"exit_iteration");
  int schedule_mode=4; parser.get(schedule_mode, "schedule_mode");

  int col_index=-5;
  for ( int size : sizes){
    //    cout<<"size = "<<size<<endl;
    col_index +=5;//5 col for each size
    filename_G = stabilizer_folder + "/toric_S_x_size_" + to_string(size) + ".mm";
    filename_H = stabilizer_folder + "/toric_S_z_size_" + to_string(size) + ".mm";
    //filename_result = error_folder + "/toric_S_size_" + to_string(size) + ".mm_rate";
    GF2mat G =MM_to_GF2mat(filename_G);      
    GF2mat H =MM_to_GF2mat(filename_H);
    //set up for BP_Decoder
    BP_Decoder bp_decoder;
    bp_decoder.init(H);
    bp_decoder.set_decode_mode_str("min sum");    
    bp_decoder.set_exit_iteration(exit_iteration);
    bp_decoder.set_debug_mode(false);
    bp_decoder.set_schedule_mode(schedule_mode);
    bp_decoder.print_info();
      
    int row_index=-1;
    timer.tic();
    for ( double ip=ip_begin;ip > ip_end;ip-=0.1){
      //cout<<"ip = "<<ip<<endl;
            row_index ++;
      //atof(argv[4]);
      p=pow(10,ip);

   
  decode( bp_decoder, G, H, p, & data,col_index, row_index, cycles, feedback, time_out, num_data_points, cores); 

	remained_time = timer.toc()/(row_index+1)*(data_rows-row_index-1);
  std::cout<<", size = "<<size
	  <<", p = "<<p
	  <<", row_index = "<<row_index<<"/"<<data_rows
	  <<", remained time for this p is "<< remained_time <<" sec"
	  <<", col_index = "<<col_index
	  <<std::endl;    
    }
  }


  string header="# header\n# sizes: ";
  for (int size : sizes){
    header += to_string(size) + ", ";
  }
  header += ("\n# p, P_c converge rate, counts_total, counts_nonconverge, timer.toc()");


  //print final result
  mat2gnudata(data,filename_data,header);
  cout<<"save final data to "<<filename_data.c_str()<<endl;
  return 0;
}

//*********************** bp decoding function by itpp

int decode( BP_Decoder bp_decoder, const GF2mat G, const GF2mat H, const double p,  mat * data, const int col_index, const int row_index, const int cycles, const int feedback, const double time_out, const int num_data_points, const int cores){
  //parameter setup
  const double bound = 0;
  const int max_repetition = feedback;//10 for best result. supposed to be zero in our set up, just for a check
  Real_Timer timer;
  const int nvar = H.cols(); // number of bits per codeword; N=2 x n x n is the size of the toric code.
  
  const bvec bitsin = zeros_b(nvar);//original zero vector
 
  RNG_randomize();
  timer.tic(); 

  BSC bsc(p); // initialize BSC, error channel with probability pp
  
  
  const double LLR=log( (1-p)/p);

  int counts_converge=0;
  int counts_nonconverge=0;
  #pragma omp parallel for num_threads(cores)
  for (int i=0;i<cycles;i++){
  if ( counts_nonconverge < num_data_points){
    vec LLRin(nvar);
    int ans=0; //the value return by bp_decoding(), which is the number of iterations and with a minus sign if it doesn't converge
    //bvec rec_bits = bitsin;   // input vector with manually-input errors, for test

    bvec rec_bits0 = bsc(bitsin);    // input vector with errors from bsc chanel
    bvec rec_bits=rec_bits0;
    QLLRvec llr_output;
    bvec error=rec_bits;
    bvec syndrome = (H*error);
    //cout<<"syndrome = "<<syndrome<<endl;
    LLRin.ones();
    LLRin = LLRin*LLR;

    vec LLRout(LLRin);//default, zero syndrome give zero error
    ans = bp_decoder.decode(syndrome,LLRin, LLRout);

    bvec bitsout = llr_output < bound; //30;//0;//output


    //enhanced feedback
    int current_iteration = ans;
    for (int i_iteration = 0;i_iteration<max_repetition;i_iteration++){
      if (ans >0){
	break;
      }
      // usleep(100000);

      LLRin = LLRout;
      LLRout.zeros();
      ans = bp_decoder.decode(syndrome,LLRin, LLRout);
      //      ans = bp_decoder.bp_syndrome_llr(syndrome,LLRin, LLRout);
      //      ans = bp_syndrome_llr(H,syndrome,LLRin, LLRout, exit_iteration, decode_mode);
      bitsout = LLRout < bound;

    }

    bitsout=bitsout+rec_bits+rec_bits0;//cancel input vector
    //bitsout=reduce_weight( bitsout, G);//remove trivial cycles
    rec_bits=rec_bits0;//replace for compatibility for saving data
#pragma omp critical  
{
    if(ans>=0){
      if ( current_iteration < 0 ){
	//cout<<"-------------------------------make it converge after iteration."<<endl;
      }
      counts_converge++;
    }else{
      counts_nonconverge++;
      //std::cout<<"counts_nonconverge = "<<counts_nonconverge<<std::endl;  
    }
}
    }
  }
  //cout<<"counts_converge="<<counts_converge<<endl;
  //double rate_converge=counts_converge*1.0/cycles;
  //this statistical method create an overall bias
  int counts_total = counts_converge+counts_nonconverge;
  double rate_converge=counts_converge*1.0/counts_total;
  cout<<"Run summary: nvar = "<<nvar<<" =  2 x "<< nvar/2;
  cout<<", p = "<<p;
  cout<<", Converge rate ="<<rate_converge
      <<", cycles = "<<cycles
      <<", counts_total = "<<counts_total
      <<", counts_nonconverge = "<<counts_nonconverge<<endl;
  //  save_result(p,rate_converge,filename_result_p);//no need to save this. can get it by counting the size of the matrix when doing gnuplot

  //save result to mat data, using row_index and col_index
  //data entries: p, rate
  data->set(row_index,col_index,p);
  data->set(row_index,col_index+1,rate_converge);
  data->set(row_index,col_index+2,counts_total);
  data->set(row_index,col_index+3,counts_nonconverge);
  data->set(row_index,col_index+4,timer.toc());
  
  timer.toc_print();
  return 0;
}
   
