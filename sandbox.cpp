#include <iostream>
using namespace std;
typedef unsigned char byte;

int main(){
    byte b = 0;
    byte mask = 128;
    b = 128;

    if ((int) (b&mask) == 0)
        cout << "bit is 0" << endl;
    else
    {
        cout << "bit is 1" << endl;
    }

    unsigned int i = 0;
    i = i << 10;
    cout << "here: " << i << endl;
    

    return 0;
}