#include <iomanip>

#include "value.h"
#include "token.h"
#include "exprs.h"
#include "lexer.h"
#include "parser.h"
#include "evaluator.h"

// 其实这个是半成品，不过由于工程量太大就做这么多吧

// ==================== REPL 主循环 ====================
int main() {
    Evaluator evaluator;
    std::string line;

    std::cout << "Calculator v1.3 \n";

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;
        if (line == "exit") break;

        try {
            // 将输入转化为tokens
            Parser parser(line);

            // 将tokens转为AST
            auto ast = parser.parse();

            // 将AST转化为最简形式的AST
            auto simplified_ast = ast->simplify();

            // 输出化简后的符号表达式
            std::cout << "Simplified: " << simplified_ast->to_string() << std::endl;

            // 尝试求值（仅当无未定义变量时）
            try {
                Value result = evaluator.evaluate(simplified_ast.get());
                if (result.is_number()) {
                    std::cout << "Result: " << std::fixed << std::setprecision(10) << result.num << std::endl;
                }
                std::cout.unsetf(std::ios::fixed);
            }
            catch (const std::runtime_error& e) {
                // 求值失败（含未定义变量），仅提示
                std::cout << "Note: " << e.what() << " (only symbolic simplification available)\n";
            }
            std::cout << std::endl;

        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n\n";
        }
    }

    std::cout << "Goodbye!\n";
    return 0;
}