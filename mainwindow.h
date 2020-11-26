#ifndef BOGGLEWINDOW_H
#define BOGGLEWINDOW_H

#include "line.h"
#include "console.h"
#include "statement.h"
#include <QMainWindow>

class mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    mainwindow(QWidget *parent = nullptr);
    ~mainwindow();
    void runCmd(QString cmd);   //接到命令后的执行函数
    void run(int startNum);
    void list();
    void clear();
    void help();
    void quit();

signals:


public slots:
    void addNewLine(QString newline);
    void iptValue(QString ipt);

private:
    Console *console;
    line *codes;
    QMap<QString, int> *varlist;
    QMap<QString, int> *funclist;

    bool needInput;
    int stopNum;
    QString iptTarget;

    static const int BOGGLE_WINDOW_WIDTH = 800;
    static const int BOGGLE_WINDOW_HEIGHT = 600;
};

#endif // BOGGLEWINDOW_H
