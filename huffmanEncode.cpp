#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include "huffmanlib.h"

using namespace std;
typedef unsigned char byte;


int main(){
    
    string filename = "test.txt";
    /*
    dataPkg encodingInfo;

    //Count frequencies of symbols in file
    unordered_map<unsigned char, unsigned long int> freq = countFreq(filename);
    encodingInfo.uniqueChars = freq.size();

    //Make vector of charNodes
    vector<charNode> nodes = makeNodes(freq);
    //freq.clear();
    
    //Generate huffman tree
    charNode* head = makeHuffmanTree(nodes, encodingInfo.uniqueChars);
    
    //Create map of huffman codes
    getCodes(head, encodingInfo.huffmanCodes);
    string encoded = encodeTree(head);
    
    //Record header and msg sizes in bits to make data buffer calculations
    encodingInfo.header_sz = getHeaderSize(encodingInfo.uniqueChars);
    encodingInfo.msg_sz = getMsgSize(freq, encodingInfo.huffmanCodes);
    */
    /*
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

    
    HuffmanData obj(filename);


    cout << "Original file size: " << obj.originalSz << " bytes" << endl;
    //cout << "Encoded size: " << encodingInfo.header_sz + encodingInfo.msg_sz << " bits" << endl;
    //byte* data = dataBuffer(head, filename, encodingInfo);
    byte* data = obj.buffer;
    cout << "Compressed file size: " << obj.compressedSz << " bytes" << endl;

    for (auto p : obj.huffmanCodes){
        cout << p.first << " " << p.second << endl;
    }

    for (int i = 0; i < obj.compressedSz; i++){
        cout << (int) data[i] << " ";
    }
    cout << endl;
    //print2D(head);


    return 0;
}