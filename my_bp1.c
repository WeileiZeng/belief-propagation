//Weilei  March 26, 2020. yse c++ async to manage multi threads
// copied from bp_decoding4.c
#include <future>
#include <string>
#include <iostream>
#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include <sstream>
#include <stdio.h>
#include "my_lib.h"
using namespace std;
using namespace itpp;



// Read the code from files and do BP decoding
//input:source file for stabilzier matrix; error propability p ;

int decode( GF2mat G, GF2mat H, double p, string filename_result_p);
  
int main(int argc, char **argv){
  Parser parser;
  parser.init(argc,argv);
  //p.set_silentmode(true);
  string filename_G, filename_H, filename_result;
  //parser.get(filename_G,"filename_G");
  //parser.get(filename_H,"filename_H");
  //parser.get(filename_result,"filename_result");
  
  vector<future<int>> pool;
  int pool_size=10;
  std::chrono::milliseconds span (100);
  //change parameter p, code size
  //char * filename_result=argv[3];//prefix for the file
  double p;
  //parser.get(p,"p");
  int sizes[]= {13,11,9,7,5};
  string stabilizer_folder="data/toric/stabilizer";
  string error_folder="data/toric/bp_decoding4";
  for ( int size : sizes){

    filename_G = stabilizer_folder + "/toric_S_x_size_" + to_string(size) + ".mm";
    filename_H = stabilizer_folder + "/toric_S_z_size_" + to_string(size) + ".mm";
    filename_result = error_folder + "/toric_S_size_" + to_string(size) + ".mm_rate";
    for ( int ip=100;ip<5001;ip+=100){
      //atof(argv[4]);
      p=ip/100000.0;//previous use 1000 division. Now use 100,000 division cause the thershold for toric codes seems to be around 0.1%.
      char filename_result_p[255];
      sprintf( filename_result_p,"%s%.5f",filename_result.c_str(),p);//append p in the file name
      string filename_result_p_string(filename_result_p);
      //cout<<filename_result_p_string<<endl;
      GF2mat G =MM_to_GF2mat(filename_G);
      GF2mat H =MM_to_GF2mat(filename_H);

      //manage thread      
      while ( pool.size() >= pool_size ){
	//wait until some of them finish
	//break;
	 for(vector <future<int>> :: iterator it = pool.begin(); it != pool.end(); ++it){
	  //printf("%d", *it);
	   if ( it->wait_for(span) == future_status::ready){
	    cout<<"."<<endl;
	    pool.erase(it);	    
	    cout<<"remove thread. "<<pool.size()<<endl;
	    it--;
	    //break;
	  }
	  }
      }
    
      // future<int> fut = async(launch::async, decode,G, H, p, filename_result_p);
      //pool.push_back(move(fut));
      pool.push_back(async(launch::async, decode,G, H, p, filename_result_p));
      
      cout<<"add new thread. "<<pool.size()<<endl;
      //int num = fut.get();
      //cout<<"num = "<<num<<endl;

      //decode( G, H, p, filename_result_p);
      //break;
    }
    //break;
  }
  return 0;
}

