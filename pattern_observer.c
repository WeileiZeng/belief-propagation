//Dec 11, modifief from bp_decoding4
//to show how error pattern evolves in bp_decoding, frame by frame (iteration by iteration)

#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include <sstream>
#include <stdio.h>
#include "my_lib.h"
#include <unistd.h> //usleep(ms)
using namespace std;
using namespace itpp;

/*

string color_text(string str){
  //return text in red color
  string color = "\033[1;31m"+str+"\033[0m";
  return color;
}
int draw_toric_x_error(bvec error_bits){
  //draw ( but not print) error for the toric code
  //in stabilizer generating matrix, we have vertex for X and plaquette for Z
  //in this print, we print X error first and then Z errors to the right
  //use plaquette to check X error and vertex to check Z errors
  int N=error_bits.length();
  int n=(int) sqrt(N/2);
  //cout<<"n="<<n<<endl;
    //      <<"N="<<N<<endl;

  //first horizontal bonds
  cout<<" ";
  for ( int j=0; j<n; j++){
      if (error_bits.get(n*0+j)){
	cout<<color_text("_ ");
      }else{
	cout<<"_ ";
      }
  }
  cout<<endl;
  for (int i=1;i<n;i++){
    for ( int j=0; j<n; j++){
      if (error_bits.get(n*(i-1)+n*n+j)){
	cout<<color_text("|");//"1";//for error bits
      }else{
	cout<<"|";
      }
      if (error_bits.get(n*i+j)){
	cout<<color_text("_");
	//for error bits
      }else{
	cout<<"_";
      }
    }
    cout<<endl;
  }
  //the last row of vertical bonds
  for ( int j=0; j<n; j++){
    if (error_bits.get(n*(n-1)+n*n+j)){
      //      std::cout<<"\033[0;32m 1 \033[0m";
      cout<<color_text("| ");
      }else{
	cout<<"| ";
      }
    }
    cout<<endl;
  return 0;

}
*/

//for a check
/*
GF2mat get_check_code734(int L){//L=7n
  //return check matrix code code [7,3,4], find definition in research note.pdf
  //this return a redundant sqaure matrix, maybe unnecessary
  GF2mat G(L,L);
  for ( int i=0;i<L;i++){
    G.set(i,i,1);
    if (i+2>L-1) {
      G.set(i,i+2-L,1);
    } else {
      G.set(i,i+2,1);
    }
    if (i+3>L-1) {
      G.set(i,i+3-L,1);
    }else{
      G.set(i,i+3,1);
    }
  }
  return G;
}

//for check
LDPC_Code get_test_LDPC_Code(){
  //convert GF2mat saved in .mm file to LDPC_Code
  //  GF2mat G=MM_to_GF2mat(filename);
  GF2mat G = get_check_code734(7*2);
  GF2mat_sparse Gs=G.sparsify();
  GF2mat_sparse_alist Gsa;
  Gsa.from_sparse(Gs);
  LDPC_Parity H(Gsa);
  LDPC_Code C(&H);
  return C;
}


LDPC_Code MM_to_LDPC_Code(char * filename){
  //convert GF2mat saved in .mm file to LDPC_Code
  GF2mat G=MM_to_GF2mat(filename);
  GF2mat_sparse Gs=G.sparsify();
  GF2mat_sparse_alist Gsa;
  Gsa.from_sparse(Gs);
  LDPC_Parity H(Gsa);
  LDPC_Code C(&H);
  return C;
}

int save_result(double p, double converge_rate, char * filename){
  mat mat_data(2,1);
  mat_data.set(0,0,p);
  mat_data.set(1,0,converge_rate);
  mat_to_MM(mat_data,filename);//dosen't work here and not sure why. error recorded in mm_write.c
  return 0;
}
*/
//find another error with the same syndrome
//it is still wired to use the original error as an input
//result: it is okay to use the original error as input. They are equivalent if we find another error with same syndrome. That error will be substracted eventually, and there "should" be no difference in a BP decoder in these two cases.
/*bvec error_transform(bvec e_in, GF2mat H){
  //input: original error and parity check matrix
  int L = (int) sqrt( H.cols()/2); //side length of toric code
  //now, just do a munual change, add a vertex operator.
  bvec e_transform(e_in);
  bin b1=1;
  e_transform(0)=b1-e_transform(0);
  e_transform(1)=b1-e_transform(1);
  int pos=7*7+1;
  e_transform(pos)=b1-e_transform(pos);
    
  //e_transform(L*L-2)=1;//b1-e_transform(L*L+2);
  e_transform(L*L*2-L+1)=b1-e_transform(L*L*2-L+1);
  return e_transform;

}
*/
/*
bvec find_error(bvec e_in, GF2mat H){
  //input: original error and parity check matrix
  //output: an error with same syndrome
  //int L = (int) sqrt( H.cols()/2); //side length of toric code
  //  bvec syndrome=H*e_in;

  GF2mat H0=H;
  bvec e_t = e_in;//change name
  bvec s=H*e_t;//syndrome s=s_x;//(s_x,s_z);
  
  H.set_size(H.rows(),H.cols()+1,true);//H=(H_x,s)//H=(H_x,H_z,s)  
  H.set_col(H.cols()-1,s);//add syndrome
  //cout<<"parity check matrix H=(S_x,S_z,s). The last column is the syndrome. "<<H<<endl;

  //  bvec e_d=zeros_b(e_t.length() );//error detected
  GF2mat T,U;
  ivec P;
  H.transpose().T_fact(T,U,P);
  GF2mat Q=T.get_submatrix(H.rows(),0,H.cols()-1,H.cols()-1);
  bvec X_t=Q.get_row(Q.rows()-1);//the error detected, X_t=(X_z,X_x,1)
  X_t.set_size(X_t.size()-1,true);
  //  cout<<H*Q.transpose()<<endl;
  //    cout<<e_t;
  cout<<"e_in  "<<e_in<<endl;
  cout<<"X_t   "<<X_t<<endl;
  cout<<"s     "<<s<<endl;
  cout<<"H0*X_t"<<H0*X_t<<endl;
  //cout<<T<<endl;
  //cout<<Q<<endl;
  
  
  
  //cout<<"size "<<X_t.size()<<endl;
  
  return X_t;
  }*/

