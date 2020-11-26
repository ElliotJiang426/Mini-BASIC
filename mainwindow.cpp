#include "mainwindow.h"

#include <QFile>
#include <QHBoxLayout>
#include <QTextEdit>
#include <iostream>
#include <QWidget>
#include <QThread>
#include <QTextStream>

mainwindow::mainwindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle("BASIC");
    this->setFixedSize(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);

    console = new Console(this);
    codes = new line();
    varlist = new QMap<QString, int>;
    funclist = new QMap<QString, int>;
    needInput = 0;
    stopNum = -1;
    iptTarget.clear();

    console->setGeometry(30, 30, 740, 540);


    console->write("Welcome to the minimal Basic.");
    console->write("Type HELP to get help.\n");

    QObject::connect(console, SIGNAL(newLineWritten(QString)), this, SLOT(addNewLine(QString)));    //将console读取新输入的信号与处理输入的槽函数连接
}
mainwindow::~mainwindow()
{
}

void mainwindow::addNewLine(QString newline){
    if(newline[0] < '0' || newline[0] > '9')                                                        //一行输入不是行号开头，说明是命令操作
    {
        QString cmd;
        for(int i = 0; newline[i] != '\0'; i++)
        cmd += newline[i];
        try{
            runCmd(cmd);
        }
        catch(QString s){
            console->write(s);
        }
        return;
    }
    codes->addNewLine(newline);                                                                     //否则为代码内容
}

void mainwindow::iptValue(QString ipt)                                                              //用于处理INPUT的特殊情况
{
    constantExp *cstexp;
    try{
        cstexp = new constantExp(ipt, *varlist);
    }
    catch(QString s){
        console->write("input is " + s);
        return;
    }

    QObject::disconnect(console, SIGNAL(newLineWritten(QString)), this, SLOT(iptValue(QString)));   //使输入信号和槽函数重新连接
    QObject::connect(console, SIGNAL(newLineWritten(QString)), this, SLOT(addNewLine(QString)));

    varlist->insert(iptTarget, cstexp->value());                                                    //完成输入后从跳出的行继续执行代码
    iptTarget.clear();
    needInput = 0;
    this->run(stopNum);
    return;
}

void mainwindow::runCmd(QString cmd)                                                                //处理命令
{
    if(cmd == "RUN"){
        try{
            this->run(-1);                                                                          //默认参数为-1，表示从头开始执行代码
        }
        catch(QString s){
            console->write("line No."+QString::number(stopNum)+s);
        }
        varlist->clear();
        funclist->clear();
        return;
    }
    if(cmd == "LIST"){
        this->list();
        return;
    }
    if(cmd == "HELP"){
        this->help();
        return;
    }
    if(cmd == "CLEAR"){
        this->clear();
        return;
    }
    if(cmd == "QUIT"){
        this->quit();
        return;
    }
    throw QString("not a legal command");                                               //上述情况均不符合，表示命令输入不正确
}

void mainwindow::run(int startNum)
{
    QString statement;
    int funcmark = 0;                                                                  //用于标记当前行是否为函数声明部分
    int callnumber = 0;                                                                 //记录调用函数的行号

    QMap<int,QString>::iterator it;
    if(startNum != -1) it = codes->lines.find(startNum)+1;                              //根据startNum判断执行代码的起点
    else it = codes->lines.begin();

    for(; it != codes->lines.end(); ++it)
    {
        stopNum = it.key();
        statement.clear();
        int i = 0;
        while(i < it.value().length() && it.value().at(i) != ' '){                      //先根据输入行的开头判断要运行的代码类型
            statement += it.value().at(i);
            i++;
        }
        i++;

        if(statement == "END"){                                                         //如果是END表示代码或函数声明结束
            statement.clear();
            while(i < it.value().length()){                                             //如果是函数声明结束，则标记接下来为代码部分
                statement += it.value().at(i);
                i++;
            }
            if(statement == "SUB") {                                                    //如果是函数执行结束，则跳到调用函数的行
                if(funcmark == 1) funcmark = 0;
                if(funcmark == 2) it = codes->lines.find(callnumber);
                callnumber = 0;
                continue;
            }
            break;
        }
        if(funcmark == 1) continue;                                                          //如果是函数声明部分，则暂时跳过不执行

        if(statement == "REM") continue;                                                //根据不同的指令执行不同的功能
        if(statement == "LET"){                                                         //赋值操作
            letstmt *stmt = new letstmt(it.value(), *varlist);
            stmt->run();
            continue;
        }
        if(statement == "PRINT"){                                                       //输出操作
            printstmt *stmt = new printstmt(it.value(), *varlist);
            stmt->run();
            console->write(*(stmt->value));
            continue;
        }
        if(statement == "INPUT"){                                                       //输入操作比较特殊，需记录当前行号，跳出此次运行，读取输入后再跳回
            QObject::disconnect(console, SIGNAL(newLineWritten(QString)), this, SLOT(addNewLine(QString)));
            QObject::connect(console, SIGNAL(newLineWritten(QString)), this, SLOT(iptValue(QString)));
            inputstmt  *stmt = new inputstmt(it.value(), *varlist);
            stmt->run();
            iptTarget += stmt->name;
            console->write(stmt->name+"?");
            needInput = 1;
            stopNum = it.key();
            break;
        }
        if(statement == "GOTO"){                                                        //跳到指定行继续运行代码
            int n = 0;
            gotostmt  *stmt = new gotostmt(it.value(), *varlist);
            stmt->run();
            n = stmt->value;
            it = codes->lines.find(n);
            if(it == codes->lines.end()) throw QString(" goes to an unknown line");
            it = codes->lines.find(n)-1;
            continue;
        }
        if(statement == "IF"){                                                          //条件语句的执行
            ifstmt *stmt = new ifstmt(it.value(), *varlist);
            stmt->run();
            int n = stmt->gotonum;
            if(n != -1) it = codes->lines.find(n)-1;
            continue;
        }
        if(statement == "SUB"){                                                         //函数的声明
            funcstmt *stmt = new funcstmt(it.value(), *varlist);
            stmt->run();
            QString funcname = stmt->funcname;
            if(funclist->contains(funcname)) throw QString(" declares a declared function");
            funclist->insert(funcname, it.key());
            funcmark = 1;
            continue;
        }
        if(statement == "CALL"){                                                        //函数的调用
            funcstmt *stmt = new funcstmt(it.value(), *varlist);
            stmt->run();
            QString funcname = stmt->funcname;
            if(!funclist->contains(funcname)) throw QString(" calls an undeclared function");
            callnumber = it.key();
            QMap<QString, int>::iterator IT;
            IT = funclist->find(funcname);
            it = codes->lines.find(IT.value());
            funcmark = 2;
            continue;
        }
        throw QString(" not a legal statement!");                                       //若均不符合，表面输入不符合要求
    }

    return;
}

void mainwindow::list()
{
    QMap<int,QString>::iterator it;
    for(it = codes->lines.begin(); it != codes->lines.end(); ++it)
    {
        console->write(QString::number(it.key(), 10) + ' ' + it.value());
    }
}

void mainwindow::help()
{
    QFile file("C:\\Users\\DELL\\Documents\\Minima_BASIC\\help.txt");               //打印帮助
    file.open(QIODevice::ReadOnly);
    QTextStream txtInput(&file);
    QString lineStr;
    while(!txtInput.atEnd())
    {
        lineStr = txtInput.readLine();
        console->write(lineStr);
    }
    file.close();
}

void mainwindow::clear()
{
    codes->clear();
}

void mainwindow::quit()
{
    this->close();
}
