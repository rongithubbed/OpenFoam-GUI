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

private slots:
    void on_Next_Button_clicked();
    void on_Previous_Button_clicked();
    void on_CheckProjectDir_Button_clicked();
    void on_SetProjectDir_Button_clicked();
    void on_BlockMesh_Button_clicked();
    void on_CheckBleDir_Button_clicked();
    void on_SetBleDir_Button_clicked();

    void on_System_TreeView_doubleClicked(const QModelIndex &index);

    void on_OpenExistFile_Button_clicked();

    void on_LoadPreset_Button_clicked();

    void on_CheckValidity_Button_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
