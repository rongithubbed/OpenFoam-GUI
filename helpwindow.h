#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QFrame>
#include <QTabWidget>
#include <helpbrowser.h>
#include <QHelpContentWidget>
#include <QHelpIndexWidget>

namespace Ui {
class HelpWindow;
}

class HelpWindow : public QFrame
{
    Q_OBJECT

public:
    explicit HelpWindow(QWidget *parent = nullptr);
    ~HelpWindow();
    void close();

private:
    Ui::HelpWindow *ui;
};

#endif // HELPWINDOW_H
