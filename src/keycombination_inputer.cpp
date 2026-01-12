#include "keycombination_inputer.h"

KeyCombinationInputer::KeyCombinationInputer(QWidget* parent)
{
    // 只接受点击获取焦点。
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    // 禁用右键上下文菜单。
    setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
    // 居中文本。
    setAlignment(Qt::AlignmentFlag::AlignCenter);
    // 禁止编辑。
    setReadOnly(true);
    // 设置光标图形为#PointingHandCursor。
    setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
    setPlaceholderText("-");
}

void KeyCombinationInputer::setWaitingText(const QString& text)
{
    waitingText_ = text;
    if (isWaitInput_)
        setText(waitingText_);
}

void KeyCombinationInputer::setKeyCombination(const QKeyCombination& keyCombination)
{
    keyq_ = keyCombination;
    updateText();
}

void KeyCombinationInputer::setKeyCombination(const QKeySequence& keySequence)
{
    if (keySequence.isEmpty())
        keyq_ = QKeyCombination();
    else
        keyq_ = keySequence[0];
    updateText();
}

QKeyCombination KeyCombinationInputer::keyCombination() const
{
    return keyq_;
}

bool KeyCombinationInputer::isVaild(int key, Qt::KeyboardModifiers mod)
{
    bool keyIsPass =
        (key >= Qt::Key::Key_A && key <= Qt::Key::Key_Z) ||
        (key >= Qt::Key::Key_F1 && key <= Qt::Key::Key_F24) ||
        (key >= Qt::Key::Key_Space && key <= Qt::Key::Key_Slash) ||
        (key >= Qt::Key::Key_Colon && key <= Qt::Key::Key_At) ||
        (key >= Qt::Key::Key_BracketLeft && key <= Qt::Key::Key_QuoteLeft) ||
        (key >= Qt::Key::Key_BraceLeft && key <= Qt::Key::Key_AsciiTilde) ||
        (key >= Qt::Key::Key_Tab && key <= Qt::Key::Key_PageDown);

    bool modIsPass =
        (mod & Qt::Modifier::CTRL) ||
        (mod & Qt::Modifier::ALT) ||
        (mod & Qt::Modifier::SHIFT);

    return keyIsPass && modIsPass;
}

void KeyCombinationInputer::updateText()
{
    setText(QKeySequence(keyq_).toString());
}

void KeyCombinationInputer::focusInEvent(QFocusEvent* event)
{
    QLineEdit::focusInEvent(event);
    if (event->type() == QEvent::Type::FocusIn)
    {
        isWaitInput_ = true;
        setText(waitingText_);
    }
}

void KeyCombinationInputer::focusOutEvent(QFocusEvent* event)
{
    QLineEdit::focusOutEvent(event);
    if (isWaitInput_)
    {
        isWaitInput_ = false;
        updateText();
    }
}

void KeyCombinationInputer::keyPressEvent(QKeyEvent* event)
{
    QLineEdit::keyPressEvent(event);

    if (isWaitInput_)
    {
        auto key = event->key();
        auto mod = event->modifiers();

        // 如果输入了ESC键，则恢复至原状态。
        if (key == Qt::Key::Key_Escape)
        {
            isWaitInput_ = false;
            updateText();
            clearFocus();
            emit inputFinished(keyq_);
        }
        // 如果输入了Delete键，则删除组合键。
        else if (key == Qt::Key::Key_Delete && mod == 0)
        {
            isWaitInput_ = false;
            keyq_ = QKeyCombination();
            updateText();
            clearFocus();
            emit inputFinished(keyq_);
        }
        // 等待一个有效输入。
        else if (isVaild(key, mod))
        {
            isWaitInput_ = false;
            keyq_ = QKeyCombination(mod, static_cast<Qt::Key>(key));
            updateText();
            clearFocus();
            emit inputFinished(keyq_);
        }
    }
}

void KeyCombinationInputer::mouseMoveEvent(QMouseEvent* event)
{
    event->ignore();
}

void KeyCombinationInputer::mouseDoubleClickEvent(QMouseEvent* event)
{
    event->ignore();
}
