#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->Mesh_Error->hide();
    ui->MeshError_Label->hide();
}

MainWindow::~MainWindow()
{
    QCoreApplication::quit();
    delete helpWindow;
    delete ui;
}

//FUNCTIONS//
//This function initializes a Tree View with the given directory path.
void SetTreeView(QDir projectPath, QString pathExtension, QTreeView * specificTreeView ){
    QDir specificPath(projectPath.path()+pathExtension);
    if(specificPath.exists())
    {
        QFileSystemModel *specificModel = new QFileSystemModel;
        specificModel->setRootPath(specificPath.path());
        specificTreeView->setModel(specificModel);
        specificTreeView->setRootIndex(specificModel->index(specificPath.path()));
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(pathExtension + " is missing.");
        msgBox.exec();
    }
}

//TAB WIDGET//
void MainWindow::on_Next_Button_clicked()
{
    ui->tabWidget->setCurrentIndex(ui->tabWidget->currentIndex()+1);
}

void MainWindow::on_Previous_Button_clicked()
{
    ui->tabWidget->setCurrentIndex(ui->tabWidget->currentIndex()-1);
}

//SETUP TAB//
void MainWindow::on_CheckProjectDir_Button_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Your current Project Directory is:");
    msgBox.setInformativeText(projectPath.path());
    msgBox.exec();
}

void MainWindow::on_SetProjectDir_Button_clicked()
{
    QStringList path;
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::Detail);
    if(dialog.exec())
    {
        path = dialog.selectedFiles();
    }
    qCritical() << path;
    if(path.count() != 0)
    {
        projectPath.setPath(path.at(0));
        ui->ProjectDir_LineEdit->setText(projectPath.path());
        SetTreeView (projectPath, "/constant", ui->PolyMesh_TreeView);
        SetTreeView (projectPath, "/system", ui->System_TreeView);
    }
}

void MainWindow::on_CheckBleDir_Button_clicked()
{
    QFile file("Blender_Directory.txt");
    if(!file.exists())
    {
        qCritical() << "Directory not found, set a directory first";
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        qCritical() << file.errorString();
    }
    QTextStream stream(&file);
    QMessageBox msgBox;
    msgBox.setText("Your current Blender Directory is:");
    msgBox.setInformativeText(stream.readAll());
    msgBox.exec();
}

void MainWindow::on_SetBleDir_Button_clicked()
{
    QFile file("Blender_Directory.txt");
    if(!file.open(QIODevice::WriteOnly))
    {
        qCritical() << file.errorString();
    }
    QTextStream stream(&file);
    stream << ui->BlenderDir_LineEdit->text();
    file.close();
}

//GEOMETRY TAB//
void MainWindow::on_BlockMesh_Button_clicked()
{
    ui->Mesh_Output->clear();
    ui->Mesh_Error->clear();
    QStringList args;
    args << "-case" << projectPath.path();
    QProcess *meshProcess = new QProcess();
    meshProcess->setCurrentReadChannel(QProcess::StandardError);
    meshProcess->start("blockMesh",args);
    meshProcess->waitForFinished();
    QString output(meshProcess->readAllStandardOutput());
    QString error(meshProcess->readAllStandardError());
    if(error == "")
    {
        ui->Mesh_Error->hide();
        ui->MeshError_Label->hide();
    }
    else
    {
        ui->Mesh_Error->show();
        ui->MeshError_Label->show();
    }
    ui->Mesh_Output->append(output);
    ui->Mesh_Error->append(error);
}

//SOLVER TAB//
//Opens the selcted file by double clicking it in the Tree View.
void MainWindow::on_System_TreeView_doubleClicked(const QModelIndex &index)
{
    QStringList args;
    args << projectPath.path() + "/system/" + ui->System_TreeView->model()->data(index.sibling(index.row(),0)).toString();
    if(QDir(projectPath.path()+"/system").exists(ui->System_TreeView->model()->data(index.sibling(index.row(),0)).toString()))
    {
        QProcess *meshProcess = new QProcess();
        meshProcess->start("pluma",args);
        meshProcess->waitForFinished();
    }
}

//Opens the selected file by clicking the "Open Existing File" Button.
void MainWindow::on_OpenExistFile_Button_clicked()
{
    if(!QDir(projectPath.path()+"/system").exists()){
        QMessageBox msgBox;
        msgBox.setText("/system is missing. Please choose a valid project folder first.");
        msgBox.exec();
        return;
    }
    QStringList args;
    //args << projectPath.path() + "/system/" + ui->System_TreeView->model()->data(ui->System_TreeView->currentIndex()).toString();
    args << projectPath.path() + "/system/" + ui->System_TreeView->model()->data(ui->System_TreeView->currentIndex().sibling(ui->System_TreeView->currentIndex().row(),0)).toString();
    if(QDir(projectPath.path()+"/system").exists(ui->System_TreeView->model()->data(ui->System_TreeView->currentIndex().sibling(ui->System_TreeView->currentIndex().row(),0)).toString()))
    {
        QProcess *meshProcess = new QProcess();
        meshProcess->start("pluma",args);
        meshProcess->waitForFinished();
    }
}

