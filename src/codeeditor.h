#pragma once

#include <QPlainTextEdit>
#include <QObject>
#include <QString>
#include <QCompleter>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;


class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = 0);


    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *e) Q_DECL_OVERRIDE;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);
    void insertCompletion(const QString &completion);
    void updateModel();
    void increaseIndentSlot();
    void decreaseIndentSlot();
    void autoIndentSlot();
private:
    QWidget *lineNumberArea;
    QCompleter *_completer;

    QString textUnderCursor() const;
    void increaseIndent();
    void decreaseIndent();
    bool increaseIndentKEvent();
    bool decreaseIndentKEvent();
    void handleDedent(QTextCursor cursor);

    bool bracketsComplete(int key);

    void completerKeyEvent(QKeyEvent *e);
    void configureStyle();
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const Q_DECL_OVERRIDE {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

