#include <iostream>
using namespace std;
typedef unsigned char byte;

int main(){
    char c = 255;
    byte b = static_cast<byte>(c);
    cout << (int) b<< endl;
    
    return 0;
}