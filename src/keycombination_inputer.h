#pragma once

#include <qevent.h>
#include <qkeysequence.h>
#include <qlineedit.h>
#include <qstring.h>

/// @brief 基于#QLineEdit的组合键输入组件。
class KeyCombinationInputer : public QLineEdit
{
    Q_OBJECT

public:
    explicit KeyCombinationInputer(QWidget* parent = nullptr);

    /// @brief 设置还未获得组合键时的文本，默认为"..."。
    void setWaitingText(const QString& text);

    void setKeyCombination(const QKeyCombination& keyCombination);

    /// @overload
    /// @note 如果序列为空，则将组合键也设置为空。
    /// @note 如果序列中的组合键数量大于1，则取第一个组合键。
    void setKeyCombination(const QKeySequence& keySequence);

    QKeyCombination keyCombination() const;

signals:
    void inputFinished(QKeyCombination keyCombination);

protected:
    static bool isVaild(int key, Qt::KeyboardModifiers mod);

    virtual void updateText();

    // 如果获得焦点，则进入组合键的输入状态。
    void focusInEvent(QFocusEvent* event) override;

    // 如果失去焦点，则取消组合键的输入状态。
    void focusOutEvent(QFocusEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

    // 忽略鼠标移动事件以禁用文本选择功能。
    void mouseMoveEvent(QMouseEvent* event) override;

    // 忽略鼠标双击事件以禁用双击文本全选功能。
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    QKeyCombination keyq_;
    QString waitingText_    = "...";
    bool isWaitInput_       = false;
};
