#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QLineEdit>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QFileSystemModel>
#include <QFileDialog>
#include <QtCharts>
#include <QChartView>
#include <QLineSeries>
#include <QKeyEvent>
#include "helpwindow.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QDir projectPath;
    bool isHelpWindowOpen=false;
private slots:
    //GENERAL
    void on_System_TreeView_doubleClicked(const QModelIndex &index);
    void on_HelpWindow_Button_clicked();
    void on_HelpWindow_closed();
    //TAB WIDGET
    void on_Previous_Button_clicked();
    void on_Next_Button_clicked();
    //SETUP TAB
    void on_CheckProjectDir_Button_clicked();
    void on_SetProjectDir_Button_clicked();
    void on_CheckBleDir_Button_clicked();
    void on_SetBleDir_Button_clicked();
    //GEOMETRY MESH TAB
    void on_BlockMesh_Button_clicked();
    //SOLVER TAB
    void on_OpenExistFile_Button_clicked();
    void on_CheckValidity_Button_clicked();
    void on_LoadPreset_Button_clicked();
    //SOLVE TAB
    void on_plotResidual_Button_clicked();
    void on_paraFoam_Button_clicked();
    void on_Solve_Button_clicked();
    void on_getResiduals_Button_clicked();
    void on_editResiduals_Button_clicked();

private:
    Ui::MainWindow *ui;
    HelpWindow *helpWindow;
    void keyPressEvent(QKeyEvent *event);
};
#endif // MAINWINDOW_H
