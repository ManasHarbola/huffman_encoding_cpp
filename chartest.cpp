#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main(){
    //int c;

    ofstream file("outputtest.bin");

    //auto str = "ab\0c"s;
    string str = "ab";
    cout << "size: " << str.size() << endl;
    file << str;
    //file << "\0"s;
    file << (unsigned char) 126;
    file << "c";
    file.close();

    ifstream infile("outputtest.bin");
    unsigned char c;
    infile >> c;
    cout << c;
    infile >> c;
    cout << c;
    infile >> c;
    cout << (int) c << endl;
    cout << c << endl;

    
    //infile.read(&c, 1);
    //cout << c;
    return 0;

}