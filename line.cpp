#include "line.h"

line::line()
{
    lineNum = 0;
}

line::~line()
{

}

line::line(line *l)
{
    lineNum = l->lineNum;
    lines = l->lines;
}

void line::addNewLine(QString newline)
{
    int i = 0;
    int newLineNum = 0;
    QString msg;

    if(newline[0] >= '0' && newline[0] <= '9'){
        while(i < newline.length() && newline[i] != ' '){                   //先将每行输入的开头读取为行号
            newLineNum = newLineNum*10 + (newline.at(i).toLatin1() - '0');
            i++;
        }
        i++;
        while(i < newline.length() && newline[i] != '\0'){                  //读取每行实际的statement
            msg += newline[i];
            i++;
        }
        if(msg.isEmpty()){                                                  //行号后面无内容表示删除当前行
             lines.remove(newLineNum);
             return;
        }
        lines.insert(newLineNum, msg);                                      //利用QMap变量保存代码
        lineNum ++;
    }

}

void line::clear()
{
    this->lineNum = 0;
    lines.clear();
}
