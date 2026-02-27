#include "settings_dialog.h"

#include <easy_translate.hpp>

#include "config.h"

#ifdef Q_OS_MAC

namespace fix
{

static QKeyCombination swapCtrlMeta(const QKeyCombination& kc) noexcept
{
    Qt::KeyboardModifiers mod = kc.keyboardModifiers();

    // 在MacOS上如果用户按下了Control/Meta键（其为`Qt::Modifier::META`/`Qt::Modifier::CTRL`），
    // 则映射至`Qt::Modifier::CTRL`/`Qt::Modifier::META`。
    if ((mod & Qt::Modifier::META) && !(mod & Qt::Modifier::CTRL))
        mod = Qt::KeyboardModifiers((mod & ~Qt::Modifier::META) | Qt::Modifier::CTRL);
    else if ((mod & Qt::Modifier::CTRL) && !(mod & Qt::Modifier::META))
        mod = Qt::KeyboardModifiers((mod & ~Qt::Modifier::CTRL) | Qt::Modifier::META);
    else
        return kc;

    return QKeyCombination(mod, kc.key());
};

} // namespace fix

#endif // Q_OS_MAC

SettingsDialog::SettingsDialog(const QString& parameter, const gbhk::KeyCombination& hotkey, QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    ui.parameterEdit->setText(parameter);
    ui.hotkeyEdit->setKeyCombination(QKeySequence::fromString(hotkey.toString().c_str()));

    connect(ui.parameterEdit, &QTextEdit::textChanged, this, [=]()
    { emit parameterChanged(ui.parameterEdit->toPlainText()); });

    connect(ui.hotkeyEdit, &KeyCombinationInputer::keyCombinationChanged, this, [=](QKeyCombination qkc)
    {
    #ifdef Q_OS_MAC
        qkc = fix::swapCtrlMeta(qkc);
    #endif // Q_OS_MAC
        auto hotkey = gbhk::KeyCombination::fromString(QKeySequence(qkc).toString().toStdString());
        emit hotkeyChanged(hotkey);
    });

    updateText();
}

void SettingsDialog::updateText()
{
    setWindowTitle(EASYTR("Settings"));
    ui.parameterLbl->setText(EASYTR("Startup Parameter"));
    ui.parameterEdit->setPlaceholderText(EASYTR("No Parameter"));
    ui.hotkeyLbl->setText(EASYTR("Run Hotkey"));
    ui.hotkeyEdit->setToolTip(EASYTR("Keying the 'ESC' to cancel and keying the 'Delete' to remove hotkey"));
}

void SettingsDialog::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        updateText();
    QDialog::changeEvent(event);
}
