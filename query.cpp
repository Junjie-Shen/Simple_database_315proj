#include <iostream>     
#include <string>    
#include <stdio.h>
using namespace std;





// test if the token is a relation name

/*
relation find_relation(string name) {
    for(int i = 0; i< relation_set.size(); i++)
        if (relation_set[i].relation_name == name)
            return relation_set[i];
        else {
            cout << "no_relation found" <<endl;
            exit(0);
        }         
}
*/

bool relation_name(Token& t) {
     string token;
     token = t.get();                                              // get next token from Token t
    if (!is_identifier(token)) {                                   // check if it is an identifier
        cout << "wrong relation_name: " << token <<endl;
        return false;
    } 
    else {                
        for(int i = 0; i < relation_list.size(); ++i) {            // test if the relation is in the global vector
            if(relation_list[i].relation_name == token) {
                t.relation_set.push(relation_list[i]);             // save the relation in stack
                t.relation_obj.push_back(token);                   // save the relation name
                t.relation_position.push_back(i);                  // save its position
                return true;
            }
        }        
        cout << "no matched relation found: "<< token <<endl;
        return false;                          
    }   
}

bool expression(Token& t);
bool condition(Token& t);

// if it is an atomic expression
bool atomExpr(Token& t) {
     string token;
     token = t.get();
     if (token == "(") {                              // if '(' found, look for expression
         bool is_expression = expression(t);    
         
         
         token = t.get();                             
         if (token ==")") {
             cout << "is (expr) = " << is_expression << endl;      
             return is_expression;    
         }
         else {
             cout<< "missing ')' after expr: " << token <<endl;
             return false;   
         }
         
     }
     else {
         t.unget();                                   // if '(" not found, look for relation_name
         return relation_name(t);    
     } 
}

// test if the token is a valid attribute name
bool attribute_name(Token& t) {
     string token;
     token = t.get(); 
         
    if (is_identifier(token)) {
        t.attribute_list.push_back(token);                      
        return true;
    }  
    else {
        t.unget();
        return false;
    }  
}

bool operand_name(Token& t) {
     string token;
     token = t.get(); 
       
    if (is_identifier(token)) {
        t.condition_stream.push_back(token);                       
        return true;
    }  
    else {
        t.unget();
        return false;
    }  
}

// test if the token is a valid literal
bool cond_literal(Token& t) {
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
        t.condition_stream.push_back(tmp);    
    }
    else {                                          // if the token is not a string, check if it is a int
        if (!is_num(token[0]) 
            && token[0] != '-'                      // positive or negative sinals
            && token[0] != '+')  return false;
         
        for (int i=1; i < token.length(); i++)
            if (!is_num(token[i]) ) 
                return false;  
                
        t.condition_stream.push_back(token);                   
    } 
    
    
    return true; 
}

// test if it is an operand
bool operand(Token& t) {
    if (operand_name(t))                            // check if operand is an attribute name 
       return true;
    else if (cond_literal(t))                       // or a literal
         return true;
    else {
         cout << "wrong operand" <<endl;
         return false;
    }
}

// check if the token is an operator
bool op(Token& t) {
    string token = t.get();
    if (is_operator(token) ) {
        t.condition_stream.push_back(token);                    
        return true;
    }
    else {
        cout << "miss op in condition"<< token <<endl; 
        return false;
    }
} 

// check if it is a valid comparison pattern 
bool comparison(Token& t) {
    string token;
    token = t.get();
    if (token == "(") {
        t.condition_stream.push_back(token);
        if (condition(t) ) {
            cout << "is_nested_condition = 1 " <<endl;                 
        }
        else {
            cout << "is_nested_condition = 0 " <<endl;  
            return false;   
        }
        token = t.get();
        if (token == ")") {
            t.condition_stream.push_back(token);      
            return true;
        }          
        else {
            cout << "missing ')' after nested condition"<<endl;
            return false;
        }  
    }
    else {
               
        t.unget();                
        return operand(t) && op(t) && operand(t); // check if it is operand op operand  
    }
}

// check if it is a valid conjunction pattern 
bool conjunction(Token& t) {
    string token;
    
    do{
        if (comparison(t))  {                      // check if it is comparison
            token = t.get();
            t.condition_stream.push_back(token);
        }
        else {
            cout << "not comparison"<<endl; 
            return false; 
        }
    } while (token == "&&");                      // if get '&&', look for another comparison
    
   
    t.unget();
    t.condition_stream.pop_back();
    return  true;       
}

// check if it is a condition
bool condition(Token& t) {
    string token;  

    do {
        if (conjunction(t))  {                     // check if it is conjunction
            token = t.get();
            t.condition_stream.push_back(token); 
        }
        else {
            cout << "not conjunction"<<endl; 
            return false;
        }
    } while (token == "||");                      // if get '||', look for another conjunction
/*    
    if (token == ")") {
        return true;
    }          
    else {
        cout << "missing ')' after condition"<<endl;
        return false;
    }  
*/   
    t.unget();
    t.condition_stream.pop_back();
    return true;              
}

