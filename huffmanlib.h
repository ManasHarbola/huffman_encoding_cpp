#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

typedef unsigned char byte;
#define NUM_NODES 511

struct charNode{
    //char symbol = -1;
    unsigned char symbol;
    unsigned long int freq = 0;
    charNode *left = NULL, *right = NULL;
};

struct compare
{
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

void print2D(charNode*);
void print2DUtil(charNode*, int);


class HuffmanEncode
{
public:
    unsigned uniqueChars = 0;
    unsigned long int headerSz = 0;
    unsigned long int msgSz = 0;
    unsigned long int originalSz = 0;
    unsigned long int compressedSz = 0;
    byte* buffer = NULL;
    unordered_map<byte, string> huffmanCodes;

    HuffmanEncode(string);
    ~HuffmanEncode();

    unordered_map<byte, unsigned long int> countFreq(string);
    vector<charNode> makeNodes(unordered_map<byte, unsigned long int>&);
    charNode* makeHuffmanTree(vector<charNode>&);
    void getCodes(charNode*, string = "");

    unsigned long int getHeaderSize();
    unsigned long int getMsgSize(unordered_map<byte, unsigned long int>&);

    byte* dataBuffer(charNode*, string);
    void encodeFile(string);

    //static void print2DUtil(charNode *root, int space);
    //static void print2D(charNode *root);  

};

HuffmanEncode::HuffmanEncode(string filename){
    unordered_map<byte, unsigned long int> freq = this->countFreq(filename);
    this->uniqueChars = freq.size();

    vector<charNode> nodes = this->makeNodes(freq);
    charNode* head = this->makeHuffmanTree(nodes);

    //Stores huffman codes into map 
    this->getCodes(head);

    this->headerSz = this->getHeaderSize();
    this->msgSz = this->getMsgSize(freq);

    for (pair<byte, unsigned long int> p : freq){
        this->originalSz += p.second;
    }

    this->buffer = this->dataBuffer(head, filename);

    //this->print2D(head);
    print2D(head);
}

HuffmanEncode::~HuffmanEncode(){
    ;
}

class HuffmanDecode
{
public:
    //Name of input compressed file
    string inputFileName = "";

    //size of compressed file in, bytes
    unsigned long int compByteSz = 0;

    //decimal value of last 3 bits in file
    unsigned int last3bits = 0;

    //size of header in bits
    unsigned long int headerBitSz = 0;
    
    //number of unique characters
    unsigned long int uniqueChars = 0;

    charNode nodes[NUM_NODES];
    charNode* head = NULL;

    //charNode* head = nodes;

    HuffmanDecode(string);
    ~HuffmanDecode();

    //Returns size of compresed file (in bytes) and value of last 3 bits of file
    pair<unsigned long int, unsigned int> getCompFileSz(); 
    
    //Get reconstructed Huffman Tree from reading header portion of file, puts charNodes into this->nodes
    void getHuffmanTree();

