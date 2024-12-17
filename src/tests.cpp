#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <functional>

// 包含你的解释器头文件
#include "pineapple.h"

class TestSuite {
private:
    struct TestResult {
        std::string name;
        bool passed;
        std::string error_message;
        double duration_ms;
    };

    std::vector<TestResult> results;
    int passed_count = 0;
    int failed_count = 0;

    class OutputCapture {
    private:
        std::streambuf* old_cout;
        std::ostringstream captured_stream;

    public:
        OutputCapture() {
            old_cout = std::cout.rdbuf();
            std::cout.rdbuf(captured_stream.rdbuf());
        }

        ~OutputCapture() {
            std::cout.rdbuf(old_cout);
        }

        std::string get_output() const {
            return captured_stream.str();
        }
    };

public:
    template<typename Func>
    void run_test(const std::string& name, Func test) {
        auto start = std::chrono::high_resolution_clock::now();
        TestResult result{name, false, "", 0.0};

        try {
            test();
            result.passed = true;
            passed_count++;
        } catch (const std::exception& e) {
            result.error_message = e.what();
            failed_count++;
        }

        auto end = std::chrono::high_resolution_clock::now();
        result.duration_ms = std::chrono::duration<double, std::milli>(end - start).count();
        results.push_back(result);
    }

    void assert_equal(const std::string& expected, const std::string& actual, const std::string& message = "") {
        if (expected != actual) {
            throw std::runtime_error(
                message + "\nExpected: " + expected + "\nActual: " + actual);
        }
    }

    void print_summary() const {
        std::cout << "\n=== Test Results ===\n";
        
        for (const auto& result : results) {
            std::cout << (result.passed ? "✓ " : "✗ ") << result.name 
                     << " (" << result.duration_ms << "ms)";
            
            if (!result.passed) {
                std::cout << "\n  Error: " << result.error_message;
            }
            std::cout << "\n";
        }

        std::cout << "\nSummary:\n"
                  << "Passed: " << passed_count << "\n"
                  << "Failed: " << failed_count << "\n"
                  << "Total:  " << results.size() << "\n"
                  << "Overall: " << (failed_count == 0 ? "PASSED" : "FAILED") << "\n";
    }

    std::string capture_output(const std::string& source) {
        OutputCapture capture;
        PineApple::Interpreter interpreter(source);
        interpreter.execute();
        return capture.get_output();
    }
};

void run_all_tests() {
    TestSuite suite;

    // 基本字符串测试
    suite.run_test("Basic String Assignment and Print", [&]() {
        auto output = suite.capture_output("$a = \"Hello, World!\"\nprint($a)");
        suite.assert_equal("Hello, World!", output);
    });

    // 变量覆写测试
    suite.run_test("Variable Overwrite", [&]() {
        std::string source = "$a = \"Old Value\"\n$a = \"New Value\"\nprint($a)";
        auto output = suite.capture_output(source);
        suite.assert_equal("New Value", output);
    });

    // 连续打印测试
    suite.run_test("Multiple Prints", [&]() {
        std::string source = "$a = \"Test\"\nprint($a)\nprint($a)";
        auto output = suite.capture_output(source);
        suite.assert_equal("TestTest", output);
    });

    // 空字符串测试
    suite.run_test("Empty String", [&]() {
        std::string source = "$a = \"\"\nprint($a)";
        auto output = suite.capture_output(source);
        suite.assert_equal("", output);
    });

    // 特殊字符测试
    suite.run_test("Special Characters", [&]() {
        std::string source = "$a = \"!@#$%^&*()\"\nprint($a)";
        auto output = suite.capture_output(source);
        suite.assert_equal("!@#$%^&*()", output);
    });

    // 语法错误测试
    suite.run_test("Syntax Error - Unclosed String", [&]() {
        bool caught = false;
        try {
            PineApple::Interpreter interpreter("$a = \"unclosed string");
            interpreter.execute();
        } catch (const PineApple::InterpreterError&) {
            caught = true;
        }
        if (!caught) {
            throw std::runtime_error("Expected syntax error was not thrown");
        }
    });

    // 打印测试结果
    suite.print_summary();
}

int main() {
    std::cout << "Running tests...\n";
    run_all_tests();
    return 0;
}