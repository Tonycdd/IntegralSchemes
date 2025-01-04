#pragma once
#include "information.hpp"
#include <sstream>
#include <stack>
#include <cctype>
#include <iostream>
#include <map>
#include <unordered_map>
#include <fstream>

class IntegralSchemes
{
private:
    // nai-dobra ideq e da suxranqvame kato kluch-stoinost, i taka shte imame tursene O(1) za dadenite sxemi, kuliiziite ne sa chestna praktika!
    // izpozlvame unodrder_map zashtoto ima po-dobra proizvodirelnost na tursene, dobavqne i triene;
    std::unordered_map<std::string, information> schemes; // tova e konteinerut, sudurjasht vsichki sxemi;
    
public:
    // dobavqme sxema
    void addScheme(const information& info){
        this->schemes[info.integralName] = info;
    }

    //tursim dadenta sxema 
    information& findScheme(const std::string& name){
        //find vrushta iterator
        auto it = this->schemes.find(name);
        if(it == this->schemes.end()){
            throw std::invalid_argument("Scheme not found!");
        }
        return it->second;
    }
    //za comandata run

    //printene na vsichki sxemi
    void listAllSchemes() const {
        for (const auto& pair : schemes) {
            pair.second.print();
        }
    }

    bool hasScheme(const std::string& name) const{
        
        
        for(const auto& pair:schemes){
            if (pair.first == name)
            {
                return true;
            }            
        }
        return false;
    }

    // we dont need other ctors here ot dtors
    IntegralSchemes() = default;    
    ~IntegralSchemes() = default;
};


class parser
{
public:
    parser() = delete;
    
    // tuk se xvurlq greshka v ctor, zatova trqbva nqkoi ot vunshniq svqt da go handelne
    explicit parser(const std::string& input,IntegralSchemes& sch) : schemes(sch)
    {
        try
        {
            parse(input);
        }
        catch(const std::invalid_argument& e)
        {
            throw;   
        }
        
    }

    // vika dtor na inforamtion
    ~parser() = default;
    information& getInfo(){
        return this->info;
    }




    IntegralSchemes& schemes; // referenciq kum konteinerut sus sxemi
    information info;

    void parse(const std::string& input){
        //potok za izvlichane na info on input
        std::istringstream ss(input);
        std::string command;

        // parsing into command
        ss >> command;

        //da vidim koq e komandata ot inputa
        if(command == "define"){
            parseDefine(ss, command);
            schemes.addScheme(this->info);
        }
        else if( command == "run"){
            int res = parseRun(ss, command);
            std::cout << "Result: " << res << std::endl;
        }
        else if(command == "all"){
            parseAll(ss, command);
        }
        else if(command == "find"){
            parseFind(ss,command);
        }
        else{
            throw std::invalid_argument("Invalid command " + command);
        }  
    };
    
    // define i(a,b) : &(a,b)
    void parseDefine(std::istringstream& ss, const std::string& command){
        std::string name, args, expression, dontNeed;
        
        // premaxvame whitespaces v nachaloto
        ss >> std::ws;

        // chete do purvata otvarqshta
        std::getline(ss,name, '(');
        //name = "i"
        if(name.empty() || name.size() > 1){
            throw std::invalid_argument("Invalid syntax for function name (empty or more than one symbol)!");
        }

        //iskame da vidim dali e bila definirana veche
        if(this->schemes.hasScheme(name)){
            throw std::invalid_argument("Here you are trying to redefine a defined intregral scheme! Integral scheme '" + name + "' is already defined!");
        }

        //chete do purvata zatvarqshta
        std::getline(ss,args,')');
        if(args.empty()){
            throw std::invalid_argument("Invalid syntax for arguments(zero args)!");
        }

        //args = "a,b"
        ss >> std::ws;

        ss >> dontNeed;
        if(dontNeed != ":"){
            throw std::invalid_argument ("Invalid syntax for command!");
        }
        // zapisali sme v dontNeed ":"

        ss>>std::ws;

        ss >> expression;
        if(!isCorrectExpression(expression, args)){
            throw std::invalid_argument("Invalid syntax for logic expression");
        }
        //expression = "&(a,b)";


        // stoinosite sa prozivolni, zatova sega go zapulvame pravilno!
        information infoCopy;

        infoCopy.command = command;
        infoCopy.integralName = name;
        // da zapulnim argumentite
        std::istringstream arg(args);
        //"a,b"
        std::string currentArg;
        while(std::getline(arg,currentArg, ',')){
            if(currentArg.size()>1){
                throw std::invalid_argument("One of the symbols is more than one char long!");
            }
            infoCopy.arguments.push_back(currentArg);
        }


        infoCopy.logic = expression;
        //prisvoqvame informaciqta na tekushtiqt obekt v parse - information
        // ne e nujno deep copy, tui kato rabotim samo sus STL konteineri i nqmae dynamic memory;
        this->info = infoCopy;
    }


