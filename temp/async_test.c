// async example
#include <iostream>       // std::cout
#include <future>         // std::async, std::future

// a non-optimized way of checking for prime numbers:

 	  
bool is_prime (int x) {
  std::cout << "wait..."<<x<<"\n";
  for ( int i = 0; i<x; i++) double a=100*15-1/4.1+133.1/2.77/3.88*i+1.3*i;
  for (int i=2; i<x; ++i) if (x%i==0) return false;
  return true;
}

int get_prime(int y){
  //return largest prime within y
  for ( int i = y; i > 1; i--){
    if (is_prime(i)) return i ;
  }
  return 1;
}

int main ()
{
  // call is_prime(313222313) asynchronously:
  //std::future<bool> fut1 = std::async (is_prime,3215031751);
  //std::future<bool> fut2 = std::async (is_prime,87178291199);
  std::future<int> fut3 = std::async (std::launch::async,get_prime,871782922);
  std::future<int> fut4 = std::async (std::launch::async,get_prime,871782920);
  int n = 40;
  std::future<int> fut[n];
  for ( int i =0;i<n;i++){
    fut[i] = std::async (std::launch::async,get_prime,71782918);
  }

  int num = fut3.get();
  
  std::cout<<"it is "<<num<<"\n";
  num = fut4.get();

  std::cout<<"it is "<<num<<"\n";

  /*
  std::cout << "Checking whether 313222313 is prime.\n";
  // ...

  bool ret = fut2.get();      // waits for is_prime to return
  if (ret) std::cout << "It is prime!\n";
  else std::cout << "It is not prime.\n";

  ret = fut1.get();      // waits for is_prime to return
  if (ret) std::cout << "It is prime!\n";
  else std::cout << "It is not prime.\n";
  */
  return 0;
}
