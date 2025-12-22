#include "about_dialog.h"

#include <easy_translate.hpp>

#include "config.h"

AboutDialog::AboutDialog(QWidget* parent) :
    QDialog(parent)
{
    ui.setupUi(this);
    ui.icon->setPixmap(QPixmap(":/icon/icon.ico"));
    ui.copyrightLbl->setText(APP_COPYRIGHT_TEXT);
    updateText();
}

void AboutDialog::updateText()
{
    setWindowTitle(EASYTR("About"));
    ui.titleLbl->setText(EASYTR(APP_TITLE));
}

void AboutDialog::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        updateText();
    QDialog::changeEvent(event);
}