    bool isCorrectExpression(const std::string& exp, const std::string& args){
        // trqbva da proverim za slednite neshta:
        // 1. dali ima raven broi ( i ) skobi
        // 2. dali ima argument, koito ne e bil definiran
        // 3. dali ima operaciq, koqto ne e pozvolena - razlichna ot &, | i ~
        
        if(itHasProblemWithBrackets(exp))
        {
            return false;
        }

        else if(itUsesUndefinedArguments(exp,args)){
            return false;
        }

        else if(itUsesUndefinedOperation(exp)){
            return false;
        }
        else if(hasMoreThanOneCharLongArgumentInLogicExpression(exp)){
            return false;
        }
        return true;
    }

    bool itHasProblemWithBrackets(const std::string& exp){
       // std::cout << "Checking brackets for expression: " << exp << std::endl;
        std::stack<char> stack;
        // &(a,,,,) = exp
        
        for (std::size_t i = 0; i < exp.size(); i++)
        {
            if(exp[i] == '('){
                stack.push(exp[i]);
            }

            else if(exp[i] == ')'){
                if(stack.empty()){
                    return true;
                }

                stack.pop();
            }
        }
        if(stack.empty()){
            return false;
        }
        return true;
    }

    bool itUsesUndefinedArguments(const std::string& exp, const std::string& args){
        //std::cout << "Checking arguments for expression: " << exp << std::endl;
        const std::string& validArguments = args;

        for (std::size_t i = 0; i < exp.size(); i++)
        {
            // ako e bukva, to shte proverim dali e ot validnite argumenti, ako ne e, to vrushtame true;
            if(isalpha(exp[i]) && validArguments.find(exp[i]) == std::string::npos){
                return true;
            }
        }
        return false;
        
    }

    bool hasMoreThanOneCharLongArgumentInLogicExpression(const std::string& expression){
        std::string valuesInString = "!&|,()"; // izbroqvame neshtata v string, koito ne ni interesuvat, no mogat da se sreshtnat
        for (std::size_t i = 0; i < expression.size() - 1; i++)
        {
            if(isalpha(expression[i]) && valuesInString.find(expression[i]) == std::string::npos){
                //znachi e bukva, togava proverqvame i sledvashtoto
                char next = expression[i+1];
                if(isalpha(next) && valuesInString.find(next) == std::string::npos){
                    return true;
                }
            }
        }
        return false;
        
    };

    bool itUsesUndefinedOperation(const std::string& exp){
        //std::cout << "Checking operations for expression: " << exp << std::endl;
        const std::string validOperations = "&|!(),";
        for (std::size_t i = 0; i < exp.size(); i++)
        {
            // moje da e bukva, ',', '(', ')', '&', '|', '~'
            if(isalpha(exp[i])){
                continue;
            } 
            
            //std::find vrushta ni poziciqta na koqto se namira turseniqt char, ako ne se nameri se vrushta nai-golqmata stoinost za size_t, koeto
            // e zadeno kato std::string::npos, t.e taka se pokazva che tozi char ne e ot pozvolenite!
            else if(validOperations.find(exp[i]) == std::string::npos){
                return true;
            }
        }
        return false;
    }

    int parseRun(std::istringstream& ss,const std::string& command){
        // run i(1,0);
        std::string integralName, args;

        // premaxvame white spaces
        ss >> std::ws;  

        std::getline(ss,integralName, '(');
        if(integralName.empty() || integralName.size() > 1){
            throw std::invalid_argument("No integral name!");
        }

        std::getline(ss,args, ')');
        // args = "1,0"
        if(args.empty()){
            throw std::invalid_argument("Invalid syntax for function arguments!");
        }

        // trqbva da prevurnem argumentite v chisla i da gi zapazim
        std::vector<int> inputValues;
        std::istringstream values(args);
        std::string currentValue;

        while(std::getline(values, currentValue, ',')){
            if(currentValue == "0" || currentValue == "1"){
                inputValues.push_back(std::stoi(currentValue));
            }
            else{
                throw std::invalid_argument("Invalid arguments! ( ONLY ones and zeros!))");
            }
        }

        //tursim sxemata v konteinera, ako ne e tam to shte se xvurli std::invalid_argument
        try
        {
            information& infoScheme = schemes.findScheme(integralName);
            if(infoScheme.arguments.size() != inputValues.size()){
                throw std::invalid_argument("Invalid number of arguments!");
            }
    
            int result = evaluateResult(infoScheme, inputValues);
            return result;
        }
        catch(const std::invalid_argument e)
        {
            std::cerr << e.what() << std::endl;
            throw;
        }
    }

