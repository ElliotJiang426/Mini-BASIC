#ifndef LINE_H
#define LINE_H

#include <console.h>
#include <QMap>

class line
{
public:
    line();
    ~line();
    line(line *L);
    void addNewLine(QString newLine);
    void clear();
    QMap<int, QString> lines;
    int lineNum;  //记录行数
private:



};

#endif // LINE_H