/*
// check if it is a selection
bool select(Token& t) {
    string token;
    token = t.get();
    bool is_condition = false;
    if (token == "(") {
        t.condition_stream.push_back(token);
        is_condition = condition(t);               // look for condition
        cout << "is cond = " << is_condition << endl;
    }
    else {
        cout << "missing '(' before condition"; 
        return false;     
    }         
    
    bool is_atomExpr = atomExpr(t);                // look for atomic expression 
    
    return is_condition && is_atomExpr ;        
}
*/


// check if it is a selection
bool select(Token& t) {
    string token;
    
    token = t.get();
    if (token != "(") {
        cout << "miss '(' before condition" << endl;      
        return false;      
    }
    
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
         
    if (atomExpr(t) ) {
        cout << "is_atomExpr = 1" <<endl;                 
    }
    else {
        cout << "is_atomExpr = 0" <<endl; 
        return false;  
    }         
    
    return true;        
}

// check if it is an attribute list
bool attribute_list(Token& t) {
     string token;
     do {
        if (attribute_name(t))                    // check attribute name
            token = t.get();
        else {
            cout << "not attribute_name"<<endl;   
            return false;
        }
     } while (token == ",");                      // if ',' look for next attribute name
     
     if (token == ")") {
        return true;
    }          
    else {
        cout << "missing ')' after attlist"<<endl;
        return false;
    }                
}

// check if it is a projection
bool project(Token& t) {
    string token;
    token = t.get();
    bool is_attribute_list = false;
    if (token == "(") {
        is_attribute_list = attribute_list(t);     // look for attribute list
        cout << "is attlist = " << is_attribute_list << endl;
    }
    else {
        cout << "missing '(' before attlist"; 
        return false;     
    }         
    
    bool is_atomExpr = atomExpr(t);                // look for atomic Expression
    cout << "is atomExpr = " << is_atomExpr << endl;
    
    return is_attribute_list && is_atomExpr ;   
}

// check if this is a rename function
bool rename(Token& t) {
    string token;
    token = t.get();
    bool is_attribute_list = false;
    if (token == "(") {
        is_attribute_list = attribute_list(t);      // look for attribute list
        cout << "is attlist = " << is_attribute_list << endl;
    }
    else {
        cout << "missing '(' before attlist"; 
        return false;     
    }         
    
    bool is_atomExpr = atomExpr(t);                 // look for atomic expression
    
    return is_attribute_list && is_atomExpr ;   
}



// check if this is an expression
bool expression(Token& t) {
     bool is_expression = false;
     string token;
     token = t.get();
     if (token == "select") {                       // check if it is a valid selection
         is_expression = select(t);  
         cout << "is select = " << is_expression << endl;
         
         if (is_expression) {
             DB_select(t);     
         }  
          
     }
     else if (token == "project") {                 // check if it is a valid projection
         is_expression = project(t);  
         cout << "is proj = " << is_expression << endl;   
         
         if (is_expression) {
             DB_project(t);
         }

     }
     else if (token == "rename") {                  // check if it is a valid rename function
         is_expression = rename(t); 
         cout << "is rename = " << is_expression << endl; 

         if (is_expression) {
             DB_rename(t);
         }
   
     }
     else {
         t.unget(); 
         if (atomExpr(t) ) {
             cout << "is_atomExpr = 1" <<endl;
         }
         else {
             cout << "is_atomExpr = 0" <<endl;
             return false;
         } 

         do {
             token = t.get();
             if (token == "+") {   
                 if (atomExpr(t) ) {             
                     cout << "is union = 1 " << endl; 
                     DB_union(t);
                 }
                 else {
                     cout << "no atomExpr after '+' "<<endl; 
                     return false; 
                 }
             }
             else if (token == "-") {   
                  if (atomExpr(t) ) { 
                     cout << "is difference = 1 " << endl; 
                     DB_difference(t);
                  }
                  else {
                     cout << "is difference = 0 " << endl; 
                     return false;  
                  }
             }
             else if (token == "*") {     
                  if (atomExpr(t) ) { 
                     cout << "is product = 1 " << endl; 
                     DB_product(t);
                  }
                  else {
                     cout << "is product = 0 " << endl; 
                     return false;  
                  }         
             }
             else {
                 break;
             }
         } while (true);
         
         if (token == ")" || token == ";")
             is_expression = true;
         else { 
             cout << "wrong token after +-* second atomExpr" <<endl;
             return false;
         }
         t.unget();
     }
     return is_expression;
}

// check if the query statement is valid
bool query(Token& t){
    string token;
    
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
    if (token != "<-") {
        cout << "miss '<-' after relation_name" << endl;      
        return false;      
    }
           
    if (expression(t) ) {
        cout << "is_expression = 1" <<endl;                 
    }
    else {
        cout << "is_expression = 0" <<endl; 
        return false;  
    }
    
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

    for (int i = 0; i < relation_list.size(); i++) {
        if (t.relation_name == relation_list[i].relation_name) {
           cout << "replacing existed relation_name: " << token <<endl;
           relation_list.erase(relation_list.begin() + i);
        }         
    } 
    
    DB_assign(t);
    
    return true;
}



















