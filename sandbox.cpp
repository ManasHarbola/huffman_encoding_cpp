#include <iostream>
#include <fstream>
using namespace std;

int main(){
    ifstream inFile("out.huff.huff");
    if (inFile){
        const auto begin = inFile.tellg();
        inFile.seekg(0, ios::end);
        const auto end = inFile.tellg();
        const auto fsize = (end - begin);
        
        /*
        char c;
        while (inFile.get(c)){
            cout << (int) (static_cast<unsigned char>(c)) << " " << endl;
        }
        */

        inFile.close();

        cout << "File size: " << fsize << " bytes" << endl;
    }

    return 0;
}