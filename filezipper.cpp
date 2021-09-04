#include<bits/stdc++.h>
using namespace std;
#define int long long 
#define en "\n"
#define PADDING_SEPARATOR char(255)
#define PSEUDO_EOF char(250)
#define CHARACTER_CODE_SEPERATOR char(253)
#define HEADER_ENTRY_SEPERATOR char(252)
#define HEADER_TEXT_SEPERATOR char(251)

inline void file() {  
    #ifndef ONLINE_JUDGE
    freopen("std_input2.txt","r",stdin);
    freopen("std_output.txt","w",stdout); 
    #endif
}

int padding ; // variable which stores padding added to binary string file which is to be compressed
unordered_map<char,string> char_code; // character codes for compression
unordered_map<char,int> freq; // frequency of characters
string binary_code=""; // binary string of given text file
unordered_map<string,char> dehuff_char_code ; // character codes for decompression
string dehuff_binary_code=""; // binary string of compressed file

struct node { 
    char data;
    int fe;

    node *left;
    node *right;

    node(char ch,int val) {
        data = ch;
        fe = val;
      
        left = NULL;
        right = NULL;
    }
};

// comparator, priority-queue STL for node comparison
struct cmp {        
    bool operator()(node *a,node *b) {   
        return (a->fe > b->fe);
    }
};

void print_tree(node *root,string str) {

    if(root == NULL)
        return;

    if(root->data != PSEUDO_EOF) {
      //  cout<<str<<" "<<root->data<<en;
        char_code[root->data] = str;
    }

    print_tree(root->left,str+'0');  
    print_tree(root->right,str+'1');

}

node *Huffman_tree() {
    priority_queue< node*,vector<node*>,cmp> pq;

    for(auto it = freq.begin();it!=freq.end();it++) {
        node *n= new node(it->first,it->second);
        pq.push(n);
    }

    while(pq.size()>1) {
        node* x = pq.top();
        pq.pop();                               

        node* y = pq.top();
        pq.pop();

        node *n = new node(PSEUDO_EOF,x->fe+y->fe);   // storing the frequency count of child to its parent
        n->left = x;
        n->right = y; 

     //   cout<<x->data<<':'<<x->fe<<" "<<y->data<<':'<<y->fe<<en;

        pq.push(n);
    }

    print_tree(pq.top(),"");     //printing character codes of every node in string format

    return pq.top();         // return the root node of Huffman Tree

}

void fillfile(string &s) {
    int r = s.size();
    int size = (r-1)/8+1;
    size*=8;

    for(int i=r;i<size;i++) {
      s+='0';
      padding++;
    }
}

void write_file(ofstream &fout) {            
    // fout<<binary_code;
    int n = binary_code.size();

    for(int i=0;i<n;i+=8) {
        string substring = binary_code.substr(i,8);

        bitset<8> bits(substring);

        char ch = char(bits.to_ulong());

        fout<<ch;
    }

}

void write_map(ofstream &fout) {

    fout<<padding<<PADDING_SEPARATOR; // Write the padding and its separator in the compress file 

    for(const auto &itr : char_code) {

        // Write character and its code with CHAR_SEP b/w them
        fout<<itr.first<<CHARACTER_CODE_SEPERATOR<<itr.second<<HEADER_TEXT_SEPERATOR;
    
    }

    fout<<HEADER_ENTRY_SEPERATOR ; // WILL SEPARATE THE CHAR CODES AND COMPRESSED TEXT

}

void frequency_count(string file) {
    ifstream fin(file);

    char ch;

    while(fin.get(ch))
    {
        freq[ch]++;
    }

    fin.close();

}

void binary_code_file(string file) {

    ifstream fin(file) ;
    char ch;
    while(fin.get(ch))
    {     
      //  cout<<char_code[ch]<<" ";
        binary_code += char_code[ch];
    }

    fillfile(binary_code);     // rounding bits to multiple of 8
}

void extract_char_code(ifstream &fin) 
{
    char ch;

    // Read padding character from compressed file
    while(fin.get(ch)) {
        if(ch==PADDING_SEPARATOR) break;
        padding=(ch-'0') ; // convert padding character to int
    }


    // reading character codes from compressed file to build dehuff tree
    bool done=0; // done with a char
    
    string code_of_char=""; // code of the character
    
    char character; // character itself

    while(fin.get(ch)) 
    {
        if(ch == HEADER_ENTRY_SEPERATOR) break ;
        // extracting character
        if(!done) 
        {
            if(ch==CHARACTER_CODE_SEPERATOR) done=1;
            else 
            {
                character=ch;
            }
        }
        // extracting code of character
        else 
        {
            if(ch==HEADER_TEXT_SEPERATOR) 
            {
                dehuff_char_code[code_of_char]=character;
                code_of_char.clear() ;
                done=0;
            }
            else 
            {
                code_of_char+=ch;
            }
        }
    }

    while(fin.get(ch)) {
        
        bitset<8> bits(ch) ;
        
        for(int i=7 ; i>=0 ; i--) {
            if(bits[i]==1) dehuff_binary_code+='1' ;
            else dehuff_binary_code+='0' ;
        }
    
    }

}

void decompress(ofstream &fout) {

    string s=""; // string for reading and matching codes from dehuff_char_code map
    
    int n=dehuff_binary_code.size();
    
    int actual_size=n-padding ; // actual size of string after removing padding
    
    for(int i=0 ; i<actual_size ; i++)
    {     
        if(dehuff_char_code.find(s)!=dehuff_char_code.end()) {
            fout<<dehuff_char_code[s];
            s.clear() ;
            s+=dehuff_binary_code[i];
        }
        else {
            s+=dehuff_binary_code[i];
        }
    }
    fout<<dehuff_char_code[s];
}

signed main() {
    file();

    string inputfile; // input file for compression
    string outputfile;  //  compressed file
    bool c;
    cout<<"MENU :\n";
    cout<<"0. TO COMPRESS\n" ;
    cout<<"1. TO DECOMPRESS\n" ; 
    cin>>c;

    /*COMPRESSION SECTION*/
    if(!c)
    {
        cout<<"-> PASTE THE INPUT YOU WANT TO COMPRESS IN input.txt \n";
        cout<<"-> COMPRESSED FILE WILL APPEAR IN compress.txt\n"; 
        cin>>inputfile;
        cin>>outputfile; 
        frequency_count(inputfile);  // count the frequency of input character in input file
          
        node *root = Huffman_tree();  // function for Huffman tree 
          
        binary_code_file(inputfile);   // creating a binary string code for inputfile

        ofstream fout(outputfile) ; // Write all at a time in compress.txt else use seeekp 

        write_map(fout);   // write map containing character codes to compress.txt

        write_file(fout);    // write character code to compress.txt

        fout.close() ;
    }
    
    /*DECOMPRESSION SECTION*/ 
    else 
    {
        cout<<"PASTE THE INPUT YOU WANT TO DECOMPRESS IN compress.txt \n";
        cout<<"DECOMPRESSED FILE WILL APPEAR IN decompress.txt\n"; 
        cin>>inputfile;
        cin>>outputfile; 

        ifstream fin(inputfile) ;

        extract_char_code(fin) ; // extract char codes and binary file for decompression

        fin.close() ;

        // for(auto itr : dehuff_char_code) {
        //     cout<<itr.first<<" "<<itr.second<<en;
        // }

      //  cout<<dehuff_binary_code<<" ";

        ofstream fout(outputfile) ; 

        decompress(fout) ; // decompress the binary coded file to text file

        fout.close() ; 

    }
}