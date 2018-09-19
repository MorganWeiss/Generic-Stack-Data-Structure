#define CATCH_CONFIG_MAIN
#include "catch.h"
#include "Stack.h"

TEST_CASE("An empty stack", "[Stack]") {
    Stack<int> stack;

	REQUIRE(stack.empty());
	REQUIRE(stack.size() == 0u);

	SECTION("inserting an element makes the map not empty") {
		stack.push(2);

		REQUIRE(!stack.empty());
	}

	SECTION("inserting an element increases the size") {
		stack.push(4);

		REQUIRE(stack.size() == 1u);
	}
    
    SECTION("pop on empty stack does nothing") {
        stack.push(6);
        stack.pop();

        REQUIRE(stack.size() == 0);
        REQUIRE(stack.empty());
    }
}

TEST_CASE("Create a stack list with multiple elements", "[Stack]") {
    Stack<int> stack;
    stack.push(2);
    stack.push(4);
    stack.push(6);
    stack.push(8);
    stack.push(10);
    
    static auto init_values = std::vector<int>{2, 4, 6, 8, 10};
    
    REQUIRE(stack.size() == init_values.size());
    REQUIRE(!stack.empty());
    REQUIRE(std::distance(stack.begin(), stack.end()) == init_values.size());
    //REQUIRE(std::equal(stack.begin(), stack.end(), init_values.begin()));
    
    SECTION("Can find elements with std::find") {
        auto found = std::find(std::begin(stack), std::end(stack), 4);
        
        REQUIRE(found != std::end(stack));
        REQUIRE(*found == 4);
    }
    
    SECTION("pop removes last element") {
        stack.pop();
        
        REQUIRE(stack.top() == 8);
        REQUIRE(stack.size() == 4);
    }
    
    SECTION("copy construction") {
        auto second_list = stack;
        
        REQUIRE(stack.size() == init_values.size());
        //REQUIRE(std::equal(stack.begin(), stack.end(), init_values.begin()));
        REQUIRE(second_list.size() == stack.size());
        //REQUIRE(std::equal(second_list.begin(), second_list.end(), stack.begin()));
    }
    
    SECTION("copy assignment") {
        auto second_list = Stack<int>{};
        
        second_list = stack;
        
        REQUIRE(stack.size() == init_values.size());
        //REQUIRE(std::equal(stack.begin(), stack.end(), init_values.begin()));
        REQUIRE(second_list.size() == stack.size());
        //REQUIRE(std::equal(second_list.begin(), second_list.end(), stack.begin()));
    }
    
    SECTION("move construction leaves original list in empty state") {
        auto second_list = Stack<int>{ std::move(stack) };
        
        REQUIRE(stack.empty());
        REQUIRE(second_list.size() == init_values.size());
       // REQUIRE(std::equal(second_list.begin(), second_list.end(), init_values.begin()));
    }

	std::cin.get();
}
