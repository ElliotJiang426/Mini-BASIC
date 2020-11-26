#ifndef STATEMENT_H
#define STATEMENT_H

#include <QString>
#include <QMap>
#include "expression.h"

class statement
{
public:
    statement(const QString &s, QMap<QString, int> &VarList);
    virtual ~statement();
    virtual void run();
    void checkVariable(QString var);
protected:
    QString *stmt;
    QMap<QString, int> *varlist;
};

class letstmt : public statement
{
public:
    letstmt(const QString &s, QMap<QString, int> &VarList): statement(s, VarList){}
   ~letstmt();
   void run();
private:
   compondExp *cpdexp;
};

class printstmt : public statement
{
public:
    printstmt(const QString &s, QMap<QString, int> &VarList): statement(s, VarList){}
    void run();
    QString *var;
    QString *value;
};

class inputstmt : public statement
{
public:
    inputstmt(const QString &s, QMap<QString, int> &VarList): statement(s, VarList){}
    void run();

    QString name;

};

class gotostmt : public statement
{
public:
    gotostmt(const QString &s, QMap<QString, int> &Varlist): statement(s, Varlist){}
    void run();
    constantExp *cstexp;
    int value;
};

class ifstmt : public statement
{
public:
    ifstmt(const QString &s, QMap<QString, int> &Varlist): statement(s, Varlist){gotonum = -1;}
    void run();

    int gotonum;
};

class funcstmt: public statement
{
public:
    funcstmt(const QString &s, QMap<QString, int> &Varlist): statement(s, Varlist){}
    void run();

    QString funcname;
};

#endif // STATEMENT_H
