#include <QtWidgets>

#include "codeeditor.h"

#include <QStringListModel>
#include <QStringList>
#include <iostream>
#include <QFontMetrics>

//Finder: http://www.codeprogress.com/cpp/libraries/qt/showQtExample.php?index=410&key=QPlainTextEditFindExample

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent), _completer(0)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    connect(this, SIGNAL(textChanged()), this, SLOT(updateModel()));

    QCompleter *completer = new QCompleter(this);
    //completer->setModel(modelFromFile(":/resources/wordlist.txt"));
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    setCompleter(new QCompleter());

    configureStyle();

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

void CodeEditor::configureStyle()
{
    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(14);

    setFont(font);

    //Tab Width
    const int tabStop = 4;  // 4 characters
    QFontMetrics metrics(font);
    setTabStopWidth(tabStop * metrics.width(' '));
}

void CodeEditor::updateModel()
{
    QStringList words = toPlainText().split(QRegExp("(\\s|\\n|\\r)+"), QString::SkipEmptyParts);
    words =  words.toSet().toList(); //Remove repeated
    _completer->setModel(new QStringListModel(words,_completer));
}

bool CodeEditor::increaseSelectionIndent()
{
    QTextCursor curs = textCursor();
    if(!curs.hasSelection())
        return false;

    int spos = curs.anchor(), epos = curs.position();

    if(spos > epos)
    {
        std::swap(spos, epos);
    }

    curs.setPosition(spos, QTextCursor::MoveAnchor);
    int sblock = curs.block().blockNumber();

    curs.setPosition(epos, QTextCursor::MoveAnchor);
    int eblock = curs.block().blockNumber();

    curs.setPosition(spos, QTextCursor::MoveAnchor);

    curs.beginEditBlock();

    for(int i = 0; i <= (eblock - sblock); ++i)
    {
        curs.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
        curs.insertText("\t");
        curs.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
    }

    curs.endEditBlock();

    curs.setPosition(spos, QTextCursor::MoveAnchor);
    curs.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);

    while(curs.block().blockNumber() < eblock)
    {
        curs.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
    }

    curs.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    setTextCursor(curs);

    return true;
}


int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}



void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(180);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);


    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void CodeEditor::setCompleter(QCompleter *completer)
{
    if (this->_completer)
        QObject::disconnect(_completer, 0, this, 0);

    this->_completer = completer;

    if (!this->_completer)
        return;

    this->_completer->setWidget(this);
    this->_completer->setCompletionMode(QCompleter::PopupCompletion);
    this->_completer->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(_completer, SIGNAL(activated(QString)),
                     this, SLOT(insertCompletion(QString)));
}

QCompleter *CodeEditor::completer() const
{
    return _completer;
}

void CodeEditor::insertCompletion(const QString& completion)
{
    if (this->_completer->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - _completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

QString CodeEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CodeEditor::focusInEvent(QFocusEvent *e)
{
    if (_completer)
        _completer->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

void CodeEditor::completerKeyEvent(QKeyEvent *e)
{
    if (_completer && _completer->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!_completer || !isShortcut) // do not process the shortcut when we have a completer
        QPlainTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!_completer || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 2
                      || eow.contains(e->text().right(1)))) {
        _completer->popup()->hide();
        return;
    }

    if (completionPrefix != _completer->completionPrefix()) {
        _completer->setCompletionPrefix(completionPrefix);
        _completer->popup()->setCurrentIndex(_completer->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(_completer->popup()->sizeHintForColumn(0)
                + _completer->popup()->verticalScrollBar()->sizeHint().width());

    if(_completer->completionModel()->rowCount() > 1)
        _completer->complete(cr); // popup it up!
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    updateModel();
    std::cout << "key: " << e->key() << std::endl;
    if(e->key() == Qt::Key_Tab)
    {
        if (increaseSelectionIndent()) return;
    }
    completerKeyEvent(e);
}
