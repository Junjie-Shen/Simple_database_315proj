#include <fstream>
#include <iomanip>
using namespace std;


vector<relation> relation_list;                                               // global relation vector


void DB_assign(Token& t) {
    string relation_name = t.get_relation_name();     
    relation r = t.get_relation();
    r.rename(relation_name);
    relation_list.push_back(r);           
}

// core create function
bool DB_create(Token& t){
    string primarykey;
	relation relationbuffer;
    attribute attributebuffer;
    string rlt_name = t.get_relation_name();	
	relationbuffer.relation_name = rlt_name;                                   // set the relation name
	for (int i = 0; i < relation_list.size(); i++) {                           // check the repeated relation name
        if (rlt_name == relation_list[i].relation_name) {
           cout << "repeated relation_name: " << rlt_name <<endl;
           return false;
        }         
    }
	for (int i = 0; i< t.attribute_type.size(); i++) {
        attributebuffer.setName(t.typed_attribute_list[i]);
        attributebuffer.setType(t.attribute_type[i]);
        relationbuffer.attribute_list.push_back(attributebuffer);              // push attribute name and type
    }
    for (int i = 0; i< t.attribute_list.size(); i++) {                         // push primary keys
        primarykey = t.attribute_list[i];
        relationbuffer.keys.push_back(primarykey);
    }
	relation_list.push_back(relationbuffer);
    return true;	
}

//core create function
bool DB_insert(Token& t){
     int found = 0;
     data elements;
     vector<data> data_elements;
     for (int i = 0; i < relation_list.size(); i++) {
         if (relation_list[i].relation_name == t.relation_obj[0]) {
             found = 1;
             if (relation_list[i].attribute_list.size() != t.literal_list.size()) {      
                 cout << "# of attributes differs from # of data: "                    
                      << relation_list[i].attribute_list.size() << " : "
                      << t.literal_list.size() << endl;
                 return false;                                            
             }
             
             for (int j = 0; j < t.literal_list.size(); j++) {
                 elements.str = t.literal_list[j];
                 if(relation_list[i].attribute_list[j].getType() == "INTEGER"){            // if int data type
                     if (!is_int(elements.str)) {
                         cout << "data type should be int: " << elements.str <<endl;       
                         return false;                       
                     }                      
					 elements.value = atoi(elements.str.c_str());                          // string to int
                 }
                 data_elements.push_back(elements);                       
             }
             relation_list[i].data_table.push_back(data_elements); 
             break;                                           
         }  
     }
     if (found == 0) {
         cout << "no matched relation found: "<< t.relation_obj[0] <<endl;
         return false;          
     }
	 return true;
}

// show the detail of the relation
void DB_show(Token& t){                                                    
	relation r = t.get_relation();
	cout <<"name: " << r.relation_name << endl;
    if (r.keys.size() != 0) {
	    cout << "keys: ";
	    for (int i = 0; i < r.keys.size(); i++)
	        cout << r.keys[i] << " ";
	    cout << endl; 
    }
    for (int i = 0; i < r.attribute_list.size(); i++)
        cout << left << setw(15) <<r.attribute_list[i].getName() <<" " ;
    cout << endl;
       
    for(int j = 0; j < r.data_table.size(); ++j){
        for(int k = 0; k < r.data_table[j].size(); ++k){
            cout << left << setw(15) << r.data_table[j][k].str <<" " ;
        }
        cout << "\n";
    }
}

bool DB_close(Token& t){ 
	for(int i = 0; i < relation_list.size(); ++i){
		if(relation_list[i].relation_name == t.relation_obj[0]){
			relation_list.erase(relation_list.begin()+i);
			return true;
		}
	}
    cout << "no matched relation found: "<< t.relation_obj[0] <<endl;
    return false;              
}

relation add(relation a, relation b, Token& t){      //returns relation elements[a] + elements[b]
    int equal = 0;
    int equalFlag = 0;
    relation plusbuffer;
    plusbuffer.relation_name = t.tokens[0];
    plusbuffer.attribute_list = a.attribute_list;
    plusbuffer.keys = a.keys;
    plusbuffer.data_table = a.data_table;
    for(int i = 0; i < a.data_table[0].size(); ++i){
        if(a.attribute_list[i] == b.attribute_list[i]){
            equal = equal + 1;
        }
        equalFlag = i + 1;
    }
    if(equal == equalFlag){
        for(int i = 0; i < b.data_table.size(); ++i){
            plusbuffer.data_table.push_back(b.data_table[i]);
            cout<<"pushed back!\n";
        }
    }
    return plusbuffer;
}