    int evaluateResult(const information& schemeInfo, std::vector<int>& values){
        // funkciqta shte se zanimava s tova da namira rezultat ot dadenata sxema
        std::stack<int> result; // shte sudurja tekushtite elementi nujni za operaciqta ni 

        // logikata e vsushnost nqkakuv string &(...)
        
        // pravim copie na logic, za da ne barnikame po nego i da go razvalim
        std::string expression = schemeInfo.logic;

        // За улеснение интегралните схеми ще използват префиксен запис за всяка функция - първо е името ѝ, 
        // а след него в скоби са изброени аргументите, разделени със запетаи. & и | могат да приемат два или повече аргументи, докато ! - само един.
        // Имената на функциите и техните аргументи са с дължина точно един символ.

        std::map<char, int> val;

        for (size_t i = 0; i < schemeInfo.arguments.size(); i++){
            // might have problem here we will see
            char ch = schemeInfo.arguments[i][0];
            val[ch] = values[i];
        }

        for (int i = expression.length() - 1; i >= 0; i--)
        {   
            if(std::isspace(expression[i]) || expression[i] == '(' || expression[i] == ')' || expression[i] == ','){
                continue;
            }

            // ako e nqkava bukva, shte izpozlvame konteinerut map, za da moje da gi svurjem pravilno stoinost - s argument;
            else if(isalpha(expression[i])){
                if(val.find(expression[i]) == val.end()){
                    // nqmame takuv key vuv val;
                    throw std::invalid_argument("No such argument in our defined arguments!");
                }
                result.push(val[expression[i]]);
            }
            else if(expression[i] == '&' || expression[i] == '|' || expression[i] == '!'){
               
                if (expression[i] == '!')
                {
                        // imame nujda ot pone edin element da imame veche v staka
                    if(result.empty()){
                        throw std::invalid_argument("Not enough operands!");
                    }
                    int operand = result.top();
                    result.pop();
                    result.push(!operand);
                }
                else 
                {
                    std::vector<int> operands;
                    // vsichki nalichni operandi ni trqbvat, tui kato te vzimat dva ili poveche argumenta
                    while (!result.empty() && operands.size() < values.size()) {
                        operands.push_back(result.top());
                        result.pop();
                    }
                    
                    if (operands.size() < 2) {
                        throw std::invalid_argument("Not enough operands!");
                    }

                    if (expression[i] == '&') {
                        int res = 1;
                        for (int op : operands) {
                            res &= op;
                        }
                        result.push(res);
                    }
                    else if (expression[i] == '|') {
                        int res = 0;
                        for (int op : operands) {
                            res |= op;
                        }
                        result.push(res);
                    }
                }
            }
        }
        if(result.empty()){
            throw std::invalid_argument("Invalid expression - there is no result!");
        }

        if(result.size() > 1){
            throw std::invalid_argument("Too many operands left!");
        }

        return result.top();

    }

