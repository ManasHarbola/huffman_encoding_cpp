#include "huffmanlib.h"

using namespace std;

void helpMsg(){
    cout << "Usage:\n"
         << "\tTo compress a file:\n"
         << "\t./huffmanEncode -c [file to compress] [name of compressed file]\n"
         << "\n\tTo decompress a file:\n"
         << "\t./huffmanEncode -d [file to decompress] [name of decompressed file]\n";
}

int main(int argc, char* argv[]){
    if (argc != 4){
        cout << "Error: Invalid # of arguments" << endl;
        helpMsg(); 
        return 1;
    }

    if (string(argv[1]) != "-c" && string(argv[1]) != "-d"){
        cout << "Error: Invalid arguments" << endl;
        helpMsg();
        return 2;
    }

    if (!ifstream(argv[2])){
        cout << "Error: file '" << string(argv[2]) << "' does not exist" << endl;
        helpMsg();
        return 3;
    }

    string inFile = string(argv[2]);
    string outFile = string(argv[3]) + ".huff";

    
    /*
    //testcase:
    charNode* t = new charNode();
    t->left = new charNode();
    t->left->left = new charNode();
    t->left->left->left = new charNode();
    t->left->left->left->symbol = 'a';
    t->left->left->right = new charNode();
    t->left->left->right->symbol = 'b';
    t->left->right = new charNode();
    t->left->right->symbol = 'c';
    t->right = new charNode();
    t->right->left = new charNode();
    t->right->left->symbol = 'd';
    t->right->right = new charNode();
    t->right->right->symbol = 'e';
    */

    if (string(argv[1]) == "-c"){
        HuffmanEncode obj(inFile);

        cout << endl << "Original file size: " << obj.originalSz << " bytes" << endl;
        cout << "Compressed file size: " << obj.compressedSz << " bytes" << endl;
        cout << endl << "Huffman Codes:" << endl << endl;

        for (auto p : obj.huffmanCodes){
            cout << p.first << ": " << p.second << endl;
        }

        byte* data = obj.buffer;

        ///* 
        for(int i = 0; i < obj.compressedSz; i++){
            cout << (int) data[i] << " ";
        }
        //*/

        obj.writeFile(outFile);
        cout << endl << "'" << outFile << "' successfully written" << endl;
        delete[] data;
    }

    else if (string(argv[1]) == "-d"){
        //code for decompression goes here
    }

    return 0;
}