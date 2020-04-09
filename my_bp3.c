//Weilei  Apr 6, 2020
// copied from my_bp1.c
#include <future>
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

int decode( GF2mat G, GF2mat H, double p, mat * data, int col_index, int row_index, int cycles, int feedback, double time_out, int num_data_points);
  
int main(int argc, char **argv){
  Parser parser;
  parser.init(argc,argv);
  //p.set_silentmode(true);
  string filename_G, filename_H, filename_result;
  //parser.get(filename_G,"filename_G");
  //parser.get(filename_H,"filename_H");
  //parser.get(filename_result,"filename_result");
  
  vector<future<int>> pool;
  vector<future<int>>::size_type pool_size=15; //max number of threads, 15 with decreasing size for best performace
  std::chrono::milliseconds span (100);
  std::chrono::milliseconds final_thread_time (10000);//10 secs
  string filename_data="gnuplot/result/my-bp3-iteration0-cycle100-min-sum.gnudat";
  //  filename_data="gnuplot/result/my-bp3-feedback5-cycle0-10000-schedule.gnudat";
  filename_data="gnuplot/result/my-bp3-feedback5-num-data-200-schedule.gnudat";
  int feedback=5;
  //  int cycle0=10000; //number of cycles for the first data points
  double time_out=2000;//time out in seconds for each data points (p and size)
  int num_data_points = 200;//data entry for each data points
    
  //change parameter p, code size
  //char * filename_result=argv[3];//prefix for the file
  double p;
  //parser.get(p,"p");
  int sizes[]= {13,11,9,7,5};
  string stabilizer_folder="data/toric/stabilizer";
  //string error_folder="data/toric/bp_decoding4";
  double ip_begin=-1.7;
  double ip_end=-2.7;    
  int data_rows = (ip_begin-ip_end)/0.1;
  mat data(data_rows,5*5);   //return result in a mat, 5 columns for each size. format defines in header
  data.zeros();

  int col_index=-5;
  for ( int size : sizes){
    //    cout<<"size = "<<size<<endl;
    col_index +=5;//5 col for each size
    filename_G = stabilizer_folder + "/toric_S_x_size_" + to_string(size) + ".mm";
    filename_H = stabilizer_folder + "/toric_S_z_size_" + to_string(size) + ".mm";
    //filename_result = error_folder + "/toric_S_size_" + to_string(size) + ".mm_rate";
    int row_index=-1;
    for ( double ip=ip_begin;ip > ip_end;ip-=0.1){
      //cout<<"ip = "<<ip<<endl;
      row_index ++;
      //atof(argv[4]);
      p=pow(10,ip);
      int cycles = 2000000;
      //      int cycles = std::pow(p,-3)/100/2*num_data_points;//around 10 data points for each point
      //      int cycles = cycle0*std::pow(10,3*(ip-ip_begin)/(ip_end-ip_begin));
      //p=ip/100000.0;//previous use 1000 division. Now use 100,000 division cause the thershold for toric codes seems to be around 0.1%.
      //char filename_result_p[255];
      //sprintf( filename_result_p,"%s%.5f",filename_result.c_str(),p);//append p in the file name
      //string filename_result_p_string(filename_result_p);
      //cout<<filename_result_p_string<<endl;
      GF2mat G =MM_to_GF2mat(filename_G);      
      GF2mat H =MM_to_GF2mat(filename_H);

      //manage thread within limit of pool size
      while ( pool.size() >= pool_size ){
	//wait until some of them finish
	//break;
	 for(vector <future<int>> :: iterator it = pool.begin(); it != pool.end(); ++it){
	  //printf("%d", *it);
	   if ( it->wait_for(span) == future_status::ready){
	     //	    cout<<"."<<endl;
	    pool.erase(it);	    
	    cout<<"remove thread. pool_size="<<pool.size()<<endl;
	    it--;
	    //break;
	  }
	 }
      }
    
      // future<int> fut = async(launch::async, decode,G, H, p, filename_result_p);
      //pool.push_back(move(fut));
      pool.push_back(async(launch::async, decode,G, H, p, &data,col_index, row_index, cycles, feedback, time_out, num_data_points) );
      
      cout<<"my_bp: add new thread. pool_size="<<pool.size()
	  <<", size = "<<size
	  <<", p = "<<p
	  <<", row_index = "<<row_index
	  <<", col_index = "<<col_index
	  <<", cycles = "<<cycles
	  <<endl;
    }
  }
  //wait a bit for last threads
  for(vector <future<int>> :: iterator it = pool.begin(); it != pool.end(); ++it){
    it->wait_for(final_thread_time);
  }

  //print prelimilary result  
  string header="#header\nsizes: ";
  for (int size : sizes){
    header += to_string(size) + ", ";
  }
  header += ("\n p, P_c converge rate, zero error vectors, weight one error, weight 2 error");
  mat2gnudata(data,filename_data,header);
  cout<<"save prelimilary data to "<<filename_data.c_str()<<endl;

  //wait all process to finish
  for(vector <future<int>> :: iterator it = pool.begin(); it != pool.end(); ++it){
    it->get();
  }

  //print final result
  mat2gnudata(data,filename_data,header);
  cout<<"save final data to "<<filename_data.c_str()<<endl;
  return 0;
}

