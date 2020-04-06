
// CPP program to demonstrate multithreading
// using three different callables.
#include <future>
#include <iostream>
#include <thread>
#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include "my_lib.h"
#include <math.h>

using namespace std;
using namespace itpp;


// A dummy function
void foo(int Z)
{
  for (int i = 0; i < Z; i++) {
            cout << "Thread using function"
	      " pointer as callable\n";
  }
}

// A callable object
class thread_obj {
 public:
  void operator()(int x)
  {
    for (int i = 0; i < x; i++)
                  cout << "Thread using function"
		    " object as  callable\n";
  }
};

//cannot compile unless it is in main()
/*
int thread_test()
{
      cout << "Threads 1 and 2 and 3 "
	"operating independently" << endl;

      // This thread is launched by using
      // function pointer as callable
      thread th1(foo, 3);

      // This thread is launched by using
      // function object as callable
      thread th2(thread_obj(), 3);

      // Define a Lambda Expression
      auto f = [](int x) {
	for (int i = 0; i < x; i++)
	              cout << "Thread using lambda"
			" expression as callable\n";
      };

      // This thread is launched by using
      // lamda expression as callable
      thread th3(f, 3);

      pthread_join(th1, NULL);
      pthread_join(th2, NULL);
	
      // Wait for the threads to finish
      // Wait for thread t1 to finish
      //th1.join();

      // Wait for thread t2 to finish
      //th2.join();

      // Wait for thread t3 to finish
      th3.join();

      return 0;
} 
*/

int bp_test(){
  cout<<"Test the bp_syndrome_llr function"<<endl;
  GF2mat H = get_check(3,7);
  //  GF2mat H = get_check(2,7);
  H = H.get_submatrix(0,0,2,6);
  cout<<"parity check H = \n"<<H<<endl;
  bvec error = zeros_b(H.cols());
  error.set(0,1);
  error.set(1,1);
  error.set(2,1);
  cout<<"input error ="<<error<<endl;
  bvec syndrome = (H*error);
  cout<<"syndrome = "<<syndrome<<endl;
  vec LLRin(H.cols());
  double p =0.1;
  double LLR=log( (1-p)/p);
  LLRin.ones();
  LLRin = LLRin*LLR;    
  vec LLRout(LLRin);//default, zero syndrome give zero error
  //LLRout.zeros();
  cout<<"LLRin  = "<< LLRin<<endl;
  //  cout<<"LLRout = "<< LLRout<<endl;
  int exit_iteration = 5;
  int iteration = bp_syndrome_llr(H,syndrome,LLRin, LLRout, exit_iteration);
  cout<<" iteration = "<<iteration <<endl;
  cout<<" LLRout = "<<LLRout<<endl;
  bvec bits_out = LLRout < 0;
  cout<<"bits_out = "<<bits_out<<endl;
  cout<<"error    = "<<error<<endl;
  bvec error_residue = error + bits_out;
  bvec syndrome_residue = H*error_residue;
  cout<<"error_residue = "<< error_residue <<endl;
  cout<<"syndrome_residue = "<< syndrome_residue <<endl;
  return 0;
}


int main(){
  //thread_test();
  bp_test();
  

}
