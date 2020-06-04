#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

int main(){
    struct t
    {
        int first = 0;
        int second = 0;
    };
    /*
    ifstream inFile("out.huff.huff");
    if (inFile){
        const auto begin = inFile.tellg();
        inFile.seekg(0, ios::end);
        const auto end = inFile.tellg();
        const auto fsize = (end - begin);
        
        char c;
        while (inFile.get(c)){
            cout << (int) (static_cast<unsigned char>(c)) << " " << endl;
        }

        inFile.close();

        cout << "File size: " << fsize << " bytes" << endl;
    }
    */
    /*
    char val = -2;
    unsigned char b = 0;
    b = static_cast<unsigned char>(val);


    cout << static_cast<int>(val) << endl;
    cout << static_cast<int>(b) << endl;
    */

    vector<t> v;
    v.push_back({1,2});

    cout << v.at(0).first << " " << v.at(0).second << endl;
    return 0;
}