relation subtract(relation a, relation b, Token& t){ //returns relation elements[b] - elements[a];
    int equal = 0;
    int equalFlag = 0;
    relation minusbuffer;
    minusbuffer.relation_name = t.tokens[0];
    minusbuffer.attribute_list = a.attribute_list;
    minusbuffer.keys = a.keys;
    minusbuffer.data_table = a.data_table;
    for(int i = 0; i < a.data_table[0].size(); ++i){
        if(a.data_table[0][i] == b.data_table[0][i]){
            equal = equal + 1;
        }
        equalFlag = i + 1;
    }
    if(equal == equalFlag){
        for(int j = 0; j < minusbuffer.data_table.size(); ++j){
            for(int i = 0; i < b.data_table.size(); ++i){
                int minuscheck = 0;
                for(int k = 0; k < minusbuffer.data_table[j].size(); ++k){
                    if(minusbuffer.data_table[j][k] == b.data_table[i][k]){
                        minuscheck = minuscheck + 1;
                    }
                }
                if(minuscheck == minusbuffer.data_table[j].size()){ minusbuffer.data_table.erase(minusbuffer.data_table.begin()+j); }
            }
        }
    }
    return minusbuffer;
}

relation multiply(relation a, relation b, Token& t){
    relation multiplybuffer;
    multiplybuffer.relation_name = t.tokens[0];
    multiplybuffer.attribute_list = a.attribute_list;
    for(int i = 0; i < b.attribute_list.size(); ++i){
        attribute attributebuffer;
        attributebuffer.setName(b.attribute_list[i].getName());
        attributebuffer.setType(b.attribute_list[i].getType());
        multiplybuffer.attribute_list.push_back(attributebuffer);
    }
    multiplybuffer.keys = a.keys;
    
    for(int k = 0; k < a.data_table.size(); ++k){
        for(int j = 0; j < b.data_table.size(); ++j){
            vector<data> multiplyrow;
            for(int i = 0; i <a.data_table[0].size(); ++i){
                multiplyrow.push_back(a.data_table[k][i]);
                cout << "table 1 pushed back! I pushed back this: " <<a.data_table[k][i].str << "\n";
            }
            for(int i = 0; i < b.data_table[0].size(); ++i){
                multiplyrow.push_back(b.data_table[j][i]);
                cout << "table 2 pushed back! I pushed back this: " <<b.data_table[j][i].str << "\n";
            }
            multiplybuffer.data_table.push_back(multiplyrow);
            cout << "row complete!\n";
        }
    }
    
    return multiplybuffer;

}

void DB_union(Token& t){
    relation a = t.get_relation();            //now: a = back, b = front.
    relation b = t.get_relation();
    t.push_relation(add(b, a, t));
}

void DB_difference(Token& t){
    relation a = t.get_relation();
    relation b = t.get_relation();
    t.push_relation(subtract(b, a, t));
}

void DB_product(Token& t){
    relation a = t.get_relation();
    relation b = t.get_relation();
    t.push_relation(multiply(b, a, t));

}
// get the right oprand in one comparison
data sec_oprand(vector<string>& input, int& index, relation& w, int row_num) {
     data literal;
     string token;
     token = input[++index];
     if (token[0] == '\"') {                                       // check the type of oprand
         literal.str = token; 
         return literal;        
     }
     else if (is_int(token) ) {
         literal.str = token;
         literal.value = atoi(token.c_str());
         return literal; 
     }
     else {
          for (int j = 0; j < w.attribute_list.size(); j++)
              if (token == w.attribute_list[j].getName())
                 return w.data_table[row_num][j];
     }
     cout << "no attribute name matched: " << token <<endl;
     return literal;
                                           
}

