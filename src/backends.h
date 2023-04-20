#ifndef BACKENDS_H_
#define BACKEDNS_H_

#include <string>
#include <iostream>


class Variable {
public:
    Variable(int line_num, std::string& name): line_num(line_num), name(name) {}

    friend std::ostream& operator<<(std::ostream& os, const Variable& v);

private:
    std::string name;
    int line_num;
};

std::ostream& operator<<(std::ostream& os, const Variable& v) {
    os << "Variable " << v.name << " " << v.line_num << "\n";
}


class Statement {
public:
    virtual ~Statement() = default;
};


class Assignement: public Statement {
public:
    Assignement(int line_num, Variable& v, std::string str):
        line_num(line_num), v(std::move(v)), str(str) {}

    friend std::ostream& operator<<(std::ostream& os, const Assignement& a);


private:
    std::string str;
    Variable v;
    int line_num;
};

std::ostream& operator<<(std::ostream& os, const Assignement& a) {
    os << "Assignement " << a.line_num << " " << a.v << " " << a.str << "\n";
}




#endif