/*
 * File: qgtextarea.cpp
 * --------------------
 *
 * @version 2018/06/25
 * - initial version
 */

#include "qgtextarea.h"
#include <QScrollBar>
#include <QTextCursor>
#include "qgcolor.h"
#include "qgfont.h"
#include "qgwindow.h"
#include "strlib.h"

_Internal_QTextEdit::_Internal_QTextEdit(QGTextArea* qgtextArea, QWidget* parent)
        : QTextEdit(parent),
          _qgtextarea(qgtextArea) {
    ensureCursorVisible();
    this->document()->setUndoRedoEnabled(false);
    connect(this, SIGNAL(textChanged()), this, SLOT(handleTextChange()));
}

void _Internal_QTextEdit::contextMenuEvent(QContextMenuEvent* event) {
    if (_qgtextarea->isContextMenuEnabled()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void _Internal_QTextEdit::handleTextChange() {
    QGEvent textChangeEvent(
                /* class  */ QGEvent::KEY_EVENT,
                /* type   */ QGEvent::KEY_TYPED,
                /* name   */ "textchange",
                /* source */ _qgtextarea);
    textChangeEvent.setActionCommand(_qgtextarea->getActionCommand());
    _qgtextarea->fireEvent(textChangeEvent);
}

void _Internal_QTextEdit::keyPressEvent(QKeyEvent* event) {
    event->accept();
    _qgtextarea->fireQGEvent(event, QGEvent::KEY_PRESSED, "keypress");
    if (event->isAccepted()) {
        QTextEdit::keyPressEvent(event);   // call super
    }
}

void _Internal_QTextEdit::keyReleaseEvent(QKeyEvent* event) {
    event->accept();
    _qgtextarea->fireQGEvent(event, QGEvent::KEY_RELEASED, "keyrelease");
    if (event->isAccepted()) {
        QTextEdit::keyReleaseEvent(event);   // call super
    }
}

void _Internal_QTextEdit::mousePressEvent(QMouseEvent* event) {
    event->accept();
    if (!_qgtextarea->hasEventListener("mousepress")) return;
    if (_qgtextarea->hasEventListener("mousepress")) {
        _qgtextarea->fireQGEvent(event, QGEvent::MOUSE_PRESSED, "mousepress");
    }
    if (event->isAccepted()) {
        QTextEdit::mousePressEvent(event);   // call super
    }
}

void _Internal_QTextEdit::mouseReleaseEvent(QMouseEvent* event) {
    event->accept();
    if (!_qgtextarea->hasEventListener("mouserelease")) return;
    if (_qgtextarea->hasEventListener("mouserelease")) {
        _qgtextarea->fireQGEvent(event, QGEvent::MOUSE_RELEASED, "mouserelease");
    }
    if (event->isAccepted()) {
        QTextEdit::mouseReleaseEvent(event);   // call super
    }
}

QSize _Internal_QTextEdit::sizeHint() const {
    if (hasPreferredSize()) {
        return getPreferredSize();
    } else {
        return QTextEdit::sizeHint();
    }
}


QGTextArea::QGTextArea(int rows, int columns, QWidget* parent)
        : _contextMenuEnabled(true) {
    _iqtextedit = new _Internal_QTextEdit(this, getInternalParent(parent));
    setRowsColumns(rows, columns);
}

QGTextArea::QGTextArea(const std::string& text, QWidget* parent)
        : _contextMenuEnabled(true) {
    _iqtextedit = new _Internal_QTextEdit(this, getInternalParent(parent));
    setText(text);
}

QGTextArea::~QGTextArea() {
    // TODO: delete _iqtextedit;
    _iqtextedit = nullptr;
}

void QGTextArea::appendFormattedText(const std::string& text, const std::string& color, const std::string& font) {
    moveCursorToEnd();

    // create a formatted block with the font and color
    QTextCharFormat format;
    if (!color.empty()) {
        format.setForeground(QBrush(QGColor::convertColorToRGB(color)));
    }
    if (!font.empty()) {
        format.setFont(QGFont::toQFont(_iqtextedit->font(), font));
    }

    QTextCursor cursor = _iqtextedit->textCursor();
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor, 1);
    cursor.insertText(QString::fromStdString(text), format);
    cursor.endEditBlock();
    _iqtextedit->setTextCursor(cursor);
    _iqtextedit->ensureCursorVisible();

    moveCursorToEnd();
}

void QGTextArea::appendHtml(const std::string& html) {
    // TODO: use insertHtml for speed?
    setHtml(getHtml() + html);
}

void QGTextArea::appendText(const std::string& text) {
    QTextCursor cursor = _iqtextedit->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor, 1);
    cursor.insertText(QString::fromStdString(text));
    moveCursorToEnd();
}

void QGTextArea::clearText() {
    _iqtextedit->clear();
}

int QGTextArea::getColumns() const {
    return (int) (getHeight() / getRowColumnSize().getWidth());
}

int QGTextArea::getCursorPosition() const {
    return _iqtextedit->textCursor().position();
}

std::string QGTextArea::getHtml() const {
    return _iqtextedit->toHtml().toStdString();
}

_Internal_QWidget* QGTextArea::getInternalWidget() const {
    return _iqtextedit;
}

