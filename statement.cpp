#include "statement.h"

statement::statement(const QString &s, QMap<QString, int> &VarList)
{
    this->stmt = new QString(s);
    this->varlist = &VarList;
}

statement::~statement()
{

}

void statement::run()
{

}

void statement::checkVariable(QString var)                                          //判定变量命名是否符合规范的辅助函数
{
    for(int i = 0; i < var.length(); i++){
        if(var.at(i) < 'a' || var.at(i) > 'z')
            if(var.at(i) <'A' || var.at(i) >'Z')
                if(var.at(i) != '_')
                    throw QString(" uses variable containing illegal character");
        if(i == 0 && var.at(i) >= '0' && var.at(i) <= '9')
            throw QString(" uses variable starting with illegal character");
    }
}

letstmt::~letstmt()
{

}

void letstmt::run()
{
    QString left;
    QString right;
    int i = 0;

    if(!stmt->contains("=")) throw QString(" is not a legal LET statement");            //检查LET语句的基本结构

    while(stmt->at(i) != ' ') i++;
    i++;

    while(stmt->at(i) != "="){                                                          //等号左边读取为被赋值的变量
        left += stmt->at(i);
        i++;
    }
    i++;

    //checkVariable(left);                                                                //检查合法性

    while(i < stmt->length()){                                                          //等号右边读取为一个表达式，将其值赋给左变量
        if(stmt->at(i) == ' ') {i++;continue;}
        right += stmt->at(i);
        i++;
    }

    cpdexp = new compondExp(*Expression::transToPost(right), *varlist);

    varlist->insert(left, cpdexp->value());
}

void printstmt::run()
{
    QString tmp;
    int i = 0;

    while(stmt->at(i) != ' ') i++;
    i++;

    while(i < stmt->length()){
        tmp += stmt->at(i);
        i++;
    }

    var = new QString(tmp);
    QMap<QString, int>::iterator it = varlist->find(*var);
    if(it == varlist->end()) throw QString(" uses undefined variable");             //使用了尚未定义的变量
    int v = it.value();
    value = new QString(QString::number(v));

}

void inputstmt::run()
{
    QString tmp;
    int i = 0;

    while(stmt->at(i) != ' ') i++;
    while(stmt->at(i) != ' ') i++;
    i++;

    while(i < stmt->length()){                                      //读取需要输入的变量名
        tmp += stmt->at(i);
        i++;
    }
    checkVariable(tmp);

    name = tmp;
}


void gotostmt::run()
{
    int i = 0;
    QString tmp;

    while(stmt->at(i) < '0' || stmt->at(i)>'9') i++;

    while(i < stmt->length()){                                      //读取跳转的行号
        tmp += stmt->at(i);
        i++;
    }

    cstexp = new constantExp(tmp, *varlist);

    this->value = cstexp->value();
}

void ifstmt::run()
{
    int i = 0;
    QString variable;
    QString opr;
    QString value;
    QString lineNum;
    QString then;


    compondExp *cpdexp1;
    compondExp *cpdexp2;
    constantExp *cstexp;

    while(i < stmt->length() && stmt->at(i) != ' ') i++;
    i++;

    while(i < stmt->length() && stmt->at(i) != ' '){                            //读取判定条件左值
        if(stmt->at(i) == '<' || stmt->at(i) == '>' || stmt->at(i) == '='){
            i--;
            break;
        }
        variable += stmt->at(i);
        i++;
    }
    i++;

    if(i < stmt->length()) opr += stmt->at(i);                                  //读取判断条件逻辑符号
    i++;

    while(i < stmt->length() &&  stmt->at(i) == ' ') i++;


    while(i < stmt->length() && stmt->at(i) != ' '){                            //读取判定条件右值
        value += stmt->at(i);
        i++;
    }
    i++;

    while(i < stmt->length() && stmt->at(i) != ' '){                            //检查语句是否符合语法要求
        then += stmt->at(i);
        i++;
    }
    if(then != "THEN") throw QString(" uses illegal IF statement");
    i++;

    while(i < stmt->length()){
        lineNum += stmt->at(i);
        i++;
    }

    cpdexp1 = new compondExp(*Expression::transToPost(variable), *varlist);
    cpdexp2 = new compondExp(*Expression::transToPost(value), *varlist);
    cstexp = new constantExp(lineNum, *varlist);

    if(opr == "<"){                                                             //进行条件运算
        if(cpdexp1->value() < cpdexp2->value()) gotonum = cstexp->value();
        return;
    }
    else if(opr == ">"){
        if(cpdexp1->value() > cpdexp2->value()) gotonum = cstexp->value();
        return;
    }
    else if(opr == "="){
        if(cpdexp1->value() == cpdexp2->value()) gotonum = cstexp->value();
        return;
    }
    throw QString(" uses illegal operator");

}

void funcstmt::run()                                                            //记录函数名
{
    int i = 0;
    QString tmp;

    while(i < stmt->length() && stmt->at(i) != ' ') i++;
    i++;

    while(i < stmt->length()){
        tmp += stmt->at(i);
        i++;
    }
    if(tmp.isEmpty()) throw QString(" has syntax fault");                       //检查函数命名有无语法错误
    checkVariable(tmp);

    funcname = tmp;
}
