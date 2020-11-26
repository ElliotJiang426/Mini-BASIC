#include "console.h"
#include <QKeyEvent>
#include <QTextLine>
#include <QTextCursor>

Console::Console(QWidget *parent) : QTextEdit(parent)
{
}

void Console::clear()
{
    this->clear();
}

void Console::write(QString msg)
{
    this->append(msg);
}

void Console::keyPressEvent(QKeyEvent *event)
{
    QTextCursor cursor = this->textCursor();
    if (event->key() == Qt::Key_Backspace){
        cursor.movePosition(QTextCursor::PreviousCharacter);
        //cursor.select(QTextCursor::LineUnderCursor);
        cursor.deleteChar();
        return;
    }
    if (event->key() == Qt::Key_Delete){
        cursor.deleteChar();
        return;
    }
    if (this->textCursor().hasSelection())
        return;
    if (event->key() == Qt::Key_Return) {

        cursor.movePosition(QTextCursor::End);
        cursor.select(QTextCursor::LineUnderCursor);
        QString lastLine = cursor.selectedText();
        lastLine = lastLine.simplified();
        emit newLineWritten(lastLine);
    }
    QTextEdit::keyPressEvent(event);
}
