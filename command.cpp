#include <iostream>     
#include <string>    
#include <stdio.h>
using namespace std;


//open commands, handle grammar: open-cmd ::== OPEN relation-name 
bool open(Token& t) {
    string token;
     token = t.get();                                              // get next token from Token t
    if (!is_identifier(token)) {                                   // check if it is an identifier
        cout << "wrong relation_name: " << token <<endl;
        return false;
    } 
    return true;  
}

//close commands, handle grammar: close-cmd ::== CLOSE relation-name
bool close(Token& t) {
    return relation_name(t);       
}

//write commands, handle grammar: write-cmd ::== WRITE relation-name
bool write(Token& t) {
    return relation_name(t);       
}

//show command, handle grammar: show-cmd ::== SHOW atomic-expr;
bool show(Token& t) {
    return atomExpr(t);       
}

//identify the attribute types given a Token class patameter:
bool attType(Token& t) {
    string token;
    token = t.get();
    
    if (token == "INTEGER" ) {
        t.attribute_type.push_back(token);      
        return true;
    }
    
    if (token.substr(0,7) != "VARCHAR") {
        cout << "no INTEGER or VARCHAR"<< token.substr(0,7) <<endl;
        return false;                  
    }
    
    t.attribute_type.push_back("VARCHAR");   
    
    if (token[7] != '(' ) {
        cout << "miss '(' after VARCHAR" <<endl;
        return false;         
    }
    
    int len = token.length();
    for (int i = 8; i < len - 1; i++)
        if (!is_num(token[i]) )  {
            cout << "not a number: " << token[i] <<endl;
            return false; 
        }
    
    if (token[len - 1] != ')' ) {
        cout << "miss ')' after VARCHAR" <<endl;
        return false;         
    }
    
    return true;    
}

// test if the token is a valid attribute name
bool typed_attribute_name(Token& t) {
     string token;
     token = t.get(); 
         
    if (is_identifier(token)) {
        t.typed_attribute_list.push_back(token);                      
        return true;
    }  
    else {
        t.unget();
        return false;
    }  
}


//identify the attribute name and its type by the given Token class
bool attName_type(Token& t) {
     if (typed_attribute_name(t) ) {                   //check the attribute name
         cout << "is_att_name = 1" <<endl;
     }
     else {
         cout << "is_att_name = 0" <<endl;
         return false;
     }
     
     if (attType(t) ) {                                //check the attribute type
         cout << "is_attType = 1" <<endl;
     }
     else {
         cout << "is_attType = 0" <<endl;
         return false;
     }  
     
     return true;         
}

// abstract the whole typed atttibute list
bool typed_attlist(Token& t) {
     string token;
     do {
        if (attName_type(t) )
            token = t.get();
        else {
            cout << "is_attName_type = 0"<<endl; 
            return false;
        }
     } while (token == ",");
     
     if (token == ")") {
        return true;
    }          
    else {
        cout << "missing ')' after typed_attlist"<<endl;
        return false;
    }    
}

// parse CREATE function
bool create(Token& t) {
    string token;
    token = t.get();
    
    if (token != "TABLE") {
        cout << "miss TABLE after CREATE" << endl;      
        return false;      
    }
    
    token = t.get();                             // get next token from Token t       
    if (is_identifier(token)) {                  // check if it is an identifier
        cout << "is relation_name = 1 " <<endl;
        t.set_relation_name(token);
    } 
    else { 
        cout << "is relation_name = 0 " <<endl; 
        return false; 
    } 
      
    token = t.get();
    if (token != "(" ) {
        cout << "miss '(' before typed-attlist" << endl;
        return false;          
    }
    
    if (typed_attlist(t) ) {
        cout << "is_typed_attlist = 1" << endl;
    }
    else {
        cout << "is_typed_attlist = 0" << endl; 
        return false;    
    }
    
    token = t.get();
    if (token != "PRIMARY" ) {
        cout << "miss PRIMARY after typed-attlist" << endl;
        return false;        
    }
    
    token = t.get();
    if (token != "KEY" ) {
        cout << "miss KEY after PRIMARY" << endl;
        return false;        
    }
    
    token = t.get();
    if (token != "(" ) {
        cout << "miss '(' before attribute_list" << endl;
        return false;          
    }
    
    if (attribute_list(t) ) {
        cout << "is_attribute_list = 1" << endl;                
    }
    else {
        cout << "is_attribute_list = 0" << endl;
        return false; 
    }
       
    return true;
         
}

