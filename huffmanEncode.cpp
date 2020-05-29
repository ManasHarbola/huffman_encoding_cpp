#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;
typedef unsigned char byte;

struct charNode{
    //char symbol = -1;
    unsigned char symbol;
    unsigned long int freq = 0;
    charNode *left = NULL, *right = NULL;
};

//Contains information needed to generate compressed huffman file
struct dataPkg{
    unsigned uniqueChars = 0;
    unsigned long int header_sz = 0;
    unsigned long int msg_sz = 0;
    unordered_map<unsigned char, string> huffmanCodes;
};

struct compare{
    bool operator()(const charNode* lhs, const charNode* rhs){
        if (lhs == NULL){
            return false;
        }
        else if (rhs == NULL){
            return true;
        }
        else{
            return lhs->freq > rhs->freq;
        }
    }
};


unordered_map<unsigned char, unsigned long int> countFreq(){
    unordered_map<unsigned char, unsigned long int> freq;

    ifstream file("test.txt");
    char c;
    while (file.get(c)){
        freq[c]++;
    }
    file.close();
    return freq;
}

vector<charNode> makeNodes(unordered_map<unsigned char, unsigned long int> freq){
    if (freq.size() == 0){
        return vector<charNode>();
    }
    else if (freq.size() == 1){
        vector<charNode> output(2);
        for (pair<unsigned char, unsigned long int> p : freq){
            output.at(0).symbol = p.first;
            output.at(0).freq = p.second;
            output.at(1).freq = p.second;
            output.at(1).left = &output.at(0);
        }
        return output;
    }
    else{
        vector<charNode> output(2 * freq.size() - 1);
        unsigned i = 0;
        for (pair<unsigned char, unsigned long int> p : freq){
            output.at(i).symbol = p.first;
            output.at(i).freq = p.second;
            i++;
        }
        return output;
    }
}

charNode* makeHuffmanTree(vector<charNode> &nodes, unsigned uniqueChars){
    if (uniqueChars == 0){
        return NULL;
    }
    else if (uniqueChars == 1){
        nodes.at(1).left = &nodes.at(0);
        nodes.at(1).freq = nodes.at(0).freq;
        return &nodes.at(1);
    }
    else{
        priority_queue<charNode*, vector<charNode*>, compare> nodeQueue;
        for (int i = 0; i < uniqueChars; i++){
            nodeQueue.push(&(nodes.at(i)));
        }

        int i = uniqueChars;
        while (nodeQueue.size() > 1){
            //Remove two smallest nodes
            charNode* tmp1 = nodeQueue.top();
            nodeQueue.pop();
            charNode* tmp2 = nodeQueue.top();
            nodeQueue.pop();
            
            //Create "links" between parent node and children
            nodes.at(i).left = tmp1;
            nodes.at(i).right = tmp2;
            //Sum frequencies of children nodes
            nodes.at(i).freq = tmp1->freq + tmp2->freq;
            nodeQueue.push(&nodes.at(i));
            i++;
        }
        //Head is always located at the last element in vector
        return &nodes.at(2 * uniqueChars - 1 - 1);
    }
}

//Assumes huffman tree is properly generated
void getCodes(charNode* head, unordered_map<unsigned char, string>& codes, string currCode=""){
    //Error Case when huffman tree is empty:
    if (head == NULL){
        return;
    }
    //Base case: Leaf node encountered
    if (head->left == NULL && head->right == NULL){
        codes[head->symbol] = currCode;
        return;
    }
    //Recursive cases:
    else{
        if (head->left != NULL){
            getCodes(head->left, codes, currCode + "0");
        }
        if (head->right != NULL){
            getCodes(head->right, codes, currCode + "1");
        }

    }
}

//Just represent binary layout of tree for now
void encodeTree(charNode* root, string &str){

    if (root->left == NULL && root->right == NULL){
        //str += ("1" + (string) (root->symbol));
        str += "1";
        str += root->symbol;
        return;
    }

    //Special Case: Huffman tree contains only one unique character
    else if (root->left != NULL && root->right == NULL){
        //str += "01";
        //str += root->left->symbol;
        str = "01" + root->left->symbol;
        return;
    }

    else{
        str += "0";
        encodeTree(root->left, str);
        encodeTree(root->right, str);
    }

}

void encodeTree2(charNode* root, string &str){
    vector<charNode*> stack;

}

