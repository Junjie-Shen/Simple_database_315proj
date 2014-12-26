#include <iostream>     
#include <string>    
#include <stdio.h>
#include <ctype.h>  
#include<vector>
#include<sstream> 
#include <stack>
#include <queue>
using namespace std;

//------------------------------------------------data & attribute class--------------------------------
// store the information of one relation
struct data {
    string str;
    int value;       

    data(): str(""), value(-9999) {}
    
    bool operator== (const data& b);
    bool operator!= (const data& b) {return (str != b.str);}
    bool operator> (const data& b);    
    bool operator< (const data& b);
    bool operator>= (const data& b);
    bool operator<= (const data& b);
};
bool data::operator==(const data& b){
	if((this->str == b.str) ){ return true; }
	else { return false; }
}

bool data::operator> (const data& b) {
    if (value == -9999 || b.value == -9999)  { 
         cout << "unmatched type: " << value << " : " << b.value <<endl;
//         exit(1);
    }     
    return (value > b.value); 
}

bool data::operator< (const data& b) {
    if (value == -9999 || b.value == -9999)  { 
         cout << "unmatched type: " << value << " : " << b.value <<endl;
//         exit(1);
    }     
    return (value < b.value); 
}

bool data::operator>= (const data& b) {
    if (value == -9999 || b.value == -9999)  { 
         cout << "unmatched type: " << value << " : " << b.value <<endl;
//         exit(1);
    }     
    return (value >= b.value); 
}

bool data::operator<= (const data& b) {
    if (value == -9999 || b.value == -9999)  { 
         cout << "unmatched type: " << value << " : " << b.value <<endl;
//         exit(1);
    }     
    return (value <= b.value); 
}

// the attribute class represents the name and type of one attribute 
class attribute {
private:
      string attribute_name;
      string attribute_type;
public:
       attribute(): attribute_name(""), attribute_type("") {}
       attribute(string name, string type): attribute_name(name), attribute_type(type) {}
       attribute(const attribute& a): attribute_name(a.attribute_name), attribute_type(a.attribute_type) {}
       attribute& operator= (const attribute& a) {
                  attribute_name = a.attribute_name; 
                  attribute_type = a.attribute_type;
                  return *this;
       } 
       string getName() {return attribute_name;}
       string getType() {return attribute_type;}
       void setName(string newName) {attribute_name = newName;}
       void setType(string newType) {attribute_type = newType;}
       bool operator==(attribute b);
};
bool attribute::operator==(attribute b){
    if((this->attribute_name == b.attribute_name)){ return true; }
    else { return false; }
}


struct attribute_literal {
    string attribute_name;
    string literal;       
};



//--------------------------------------relation class -----------------------------------------

class relation {
public:
    string relation_name;
    vector<attribute> attribute_list;
    vector<vector<data> > data_table;
    vector<string> keys;                                                     //primary keys
    
    void rename(string s) {relation_name = s;}
    vector<data> get_row(int row_num) {return data_table[row_num];}          // get one row of data
    vector<data> get_col(int col_num) {                                      // get one col of data
        vector<data> col;
        for (int i = 0; i < data_table.size(); i++)
            col.push_back(data_table[i][col_num]);
        return col;        
    }            
/*  constructor
    copy constructor
    copy assignment
    insert ( );
    dele ( );
    update ( );
    
    rename();
    search ( );
*/
    void print ();
    bool NameisEmpty( ) { 
         if (relation_name == "") return true;
         else return false;    
    }
    bool DataisEmpty( ) { 
         if (data_table.size() == 0) return true;
         else return false;
    }
             
};

void relation::print() {
    cout << "name: " <<relation_name << endl;
    if (keys.size() != 0) {
        cout << "keys: ";
        for (int i = 0; i < keys.size(); i++)
            cout << keys[i] << " ";
        cout << endl; 
    }
    for (int i = 0; i < attribute_list.size(); i++)
        cout << attribute_list[i].getName() <<" " ;
    cout << endl;
       
    for(int j = 0; j < data_table.size(); ++j){
        for(int k = 0; k < data_table[j].size(); ++k){
            cout << data_table[j][k].str <<" " ;
        }
        cout << "\n";
    }
}

//------------------------------------------------token class -----------------------------------------

class Token {
private:
    string buffer; 
    int size;
    int index;
            
    vector<string> lexer(string input);           // do the lexical analysis      
    
public:
    vector<string> tokens;                        // store the input statement       
    string relation_name;                         // store an unexisted relation name for create or query
    vector<string> relation_obj;                  // store the existed relation name
    vector<int> relation_position;                // the postion of the existed relation in the global vector
    stack <relation> relation_set;                // store the relation get from the curry function
    vector<string> condition_stream;              // abstract the condition from the statement
    vector<vector<string> > condition_set;        // for different condition
       
