#include <iostream>
#include <stack>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <stdexcept>
#include <cctype>
#include <string>

using namespace std;

class Solution {
    unordered_map<string, double> variables;

public:
    bool isOperator(const string& s) {
        return s == "+" || s == "-" || s == "*" || s == "/" || s == "^" || s == "%" || s == "(" || s == ")";
    }

    int prec(const string& s) {
        if (s == "^") return 5;
        if (s == "*" || s == "/" || s == "%") return 4;
        if (s == "+" || s == "-") return 3;
        return 0;
    }

    bool isVariableName(const string& s) {
        if (s.empty() || isOperator(s) || s == "=" || s == "(" || s == ")") return false;
        if (!isalpha(s[0]) && s[0] != '_') return false;
        for (char c : s) {
            if (!isalnum(c) && c != '_') return false;
        }
        return true;
    }

    bool isFunction(const string& s) {
        static const unordered_set<string> functions = {
            "sqrt", "sin", "cos", "tan", "log", "ln", "abs", "exp"
        };
        return functions.count(s);
    }

    vector<string> tokenize(const string& s) {
        vector<string> ans;
        int n = s.size();
        int i = 0;

        while (i < n) {
            if (isspace(s[i])) {
                i++;
                continue;
            }

            string prevToken = ans.empty() ? "" : ans.back();

            if (s[i] == '-') {
                bool isUnary = (ans.empty() || prevToken == "(" || isOperator(prevToken) || prevToken == "=");

                int j = i + 1;
                while (j < n && isspace(s[j])) j++;

                if (isUnary && j < n && s[j] == '(') {
                    ans.push_back("0");
                    ans.push_back("-");
                    i = j;
                } else if (isUnary) {
                    string temp = "-";
                    i++;
                    while (i < n && isspace(s[i])) i++;
                    bool hasDecimal = false;
                    while (i < n && (isdigit(s[i]) || s[i] == '.')) {
                        if (s[i] == '.') {
                            if (hasDecimal) break;
                            hasDecimal = true;
                        }
                        temp += s[i++];
                    }
                    ans.push_back(temp);
                } else {
                    ans.push_back("-");
                    i++;
                }
            }
            else if (isdigit(s[i]) || s[i] == '.') {
                string temp = "";
                bool hasDecimal = false;
                while (i < n && (isdigit(s[i]) || s[i] == '.')) {
                    if (s[i] == '.') {
                        if (hasDecimal) break;
                        hasDecimal = true;
                    }
                    temp += s[i++];
                }
                ans.push_back(temp);
            }
            else if (isalpha(s[i]) || s[i] == '_') {
                string temp = "";
                while (i < n && (isalnum(s[i]) || s[i] == '_')) {
                    temp += s[i++];
                }
                if (i < n && s[i] == '(') {
                    ans.push_back(temp);  // function name
                } else {
                    ans.push_back(temp);  // variable name
                }
            }
            else if (s[i] == '=') {
                ans.push_back("=");
                i++;
            }
            else {
                ans.push_back(string(1, s[i]));
                i++;
            }
        }

        return ans;
    }

    vector<string> convertPostfix(vector<string>& infix) {
        vector<string> postfix;
        stack<string> s;

        for (const string& str : infix) {
            if (!isOperator(str) && !isFunction(str)) {
                postfix.push_back(str);
            } else if (isFunction(str)) {
                s.push(str);
            } else if (str == "(") {
                s.push(str);
            } else if (str == ")") {
                bool matched = false;
                while (!s.empty()) {
                    if (s.top() == "(") {
                        s.pop();
                        if (!s.empty() && isFunction(s.top())) {
                            postfix.push_back(s.top()); // push function call
                            s.pop();
                        }
                        matched = true;
                        break;
                    }
                    postfix.push_back(s.top());
                    s.pop();
                }
                if (!matched) throw runtime_error("Syntax error: mismatched parentheses.");
            } else {
                while (!s.empty() && s.top() != "(" &&
                       (prec(s.top()) > prec(str) || (prec(s.top()) == prec(str) && str != "^"))) {
                    postfix.push_back(s.top());
                    s.pop();
                }
                s.push(str);
            }
        }

        while (!s.empty()) {
            if (s.top() == "(") throw runtime_error("Syntax error: mismatched parentheses.");
            postfix.push_back(s.top());
            s.pop();
        }

        return postfix;
    }

    double evaluate(double op1, const string& op, double op2) {
        if (op == "^") return pow(op1, op2);
        if (op == "*") return op1 * op2;
        if (op == "/") {
            if (fabs(op2) < 1e-9) throw runtime_error("Math error: division by zero.");
            return op1 / op2;
        }
        if (op == "%") {
            if (fabs(op2) < 1e-9) throw runtime_error("Math error: modulo by zero.");
            return fmod(op1, op2);
        }
        if (op == "+") return op1 + op2;
        if (op == "-") return op1 - op2;
        throw runtime_error("Unknown operator: " + op);
    }

    double evalRPN(vector<string>& tokens) {
        stack<double> st;
        for (const string& s : tokens) {
            if (isOperator(s)) {
                if (st.size() < 2) throw runtime_error("Syntax error: insufficient operands.");
                double op2 = st.top(); st.pop();
                double op1 = st.top(); st.pop();
                st.push(evaluate(op1, s, op2));
            }
            else if (isFunction(s)) {
                if (st.empty()) throw runtime_error("Missing argument for function: " + s);
                double val = st.top(); st.pop();

                if (s == "sqrt") st.push(sqrt(val));
                else if (s == "sin") st.push(sin(val));
                else if (s == "cos") st.push(cos(val));
                else if (s == "tan") st.push(tan(val));
                else if (s == "log") {
                    if (val <= 0) throw runtime_error("Math error: log domain.");
                    st.push(log10(val));
                }
                else if (s == "ln") {
                    if (val <= 0) throw runtime_error("Math error: ln domain.");
                    st.push(log(val));
                }
                else if (s == "abs") st.push(fabs(val));
                else if (s == "exp") st.push(exp(val));
                else throw runtime_error("Unknown function: " + s);
            }
            else {
                if (isVariableName(s)) {
                    if (!variables.count(s)) throw runtime_error("Undefined variable: " + s);
                    st.push(variables[s]);
                } else {
                    try {
                        st.push(stod(s));
                    } catch (...) {
                        throw runtime_error("Invalid number: " + s);
                    }
                }
            }
        }
        if (st.size() != 1) throw runtime_error("Syntax error: invalid expression.");
        return st.top();
    }

    double calculate(string input) {
        try {
            vector<string> tokens = tokenize(input);

            if (tokens.size() >= 3 && tokens[1] == "=" && isVariableName(tokens[0])) {
                string varName = tokens[0];
                vector<string> rhs(tokens.begin() + 2, tokens.end());
                vector<string> postfix = convertPostfix(rhs);
                double value = evalRPN(postfix);
                variables[varName] = value;
                return value;
            }

            vector<string> postfix = convertPostfix(tokens);
            return evalRPN(postfix);
        }
        catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            return NAN;
        }
    }
};

int main() {
    Solution solver;
    string expr;

    cout << "Enter an arithmetic expression (type 'exit' to quit):\n";

    while (true) {
        cout << "> ";
        getline(cin, expr);
        if (expr == "exit") break;
        if (expr.empty()) continue;

        double result = solver.calculate(expr);
        if (!isnan(result)) {
            cout << "Result: " << result << endl;
        }

        cout << "-----------------------------" << endl;
    }

    return 0;
}