    //Writes file to specified filename using ifstream object
    void decodeFile(string);
};

HuffmanDecode::HuffmanDecode(string filename)
{
    this->inputFileName = filename;
    pair<unsigned long int, unsigned int> p = this->getCompFileSz();
    this->compByteSz = p.first;
    this->last3bits = p.second;
    //p no longer needed
    p.~pair();

    getHuffmanTree();

    if (this->uniqueChars > 0){
        this->head = &this->nodes[0];
        this->headerBitSz = (2 * this->uniqueChars - 1) + (8 * this->uniqueChars);
    }
    //print2D(this->head);
    //cout << this->headerBitSz << endl;
}

HuffmanDecode::~HuffmanDecode()
{
}

unordered_map<byte, unsigned long int> HuffmanEncode::countFreq(string filename){
    unordered_map<unsigned char, unsigned long int> freq;
    ifstream file(filename.c_str());
    char c;
    while (file.get(c)){
        freq[c]++;
    }
    file.close();
    
    return freq;
}

unsigned long int HuffmanEncode::getHeaderSize(){
    if (this->uniqueChars == 1){
        return 3 + 16;
    }

    else{
        return (2 * this->uniqueChars - 1) + (8 * this->uniqueChars);
    }
}

unsigned long int HuffmanEncode::getMsgSize(unordered_map<byte, unsigned long int>& freq){
    unsigned long int sz = 0;
    
    for (pair<unsigned char, unsigned long int> p : freq){
        sz += p.second * this->huffmanCodes[p.first].length();
    }

    return sz;
}

vector<charNode> HuffmanEncode::makeNodes(unordered_map<byte, unsigned long int>& freq){
    if (freq.size() == 0){
        return vector<charNode>();
    }
    /*
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
    */
    else{
        if(freq.size() == 1){
            freq[static_cast<byte>(0)] = 0;
        }

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

charNode* HuffmanEncode::makeHuffmanTree(vector<charNode>& nodes){
    if (this->uniqueChars == 0){
        return NULL;
    }
    else if (this->uniqueChars == 1){
        nodes.at(2).left = &nodes.at(0);
        nodes.at(2).right = &nodes.at(1);
        nodes.at(2).freq = nodes.at(0).freq + nodes.at(1).freq;
        return &nodes.at(2);
    }
    else{
        priority_queue<charNode*, vector<charNode*>, compare> nodeQueue;
        for (int i = 0; i < this->uniqueChars; i++){
            nodeQueue.push(&(nodes.at(i)));
        }

        int i = this->uniqueChars;
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
        return &nodes.at(2 * this->uniqueChars - 1 - 1);
    }
}

void HuffmanEncode::getCodes(charNode* head, string currCode /*=""*/){
    //Error Case when huffman tree is empty:
    if (head == NULL){
        return;
    }
    //Base case: Leaf node encountered
    if (head->left == NULL && head->right == NULL){
        this->huffmanCodes[head->symbol] = currCode;
        return;
    }
    //Recursive cases:
    else{
        if (head->left != NULL){
            getCodes(head->left, currCode + "0");
        }
        if (head->right != NULL){
            getCodes(head->right, currCode + "1");
        }
    }
}

//Returns buffer of entire encoded data (header + message)
byte* HuffmanEncode::dataBuffer(charNode* head, string filename){
    unsigned long int sz_bytes = this->headerSz + this->msgSz;
    
    //Number of zero bits to add at end of file
    int zero_bits = 7 - ((sz_bytes - 6) % 8);
    //Value of last three bits in file to add at end of file
    byte last_bits = (sz_bytes - 6) % 8;

    sz_bytes += 10 - ((sz_bytes - 6) % 8);

    //sz_bits will always be divisible by 8 because of added bits above
    sz_bytes /= 8;

    this->compressedSz = sz_bytes;

    byte* data = new byte[sz_bytes]();

    //This is the byte we're going to use to store our memory through
    byte byte_frame = 0;
    unsigned long int bits_written = 0; //Track how many bits are in array and in byte_frame
    bool byte_empty = true; //Indicates if data is currently in byte_frame
    unsigned long int idx = 0; //Where byte will be written

    //Write header to buffer first
    vector<charNode*> stack;
    stack.push_back(head);

    while (stack.empty() == false){
        charNode* curr = stack.at(stack.size() - 1);
        stack.pop_back();

        //Null Pointer
        if (curr == NULL){
           // Do nothing 
        }
        //Leaf Node
        else if (curr->left == NULL && curr->right == NULL){
            //str += "1";
            //str += curr->symbol;
            byte_frame = byte_frame << 1;
            byte_frame += 1;
            ++bits_written;
            if (byte_empty == true){
                byte_empty == false;
            }
            
            //mask for copying symbol data to byte_frame
            byte mask = 128;

            //Keep filling bits until byte_frame is full
            while (bits_written % 8 != 0){
                byte_frame = byte_frame << 1;
                //Check if bit was 1
                if ((int) (curr->symbol&mask) != 0){
                    byte_frame += 1;
                }
                ++bits_written;
                if (byte_empty == true){
                    byte_empty = false;
                }

                //Safely shift mask one bit to the right;      
                mask /= 2;
            }

            //Write byte_frame to data
            data[idx] = byte_frame;
            ++idx;
            byte_frame = 0;
            byte_empty = true;
            
            //fill remaining bits from symbol into byte_frame
            while (mask != 0){
                //byte_frame *= 2;
                byte_frame = byte_frame << 1;

                //Check if bit was 1
                if ((int) (curr->symbol&mask) != 0){
                    byte_frame += 1;
                }
                ++bits_written;
                if (byte_empty == true){
                    byte_empty = false;
                }
                mask /= 2;
            }

            if (bits_written % 8 == 0){
                data[idx] = byte_frame;
                ++idx;
                byte_frame = 0;
                byte_empty = true;
            }
        }
        //Parent Node
        else if (curr->left != NULL || curr->right != NULL){
            //str += "0";
            
            //Shift byte_frame over first
            byte_frame = byte_frame << 1;
            //Because we're inserting 0 at end, we dont add anything to byte_frame

            ++bits_written;
            if (byte_empty == true){
                byte_empty = false;
            }

            //Check if another byte needs to be written to buffer
            if (bits_written % 8 == 0){
                data[idx] = byte_frame;
                ++idx;
                byte_frame = 0;
                byte_empty = true;
            }

            if (curr->right != NULL)
                stack.push_back(curr->right);
            if (curr->left != NULL)
                stack.push_back(curr->left);
        }
    }

    //Write encoded data to file
    //At this point, we know byte_frame is either empty or partly full, but NOT full
    ifstream file(filename.c_str());
    char c;
    while (file.get(c)){
        //cast to unsigned char byte
        byte b = static_cast<byte>(c);
        //string huffCode = encodingInfo.huffmanCodes[b];
        for (char bit : this->huffmanCodes[b]){
            byte_frame = byte_frame << 1;
            if (bit == '1'){
                byte_frame += 1;
            }
            ++bits_written;
            if (byte_empty == true){
                byte_empty = false;
            }

            //Check if byte_frame is full
            if (bits_written % 8 == 0){
                data[idx] = byte_frame;
                ++idx;
                byte_frame = 0;
                byte_empty = true;
            }
        }

    }

    //pad with zero bits
    while (zero_bits > 0){
        byte_frame = byte_frame << 1;
        ++bits_written;
        if (byte_empty == true){
            byte_empty = false;
        }
        if (bits_written % 8 == 0){
            data[idx] = byte_frame;
            ++idx;
            byte_frame = 0;
            byte_empty = true;
        }

        --zero_bits;
    }

    //add last three bits, we know byte_frame is filled with 5 bits
    byte mask = 4;
    while (mask > 0){
        byte_frame = byte_frame << 1;
        if ((int) (last_bits&mask) != 0){
            byte_frame += 1;
        }
        ++bits_written;
        mask /= 2;
    }
    
    //write final bit
    if (bits_written % 8 == 0){
        data[idx] = byte_frame;
        ++idx;
        byte_frame = 0;
        byte_empty = true;
    }
    return data;
}

void HuffmanEncode::encodeFile(string filename){
    if (this->buffer != NULL){
        ofstream file(filename.c_str(), ios::out | ios::binary);
        file.write((char*) this->buffer, this->compressedSz);
    }
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
    if (root->left == NULL and root->right == NULL && (root->symbol != '\n' || root->symbol != '\0'))
        cout<<"(" << root->freq << ", " << root->symbol << ")" <<"\n";  
    //else if ((int) root->symbol == -1)
    else if (root->left != NULL && root->right != NULL)
        cout<<"(" << root->freq << ", " << "[PARENT]" << ")" <<"\n";
    else if ((int) root->symbol == 10)
        cout<<"(" << root->freq << ", " << "\\n" << ")" <<"\n"; 
    else
        cout<<"(" << root->freq << ", " << "\\0" << ")" <<"\n"; 


    // Process left child  
    print2DUtil(root->left, space);  
}  
  
// Wrapper over print2DUtil()  
void print2D(charNode *root)  
{  
    // Pass initial space count as 0  
    print2DUtil(root, 0);  
}  

pair<unsigned long int, unsigned int> HuffmanDecode::getCompFileSz(){
    ifstream inFile(this->inputFileName.c_str());
    pair<unsigned long int, unsigned int> p = {0,0};

    if (inFile){
        const auto begin = inFile.tellg();
        inFile.seekg(0, ios::end);
        const auto end = inFile.tellg();

        p.first = (end - begin);
        
        inFile.seekg(-1, ios::end);
        char c;
        inFile.get(c);
        inFile.close();

        byte b = static_cast<byte>(c);
        //mask b with 0b00000111
        b = b & static_cast<byte>(7);
        p.second = (unsigned int) b;
    }

    return p;
}

//For now, assume any *.huff is properly constructed and not tampered.
void HuffmanDecode::getHuffmanTree(){
    //FYI, smallest huffman tree has to be 3 + 16 = 19 bits long (2 bytes + 3 bits) 
    //In general, the header size should (2 * uniqueChars - 1) + (8 * uniqueChars) bits
    //a header of '0001a1b1c01d1e' represents this:
    /*
          / \
        /\   /\
      /\  c d  e
    a  b 
    */

    ifstream inFile(this->inputFileName.c_str());
    char c;
    byte byte_frame = 0;
    byte mask = 128;
    unsigned long int bits_read = 0;
    unsigned long int curr_idx = 0;

    vector<charNode*> stack;
    charNode empty_node = {(unsigned char)0, 0, NULL, NULL};
    if (bits_read == 0){
        inFile.get(c);
        byte_frame = static_cast<byte>(c);
        //inFile.seekg(ios::beg);
        
        //cout << (int)byte_frame << " " << (int)mask << " " << (int)((byte_frame & mask) != static_cast<byte>(0)) << endl;

        if ((byte_frame & mask) != static_cast<byte>(0)){ 
            cout << "Invalid header" << endl;
            inFile.close();
            return;
        }
        else{
            this->nodes[curr_idx] = empty_node;
            //push root into stack 
            stack.push_back(&this->nodes[curr_idx]);
            curr_idx++;
            bits_read++;
            mask = static_cast<int>(pow(2, 7 - (bits_read % 8)));
        }
    }

    //When loop begins, mask == 64 (guaranteed)
    while (!stack.empty()){
        //leaf node encountered
        //cout << (int)byte_frame << " " << (int)mask << " " << (int)((byte_frame & mask) != static_cast<byte>(0)) << endl;

        if ((byte_frame & mask) != static_cast<byte>(0)){
            bits_read += 1;

            //xrrrrrrr rxxxxxxx
            //xxrrrrrr rrxxxxxx
            //xxxrrrrr rrrxxxxx
            //xxxxrrrr rrrrxxxx
            //xxxxxrrr rrrrrxxx
            //xxxxxxrr rrrrrrxx
            //xxxxxxxr rrrrrrrx
            //xxxxxxxx rrrrrrrr

            //cout << "Before mask: " << (int) mask << endl;

            //initialize mask to grab character symbol 
            if (bits_read % 8 == 0){
                mask = static_cast<byte>(0);
            }
            else{
                mask = static_cast<byte>(pow(2, 8 - (bits_read % 8)) - 1);
            }
            
            //cout << "After mask: " << (int) mask << endl;

            //Grab portion of character
            byte_frame = byte_frame & mask;

            //Left shift byte_frame to get fill in remaining bits
            byte_frame *= static_cast<int>(pow(2, (bits_read % 8)));

            //Retrieve next byte
            inFile.get(c);

            byte byte_tmp = static_cast<byte>(c);
            
            //Strip off bits on the right
            byte remaining = byte_tmp;

            if (bits_read % 8 != 0){
                remaining /= static_cast<int>(pow(2, 8 - (bits_read % 8)));
            }

            //Piece together byte_frame 
            byte_frame = byte_frame | remaining;

            //Create leaf node and assign symbol value
            this->nodes[curr_idx] = empty_node;
            this->nodes[curr_idx].symbol = byte_frame;

            //Check which children current parent has and link child to parent
           if (stack.at(stack.size() - 1)->left == NULL){
               stack.at(stack.size() - 1)->left = &this->nodes[curr_idx];
           }
           else{
               stack.at(stack.size() - 1)->right = &this->nodes[curr_idx];
           }

            //Pop parent node off stack only if it has both children
            if (stack.at(stack.size() - 1)->left != NULL && stack.at(stack.size() - 1)->right != NULL){
                stack.pop_back();
            }

            //byte_tmp was not completely copied onto byte_frame
            if (bits_read % 8 != 0){
                byte_frame = byte_tmp;
            }
            //byte_tmp was completely read
            else{
                inFile.get(c);
                byte_frame = static_cast<byte>(c);
            }

            //Update bits_read, curr_idx, and uniqueChars
            bits_read += 8;
            curr_idx++;
            this->uniqueChars++;
            //Set mask to next value
            mask = static_cast<int>(pow(2, 7 - (bits_read % 8)));
        }

        //Parent node encountered
        else{
            bits_read++;

            if (bits_read % 8 == 0){
                inFile.get(c);
                byte_frame = static_cast<byte>(c);
            }
            mask = static_cast<int>(pow(2, 7 - (bits_read % 8)));

            //Create child node
            //this->nodes.push_back(empty_node);

            this->nodes[curr_idx] = empty_node;

            //Determine location of child node

            if (stack.at(stack.size() - 1)->left == NULL){
                stack.at(stack.size() - 1)->left = &this->nodes[curr_idx];
            }
            else{
                stack.at(stack.size() - 1)->right = &this->nodes[curr_idx];
            }

            //Pop from stack only if parent has both children
            if (stack.at(stack.size() - 1)->left != NULL && stack.at(stack.size() - 1)->right != NULL){
                stack.pop_back();
            }
            
            //Add child node to stack
            stack.push_back(&this->nodes[curr_idx]);

            //Update curr_idx
            curr_idx++;
        }
    }
    
    this->headerBitSz = bits_read;
    inFile.close();
}

//Assumes file at this->inputFileName is not empty and that this->head is not null
void HuffmanDecode::decodeFile(string filename){
    ifstream encodedFile(this->inputFileName.c_str());
    ofstream decodedFile(filename.c_str(), std::ios::out | std::ios::binary);
    if (!encodedFile || !decodedFile){
        cout << "Error: Something went wrong" << endl;
        return;
    }
    
    std::streamoff offset = (this->headerBitSz/8) + (int)(this->headerBitSz % 8 != 0) - 1;
    //Skip to encoded portion of file
    encodedFile.seekg(offset, ios::beg);

    charNode* curr = this->head;
    if (curr == NULL){
        return;
    }

    char c;
    byte byte_frame = static_cast<byte>(0);
    byte mask = static_cast<byte>(pow(2, 7 - (this->headerBitSz % 8)));
    unsigned long int bits_read = 0;
    unsigned long int encodedBitSz = this->compByteSz*8 - this->headerBitSz - 3 - (7 - this->last3bits);
    
    while(bits_read < encodedBitSz){
        if (bits_read == 0 || mask == static_cast<byte>(0)){
            encodedFile.get(c);
            byte_frame = static_cast<byte>(c);

            if (mask == static_cast<byte>(0)){
                mask = 128;
            }
        }

        if ((byte_frame & mask) != static_cast<byte>(0)){
            curr = curr->right;            
        }
        else{
            curr = curr->left;
        }

        bits_read++;
        mask /= 2;
        if (curr->left == NULL && curr->right == NULL){
            //Write character to file
            decodedFile.write(reinterpret_cast<const char*>(&curr->symbol), sizeof(byte));
            curr = this->head;
        }
    }
    
    encodedFile.close();
    decodedFile.close();
}
