#pragma once
#include<string>
#include <vector>
#include <iostream>

//define i(a,b) : &(a,b);
struct information
{
    std::string command; // za imeto na tekushtata commanda - 'define', 'run', 'find', 'all';
    std::string integralName; // i
    std::vector<std::string> arguments; //(a,b) -> {a,b};
    std::string logic; // &(a,b)

    //default ctor
    information() = default;
    //ctor with arguments
    
    information(const std::string& c,const std::string& integralN,const std::vector<std::string>& arg,const std::string& l) : command(c),integralName(integralN), 
    arguments(arg), logic(l) {};

    void print() const{
        std::cout << "Scheme name: "<< integralName << std::endl;
        std::cout << "Arguments: " ;
        for (std::size_t i = 0; i < arguments.size(); i++)
        {
           std::cout << arguments[i]; 
        }
        std::cout << std::endl;
         // vinagi shte e samo edin
        std::cout << "Logic: " << logic;
        
    }
    //dtor - default zashtoto izpozlvame stl, a tam te si imat definirani dtor
    ~information() = default;
};
