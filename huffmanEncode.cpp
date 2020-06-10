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
    //Check # of args
    if (argc != 4){
        cout << "Error: Invalid # of arguments" << endl;
        helpMsg(); 
        return 1;
    }
    //Check if args are valid
    if (string(argv[1]) != "-c" && string(argv[1]) != "-d"){
        cout << "Error: Invalid arguments" << endl;
        helpMsg();
        return 2;
    }
    //Check if file exists if being compressed of decompressed
    if (!ifstream(argv[2])){
        cout << "Error: file '" << string(argv[2]) << "' does not exist" << endl;
        helpMsg();
        return 3;
    }

    string inFilename = string(argv[2]);
    string outFilename; 

    if (string(argv[1]) == "-c"){
        outFilename = string(argv[3]) + ".huff"; 
        HuffmanEncode obj(inFilename);

        cout << endl << "Original file size: " << obj.originalSz << " bytes" << endl;
        cout << "Compressed file size: " << obj.compressedSz << " bytes" << endl;
        cout << endl << "Huffman Codes:" << endl << endl;

        for (pair<byte, string> p : obj.huffmanCodes){
            cout << p.first << ": " << p.second << endl;
        }

        byte* data = obj.buffer;

        /* 
        for(int i = 0; i < obj.compressedSz; i++){
            cout << (int) data[i] << " ";
        }
        */

        obj.encodeFile(outFilename);
        cout << endl << "'" << outFilename << "' successfully written" << endl;
        delete[] data;
    }

    else if (string(argv[1]) == "-d"){
        outFilename = string(argv[3]);
        HuffmanDecode obj(inFilename);
        obj.decodeFile(outFilename);
        cout << endl << "'" << outFilename << "' successfully written" << endl;
    }

    return 0;
}