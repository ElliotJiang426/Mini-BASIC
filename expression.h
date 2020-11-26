#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <QString>
#include <QList>
#include <QMap>

class Expression
{
public:
    Expression();
    Expression(const QString &l, QMap<QString, int> &varlist);
    ~Expression();
    static QList<QString>* transToPost(const QString &ss);
    int value();

protected:
    QList<QString> *revPol;

    Expression *preExp;
    Expression *postExp;
};

class constantExp: public Expression
{
public:
    constantExp(const QString &l, QMap<QString, int> &varlist);
    ~constantExp();
    int value();
private:
    int v;
};

class identifierExp: public Expression
{
public:
    identifierExp(const QString &l, QMap<QString, int> &varlist);
    ~identifierExp();
    int value();
private:
    int v;
    QString k;
};

class compondExp
{
public:
    compondExp(QList<QString> &l, QMap<QString, int> &varlist);
    ~compondExp();
    int value();
private:
    compondExp *cpdexp1;
    compondExp *cpdexp2;
    QString opr;
    int v;
};

#endif // EXPRESSION_H