// test if the token is a valid literal
bool literal(Token& t) {
    string token = t.get();
    
    if (token[0] == '\"') {                         // check if the token is null
        string tmp;
        int s = token.length()-1;
        if (s < 0) {
            cout << "0-length literal" << endl;  
            return false; 
        } 
        tmp = token;
        while (token[s] != '\"') {                  // look for the second "
            token = t.get();               
            s = token.length() - 1;      
            if (token == "" || token[0] == '\"') {
                cout << " miss second double quote in literal" <<endl;
                return false;
            }
            tmp = tmp + " " + token;
        }
     t.literal_list.push_back(tmp);       
    }
    else {                                          // if the token is not a string, check if it is a int
        if (!is_num(token[0]) 
            && token[0] != '-'                      // positive or negative sinals
            && token[0] != '+')  return false;
         
        for (int i=1; i < token.length(); i++)
            if (!is_num(token[i]) ) 
                return false;   
     t.literal_list.push_back(token);                             
    } 
    
    return true; 
}

// check the attribute name = literal in UPDATE function
bool att_assign(Token& t) {
     string token;
     
     if (attribute_name(t) ) {
         cout << "is att_name = 1" <<endl;
     }
     else {
         cout << "is att_name = 0" <<endl; 
         return false;    
     }
     
     token = t.get();
     if (token != "=") {
        cout << "miss '=' " << endl;      
        return false;      
     }
     
     if (literal(t) ) {
         cout << "is literal = 1" <<endl;
     }
     else {
         cout << "is literal = 0" <<endl; 
         return false;    
     }
     
     return true;  
}

// check if the update statement is valid
bool update(Token& t) {
    string token;
    
    if (relation_name(t) ) {
        cout << "is_relation = 1" <<endl;                 
    }
    else {
        cout << "is_relation = 0" <<endl; 
        return false;  
    }
    
    token = t.get();
    if (token != "SET") {
        cout << "miss SET " << endl;      
        return false;      
    }
    
    token = t.get();
    if (token == "(") {
       do {
           if (att_assign(t) )
               token = t.get();
           else {
               cout << "is att_assign = 0"<<endl; 
               return false;
           }
       } while (token == ","); 
       
       if (token != ")") {
           cout << "miss ')' after attribute = literal: " << endl;      
           return false;      
       }            
    }
    else { 
        t.unget();     
        do {
            if (att_assign(t) )
                token = t.get();
            else {
                cout << "is att_assign = 0"<<endl; 
                return false;
            }
        } while (token == ",");
        t.unget();       
    }
    
    token = t.get(); 
    if (token != "WHERE") {
        cout << "miss WHERE: " << token << endl;      
        return false;      
    }
    
    token = t.get();
    if (token == "(") {
        if (condition(t) ) {
            cout << "is_condition = 1" <<endl; 
            t.push_condition();                             
        }
        else {
            cout << "is_condition = 0" <<endl; 
            return false;  
        }
        token = t.get();
        if (token != ")") {
            cout << "missing ')' after condition" << endl;      
            return false;      
        }
        return true;           
    }
    else {
        t.unget();
        if (condition(t) ) {
            cout << "is_condition = 1" <<endl;
            t.push_condition(); 
            return true;                 
        }
        else {
            cout << "is_condition = 0" <<endl; 
            return false;  
        } 
    }
    
}

// parse the delete function
bool Delete(Token& t) {
	string token;
	
    token = t.get();
    if (token != "FROM") {
        cout << "miss FROM after DELETE " << endl;      
        return false;      
    }
    
    if (relation_name(t) ) {
        cout << "is_relation = 1" <<endl;                 
    }
    else {
        cout << "is_relation = 0" <<endl; 
        return false;  
    }
    
    token = t.get();
    if (token != "WHERE") {
        cout << "miss WHERE after relation_name " << endl;      
        return false;      
    }
    
    token = t.get();
    if (token == "(") {
        if (condition(t) ) {
            cout << "is_condition = 1" <<endl; 
            t.push_condition();                             
        }
        else {
            cout << "is_condition = 0" <<endl; 
            return false;  
        }
        token = t.get();
        if (token != ")") {
            cout << "missing ')' after condition" << endl;      
            return false;      
        }
        return true;           
    }
    else {
        t.unget();
        if (condition(t) ) {
            cout << "is_condition = 1" <<endl;
            t.push_condition(); 
            return true;                 
        }
        else {
            cout << "is_condition = 0" <<endl; 
            return false;  
        } 
    }
}

