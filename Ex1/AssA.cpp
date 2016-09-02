#include<iostream>
#include<new>

void allocate(int value, long long i)
{
  int *ptr = new int[1000000000];
  *ptr = value;
  std::cout << "Test of allocated memory: "<< i <<std::endl <<  *ptr << *(ptr+100000) << std::endl;
}

int main()
{
  try {
    long long i = 0;
    for (;;) {
      allocate(55, i++);
    }
  }
  catch (const std::bad_alloc& e){
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