// deal with oprand op oprand
bool parse_operation(vector<string>& input, int& index, relation& w, int row_num) {
     string token;
     token = input[++index];
     int att_1 = -1;
     for (int j = 0; j < w.attribute_list.size(); j++)
         if (token == w.attribute_list[j].getName())
             att_1 = j;
     
     if (att_1 == -1) {
         cout << "attribute_name in condition doesn't match: " << token << endl;
         return false;        
     }
     
     token = input[++index];
     if (token == "==") {
         return (w.data_table[row_num][att_1] == sec_oprand(input, index, w, row_num));      
     }
     if (token == "!=") {
         return (w.data_table[row_num][att_1] != sec_oprand(input, index, w, row_num));        
     }
     if (token == "<") {
         return (w.data_table[row_num][att_1] < sec_oprand(input, index, w, row_num));       
     }
     if (token == ">") {
         return (w.data_table[row_num][att_1] > sec_oprand(input, index, w, row_num));       
     }
     if (token == "<=") {
         return (w.data_table[row_num][att_1] <= sec_oprand(input, index, w, row_num));        
     }
     if (token == ">=") {
         return (w.data_table[row_num][att_1] >= sec_oprand(input, index, w, row_num)); 
     }     
}

bool parse_condition(vector<string>& input, int& index, relation& w, int row_num);

bool parse_comparison(vector<string>& input, int& index, relation& w, int row_num) {
    string token;
    bool valid = false;
    token = input[++index];
    if (token == "(") {
        valid = parse_condition(input, index, w, row_num);  
        if (input[++index] != ")") {
            cout << "need ')' after nested condition" <<endl; 
        }      
    }
    else {
        index--;
        valid = parse_operation(input, index, w, row_num);              
    }
    return valid;
}

bool parse_conjunction(vector<string>& input, int& index, relation& w, int row_num) {
    string token;
    bool valid = true;
    do {
         valid = valid && parse_comparison(input, index, w, row_num);                    // check if it is comparison
         if (index > input.size()-3 )
             break;
         token = input[++index];
    } while (token == "&&");
    index--;
    return valid;
}

bool parse_condition(vector<string>& input, int& index, relation& w, int row_num) {
     string token;
     bool valid = false;
     do {
         valid = valid || parse_conjunction(input, index, w, row_num);                    // check if it is conjunction
         if (index > input.size()-3 )
             break;
         token = input[++index];
    } while (token == "||");                                                    // if get '||', look for another conjunction
    index--;                    
    return valid;
}

void DB_select(Token& t) {
    vector<string> input = t.get_condition();
    
    relation r;
    relation w = t.get_relation();
    
    
    for (int i = 0; i < w.data_table.size(); i++) {
        int index = -1;

        if (parse_condition(input, index, w, i) )
            r.data_table.push_back(w.get_row(i));
    }
    r.attribute_list.swap(w.attribute_list);      
    t.push_relation(r);    
}

void DB_project(Token& t) {
     relation w;
     relation r = t.get_relation();
     vector<data> tmp;
     vector<vector<data> > temp;
     int num = t.attribute_list.size();
     cout << "num of att_list: " << num << endl;

     int total = r.attribute_list.size();

// check match
     for(int i = 0; i < num; i++) {
         for (int j = 0; j < total; j++) {
             if (t.attribute_list[i] == r.attribute_list[j].getName() ) {
                 tmp = r.get_col(j);
                 temp.push_back(tmp);
                 attribute att;
                 att.setName(r.attribute_list[j].getName() );
                 att.setType(r.attribute_list[j].getType() );
                 w.attribute_list.push_back(att);
                 break;
             }
         }
     }
     
     
     for (int i= 0; i < r.data_table.size(); i++) {
        tmp.clear();
        for(int j = 0; j < temp.size(); j++) {          
            tmp.push_back(temp[j][i]);
        }
        w.data_table.push_back(tmp);
     }

     t.push_relation(w);  
}

void DB_rename(Token& t) {
     relation r = t.get_relation();
     for (int i = 0; i < r.attribute_list.size(); i++)
        r.attribute_list[i].setName(t.attribute_list[i]);


     t.push_relation(r);    
}

data str_to_data(string token) {
    data literal;
    if (token[0] == '\"') {
         literal.str = token;        
    }
    else if (is_int(token) ) {
         literal.str = token;
         literal.value = atoi(token.c_str());
    }
    return literal;
}

void DB_update(Token& t) {
    vector<string> input = t.get_condition();
    int id = t.relation_position[0];

    // todo check attlist_name matched or not
    for (int i = 0; i < relation_list[id].data_table.size(); i++) {
        int index = -1;
        if (parse_condition(input, index, relation_list[id], i) ) {
            for (int j = 0; j < t.attribute_list.size(); j++) {
                for (int k = 0; k < relation_list[id].attribute_list.size(); k++) {
                    if (t.attribute_list[j] == 
                                 relation_list[id].attribute_list[k].getName() ) {
                        relation_list[id].data_table[i][k] = str_to_data(t.literal_list[j]);
                        break;
                    }
                }
            }
        }
    }     
}