/*
void encodeTree(charNode* root, vector<bool> &vec, unsigned int idx=0){

    if (root->left == NULL && root->right == NULL){
        //str += ("1" + (string) (root->symbol));
        str += "1";
        str += root->symbol;
        return;
    }

    //Special Case: Huffman tree contains only one unique character
    else if (root->left != NULL && root->right == NULL){
        //str += "01";
        //str += root->left->symbol;
        str = "01" + root->left->symbol;
        return;
    }

    else{
        //str += "0";
        vec.at(idx) = 0;
        encodeTree(root->left, vec, idx+1);
        encodeTree(root->right, vec, idx+2);
    }

}
*/

//Returns size of header, in bits
unsigned long int getHeaderSize(unsigned uniqueChars){
    return (2 * uniqueChars - 1) + (8 * uniqueChars);
}

//Returns size of encoded msg, in bits
unsigned long int getMsgSize(unordered_map<unsigned char, unsigned long int> &freq, unordered_map<unsigned char, string> &codes){
    unsigned long int sz = 0;
    
    for (pair<unsigned char, unsigned long int> p : freq){
        sz += p.second * codes[p.first].length();
    }

    return sz;
}


//Returns buffer of entire encoded data (header + message)
unsigned char* dataBuffer(ifstream& inFile, dataPkg& encodingInfo){
    unsigned long int sz_bytes = ((encodingInfo.header_sz + encodingInfo.msg_sz) / 8) + (int) ((encodingInfo.header_sz + encodingInfo.msg_sz) % 8 != 0);

    unsigned char* ref = new unsigned char[sz_bytes];

    //This is the byte we're going to use to store our memory through
    byte byte_frame = 0;

    unsigned long int i = 0;
    unsigned long int header_bytes = encodingInfo.header_sz / 8;
    unsigned int header_remainder = encodingInfo.header_sz % 8;
    
    while (header_bytes > 0){
        
        --header_bytes;
    }

    //Step 1: put header data into file




}

#define COUNT 10

void print2DUtil(charNode *root, int space)  
{  
    // Base case  
    if (root == NULL)  
        return;  
  
    // Increase distance between levels  
    space += COUNT;  
  
    // Process right child first  
    print2DUtil(root->right, space);  
  
    // Print current node after space  
    // count  
    cout<<endl;  
    for (int i = COUNT; i < space; i++)  
        cout<<" ";  
    
    //if ((int) root->symbol != -1 && root->symbol != '\n')
    if (root->left == NULL and root->right == NULL && root->symbol != '\n')
        cout<<"(" << root->freq << ", " << root->symbol << ")" <<"\n";  
    //else if ((int) root->symbol == -1)
    else if (root->left != NULL && root->right != NULL)
        cout<<"(" << root->freq << ", " << "[PARENT]" << ")" <<"\n";
    else
        cout<<"(" << root->freq << ", " << "\\n" << ")" <<"\n";  

    // Process left child  
    print2DUtil(root->left, space);  
}  
  
// Wrapper over print2DUtil()  
void print2D(charNode *root)  
{  
    // Pass initial space count as 0  
    print2DUtil(root, 0);  
}  
  

int main(){
    dataPkg encodingInfo;

    //Count frequencies of symbols in file
    unordered_map<unsigned char, unsigned long int> freq = countFreq();
    encodingInfo.uniqueChars = freq.size();

    //Make vector of charNodes
    vector<charNode> nodes = makeNodes(freq);
    //freq.clear();
    
    //Generate huffman tree
    charNode* head = makeHuffmanTree(nodes, encodingInfo.uniqueChars);
    
    //Create map of huffman codes
    getCodes(head, encodingInfo.huffmanCodes);
    string encoded = "";
    encodeTree(head, encoded);
    
    //Record header and msg sizes in bits to make data buffer calculations
    encodingInfo.header_sz = getHeaderSize(encodingInfo.uniqueChars);
    encodingInfo.msg_sz = getMsgSize(freq, encodingInfo.huffmanCodes);

    vector<bool> encodedHeader(encodingInfo.header_sz);
    encodeTree(head, encodedHeader);


    /*
    charNode* test = new charNode();
    test->left = new charNode();
    test->left->left = new charNode();
    test->left->left->left = new charNode();
    test->left->left->left->symbol = 'a';
    test->left->left->right = new charNode();
    test->left->left->right->symbol = 'b';
    test->left->right = new charNode();
    test->left->right->symbol = 'c';
    test->right = new charNode();
    test->right->left = new charNode();
    test->right->left->symbol = 'd';
    test->right->right = new charNode();
    test->right->right->symbol = 'e';
    //test->left = new charNode();
    //test->left->symbol = 'a';
    encoded = "";
    encodeTree(test, encoded);
    */


    cout << encoded << endl;

    cout << "Header size: " << getHeaderSize(encodingInfo.uniqueChars) << " bits" << endl;
    
    //print2D(head);


    
    return 0;
}