//*********************** bp decoding function by itpp

int decode( GF2mat G, GF2mat H, double p,  mat * data, int col_index, int row_index, int cycles, int feedback, double time_out, int num_data_points){
  //parameter setup
  //int decode_mode = 2;
  //  int cycles=1000;//10000;//number of cycles: fro toric code, 10000 give reletively clear result
  int exit_at_iteration=9;//parameter for bp decoding set_exit_condition()
  //  int bound= (int) -4096 * log (p/(1-p));// -300; see note.pdf on how to choose bound
  int bound = 0;
  int max_repetition = feedback;//10 for best result. supposed to be zero in our set up, just for a check


  //set up for BP_Decoder
  BP_Decoder bp_decoder;
  bp_decoder.init(H);
  bp_decoder.set_decode_mode_str("min sum");
  bp_decoder.set_exit_iteration(exit_at_iteration);
  bp_decoder.set_debug_mode(false);
  bp_decoder.set_schedule_mode(1);
  bp_decoder.print_info();
  
  Real_Timer timer;
  LDPC_Code C=GF2mat_to_LDPC_Code(H);  //convert GF2mat saved in .mm file to LDPC_Code
  //    LDPC_Code C=MM_to_LDPC_Code(filename_G);  //convert GF2mat saved in .mm file to LDPC_Code
  //LDPC_Code C = get_test_LDPC_Code();
  
  //bp decoding set up
  C.set_exit_conditions(exit_at_iteration,true,true);//high perperformance. This number of iteration would not affect small errors which converges and stop very fast. Only affect those doesn't converge soon or not converge at all.
  //  C.set_exit_conditions(50,true,true);  // 50 iterations,check syndrome always
  //C.set_llrcalc(LLR_calc_unit(12,300,7)); //(recommended settings with "exact" computation via high resolution lookup table)
  C.set_llrcalc(LLR_calc_unit(12,300,7)); // fast mode; see "llr.h" //not good resolution for the result, produce more errors
  //  cout << C << endl;
  int N = C.get_nvar(); // number of bits per codeword; N=2 x n x n is the size of the toric code.
  
  bvec bitsin = zeros_b(N);//original zero vector
  GF2mat E_input_converge(bitsin, false),E_input_nonconverge(bitsin, false),E_output_converge(bitsin,false),E_output_nonconverge(bitsin, false);//false for row vector;//These two GF2mat saves the input errors and their output after bp_decoding. They are clasified into convergeds cases and non converged cases.
  RNG_randomize();
  timer.tic(); 

  //double pp=p;//=0.05;//test error probability //=pvals(j);   //  double pp=pow(10.0,-pvals(j));
  //pp=0.01  ans~1;pp=0.03  ans=-2500
  BSC bsc(p); // initialize BSC, error channel with probability pp
  int ans=0; //the value return by bp_decoding(), which is the number of iterations and with a minus sign if it doesn't converge
      


  int counts_converge=0;
  int counts_nonconverge=0;
  //int counts_weight_zero=0;
  //int counts_weight_one=0;
  //int counts_weight_two=0;
  for (int i=0;i<cycles;i++){
    //bvec rec_bits = bitsin;   // input vector with manually-input errors, for test

    bvec rec_bits0 = bsc(bitsin);    // input vector with errors from bsc chanel
    //bvec rec_bits = find_error(rec_bits0,H);//switch to en error with same syndrome
    bvec rec_bits=rec_bits0;
    
    //    bvec rec_bits( error_transform(rec_bits0,MM_to_GF2mat(filename_G)) ); //for test


    vec s(N), s0(N); // input LLR version
    QLLRvec llr_output;
    for(int i1=0;i1<N;i1++)      s(i1)=s0(i1)=(rec_bits(i1)==1)?-log(1.0/p-1.0):log(1.0/p-1.0);
    
    
    QLLRvec llr_input=C.get_llrcalc().to_qllr(s);
    //cout<<"llr_input="<<llr_input<<endl;

    
    //    int ans0;
    //ans0=C.bp_decode(llr_input, llr_output);
    //replace the above line using my bp decoder

    bvec error=rec_bits;
    bvec syndrome = (H*error);
    //cout<<"syndrome = "<<syndrome<<endl;
    vec LLRin(H.cols());
    //    double p =0.01;
    double LLR=log( (1-p)/p);
    LLRin.ones();
    LLRin = LLRin*LLR;
    vec LLRout(LLRin);//default, zero syndrome give zero error
    //LLRout.zeros();
    //cout<<"LLRin  = "<< LLRin<<endl;
    //  cout<<"LLRout = "<< LLRout<<endl;
    //    int exit_iteration = exit_at_iteration*1;
    //    ans = bp_syndrome_llr(H,syndrome,LLRin, LLRout, exit_iteration, decode_mode);
    ans = bp_decoder.decode(syndrome,LLRin, LLRout);

    //compare the result
    bool compare = false;
    if (compare) {
      int ans0;
      ans0=C.bp_decode(llr_input, llr_output);
      //replace the above line using my bp decoder
      if (ans0>0 && ans<0){
	cout<<"Lose: ans0 = "<<ans0<<", ans = "<<ans<<endl;
	draw_toric_x_error( error,"error");
	draw_toric_x_error( error + (LLRout < bound),"eror + LLRout: ");
	draw_toric_x_error( llr_output < bound,"llr_output:");
      }
    }
    bvec bitsout = llr_output < bound; //30;//0;//output

    int current_iteration = ans;
    for (int i_iteration = 0;i_iteration<max_repetition;i_iteration++){
      if (ans >0){
	break;
      }
      // usleep(100000);

      LLRin = LLRout;
      LLRout.zeros();
      ans = bp_decoder.bp_syndrome_llr(syndrome,LLRin, LLRout);
      //      ans = bp_syndrome_llr(H,syndrome,LLRin, LLRout, exit_iteration, decode_mode);
      bitsout = LLRout < bound;

    }

    bitsout=bitsout+rec_bits+rec_bits0;//cancel input vector
    //bitsout=reduce_weight( bitsout, G);//remove trivial cycles
    rec_bits=rec_bits0;//replace for compatibility for saving data

    if(ans>=0){
      if ( current_iteration < 0 ){
	//cout<<"-------------------------------make it converge after iteration."<<endl;
      }
      counts_converge++;
    }else{
      counts_nonconverge++;
      //time control
      if (timer.toc()>time_out){
	break;
      }else if (counts_nonconverge > num_data_points){
	break;
      }
      
    }
  }
  //cout<<"counts_converge="<<counts_converge<<endl;
  //double rate_converge=counts_converge*1.0/cycles;
  //this statistical method create an overall bias
  int counts_total = counts_converge+counts_nonconverge;
  double rate_converge=counts_converge*1.0/counts_total;
  cout<<"N = "<<N<<" =  2 x "<< N/2;
  cout<<", p = "<<p;
  cout<<", Converge rate ="<<rate_converge
      <<", cycles = "<<cycles
      <<", counts_total = "<<counts_total
      <<", counts_nonconverge = "<<counts_nonconverge<<endl;
  //  save_result(p,rate_converge,filename_result_p);//no need to save this. can get it by counting the size of the matrix when doing gnuplot

  //save result to mat data, suing row_index and col_index
  //data entries: p, rate
  data->set(row_index,col_index,p);
  data->set(row_index,col_index+1,rate_converge);
  data->set(row_index,col_index+2,counts_total);
  
  timer.toc_print();
  return 0;
}
   