    vector<string> typed_attribute_list;          // for typed attribute in CREATE function
    vector<string> attribute_list;                
    vector<string> attribute_type;
    vector<string> literal_list;                  // store the literal list in statement
                                     
    Token(): buffer(""), index(0), size(0) {}	  
    Token(string s): buffer(""), index(0)
               {tokens = lexer(s); size = tokens.size();}      
               
                                                                      
    string get();                                 // get the next token
    void unget() {index--;}                       // putback the token
    
    
    string get_relation_name() {if (relation_name != "") return relation_name;
                                else cerr << "no relation_name"; exit(1);}
    void set_relation_name(string new_relation_name) {relation_name = new_relation_name;}

    void push_relation(relation rltn) {relation_set.push(rltn);}
    relation get_relation();                      // get one relation for next curry function
    
    void push_condition();
    vector<string> get_condition();
  
};
// get the next token in the statement
string Token::get() {
    string s;
    if (index < tokens.size()) {   
        s = tokens[index];
        index++;
    }
    else {
        s = "";
    }
    return s;
}

// get one relation from stack
relation Token::get_relation() {
          relation r = relation_set.top();
          relation_set.pop();
          return r;
}

void Token::push_condition() {
    condition_set.push_back(condition_stream);
    condition_stream.clear();     
}

vector<string> Token::get_condition() {
    vector<string> cond = condition_set.back();
    condition_set.pop_back();
    return cond;               
}



//takes input, places each string sequence separated by spaces into arguments vector
vector<string> Token::lexer(string input){ 
	string userinput = input;
	vector<string> tokenlist;                    //vector store the analyzed tokens
	string s;	
	
	stringstream lexerstream;                    // store statement into stringstream 
	lexerstream << userinput;            
	while(lexerstream >> userinput){             // get one word from stringstream 
		int check = 0;
		int SEMIcounter = 0;
		int RPARENcounter = 0;
		int COMMAcounter = 0;
		string varchar = "VARCHAR";
		if(userinput[0] == '('){
			while(userinput[0] == '('){
				s = "(";
				tokenlist.push_back(s);
				userinput.erase(userinput.begin());
			}
		}
		if(userinput[userinput.size()-1] == ';'){
			userinput.erase(userinput.begin()+userinput.size()-1);
			SEMIcounter = SEMIcounter + 1;
		}
		if(userinput[userinput.size()-1] == ','){
			userinput.erase(userinput.begin()+userinput.size()-1);
			COMMAcounter = COMMAcounter + 1;
		}
		if(userinput[userinput.size()-1] == ')'){
			for(int i = 0; i < varchar.size(); ++i){
				if(userinput[i] == varchar[i]){
					check = check + 1;
				}
			}
			if(check == 7){
				int iteration = 0;
				if(userinput[9] == ')'){
					iteration = userinput.size() - 10;
				}
				else if(userinput[9] != ')'){
					iteration = userinput.size() - 11;
				}
				
				for(int i = 0; i < iteration; ++i){
					userinput.erase(userinput.begin()+userinput.size()-1);
				}
				RPARENcounter = iteration;
			}
			else{
				while(userinput[userinput.size()-1] == ')'){
					userinput.erase(userinput.begin()+userinput.size()-1);
					RPARENcounter = RPARENcounter + 1;
				}
			}
		}

		s = userinput;
		tokenlist.push_back(s); //
		
		for(int j = 0; j < RPARENcounter; ++j){
			s = ")";
			tokenlist.push_back(s);
		}
		for(int i = 0; i < COMMAcounter; ++i){
			s = ",";
			tokenlist.push_back(s);
		}
		for(int k = 0; k < SEMIcounter; ++k){
			s = ";";
			tokenlist.push_back(s);
		}

	}
	
	return tokenlist;
}


//-------------------------------------------help functions---------------------------------------

// test if the token is an operator
bool is_operator(string s) {
    return (s == "==") || (s == "!=") || (s == "<") 
        || (s == ">") || (s == "<=") || (s == ">=");  
}

bool is_num(char ch)
{
    return ch == '0' || ch == '1' || ch == '2' || ch == '3'
        || ch == '4' || ch == '5' || ch == '6' || ch == '7'
        || ch == '8' || ch == '9';
}

bool is_int(string s) {
    if (!is_num(s[0]) && s[0] != '-' && s[0] != '+')    // positive or negative sinals            
            return false;
         
    for (int i=1; i < s.length(); i++)
        if (!is_num(s[i]) ) 
                return false;  
    return true;
}

bool is_alpha(char ch)
{

    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_'))
    {
        return true;
    }
    else
        return false;
}

// test if the token is a valid identifier
bool is_identifier(string s)
{
    if (!is_alpha(s[0]))    // first char must be alpha
        return false;
    else {
       for (int i = 1; i< s.length(); i++)  // all the other char must be either alpha or num
          if (!is_alpha(s[i]) && !is_num(s[i])) return false;   
          
       return true;             
    }
       
}




