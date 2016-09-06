#include "LinkedList.h"


int main(int argc, char **argv)
{
    std::cout << "Create list\n";
    LinkedList list;

    std::cout << "append valued 0 to 9\n";
    for(int i=0; i<10; i++)
    {
        list.Append(i);
    }
    list.Print();

    std::cout << "List sum: " << list.GetSum() << std::endl;

    std::cout << "insert value in the middle of the list\n";
    list.Insert(5, 99);
    list.Print();

    std::cout << "Get inserted value: " << list.get_At(5) << std::endl;
    list.Print();

    std::cout << "Extract inserted value: " << list.Extract(5) << std::endl;
    list.Print();

    std::cout << "Remove all but the last value\n";

    while(list.get_Length() > 1){
        list.Extract(0);
    }
    list.Print();

    std::cout << "Remove the last value\n";
    list.Extract(0);
    list.Print();

    std::cout << "Delete list\n";
    list.Delete();

    return 0;
}