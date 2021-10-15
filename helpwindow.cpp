#include "helpwindow.h"
#include "ui_helpwindow.h"

HelpWindow::HelpWindow(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::HelpWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    QHelpEngine *helpEngine = new QHelpEngine(QApplication::applicationDirPath()+"/docs/OpenFOAM Docs.qhc",this);
    helpEngine->setupData();

    QTabWidget *tWidget = new QTabWidget(this);
    tWidget->setMaximumWidth(200);
    tWidget->setMinimumWidth(200);
    tWidget->addTab(helpEngine->contentWidget(),"Contents");
    tWidget->addTab(helpEngine->indexWidget(),"Index");

    HelpBrowser *textViewer = new HelpBrowser(helpEngine,this);
    textViewer->setSource(QUrl("qthelp://openfoamdocs/doc/Setup.html"));
    HelpWindow::layout()->addWidget(tWidget);
    HelpWindow::layout()->addWidget(textViewer);

    connect(helpEngine->contentWidget(),SIGNAL(linkActivated(QUrl)),textViewer,SLOT(setSource(QUrl)));

}

HelpWindow::~HelpWindow()
{
    delete ui;
}