int decode( GF2mat G, GF2mat H, double p, string filename_result_p){
  //parameter setup
  int cycles=50;//10000;//number of cycles: fro toric code, 10000 give reletively clear result
  int exit_at_iteration=50;//parameter for bp decoding set_exit_condition()
  //  int bound= (int) -4096 * log (p/(1-p));// -300; see note.pdf on how to choose bound
  int bound = 0;
  int max_repetition = 20;//10 for best result. supposed to be zero in our set up, just for a check
  
  
  Real_Timer timer;

  LDPC_Code C=GF2mat_to_LDPC_Code(H);  //convert GF2mat saved in .mm file to LDPC_Code
  //    LDPC_Code C=MM_to_LDPC_Code(filename_G);  //convert GF2mat saved in .mm file to LDPC_Code
  //LDPC_Code C = get_test_LDPC_Code();
  
  //bp decoding set up
  C.set_exit_conditions(exit_at_iteration,true,true);//high perperformance. This number of iteration would not affect small errors which converges and stop very fast. Only affect those doesn't converge soon or not converge at all.
  //  C.set_exit_conditions(50,true,true);  // 50 iterations,check syndrome always
  //C.set_llrcalc(LLR_calc_unit(12,300,7)); //(recommended settings with "exact" computation via high resolution lookup table)
  C.set_llrcalc(LLR_calc_unit(12,0,7)); // fast mode; see "llr.h" //not good resolution for the result, produce more errors
  //  cout << C << endl;
  int N = C.get_nvar(); // number of bits per codeword; N=2 x n x n is the size of the toric code.
  
  bvec bitsin = zeros_b(N);//original zero vector
  GF2mat E_input_converge(bitsin, false),E_input_nonconverge(bitsin, false),E_output_converge(bitsin,false),E_output_nonconverge(bitsin, false);//false for row vector;//These two GF2mat saves the input errors and their output after bp_decoding. They are clasified into convergeds cases and non converged cases.
  RNG_randomize();
  timer.tic(); 

  double pp=p;//=0.05;//test error probability //=pvals(j);   //  double pp=pow(10.0,-pvals(j));
  //pp=0.01  ans~1;pp=0.03  ans=-2500
  BSC bsc(pp); // initialize BSC, error channel with probability pp
  int ans=0; //the value return by bp_decoding(), which is the number of iterations and with a minus sign if it doesn't converge
      


  int counts_converge=0;
  for (int i=0;i<cycles;i++){
    //bvec rec_bits = bitsin;   // input vector with manually-input errors, for test
    //    C.set_exit_conditions(2500);
    bvec rec_bits0 = bsc(bitsin);    // input vector with errors from bsc chanel
    bvec rec_bits = find_error(rec_bits0,H);//switch to en error with same syndrome
    
    //    bvec rec_bits( error_transform(rec_bits0,MM_to_GF2mat(filename_G)) ); //for test

    //manual input error for test
    //    rec_bits = bitsin; //zero vector
    //horizontally aligned double error 
    //    rec_bits(0)=1;    rec_bits(1)=1; //not converge, loop between edges in a vertex
    //perpendicular double error
    //    rec_bits(1+5)=1;    rec_bits(25+1)=1; //not converge, the error does not change, r vibirate around -600 or -300
	
    //rec_bits(1)=1;    rec_bits(25+2)=1; //same as above
    //vertically aligned double error
    //rec_bits(25+1)=1;    rec_bits(25+5+1)=1; // not converge but give the right result when seting delta K to be 30 or -30, need ieration max = 2500.
    //check logical error. weight 5 causes decoding failure, but weight 4 can be reduced to smaller weight, and can then be decoded.
    //rec_bits(0)=1;
    //rec_bits(5)=1;
    //rec_bits(10)=1;
    //rec_bits(16)=1;
      //rec_bits(20)=1;
      //rec_bits(21)=1;
      //rec_bits(16+25)=1;
      //rec_bits(21+25)=1;

    //check vertex operator. good result.
    //    rec_bits(5)=1;
    //    rec_bits(5+1)=1;
    //    rec_bits(25+1)=1;
    //    rec_bits(25+5+1)=1;
	

    //check for 743 code
    //rec_bits(0)=1;
    //rec_bits(1)=1;
    //rec_bits(2)=1;
    //rec_bits(4)=1;
    //rec_bits(5)=1;

    //    rec_bits(38-1)=1;
    //rec_bits(43-1)=1;
    
    //cout<<"rec_bits="<<rec_bits<<endl;
    vec s(N), s0(N); // input LLR version 
    QLLRvec llr_output;
    for(int i1=0;i1<N;i1++)
      s(i1)=s0(i1)=(rec_bits(i1)==1)?-log(1.0/pp-1.0):log(1.0/pp-1.0);//convert to input format for bp_decode  
    //cout<<"pp="<<pp<<"\t log(1.0/pp-1.0)="<<log(1.0/pp-1.0)<<endl;
    //cout<<"s="<<s<<endl;
  
    QLLRvec llr_input=C.get_llrcalc().to_qllr(s);
    //cout<<"llr_input="<<llr_input<<endl;
    ans=C.bp_decode(llr_input, llr_output);
    //cout<<"iteration: ans="<<ans<<", ";
    //    int bound= (int) 4096 * log (p/(1-p));// -300; see note.pdf on how to choose bound
    //    bound=0;
    //cout<<"bound value: "<<bound<<", below which means error"<<endl;
    bvec bitsout = llr_output < bound; //30;//0;//output

    //debug: check why bound increase the weight
    /*if (true){
      bvec bound0 = llr_output <0;
      bvec bounds = llr_output <-6000;
      int w = weight(bounds)-weight(bound0);
      if ( w >0){
	cout<<endl;
	cout<<"------------------------------------------------------------------------------------Find w < 0 "<<endl;
	cout<<endl;
      }
      }*/
    
    
    //bvec bitsout2 = llr < -bound;// for a check
    //it turns out that 0 is not a good choice, but 30 or -30 is a good choice. The idea is, when BP deesn't converge, it loop between some bonds in a cycle, with small K value close to zero. By choosing a critical value greater or smaller than those small value, the output bits will either include those bits or discard those bits.

    //    int max_iteration = 10;
    //int current

    //int max_repetition = 0;//5;//100; usually converge to zero with 3
    int current_iteration = ans;
    for (int i_iteration = 0;i_iteration<max_repetition;i_iteration++){
      if (ans >0){
	break;
      }
      // usleep(100000);
      //cout<<"iteration: "<<i_iteration<<", current_iteration = "<<current_iteration<<", ";
      //      cout<<"bits_out, bound = "<<bound<<endl;
      //      draw_toric_x_error(bitsout);
      //cout<<"effective output bits, bound = "<<bound<<endl;
      //draw_toric_x_error(rec_bits+bitsout+rec_bits0);

      //check value of K, to compare value of bound
      //cout<<"min abs in llr: "<<min(abs(llr_output))<<endl; //it is either large, or smaller than bound

      //repeat decoding
      //      C.get_llrcalc().to_qllr(s)
      llr_input = C.get_llrcalc().to_qllr( C.get_llrcalc().to_double(llr_output));//not sure if needed, just want to resign llr_output to llr_input
      ans = C.bp_decode(llr_input,llr_output);
      bitsout = llr_output < bound;
    }

    bitsout=bitsout+rec_bits+rec_bits0;//cancel input vector
    bitsout=reduce_weight( bitsout, G);//remove trivial cycles
    rec_bits=rec_bits0;//replace for compatibility for saving data
    
    if(ans>=0){
      if ( current_iteration < 0 ){
	//cout<<endl;
	//cout<<"-------------------------------make it converge after iteration."<<endl;
	//cout<<endl;
      }
      /*
      cout<<"llr="<<llr_output<<endl;
      cout<<"rec_bits="<<rec_bits<<endl;
      cout<<"bitsout ="<<bitsout<<endl;
      //cout<<"diff   = "<<bitsin+bitsout<<endl;
      cout<<"diff    ="<<rec_bits+bitsout<<endl;
      */
      //check min weight of bitsout?
      E_input_converge = append_vector(E_input_converge, rec_bits);
      E_output_converge = append_vector(E_output_converge, bitsout);//maybe all zero, but no harm to check            
      counts_converge++;
    }else{
      /*
       cout<<"not converge when i="<<i<<endl
	  <<"rec_bits= "<<rec_bits<<endl
	  <<"bitsout = "<<bitsout<<endl;
      cout<<"diff    ="<<rec_bits+bitsout<<endl;
      cout<<"llr = "<<llr_output<<endl;
      cout<<"NC, \# of errors in rec_bits = "<<BERC::count_errors(bitsin,rec_bits);
      cout<<", \# of errors in bitsout  = "<<BERC::count_errors(bitsin,bitsout)<<endl;
      */

      E_input_nonconverge = append_vector(E_input_nonconverge,rec_bits);
      E_output_nonconverge = append_vector(E_output_nonconverge,bitsout);//this would be the input error for the random window decoder.
      // break;//break here to just print the first non converged error
    }
    //    cout<<"rec_bits0"<<endl;
    //draw_toric_x_error(rec_bits0);
    /*
cout<<"rec_bits"<<endl;
    draw_toric_x_error(rec_bits);
    cout<<"bits_out, bound = "<<bound<<endl;
    draw_toric_x_error(bitsout);
    //    cout<<"bits_out2, bound = "<< -bound<<endl;
    //draw_toric_x_error(bitsout2);
    cout<<"iteration  ans = "<<ans<<endl;
    */
  }
  //cout<<"counts_converge="<<counts_converge<<endl;
  double rate_converge=counts_converge*1.0/cycles;
  cout<<"N = "<<N<<" =  2 x "<< N/2;
  cout<<", p = "<<p;
  cout<<", Converge rate ="<<rate_converge<<endl;
  //  save_result(p,rate_converge,filename_result_p);//no need to save this. can get it by counting the size of the matrix when doing gnuplot
  
  timer.toc_print();
  return 0;
}
   