std::string QGTextArea::getPlaceholder() const {
    return _iqtextedit->placeholderText().toStdString();
}

GDimension QGTextArea::getRowColumnSize() const {
    QFontMetrics m(_iqtextedit->font());
    return GDimension(m.width(QString::fromStdString("mmmmmmmmmm")) / 10.0, m.lineSpacing() + 2);
}

int QGTextArea::getRows() const {
    return (int) (getHeight() / getRowColumnSize().getHeight());
}

std::string QGTextArea::getSelectedText() const {
    QTextCursor cursor = _iqtextedit->textCursor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    if (end > start) {
        return getText().substr(start, end - start);
    } else {
        return "";
    }
}

std::string QGTextArea::getText() const {
    return _iqtextedit->toPlainText().toStdString();
}

std::string QGTextArea::getType() const {
    return "QGTextArea";
}

QWidget* QGTextArea::getWidget() const {
    return static_cast<QWidget*>(_iqtextedit);
}

bool QGTextArea::isContextMenuEnabled() const {
    return _contextMenuEnabled;
}

bool QGTextArea::isEditable() const {
    return !_iqtextedit->isReadOnly();
}

bool QGTextArea::isLineWrap() const {
    return !_iqtextedit->lineWrapMode() != QTextEdit::NoWrap;
}

void QGTextArea::moveCursorToEnd() {
    QTextCursor cursor = _iqtextedit->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor, 1);
    _iqtextedit->setTextCursor(cursor);
    _iqtextedit->ensureCursorVisible();
}

void QGTextArea::moveCursorToStart() {
    QTextCursor cursor = _iqtextedit->textCursor();
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
    _iqtextedit->setTextCursor(cursor);
    _iqtextedit->ensureCursorVisible();
}


void QGTextArea::removeKeyListener() {
    removeEventListeners({"keypress",
                          "keyrelease",
                          "keytype"});
}

void QGTextArea::removeMouseListener() {
    removeEventListeners({"mousepress",
                          "mouserelease"});
}

void QGTextArea::removeTextChangeListener() {
    removeEventListener("textchange");
}

void QGTextArea::scrollToBottom() {
    QScrollBar* scrollbar = _iqtextedit->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
    scrollbar->setSliderPosition(scrollbar->maximum());
}

void QGTextArea::scrollToTop() {
    QScrollBar* scrollbar = _iqtextedit->verticalScrollBar();
    scrollbar->setValue(0);
    scrollbar->setSliderPosition(0);
}

void QGTextArea::setColumns(int columns) {
    double desiredWidth = getRowColumnSize().getWidth() * columns;
    setPreferredSize(desiredWidth, getHeight());
    setSize(desiredWidth, getHeight());
}

void QGTextArea::setContextMenuEnabled(bool enabled) {
    _contextMenuEnabled = enabled;
}

void QGTextArea::setCursorPosition(int index) {
    QTextCursor cursor(_iqtextedit->textCursor());
    cursor.setPosition(index, QTextCursor::MoveAnchor);
    _iqtextedit->setTextCursor(cursor);
    _iqtextedit->ensureCursorVisible();
}

void QGTextArea::setEditable(bool value) {
    _iqtextedit->setReadOnly(!value);
}

void QGTextArea::setHtml(const std::string& html) {
    _iqtextedit->setHtml(QString::fromStdString(html));
}

void QGTextArea::setPlaceholder(const std::string& text) {
    _iqtextedit->setPlaceholderText(QString::fromStdString(text));
}

void QGTextArea::setRows(int rows) {
    double desiredHeight = getRowColumnSize().getHeight() * rows;
    setPreferredSize(getWidth(), desiredHeight);
    setSize(getWidth(), desiredHeight);
}

void QGTextArea::setRowsColumns(int rows, int columns) {
    double desiredWidth = getRowColumnSize().getWidth() * columns;
    double desiredHeight = getRowColumnSize().getHeight() * rows;
    setPreferredSize(desiredWidth, desiredHeight);
    setSize(desiredWidth, desiredHeight);
}

void QGTextArea::setText(const std::string& text) {
    _iqtextedit->setText(QString::fromStdString(text));
}

void QGTextArea::setKeyListener(QGEventListener func) {
    _iqtextedit->setFocusPolicy(Qt::StrongFocus);
    setEventListeners({"keypress",
                       "keyrelease",
                       "keytype"}, func);
}

void QGTextArea::setKeyListener(QGEventListenerVoid func) {
    _iqtextedit->setFocusPolicy(Qt::StrongFocus);
    setEventListeners({"keypress",
                       "keyrelease",
                       "keytype"}, func);
}

void QGTextArea::setMouseListener(QGEventListener func) {
    setEventListeners({"mousepress",
                       "mouserelease"}, func);
}

void QGTextArea::setMouseListener(QGEventListenerVoid func) {
    setEventListeners({"mousepress",
                       "mouserelease"}, func);
}

void QGTextArea::setLineWrap(bool wrap) {
    _iqtextedit->setLineWrapMode(wrap ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
}

void QGTextArea::setTextChangeListener(QGEventListener func) {
    setEventListener("textchange", func);
}

void QGTextArea::setTextChangeListener(QGEventListenerVoid func) {
    setEventListener("textchange", func);
}