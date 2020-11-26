#include "expression.h"

Expression::Expression(const QString &l, QMap<QString, int> &varlist)
{
    revPol = transToPost(l);
}

Expression::~Expression()
{

}

QList<QString>* Expression::transToPost(const QString &ss){    //将表达式转换为后缀表达式
    QList<QString> *result = new QList<QString>;

    QList<QChar> opr;
    int i = 0;
    QString tmp;
    while( i < ss.length()){
        if(ss[0] == '-') {
            tmp = "0";
        }

        if(ss[i] == ' ') result->push_back(tmp);

        if(ss[i] >= '0' && ss[i] <= '9'){
            tmp += ss[i];
        }
        if((ss[i] >= 'a' && ss[i] <= 'z') || (ss[i] >= 'A' && ss[i] <= 'Z')){
            tmp += ss[i];
        }
        if(ss[i] == '(') {
            opr.push_back(ss[i]);
            if(ss[i+1] == '-') tmp = "0";
        }
        if(ss[i] == '+' || ss[i] == '-') {
            if(!tmp.isEmpty()){
                result->push_back(tmp);
                tmp.clear();
            }
            while(!opr.isEmpty() && (opr.last() == '+' || opr.last() == '-' || opr.last() == '*' || opr.last() == '/' || opr.last() == '^')) {
                tmp += opr.last();
                result->push_back(tmp);
                tmp.clear();
                opr.pop_back();
            }
            opr.push_back(ss[i]);
        }
        if(ss[i] == '*' || ss[i] == '/'){
            if(!tmp.isEmpty()) {
                result->push_back(tmp);
                tmp.clear();
            }
            if(i+1 < ss.length() && ss[i+1] == '*'){
                opr.push_back('^');
                i=i+2;
                continue;
            }
            while(!opr.isEmpty() && (opr.last() == '^' || opr.last() == '*' || opr.last() == '/')){
                tmp += opr.last();
                result->push_back(tmp);
                tmp.clear();
                opr.pop_back();
            }
            opr.push_back(ss[i]);
        }
        if(ss[i] == ')'){
            if(!tmp.isEmpty()){
                result->push_back(tmp);
                tmp.clear();
            }
            while(!opr.isEmpty() && (opr.last() != '(' && opr.last() != '0')){
                tmp += opr.last();
                result->push_back(tmp);
                tmp.clear();
                opr.pop_back();
            }
            opr.pop_back();
        }

        i++;
    }
    if(!tmp.isEmpty()) {
        result->push_back(tmp);
        tmp.clear();
    }


    while(!opr.isEmpty()){// && opr.last() != '0'
        tmp += opr.last();
        result->push_back(tmp);
        tmp.clear();
        opr.pop_back();
    }

    return result;
}

constantExp::constantExp(const QString &l, QMap<QString, int> &varlist): Expression (l, varlist)
{
    int num = 0;
    if(l.isEmpty()) throw QString(" requires a constant expression");
    for(int i = 0; i<l.size(); i++){
        if(l[i] < '0' || l[i] > '9') throw QString(" not a legal constant expression");      //包含非数字的字符
        num = num*10 + (l[i].toLatin1() - '0');
    }
    v = num;
    //varlist.insert(l, num);
}

constantExp::~constantExp()
{

}

int constantExp::value()
{
    return v;
}

identifierExp::identifierExp(const QString &l, QMap<QString, int> &varlist): Expression (l, varlist)
{
    k += l;
    k = k.simplified();

    /*for(int i = 0; i < k.length(); i++){
        if(k.at(i) < 'a' || k.at(i) > 'z')
            if(k.at(i) <'A' || k.at(i) >'Z')
                if(k.at(i) != '_')
                    throw QString(" uses variable containing illegal character");   //变量名包含非法字符
        if(i == 0 && k.at(i) >= '0' && k.at(i) <= '9')
            throw QString(" uses variable starting with illegal character");        //变量名首个字符为数字
    }*/

    QMap<QString, int>::iterator it = varlist.find(k);
    if(it == varlist.end())
        throw QString(" uses undefined variable");                                  //调用了未定义的参数
    v = it.value();
}

identifierExp::~identifierExp()
{

}

int identifierExp::value()
{
    return v;
}

compondExp::compondExp(QList<QString> &revpol, QMap<QString, int> &varlist)
{
    QList<QString> tmpstack1;
    QList<QString> tmpstack2;
    QString tmpstr;
    int i = 0;

    if(revpol.last().at(0) == '+' || revpol.last().at(0) == '-' || revpol.last().at(0) == '*' || revpol.last().at(0) == '/' || revpol.last().at(0) == '^'){
        opr += revpol.last();
        revpol.pop_back();

        while(revpol.last().at(0) == '+' || revpol.last().at(0) == '-' || revpol.last().at(0) == '*' || revpol.last().at(0) == '/' || revpol.last().at(0) == '^'){
            tmpstack1.push_back(revpol.last());
            revpol.pop_back();
            i++;
        }
        i++;
        while(i>0){
            tmpstack2.push_front(revpol.last());
            revpol.pop_back();
            i--;
        }

        while(!tmpstack1.isEmpty()){
            tmpstack2.push_back(tmpstack1.last());
            tmpstack1.pop_back();
        }                                                               //递归分解后缀表达式

        cpdexp1 = new compondExp(tmpstack2, varlist);
        cpdexp2 = new compondExp(revpol,varlist);

        if(opr == '+') v = cpdexp1->value() + cpdexp2->value();
        if(opr == '*') v = cpdexp1->value() * cpdexp2->value();
        if(opr == '-') v = cpdexp2->value() - cpdexp1->value();
        if(opr == '/') v = cpdexp2->value() / cpdexp1->value();
        if(opr == '^'){
            v = 1;
            for(int i=0; i<cpdexp1->value(); i++){
                v = v*cpdexp2->value();
            }
        }
    }
    else if(revpol.last().at(0) >= '0' && revpol.last().at(0) <= '9'){  //表达式只包含一个常数
        tmpstr += revpol.last();
        constantExp *c = new constantExp(tmpstr.simplified(), varlist);
        v = c->value();
    }
    else {                                                              //表达式只包含一个变量
        tmpstr += revpol.last();
        identifierExp *i = new identifierExp(tmpstr.simplified(), varlist);
        v = i->value();
    }
}

compondExp::~compondExp()
{

}

int compondExp::value()
{

    return v;
}
