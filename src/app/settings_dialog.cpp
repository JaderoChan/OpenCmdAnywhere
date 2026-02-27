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

static QKeyCombination gbhkKcToQtKc(const gbhk::KeyCombination& kc)
{
    QKeyCombination qkc = QKeySequence::fromString(kc.toString().c_str())[0];
#ifdef Q_OS_MAC
    return fix::swapCtrlMeta(qkc);
#else
    return qkc;
#endif // Q_OS_MAC
}

static gbhk::KeyCombination qtKcToGbhkKc(const QKeyCombination& qkc)
{
#ifdef Q_OS_MAC
    QString kcStr = QKeySequence(fix::swapCtrlMeta(qkc)).toString();
#else
    QString kcStr = QKeySequence(qkc).toString();
#endif // Q_OS_MAC
    return gbhk::KeyCombination::fromString(kcStr.toStdString());
}

SettingsDialog::SettingsDialog(const QString& parameter, const gbhk::KeyCombination& hotkey, QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    ui.parameterEdit->setText(parameter);
    ui.hotkeyEdit->setKeyCombination(gbhkKcToQtKc(hotkey));

    connect(ui.parameterEdit, &QTextEdit::textChanged, this, [=]()
    { emit parameterChanged(ui.parameterEdit->toPlainText()); });

    connect(ui.hotkeyEdit, &KeyCombinationInputer::keyCombinationChanged, this, [=](QKeyCombination qkc)
    {
        auto hotkey = qtKcToGbhkKc(qkc);
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
