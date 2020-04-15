//updated on Dec 9.
//using optimizations (compared with bp_decoding3.c)
// bits_out = llr_output < bound,
// repeat decoding
//the input become syndrome, not the error vector. (this should not be the difference or produce any differenc ein the result. If I have time, I will implement it in bp_decoding3.c
//exit at 200 ant repeat 5 times

//In order to change to z error only, there is no change needed for this code. Just use the new generating matrix. 2/10/2018
//Compared to bp_decoding2.c, in this file,I save all data for errors that converge or not converge in bp decoding.
//In this file, for toric code, we use stabilizer S=(S_x,S_z).
//errors are in the format e=(e_z,e_x). (bistin, recbits, bitsout) //not checked excatly, based on the assumption that the BP decoding in itpp doesnot know the difference between X and Z errors and operators.
//hello world

#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include <sstream>
#include <stdio.h>
#include "my_lib.h"
using namespace std;
using namespace itpp;



// Read the code from files and do BP decoding
//input:source file for stabilzier matrix; error propability p ;

int main(int argc, char **argv){
  char * filename_G=argv[1]; GF2mat G =MM_to_GF2mat(filename_G);
  char * filename_H=argv[2]; GF2mat H =MM_to_GF2mat(filename_H);
  char * filename_result=argv[3];//prefix for the file
  double p=atof(argv[4]);
  p=p/100000.0;//previous use 1000 division. Now use 100,000 division cause the thershold for toric codes seems to be around 0.1%.
  //  cout<<"prob p ="<<p<<endl;
  //cout<<"input file -->"<<filename_G<<endl;

  //parameter setup
  int cycles=3000;//10000;//number of cycles: fro toric code, 10000 give reletively clear result
  int exit_at_iteration=50;//parameter for bp decoding set_exit_condition()
  //  int bound= (int) -4096 * log (p/(1-p));// -300; see note.pdf on how to choose bound
  int bound = 0;
  int max_repetition = 20;//10 for best result. supposed to be zero in our set up, just for a check
  

  
  char filename_result_p[255];
  sprintf( filename_result_p,"%s%.5f",filename_result,p);//append p in the file name
  
  Real_Timer timer;

  LDPC_Code C=GF2mat_to_LDPC_Code(H);  //convert GF2mat saved in .mm file to LDPC_Code
  //    LDPC_Code C=MM_to_LDPC_Code(filename_G);  //convert GF2mat saved in .mm file to LDPC_Code
  //LDPC_Code C = get_test_LDPC_Code();
  
  //LDPC_Code C(filename_G);//load code if saved in .it file with LDPC_Code format
  //vec pvals = "0.01:0.01:0.1"; p.get(pvals,"pvals");// start:increment:end=inclusive

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
	cout<<endl;
	cout<<"-------------------------------make it converge after iteration."<<endl;
	cout<<endl;
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
  
  //save errors to files
  char filename_result_ic[255];
  sprintf( filename_result_ic,"%s%.5f_input_converge",filename_result,p);//append p to the file name
  //cout<<E_input_converge.rows()<<endl;
  GF2mat_to_MM(E_input_converge,filename_result_ic);  
    
  char filename_result_in[255];
  sprintf( filename_result_in,"%s%.5f_input_nonconverge",filename_result,p);//append p to the file name
  GF2mat_to_MM(E_input_nonconverge,filename_result_in);  

  char filename_result_oc[255];
  sprintf( filename_result_oc,"%s%.5f_output_converge",filename_result,p);//append p to the file name
  GF2mat_to_MM(E_output_converge,filename_result_oc);  

  char filename_result_on[255];
  sprintf( filename_result_on,"%s%.5f_output_nonconverge",filename_result,p);//append p to the file name
  GF2mat_to_MM(E_output_nonconverge,filename_result_on);  
  
  //  cout<<E_input_nonconverge<<endl;
  // cout<<E_input_converge<<endl;
  timer.toc_print();
  return 0;
}
   