// Read the code from files and do BP decoding
//input:source file for stabilzier matrix; error propability p ;

int main(int argc, char **argv){
  char * filename_G=argv[1]; GF2mat G =MM_to_GF2mat(filename_G);
  char * filename_H=argv[2]; GF2mat H =MM_to_GF2mat(filename_H);
  char * filename_result=argv[3];//prefix for the file
  double p=atof(argv[4]);
  p=p/100000.0;//previous use 1000 division. Now use 100,000 division cause the thershold for toric codes seems to be around 0.1%

  //parameter setup
  int cycles=1;//10000;//number of cycles: fro toric code, 10000 give reletively clear result
  int exit_at_iteration=999;//parameter for bp decoding set_exit_condition()
  //  int bound= (int) -4096 * log (p/(1-p));// -300; see note.pdf on how to choose bound
  int  bound = 0;
  int max_repetition = 0;//10 for best result. supposed to be zero in our set up, just for a check       
  
  /*char * filename_G=argv[1];
  char * filename_result=argv[2];//prefix for the file
  double p=atof(argv[3]);
  p=p/100000.0;//previous use 1000 division. Now use 100,000 division cause the thershold for toric codes seems to be around 0.1%.*/
  //cout<<"prob p ="<<p<<endl;
  // cout<<"input file -->"<<filename_G<<endl;
  
  char filename_result_p[255];
  sprintf( filename_result_p,"%s%.5f",filename_result,p);//append p in the file name
  
  Real_Timer timer;

  LDPC_Code C=GF2mat_to_LDPC_Code(H);  //convert GF2mat saved in .mm file to LDPC_Code
  //LDPC_Code C = get_test_LDPC_Code();
  
  //LDPC_Code C(filename_G);//load code if saved in .it file with LDPC_Code format
  //vec pvals = "0.01:0.01:0.1"; p.get(pvals,"pvals");// start:increment:end=inclusive

  //bp decoding set up
  C.set_exit_conditions(exit_at_iteration);//high perperformance. This would not affect small errors which converges and stop very fast. Only affect those doesn't converge soon or not converge at all.
  //  C.set_exit_conditions(50,true,true);  // 50 iterations,check syndrome always
  C.set_llrcalc(LLR_calc_unit(12,300,7)); //(recommended settings with "exact" computation via high resolution lookup table)
  //C.set_llrcalc(LLR_calc_unit(12,0,7)); // fast mode; see "llr.h" //not good resolution for the result, produce more errors
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
    //    bvec rec_bits = bitsin;   // input vector with manually-input errors, for test
    //    C.set_exit_conditions(1011);//,true, false);
    //1011 seems to be a min value for good result. there is no difference when increase to 10111, but the repeated decoding can reduce the error quickly. why?
    
    bvec rec_bits0 = bsc(bitsin);    // input vector with errors from bsc chanel
    //    rec_bits0(1+7)=1;    rec_bits0(49+1)=1; //not converge, the// error does not change, r vibirate around -600 or -300
    
    //draw_toric_x_error(rec_bits0);

    /*    //check logical error
    int L=(int) sqrt(N/2);
    bin bone=1;
    for (int i_logical=0;i_logical< L; i_logical++)
      //int i_logical = 1;
      rec_bits.set(i_logical*L, rec_bits(i_logical*L)? 0:1  );
    
    draw_toric_x_error(rec_bits);
    if (0)
      return 0;
    */
    
    //    bvec rec_bits1( error_transform(rec_bits0,MM_to_GF2mat(filename_G)) ); //for test

    //bvec rec_bits= find_error(rec_bits0,H) ;
    bvec rec_bits = rec_bits0;
    rec_bits.zeros();
    rec_bits0.zeros();

    switch ( 13) {
    case 1:
      rec_bits.set(0,1);
      rec_bits0.set(0,1);
      rec_bits.set(1,1);
      rec_bits0.set(1,1);
      break;
    case 11:
      rec_bits.set(63,1);
      rec_bits0.set(63,1);
      rec_bits.set(56,1);
      rec_bits0.set(56,1);
      break;
    case 12:
      rec_bits.set(49,1);
      rec_bits0.set(49,1);
      rec_bits.set(56,1);
      rec_bits0.set(56,1);
      break;
    case 13:
      rec_bits.set(35,1);
      rec_bits0.set(35,1);
      rec_bits.set(36,1);
      rec_bits0.set(36,1);
      break;
    }
    
    //    cout<<rec_bits<<endl;
    //cout<<rec_bits0<<endl;
    
    //manual input error for test
    //    rec_bits = bitsin; //zero vector
    //horizontally aligned double error 
    //rec_bits(0)=1;    rec_bits(1)=1; //not converge, loop between edges in a vertex
    //perpendicular double error
    //rec_bits(1+5)=1;    rec_bits(25+1)=1; //not converge, the error does not change, r vibirate around -600 or -300
	
    //rec_bits(1)=1;    rec_bits(25+2)=1; //same as above
    //vertically aligned double error
    //rec_bits(25+1)=1;    rec_bits(25+5+1)=1; // not converge but give the right result when seting delta K to be 30 or -30, need ieration max = 2500.
    //check logical error. weight 5 causes decoding failure, but weight 4 can be reduced to smaller weight, and can then be decoded.
    //rec_bits(0)=1;
    //rec_bits(5)=1;
    //rec_bits(10)=1;
    //    rec_bits(16)=1;
      //rec_bits(20)=1;
    //rec_bits(16+25)=1;
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
    vec s(N); // input LLR version 
    QLLRvec llr_output;
    for(int i1=0;i1<N;i1++)
      s(i1)=(rec_bits(i1)==1)?-log(1.0/pp-1.0):log(1.0/pp-1.0);//convert to input format for bp_decode  
    //cout<<"pp="<<pp<<"\t log(1.0/pp-1.0)="<<log(1.0/pp-1.0)<<endl;
    //cout<<"s="<<s<<endl;
  
    QLLRvec llr_input=C.get_llrcalc().to_qllr(s);
    //cout<<"llr_input="<<llr_input<<endl;
    ans=C.bp_decode(llr_input, llr_output);
    cout<<"iteration: ans="<<ans<<", ";
    //    int bound=(int) -4096 * log(p/(1-p));//a small positive delta value
    //    bound=0;
    cout<<"bound value: "<<bound<<", below which means error"<<endl;
    //cancel input vector. (flip the sign accordingly)
    //    for (int i_cancel = 0;i_cancel<llr_output.size();i_cancel++)
    //llr_output(i_cancel) = llr_output(i_cancel) * ( 1-(int) rec_bits(i_cancel)*2);
    //    bvec bits_bridge = llr_output < -300;
    
    //    bvec bitsout = llr_output < bound; //30;//0;//output
    bvec bitsout = qllr_to_bvec(llr_output , bound);
    //    bvec bitsout2 = llr < -bound;// for a check
    //it turns out that 0 is not a good choice, but 30 or -30 is a good choice. The idea is, when BP deesn't converge, it loop between some bonds in a cycle, with small K value close to zero. By choosing a critical value greater or smaller than those small value, the output bits will either include those bits or discard those bits.
    
    if(ans>=0){
      /*cout<<"llr="<<llr<<endl;
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
      /*cout<<"not converge when i="<<i<<endl
	  <<"rec_bits= "<<rec_bits<<endl
	  <<"bitsout = "<<bitsout<<endl;
      cout<<"diff    ="<<rec_bits+bitsout<<endl;
      cout<<"llr = "<<llr<<endl;
      cout<<"NC, \# of errors in rec_bits = "<<BERC::count_errors(bitsin,rec_bits);
      cout<<", \# of errors in bitsout  = "<<BERC::count_errors(bitsin,bitsout)<<endl;
      */

      E_input_nonconverge = append_vector(E_input_nonconverge,rec_bits);
      E_output_nonconverge = append_vector(E_output_nonconverge,bitsout);//this would be the input error for the random window decoder.
      // break;//break here to just print the first non converged error
    }
    cout<<"rec_bits"<<endl;
    draw_toric_x_error(rec_bits);
    //int max_iteration = 10;//100; usually converge to zero with 3
    int current_iteration = ans;
    for (int i_iteration = 0;i_iteration<max_repetition;i_iteration++){
      if (current_iteration > 0 ){
	cout<<"-------------------------------- ---------------------converge when i_iteration ="<<i_iteration<<endl;
	break;
      }
	
      //      usleep(100000);
      cout<<"iteration: "<<i_iteration<<", current_iteration = "<<current_iteration<<", ";
      //      cout<<"bits_out, bound = "<<bound<<endl;
      //      draw_toric_x_error(bitsout);
      //cout<<"effective output bits, bound = "<<bound<<endl;
      //draw_toric_x_error(rec_bits+bitsout+rec_bits0);

      //check value of K, to compare value of bound
      //cout<<"min abs in llr: "<<min(abs(llr_output))<<", max"<<max(abs(llr_output))<<", ratio"<<bound/max(abs(llr_output))<<endl; //it is either large, or smaller than bound
      
      //repeat decoding
      //      C.get_llrcalc().to_qllr(s)
      llr_input = C.get_llrcalc().to_qllr( C.get_llrcalc().to_double(llr_output));
      current_iteration = C.bp_decode(llr_input,llr_output);
      bitsout = llr_output < bound;
      if (current_iteration > 0 ){
	//cout<<"-------------------------------- ---------------------converge when i_iteration ="<<i_iteration<<endl;
	//	break;
      }
    }
    
    
    //cout<<"rec_bits"<<endl;
    //draw_toric_x_error(rec_bits);
    //cout<<"bits_out, bound = "<<bound<<endl;
    //draw_toric_x_error(bitsout);
    //    cout<<"bits_out2, bound = "<< -bound<<endl;
    //draw_toric_x_error(bitsout2);
    cout<<"iteration  ans = "<<current_iteration<<endl;

    //final check
    cout<<"rec_bits0"<<endl;
    draw_toric_x_error(rec_bits0);
    cout<<"sum"<<endl;
    draw_toric_x_error(rec_bits+bitsout+rec_bits0);
    cout<<"reduced sum (error remained after error correction)"<<endl;
    //    draw_toric_x_error(reduce_weight((rec_bits+bitsout+rec_bits0),G));
    //    cout<<"reduced weight = "<<weight(reduce_weight((rec_bits+bitsout+rec_bits0),G))<<endl;
    /*    bvec bits_bound = llr_output < -bound;
    cout<<"difference in +bound and -bound"<<endl;
    draw_toric_x_error(bitsout+bits_bound);*/
    //final check to find suitable value for bound
    //Sort<QLLRvec> ss;
    //    Vec<int> aa = (abs(llr_output));
    //ss.sort(0,llr_output.size(),llr_output);
    //    cout<<llr_output<<endl;
    cout<<"bound = "<<bound;
    cout<<", min abs in llr: "<<min(abs(llr_output))<<", max"<<max(abs(llr_output))<<", ratio"<<((double)bound)/max(abs(llr_output))<<endl; //it is either large, or smaller than bound
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
   
