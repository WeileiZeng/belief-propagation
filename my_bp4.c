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

int decode( BP_Decoder, GF2mat G, GF2mat H, double p, mat * data, int col_index, int row_index, int cycles, int feedback, double time_out, int num_data_points);
  
int main(int argc, char **argv){
  Parser parser;
  parser.init(argc,argv);
  //p.set_silentmode(true);
  string filename_G, filename_H;
  //parser.get(filename_G,"filename_G");
  Real_Timer timer;   double remained_time; //remained time for each size
  vector<future<int>> pool;
  int cores=32; parser.get(cores, "cores");
  vector<future<int>>::size_type pool_size = cores+2; //max number of threads, 15 with decreasing size for best performace
  std::chrono::milliseconds span (100);
  std::chrono::milliseconds final_thread_time (10000);//10 secs before prelimilary result print
  string filename_data;
  filename_data="gnuplot/result/my-bp4-iter-9-fb-25-data-100-schedule-3-hpcc.gnudat"; 
  parser.get(filename_data,"filename_data");
  
  int feedback=25; parser.get(feedback,"feedback");
  //  int cycles = 1000000;//70 sec for 2,000,000
  //  double time_out=200;//time out in seconds for each data points (p and size)
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

  //split tasks into smaller chunks
  int chunk_num_data_points=10;//number of data points in each chunk
  int chunk_size=num_data_points/chunk_num_data_points; //number of chunks for each p and size
  int chunk_num_for_each_size = chunk_size*((ip_begin-ip_end)/0.1);

  //timeout should be 5 times longer in hpcc
  double chunk_time_out = 100.0;//time_out/chunk_size;
  parser.get(chunk_time_out,"chunk_time_out");
  int chunk_cycles=chunk_num_data_points*1000;//1000 for prob 1/1000

  mat chunk_data(data_rows*chunk_size,5*5);
  chunk_data.zeros();


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
      //      row_index ++;
      //atof(argv[4]);
      p=pow(10,ip);

      for ( int ic=0; ic<chunk_size; ic++){
	row_index ++;
	//manage thread within limit of pool size
	while ( pool.size() >= pool_size ){
	  //wait until some of them finish
	  //break;
	  for(vector <future<int>> :: iterator it = pool.begin(); it != pool.end(); ++it){
	    //printf("%d", *it);
	    if ( it->wait_for(span) == future_status::ready){
	      //	    cout<<"."<<endl;
	      pool.erase(it);	    
	      //cout<<"remove thread. pool_size="<<pool.size()<<endl;
	      it--;
	      //break;
	    }
	  }
	}
    
	// future<int> fut = async(launch::async, decode,G, H, p, filename_result_p);
	//pool.push_back(move(fut));
	//pool.push_back(async(launch::async, decode, bp_decoder, G, H, p, & data,col_index, row_index, cycles, feedback, time_out, num_data_points) );
	pool.push_back(async(launch::async, decode, bp_decoder, G, H, p, & chunk_data,col_index, row_index, chunk_cycles, feedback, chunk_time_out, chunk_num_data_points) );

	remained_time = timer.toc()/(row_index+1)*chunk_num_for_each_size;
      cout<<"my_bp: add new thread. pool_size="<<pool.size()
	  <<", size = "<<size
	  <<", p = "<<p
	  <<", row_index = "<<row_index<<"/"<<chunk_num_for_each_size
	  <<", remained time for this p is "<< remained_time <<" sec"
	  <<", col_index = "<<col_index
	  <<", chunk_cycles = "<<chunk_cycles
	  <<endl;

      }
    }
  }
  //finish adding all the threads and most of them are done
  
  //wait a bit for last threads
  for(vector <future<int>> :: iterator it = pool.begin(); it != pool.end(); ++it){
    it->wait_for(final_thread_time);
  }

  //print prelimilary result  
  string header="# header\n# sizes: ";
  for (int size : sizes){
    header += to_string(size) + ", ";
  }
  header += ("\n# p, P_c converge rate, counts_total, counts_nonconverge, timer.toc()");
  //  mat2gnudata(data,filename_data,header);
  //cout<<"save prelimilary data to "<<filename_data.c_str()<<endl;

  //wait all process to finish
  for(vector <future<int>> :: iterator it = pool.begin(); it != pool.end(); ++it){
    it->get();
  }

  //process chunk_data to data
  int temp_index;
  double value;
  for ( int i =0; i<data.rows();i++){
    for ( int j = 0; j< data.cols();j++){
      value=0;
      for ( int k =0;k<chunk_size; k++){
	temp_index = i* chunk_size+k;
	value += chunk_data(temp_index,j);
      }
      value = value/chunk_size;
      data.set(i,j,value);
    }
  }

  //print final result
  mat2gnudata(data,filename_data,header);
  cout<<"save final data to "<<filename_data.c_str()<<endl;
  return 0;
}

//*********************** bp decoding function by itpp

int decode( BP_Decoder bp_decoder, GF2mat G, GF2mat H, double p,  mat * data, int col_index, int row_index, int cycles, int feedback, double time_out, int num_data_points){
  //parameter setup
  //int decode_mode = 2;
  //  int cycles=1000;//10000;//number of cycles: fro toric code, 10000 give reletively clear result
  //  int exit_at_iteration=9;//parameter for bp decoding set_exit_condition()
  //  int bound= (int) -4096 * log (p/(1-p));// -300; see note.pdf on how to choose bound
  int bound = 0;
  int max_repetition = feedback;//10 for best result. supposed to be zero in our set up, just for a check
  
  Real_Timer timer;
  int nvar = H.cols();
  //int N = C.get_nvar(); // number of bits per codeword; N=2 x n x n is the size of the toric code.
  
  bvec bitsin = zeros_b(nvar);//original zero vector
 
  RNG_randomize();
  timer.tic(); 

  BSC bsc(p); // initialize BSC, error channel with probability pp
  int ans=0; //the value return by bp_decoding(), which is the number of iterations and with a minus sign if it doesn't converge
  vec LLRin(nvar);
  double LLR=log( (1-p)/p);

  int counts_converge=0;
  int counts_nonconverge=0;
  for (int i=0;i<cycles;i++){
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

    if(ans>=0){
      if ( current_iteration < 0 ){
	//cout<<"-------------------------------make it converge after iteration."<<endl;
      }
      counts_converge++;
    }else{
      counts_nonconverge++;
      //time control
      if (timer.toc()>time_out){
	std::cout<<"timeout: "<<time_out<<std::endl;
	break;
      }
      else if (counts_nonconverge == num_data_points/2){
	//change number of cycles
	cycles =min(cycles, i *2);
	cycles=max(cycles, 50);
	std::cout<<"update cycles to "<<cycles<<std::endl;
	//std::cout<<i<<","<<counts_nonconverge<<", "
	//	 << num_data_points<<std::endl;
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
   