bool is_literal_list(Token& t)      //detect literal list in sequence like :
{                                   //literal, literal, literal)
   string token;
   do
   {
      if(literal(t))
         token = t.get();
      else {
         return false;
      }
   }
   while(token == ",");
   if(token == ")")
      return true;
   else
      return false;
}

bool insert(Token& t)
{
    string token;
	
    token = t.get();
    if (token != "INTO") {
        cout << "miss INTO after INSERT " << endl;      
        return false;      
    }
    
    if (relation_name(t) ) {
        cout << "is_relation = 1" <<endl;                 
    }
    else {
        cout << "is_relation = 0" <<endl; 
        return false;  
    }
    
    token = t.get();
    if (token != "VALUES") {
        cout << "miss VALUES " << endl;      
        return false;      
    }
    
    token = t.get();
    if (token != "FROM") {
        cout << "miss FROM after VALUES " << endl;      
        return false;      
    }
    
    token = t.get();
    if (token != "(") {
        if (token != "RELATION") {
            cout << "no '(' or RELATION found " <<endl;      
            return false;
        }
        
        if (expression(t) ) {
            cout << "is_expr = 1" <<endl; 
            return true;                
        }
        else {
            cout << "is_expr = 0" <<endl; 
            return false;  
        }          
    }
    else {
         if(is_literal_list(t) ) {
            cout << "is_literal_list = 1" <<endl;             
            return true;
         }
         else {
            cout << "is_literal_list = 0" <<endl;  
            return false;
         }
    }   
}

// check the semicolum after each statement
bool is_semicolum(Token& t) {
    string token;
    token = t.get();
    if (token != ";" ) {
        cout << "require ';' before: " << token << endl;
        return false;          
    }
     
    token = t.get();
    if (token != "" ) {
        cout << "some token after ';': " << token << endl;
        return false;          
    }

    return true;
}

bool command(Token& t){
     bool valid_cmd = false;
     string token;
     token = t.get();
     if (token == "OPEN" ) {
          valid_cmd = open(t) && is_semicolum(t);
          
           if (valid_cmd) {
             DB_open(t);     
         } 
        
     }
     
     if (token == "CLOSE" ) {
          valid_cmd = close(t) && is_semicolum(t);    
          
           if (valid_cmd) {
             DB_close(t);     
         } 
    
     }
     
     if (token == "WRITE" ) {
          valid_cmd = write(t) && is_semicolum(t);   
         
           if (valid_cmd) {
             DB_write(t);     
         } 
          
     }
     
     if (token == "EXIT" ) {
          valid_cmd = is_semicolum(t);     
     }
     
     if (token == "SHOW" ) {
          valid_cmd = show(t) && is_semicolum(t);   
          
           if (valid_cmd) {
             DB_show(t);     
         } 
    
     }
     
     if (token == "CREATE" ) {
          valid_cmd = create(t) && is_semicolum(t); 
          
           if (valid_cmd) {
             DB_create(t);     
         } 
      
     }
     
     if (token == "UPDATE" ) {
          valid_cmd = update(t) && is_semicolum(t);  
          
           if (valid_cmd) {
             DB_update(t);     
         } 
 
     }
     
     if (token == "INSERT" ) {   
          valid_cmd = insert(t) && is_semicolum(t);
          
           if (valid_cmd) {
             DB_insert(t);     
         } 
       
     }
     
     if (token == "DELETE" ) {
          valid_cmd = Delete(t) && is_semicolum(t);
          
           if (valid_cmd) {
             DB_delete(t);     
         } 
       
     }
 /*    
     token = t.get();
     if (token != ";" ) {
        cout << "require ';' before: " << token << endl;
        return false;          
     }
     
     token = t.get();
     if (token != "" ) {
        cout << "some token after ';': " << token << endl;
        return false;          
     }
 */      
    return valid_cmd;  
}
