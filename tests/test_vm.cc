#include <gtest/gtest.h>
#include <sstream>
#include <iostream>

#include "theatre_script.hh"

using namespace theatre;

TEST(ScriptTests, Sum) {
	Any result = RunScript(R"(
		PUSH 5
		PUSH 7
		ADD
	)");
	
	ASSERT_STREQ(result.GetTypeName(), "int");
	ASSERT_EQ(result.Extract<int>(), 12);
}

TEST(ScriptTests, Subtract) {
	Any result = RunScript(R"(
		PUSH 7
		PUSH 2
		SUB
	)");
	
	ASSERT_STREQ(result.GetTypeName(), "int");
	ASSERT_EQ(result.Extract<int>(), -5);
}

TEST(ScriptTests, Multiply) {
	Any result = RunScript(R"(
		PUSH 10
		PUSH 5
		MUL
	)");
	
	ASSERT_STREQ(result.GetTypeName(), "int");
	ASSERT_EQ(result.Extract<int>(), 50);
}

TEST(ScriptTests, Divide) {
	Any result = RunScript(R"(
		PUSH 5
		PUSH 10
		DIV
	)");
	
	ASSERT_STREQ(result.GetTypeName(), "int");
	ASSERT_EQ(result.Extract<int>(), 2);
}

TEST(ScriptTests, StandardOutput) {
	
	std::stringstream dummyCout{};

	Any result = RunScript(R"(
		PUSH 3
		PUSH 4
		ADD
		PUSH Sum is: {}
		CALL print
	)", dummyCout);
	
	ASSERT_EQ(dummyCout.str(), "Sum is: 7");
}