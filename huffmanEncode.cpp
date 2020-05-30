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
    unsigned long int compressed_sz = 0;
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


unordered_map<unsigned char, unsigned long int> countFreq(string filename){
    unordered_map<unsigned char, unsigned long int> freq;

    ifstream file(filename.c_str());
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

//Assumes either null root or huffman tree with AT LEAST one character frequency
string encodeTree(charNode* root){
    //Special Case 1: Null Pointer
    if (root == NULL){
        return "";
    }
    string str = "";

    vector<charNode*> stack;
    stack.push_back(root);

    while (stack.empty() == false){
        charNode* curr = stack.at(stack.size() - 1);
        stack.pop_back();

        //Null Pointer
        if (curr == NULL){
           // Do nothing 
        }
        //Leaf Node
        else if (curr->left == NULL && curr->right == NULL){
            str += "1";
            str += curr->symbol;
        }
        //Parent Node
        else if (curr->left != NULL || curr->right != NULL){
            str += "0";
            if (curr->right != NULL)
                stack.push_back(curr->right);
            if (curr->left != NULL)
                stack.push_back(curr->left);
        }
    }
    return str;
}


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
byte* dataBuffer(charNode* head, string filename, dataPkg& encodingInfo){
    unsigned long int sz_bytes = encodingInfo.header_sz + encodingInfo.msg_sz;
    
    //Number of zero bits to add at end of file
    int zero_bits = 7 - ((sz_bytes - 6) % 8);
    //Value of last three bits in file to add at end of file
    byte last_bits = (sz_bytes - 6) % 8;

    sz_bytes += 10 - ((sz_bytes - 6) % 8);

    //sz_bits will always be divisible by 8 because of added bits above
    sz_bytes /= 8;

    encodingInfo.compressed_sz = sz_bytes;

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
        for (char bit : encodingInfo.huffmanCodes[b]){
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

unsigned long int file_sz_bytes(unordered_map<unsigned char, unsigned long int> freq){
    unsigned long int val = 0;
    for (auto p : freq){
        val += (p.second);
    }
    return val;
}
  

int main(){
    string filename = "t1.txt";
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

    //cout << encoded << endl;

    //cout << "Header size: " << getHeaderSize(encodingInfo.uniqueChars) << " bits" << endl;
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

    ifstream test; 
    dataPkg t_data;
    t_data.header_sz = getHeaderSize(5);
    t_data.msg_sz = 80;
    byte* data = dataBuffer(t, test, t_data);
    unsigned int long n = (t_data.header_sz / 8) + ((int) (t_data.header_sz % 8 != 0));
    for (int i = 0; i < n; i++){
        cout << (int) data[i] << " ";
    }
    */
    cout << "Original file size: " << file_sz_bytes(freq) << " bytes" << endl;
    //cout << "Encoded size: " << encodingInfo.header_sz + encodingInfo.msg_sz << " bits" << endl;
    byte* data = dataBuffer(head, filename, encodingInfo);
    cout << "Compressed file size: " << encodingInfo.compressed_sz << " bytes" << endl;

    //print2D(head);
    cout << endl;

    for (auto p : encodingInfo.huffmanCodes){
        cout << p.first << " " << p.second << endl;
    }

    for (int i = 0; i < encodingInfo.compressed_sz; i++){
        cout << (int) data[i] << " ";
    }
    //print2D(head);


    
    return 0;
}