    void parseAll(std::istringstream& ss,const std::string& command){
        std::string integralName;

        ss>>std::ws;

        ss >> integralName;
        if (integralName.empty() || integralName.size() > 1)
        {
            throw std::invalid_argument("Not valid integralName! (zero or more than one characters long)");
        }

        //dali e definirana izobshto tazi integralna sxema?
         try
        {
            // tursim dadenata sxema v konteintera
            information& infoScheme = schemes.findScheme(integralName);

            std::cout << "Truth table for " + infoScheme.integralName << std::endl;

            //purvo da generirame samite argumenti -> t.e purviqt red na tablicata ni
            int it = 0;
            while(it < infoScheme.arguments.size()){
                std::cout << infoScheme.arguments[it] << " | "; // -> a | b | 
                ++it;
            }
            std::cout << "out" << std::endl; // -> a | b | out
            
            //sega ostavat dolnite redove
            // kmobinaciite sa 2^broqt argumenti
            int combinations = (1 << infoScheme.arguments.size());
            
            //edin vunshen for za redovete na tablicata
            for (int i = 0; i < combinations; i++)
            {
                std::vector<int> values;
                // it = infoScheme.arguments.size();
                // vutreshen for za kolonite
                for (int j = 0; j < it; j++)
                {   
                    //!!!!!iskame j-ti bit ot i, sled koeto pravim pobitovo i, za da vzemem stoinostta my, tui kato znaem che & zapazva bita!!!!
                    // ne bix se setil sam za tova vzimane na bit
                    int bit = (i >> j) & 1; // 0 or 1
                    values.push_back(bit);
                    std::cout << bit << "|"; // -> 0 | 1 |
                }
                //sega iskame da izprintim samiqt rezultat
                int res = evaluateResult(infoScheme, values);
                std::cout << res << std::endl;
            }
        }
        catch(const std::invalid_argument e)
        {
            std::cerr << e.what() << std::endl;
            throw;
        }

    }

    void parseFind(std::istringstream& ss1,const std::string& command){
        /*
TABLICA NA ISTINNOST
a b c  F(a,b,c)
0 0 0 1
0 0 1 0
0 1 0 1
0 1 1 1
1 0 0 0
1 0 1 0
1 1 0 0
1 1 1 1

Ot lekciqta znaem: 

SINTEZ PO '1'

    (!a && !b && !c) || (!a && b && !c) || (!a && b && c) || (a && b && c);
    
    //nqmame nujna ot skobite, tui kato prioritetut e qsen i nqma da ima dvuznachie!;
    
 
SINTEZ PO '0'

    (a || b || !c) && (!a || b || c) && (!a || b || !c) && (!a || !b || c); 
*/

          //failov potok za chetene
          std::ifstream file("synthes.txt");
          
          //ima li problem s file?
          if(!file.is_open()){
            std::cerr << "Error opening file!" << std::endl;
            return;
          }
        

          //file e otvoren korektno inache
         // v nego e tablicata za istinnost, trqbva da zaredim samite vektori
         std::string currentLine;

         std::getline(file,currentLine);
        
        //kolko vektora ni trqbvat?
        std::istringstream ss(currentLine); // -> "a b c out"
        std::string currentarg;

        std::vector<std::string> synthesArguments; // a,b,c
        while (ss >> currentarg)
        {

            if(currentarg == "out"){
                continue;
            }
            synthesArguments.push_back(currentarg);

        }

        //imame li argumenti
        if(synthesArguments.size() == 0){
            std::cerr << "Missing variables!" << std::endl;
            return;
        }


        //zarejdame izraz
        std::string result;

        // tova shte e za da znaem koga da pishem || 
        bool firstTermInSynthes = true;

        //zapochvame da zarejdame ot vtori red nadoly
        while (std::getline(file,currentLine)) // 0 | 0 | 0 | 0
        {
            std::istringstream line(currentLine);
            int curr;
            
            std::vector<int> values;

            while(line >> curr){
                values.push_back(curr);
                // na posledna poziciq shte bude elementut ot out, pravim sintez po nego, ako e 1
                 if (line.peek() == '|') line.ignore(); // da propusnem simova |, koito e sled vsqko chislo po pravilo
            }
            // values = {0,0,0,0};
            // proverqvame posledniqt element
            //ako e 1 sintezirame izraz
            if(values.back() == 1)
            {

                if(!firstTermInSynthes){
                    result += " || ";                
                }
                
                for (std::size_t i = 0; i < synthesArguments.size() ;++i)
                {   

                    //taka ne dobavqme poslednoto &&
                    if (i > 0){
                        result += " && ";
                    } 

                    result += (values[i] == 1) ? synthesArguments[i] : "!" + synthesArguments[i];
                }
                firstTermInSynthes = false;
            } 
            //ako e 0 ne sintezirame 
        }
        
        printFile();
        file.close();

        std::cout << "Synt_1: "<< result << std::endl;
    };

    void printFile(){
        std::ifstream file("synthes.txt");
        if(!file.is_open()){
            std::cerr<< "Error opening file occured!";
            return;   
        }
        
        std::string curr;
        std::getline(file,curr);

        while(std::getline(file,curr)){
            std::cout << curr << std::endl;
        }

        file.close();
    }
};
            