//This function generates a solver specific preset for the fvSolution file.
void MainWindow::on_LoadPreset_Button_clicked()
{
    if(!QDir(projectPath.path()+"/system").exists()){
        QMessageBox msgBox;
        msgBox.setText("/system is missing. Please choose a valid project folder first.");
        msgBox.exec();
    }
    else if(QDir(projectPath.path()+"/system").exists("fvSolution")){
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Warning", "A fvSolution file allready exists for this project, do you want to overwrite it", QMessageBox::Yes|QMessageBox::Cancel);
            if (reply == QMessageBox::Yes){
                QString args;
                args = projectPath.path() + "/system";
                if (ui->LoadPreset_ComboBox->currentIndex() == 0){
                    QProcess *copyS1 = new QProcess();
                    copyS1->setWorkingDirectory("/home/alejandro/OpenFoam-GUI/Presets");
                    copyS1->start("cp SimpleFoam " + args);
                    copyS1->waitForFinished();
                }
                else if (ui->LoadPreset_ComboBox->currentIndex() == 1){
                    QProcess *copyS2 = new QProcess();
                    copyS2->setWorkingDirectory("/home/alejandro/OpenFoam-GUI/Presets");
                    copyS2->start("cp PisoFoam " + args);
                    copyS2->waitForFinished();
                }
            }
    }
    else{
        QString args;
        args = projectPath.path() + "/system";
        if (ui->LoadPreset_ComboBox->currentIndex() == 0){
            QProcess *copyS1 = new QProcess();
            copyS1->setWorkingDirectory("/home/alejandro/OpenFoam-GUI/Presets");
            copyS1->start("cp SimpleFoam " + args);
            copyS1->waitForFinished();
        }
        else if (ui->LoadPreset_ComboBox->currentIndex() == 1){
            QProcess *copyS2 = new QProcess();
            copyS2->setWorkingDirectory("/home/alejandro/OpenFoam-GUI/Presets");
            copyS2->start("cp PisoFoam " + args);
            copyS2->waitForFinished();
        }

    }
}

//This function verfies if fvSolution has a definition for all the variables contained in /0.
void MainWindow::on_CheckValidity_Button_clicked()
{
    QDir specificPath(projectPath.path()+"/0");
    if(specificPath.exists()){
         QStringList var;
         var << specificPath.entryList();
         var.removeAll(".");
         var.removeAll("..");
         if(!QDir(projectPath.path()+"/system").exists("fvSolution")){
             QMessageBox msgBox;
             msgBox.setText("fvSolution is missing.");
             msgBox.exec();
             return;
         }
         QFile file(projectPath.path()+"/system/fvSolution");
         if(!file.open(QIODevice::ReadOnly))
         {
             qCritical() << file.errorString();
         }
         int countVal = 0;
         for (const auto& i : var){
             QTextStream in(&file);
             qDebug() << i;
             while(!in.atEnd()){
                 QString line = in.readLine();
                 line = line.simplified();
                 line.replace( " ", "" );
                 qDebug() << line;
                 if(line == i){
                     qDebug() << i << "found";
                     ++countVal;
                     break;
                 }
             }
             in.seek(0);
         }
         if (countVal == var.length()){
             QMessageBox msgBox;
             msgBox.setText("Valid");
             msgBox.exec();
             return;
         }

    }
    else{
        QMessageBox msgBox;
        msgBox.setText("/0 is missing. Please choose a valid project folder first.");
        msgBox.exec();
    }

}


void MainWindow::on_HelpWindow_Button_clicked()
{
    /*
     * Memory Leak ?
     */
    if(!isHelpWindowOpen)
    {
        helpWindow = new HelpWindow();
        connect(helpWindow,SIGNAL(destroyed(QObject*)),this,SLOT(on_HelpWindow_closed()));
        helpWindow->show();
        isHelpWindowOpen=true;
        qCritical() << "HelpWindow created";
    }
}

void MainWindow::on_HelpWindow_closed()
{
    qCritical() << "HelpWindow destroyed";
    isHelpWindowOpen=false;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_F1)
    {
        on_HelpWindow_Button_clicked();
    }
}