void DB_delete(Token& t) {
     vector<string> input = t.get_condition();
     int id = t.relation_position[0];

      for (int i = 0; i < relation_list[id].data_table.size(); i++) {
          int index = -1;
          if (parse_condition(input, index, relation_list[id], i) ) {
             relation_list[id].data_table.erase(relation_list[id].data_table.begin()+i);
             i=0;
          }
      }
}


void DB_open(Token& t){
     
    relation relationbuffer;
    ifstream openstream;
    string stringbuffer;
    string filename = t.tokens[1];
    filename = filename + ".db";
    openstream.open(filename);
    openstream >> stringbuffer;
    relationbuffer.relation_name = stringbuffer;
    openstream >> stringbuffer;
    while(stringbuffer != "ATTRIBUTES"){
        openstream >> stringbuffer;
        if(stringbuffer != "ATTRIBUTES"){
            relationbuffer.keys.push_back(stringbuffer);
        }
    }
    while(stringbuffer != "DATA"){
        openstream >> stringbuffer;
        if(stringbuffer != "DATA"){
            attribute attributebuffer;
            attributebuffer.setName(stringbuffer);
            openstream >> stringbuffer;
            attributebuffer.setType(stringbuffer);
            relationbuffer.attribute_list.push_back(attributebuffer);
        }
    }
    
    openstream >> stringbuffer;
    //DATA "hello there sir" ROW END

    while(stringbuffer != "END"){
        vector<data> data_elements;
        while(stringbuffer != "ROW"){
            if(stringbuffer != "ROW"){
                data databuffer;
				if(stringbuffer[stringbuffer.size()-1] == '\"'){
					databuffer.str = stringbuffer;
				}
				else if(stringbuffer[0] == '\"'){
                    databuffer.str = stringbuffer;
                    openstream >> stringbuffer;
                    while(stringbuffer[stringbuffer.size()-1] != '\"'){
                        databuffer.str = databuffer.str + " " + stringbuffer;
                        openstream >> stringbuffer;
                    }
                    if(stringbuffer[stringbuffer.size()-1] == '\"'){
                        databuffer.str = databuffer.str + " " + stringbuffer;
                    }
                }
				else{
					databuffer.str = stringbuffer;
				}
                data_elements.push_back(databuffer);
                openstream >> stringbuffer;
            }
        }
        relationbuffer.data_table.push_back(data_elements);
        openstream >> stringbuffer;
    }
	
	for(int i = 0; i < relationbuffer.attribute_list.size(); ++i){
		if(relationbuffer.attribute_list[i].getType() == "INTEGER"){
			for(int j = 0; j < relationbuffer.data_table.size(); ++j){
				relationbuffer.data_table[j][i].value = atoi(relationbuffer.data_table[j][i].str.c_str());
			}
		}
	}
	
    relation_list.push_back(relationbuffer);
    cout << "the name of the relation I pushed back is: "<<relationbuffer.relation_name<<"\n";

}

void DB_write(Token t){
     
    relation writebuffer;
    string rlt_name = t.tokens[1];  
    for (int i = 0; i < relation_list.size(); i++) {
        if (rlt_name == relation_list[i].relation_name) {
            writebuffer = relation_list[i];
        }         
    }
    string filename = writebuffer.relation_name;
    filename = filename + ".db";
    ofstream writestream;
    writestream.open(filename.c_str());
    writestream << writebuffer.relation_name << "\n";
    writestream << "KEYS ";
    for(int i = 0; i < writebuffer.keys.size(); ++i){
        writestream << writebuffer.keys[i] << " ";
    }
    writestream << "\n";
    writestream << "ATTRIBUTES ";
    for (int i = 0; i< writebuffer.attribute_list.size(); i++) {
        writestream << writebuffer.attribute_list[i].getName() << " ";
        writestream << writebuffer.attribute_list[i].getType() << " ";
    }
    writestream << "\n";
    writestream << "DATA ";
    for(int i = 0; i < writebuffer.data_table.size(); ++i){
        for(int j = 0; j < writebuffer.data_table[i].size(); ++j){
            writestream << writebuffer.data_table[i][j].str << " ";
        }
        writestream << "ROW ";
    }
    writestream << "END";
    
    
    writestream.close();
    
   
}




