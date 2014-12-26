#include <iostream>     
#include <string>    
#include <stdio.h>
#include <ctype.h>  
#include<vector>
#include<sstream> 
#include "token.cpp"
#include "DBcore.cpp"
#include "query.cpp"
#include "command.cpp"

using namespace std;


// check if the first token is a key word of command
bool is_command (string first_token) {
     if (first_token == "OPEN"
      || first_token == "CLOSE"
      || first_token == "WRITE"
      || first_token == "EXIT"
      || first_token == "SHOW"
      || first_token == "CREATE"
      || first_token == "UPDATE"
      || first_token == "INSERT"
      || first_token == "DELETE" ) return true;
      
     else 
         return false;     
}

void program (string input) {
    Token t(input);
    
    string token = t.get();
    t.unget();
          
    if (is_command(token) ) {
        if (command(t) ) {                       // test if the input is a valid command
            cout << "successful operation" << endl;
        }    
        else
            cout << "invalid statement" <<endl;
    }
    else {
        if (query(t) ) {                         // test if the input is a valid query
              cout << "successful operation" << endl;  
        }
        else
            cout << "invalid statement" <<endl; 
    }
     cout << "number of relation: " << relation_list.size() << endl;
}


int main () {     
    string input;
    cout << "Enter one statement each time (type EXIT; to quit)"<<endl; 
    while (getline(cin, input)) {               // keep getting the statement
         if (input != "EXIT;")
             program(input);
         else
             break;
    }

    return 0;
}
