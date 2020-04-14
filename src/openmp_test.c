#include <stdio.h>
#include <iostream>
#include <omp.h>
#include <ctime> //for timer

#include <chrono> //for wall time, using c++11

void myFunc(int t=1){
  std::cout<<"start t = "<<t<<std::endl;
  for ( int j = 0; j<10000*(t+3); j++){
      for ( int k = 0; k <10000; k++){
	int a = 1;
	a++;
	a--;
      }
    }
  std::cout<<"                  end t = "<<t<<std::endl;
    return;
}

void test1(){

    #pragma omp parallel for num_threads(32)
  //  #pragma omp parallel for
  for (int i = 0; i < 40; i++)
    { 
	myFunc(i);
    }
  return;
}

void test2(){
  int i=0;
  #pragma omp parallel num_threads(100)
  {
    //    printf("https://helloacm.com\n");     
    //    #pragma omp critical 
    {
      i++;
      std::cout<<i<<std::endl;
    }
    //long block
    myFunc();

    int id = omp_get_thread_num();
    int data = id;
    int total = omp_get_num_threads();
    printf("Greetings from process %d out of %d with Data %d\n", id, total, data);

  }
    std::cout<<"final "<<i<<std::endl;
  return;
}

int main() {

  auto wcts = std::chrono::system_clock::now();

  std::clock_t start;
  double duration;

  start = std::clock();

  /* Your algorithm here */
  test1();


  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

  std::cout<<"duration: "<< duration <<" secs\n";

  //  do_some_fancy_stuff();
  std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wcts);
  std::cout << "Finished in " << wctduration.count() << " seconds [Wall Clock]" << std::endl;

  return 0;

}
