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


class HuffmanData
{
/*
private:
    unsigned uniqueChars = 0;
    unsigned long int headerSz = 0;
    unsigned long int msgSz = 0;
    unsigned long int compressedSz = 0;
    byte* buffer = NULL;
    unordered_map<byte, string> huffmanCodes;
*/
public:
    unsigned uniqueChars = 0;
    unsigned long int headerSz = 0;
    unsigned long int msgSz = 0;
    unsigned long int originalSz = 0;
    unsigned long int compressedSz = 0;
    byte* buffer = NULL;
    unordered_map<byte, string> huffmanCodes;

    HuffmanData(string);
    ~HuffmanData();
    unordered_map<byte, unsigned long int> countFreq(string);
    vector<charNode> makeNodes(unordered_map<byte, unsigned long int>&);
    charNode* makeHuffmanTree(vector<charNode>&);
    unsigned long int getHeaderSize();
    unsigned long int getMsgSize(unordered_map<byte, unsigned long int>&);

    void getCodes(charNode*, string = "");
    byte* dataBuffer(charNode*, string);
    void writeFile(string);

    void print2DUtil(charNode *root, int space);
    void print2D(charNode *root);  

};

HuffmanData::HuffmanData(string filename){
    unordered_map<byte, unsigned long int> freq = this->countFreq(filename);
    this->uniqueChars = freq.size();

    vector<charNode> nodes = this->makeNodes(freq);
    charNode* head = this->makeHuffmanTree(nodes);
    
    this->getCodes(head);

    this->headerSz = this->getHeaderSize();
    this->msgSz = this->getMsgSize(freq);
    
    for (pair<byte, unsigned long int> p : freq){
        this->originalSz += p.second;
    }

    this->buffer = this->dataBuffer(head, filename);

    this->print2D(head);
}

HuffmanData::~HuffmanData(){
    ;
}

unordered_map<byte, unsigned long int> HuffmanData::countFreq(string filename){
    unordered_map<unsigned char, unsigned long int> freq;
    ifstream file(filename.c_str());
    char c;
    while (file.get(c)){
        freq[c]++;
    }
    file.close();
    
    return freq;
}

unsigned long int HuffmanData::getHeaderSize(){
    return (2 * this->uniqueChars - 1) + (8 * this->uniqueChars);
}

unsigned long int HuffmanData::getMsgSize(unordered_map<byte, unsigned long int>& freq){
    unsigned long int sz = 0;
    
    for (pair<unsigned char, unsigned long int> p : freq){
        sz += p.second * this->huffmanCodes[p.first].length();
    }

    return sz;
}

vector<charNode> HuffmanData::makeNodes(unordered_map<byte, unsigned long int>& freq){
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

charNode* HuffmanData::makeHuffmanTree(vector<charNode>& nodes){
    if (this->uniqueChars == 0){
        return NULL;
    }
    else if (this->uniqueChars == 1){
        nodes.at(1).left = &nodes.at(0);
        nodes.at(1).freq = nodes.at(0).freq;
        return &nodes.at(1);
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

void HuffmanData::getCodes(charNode* head, string currCode /*=""*/){
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
byte* HuffmanData::dataBuffer(charNode* head, string filename){
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

void HuffmanData::writeFile(string filename){
    if (this->buffer != NULL){
        ofstream file(filename.c_str(), ios::out | ios::binary);

        for (unsigned long int i = 0; i < this->compressedSz; i++){
            file.write((char*) &this->buffer[i], sizeof(char));
        }
    }
}

#define COUNT 10

void HuffmanData::print2DUtil(charNode *root, int space)  
{  
    // Base case  
    if (root == NULL)  
        return;  
  
    // Increase distance between levels  
    space += COUNT;  
  
    // Process right child first  
    this->print2DUtil(root->right, space);  
  
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
    this->print2DUtil(root->left, space);  
}  
  
// Wrapper over print2DUtil()  
void HuffmanData::print2D(charNode *root)  
{  
    // Pass initial space count as 0  
    this->print2DUtil(root, 0);  
}  
 