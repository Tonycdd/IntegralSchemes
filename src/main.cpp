#include <iostream>
#include "parser.hpp"

void printWelcomeMessage(){
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout <<"  !WARNING!      !WARNING!          !WARNING!      !WARNING!      !WARNING!      !WARNING!       !WARNING!     " << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "Please follow the following example of how to use the program!" << std::endl;
    std::cout << "When you use any of the commands'define', 'run', 'all', 'find', please follow the following construction: define i(a,b) : &(a,b) , the current version of this program canno't recognize spaces ;(" << std::endl;
    std::cout << "So don't use it cause it wont add your scheme if u use spaces between the arguments" << std::endl; 
    std::cout << "EXAMPLE: define j(a, b, c, d): &(a, | (b,c), !(d)) -> should be written as it follows: define j(a,b,c,d) : &(a,|(b,c),!(d))" << std::endl;

    std::cout << "Digital Circuit Simulator" << std::endl;
    std::cout << "Enter commands (define, run, all,find) or 'exit' to quit" << std::endl;
}
int main(){
    //something to work on : ne raboti ako ia spaces between the args v expressiona // define i(a,b) : |(a, b,)

    IntegralSchemes schemes;
    std::string command;
    
    printWelcomeMessage();

 
    while(true) {
        std::cout << "> ";
        std::getline(std::cin, command);
        
        if(command == "exit") break;
        if(command.empty()) continue;
        
        try {
            parser p(command, schemes);
            //std::cout << "We defined current scheme!" << std::endl; 
            //schemes.listAllSchemes();
            //std::cout << std::endl;
        }
        catch(const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}   