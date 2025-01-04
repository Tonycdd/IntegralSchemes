#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "../src/parser.hpp"
#include "../src/information.hpp"

TEST_CASE("Adding and finding a valid scheme") {
    IntegralSchemes schemes;
    std::string input = "define i(a,b) : &(a,b)";
    parser p(input, schemes);
    
    REQUIRE(schemes.hasScheme("i"));
    information& info = schemes.findScheme("i");
    REQUIRE(info.integralName == "i");
    REQUIRE(info.arguments.size() == 2);
    REQUIRE(info.arguments[0] == "a");
    REQUIRE(info.arguments[1] == "b");
    REQUIRE(info.logic == "&(a,b)");
}

TEST_CASE("define() throws when you try to redifine a scheme"){
    IntegralSchemes schemes;
    std::string input = "define i(a,b) : &(a,b)";
    parser p(input, schemes);

    REQUIRE(schemes.hasScheme("i") == true);
    std::string input2 = "define i(c) : !c";
    try {
        parser p2(input2, schemes);
        FAIL("Expected exception not thrown.");
    } catch (const std::invalid_argument& e) {
        REQUIRE(std::string(e.what()) == "Here you are trying to redefine a defined intregral scheme! Integral scheme 'i' is already defined!");
    }
}

TEST_CASE("define() throws when you enter an invalid integralScheme name"){
    IntegralSchemes schemes;
    std::string input = "define ik(a,b) : &(a,b)";

    try
    {
        parser p(input,schemes);
        FAIL("Expected exception not thrown.");

    }
    catch(const std::invalid_argument& e)
    {
        REQUIRE(std::string(e.what()) == "Invalid syntax for function name (empty or more than one symbol)!");
    }
}


TEST_CASE("define() throws when you enter arguments with size <= 0") {
    IntegralSchemes schemes;
    std::string input = "define i() : &()";  

    try {
        parser p(input, schemes);
        FAIL("Expected exception not thrown.");
    }
    catch (const std::invalid_argument& e) {
        REQUIRE(std::string(e.what()) == "Invalid syntax for arguments(zero args)!");
    }
}


TEST_CASE("define() throws when the expression has syntax issues") {
    IntegralSchemes schemes;
    std::string input = "define i(a,b) : &(a,b))";  //oshte edna skoba ima v poveche

    try {
        parser p(input, schemes);
        FAIL("Expected exception not thrown.");
    }
    catch (const std::invalid_argument& e) {
        REQUIRE(std::string(e.what()) == "Invalid syntax for logic expression");
    }
}

TEST_CASE("define() throws when invalid argument symbols are used in logic expression") {
    IntegralSchemes schemes;
    std::string input = "define i(a,b) : &($,b)";  
    try {
        parser p(input, schemes);
        FAIL("Expected exception not thrown.");
    }
    catch (const std::invalid_argument& e) {
        REQUIRE(std::string(e.what()) == "Invalid syntax for logic expression");
    }
}

TEST_CASE("define() throws when an undefined argument is used in the logic expression") {
    IntegralSchemes schemes;
    std::string input = "define i(a,b) : &(a,c)";  

    try {
        parser p(input, schemes);
        FAIL("Expected exception not thrown.");
    }
    catch (const std::invalid_argument& e) {
        REQUIRE(std::string(e.what()) == "Invalid syntax for logic expression");
    }
}

TEST_CASE("define() handles complex expression with multiple arguments") {
    IntegralSchemes schemes;
    std::string input = "define f(a,b,c) : &(a,|(b,(!c)))"; 
    parser p(input, schemes);
    
    REQUIRE(schemes.hasScheme("f"));
    information& info = schemes.findScheme("f");
    REQUIRE(info.integralName == "f");
    REQUIRE(info.arguments.size() == 3);
    REQUIRE(info.arguments[0] == "a");
    REQUIRE(info.arguments[1] == "b");
    REQUIRE(info.arguments[2] == "c");
    REQUIRE(info.logic == "&(a,|(b,(!c)))");
}

TEST_CASE("Parser throws an error with undefined command")
{
    IntegralSchemes schemes;
    std::string input = "wow i(a,b) : &(a,b)";

    try
    {
        parser p(input,schemes);
    }
   catch (const std::invalid_argument& e) {
        REQUIRE(std::string(e.what()) == "Invalid command wow");
    }
}

TEST_CASE("run() with correct arguments") {
    IntegralSchemes schemes;
    std::string input = "define i(a,b) : &(a,b)";
    parser p(input, schemes);  

    std::string runInput = " i(1,0)";
    std::istringstream ss(runInput);
    int result = p.parseRun(ss, "run");
    REQUIRE(result == 0);  //rezultata trqbva da e 0 tui kato 1 & 0 = 0
}

TEST_CASE("Invalid run() with missing arguments") {
    IntegralSchemes schemes;
    std::string input = "define i(a,b) : &(a,b)";
    parser p(input, schemes); 

    std::string runInput = " i()";  
    try {
        std::istringstream ss(runInput);
        p.parseRun(ss, "run");
        FAIL("Expected exception not thrown.");
    } catch (const std::invalid_argument& e) {
        REQUIRE(std::string(e.what()) == "Invalid syntax for function arguments!");
    }
}

TEST_CASE("Invalid run() with non-zero, non-one arguments") {
    IntegralSchemes schemes;
    std::string input = "define i(a,b) : &(a,b)";
    parser p(input, schemes);  

    std::string runInput = " i(2,0)";  
    try {
        std::istringstream ss(runInput);
        p.parseRun(ss, "run");
        FAIL("Expected exception not thrown.");
    } catch (const std::invalid_argument& e) {
        REQUIRE(std::string(e.what()) == "Invalid arguments! ( ONLY ones and zeros!))");
    }
}

TEST_CASE("Invalid run() with wrong number of arguments") {
    IntegralSchemes schemes;
    std::string input = "define i(a,b) : &(a,b)";
    parser p(input, schemes);  

    std::string runInput = " i(1,0,1)";  
    try {
        std::istringstream ss(runInput);
        p.parseRun(ss, "run");
        FAIL("Expected exception not thrown.");
    } catch (const std::invalid_argument& e) {
        REQUIRE(std::string(e.what()) == "Invalid number of arguments!");
    }
}

TEST_CASE("Run() with invalid expression - not enough operands for AND") {
    IntegralSchemes schemes;
    std::string input = "define i(a,b) : &(a,b)";
    parser p(input, schemes);  

    std::string runInput = " i(1)";  
    try {
        std::istringstream ss(runInput);
        p.parseRun(ss, "run");
        FAIL("Expected exception not thrown.");
    } catch (const std::invalid_argument& e) {
        REQUIRE(std::string(e.what()) == "Invalid number of arguments!");
    }
}

TEST_CASE("Run() with logical NOT operator") {
    IntegralSchemes schemes;
    std::string input = "define i(a) : !(a)";
    parser p(input, schemes);  

    std::string runInput = " i(1)";
    std::istringstream ss(runInput);
    int result = p.parseRun(ss, "run");

    REQUIRE(result == 0);  // resultata na !1 = 0
}

TEST_CASE("Run() with complex logic expression") {
    IntegralSchemes schemes;
    std::string input = "define i(a,b,c) : |(a,&(b,c))";
    parser p(input, schemes);  

    std::string runInput = " i(1,0,1)";
    std::istringstream ss(runInput);
    int result = p.parseRun(ss, "run");

    REQUIRE(result == 1);  // rezultata e  |(1,&(0,1)) = |(1,0) = 1
}
