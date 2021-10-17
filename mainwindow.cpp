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

//This function creates a new folder with the given folder name in the given directory
void CreateNewFolder(QString FolderName, QString Dir){
    QString dir(Dir + "/" +FolderName);
    if (!QDir(dir).exists()){
        QDir().mkdir(dir);
        QMessageBox msgBox;
        msgBox.setText(FolderName + " was created.");
        msgBox.exec();
    }
}

//This function displays a message box that lets the user know a invalid directory was chosen
void InvalidDir(QString Dir){
    QMessageBox msgBox;
    msgBox.setText(Dir + " is not a valid Directory. Please choose another Directory.");
    msgBox.exec();
}

//GENERAL//
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

void MainWindow::on_CheckProjectDir_Button_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Your current Project Directory is:");
    msgBox.setInformativeText(projectPath.path());
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
    if (!QDir(ui->BlenderDir_LineEdit->text()).exists()){
        InvalidDir(ui->BlenderDir_LineEdit->text());
        return;
    } else {
        stream << ui->BlenderDir_LineEdit->text();
        QMessageBox msgBox;
        msgBox.setText("The Blender Directory was succesfully set to: ");
        msgBox.setInformativeText(ui->BlenderDir_LineEdit->text());
        msgBox.exec();
    }
    file.close();
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

//Creates new folders and project folders
void MainWindow::on_New_Button_clicked()
{
    if (ui->New_ComboBox->currentIndex() == 0){
        bool ok;
        QString ProDir = QInputDialog::getText(this, tr("Where should be the project folder be located?"),
                                                 tr("Path:"), QLineEdit::Normal,
                                                 projectPath.path(), &ok);
        if (!QDir(ProDir).exists()){
            InvalidDir(ProDir);
            return;
        }else if (QDir(ProDir).exists() && ok){
            QString ProName = QInputDialog::getText(this, tr("Choose a Name for the Project"),
                                                     tr("Name:"), QLineEdit::Normal,
                                                     "New_Project", &ok);
            CreateNewFolder (ProName,ProDir);
            if (QDir(ProDir+"/"+ProName).exists()){
                qDebug() << ProDir+ProName;
                CreateNewFolder ("/system",ProDir+"/"+ProName);
                CreateNewFolder ("/constant",ProDir+"/"+ProName);
                CreateNewFolder ("/0",ProDir+"/"+ProName);
            }
        }
    }
    if (ui->New_ComboBox->currentIndex() == 1){
        bool ok;
        QString FolderName = "/system";
        QString Dir = QInputDialog::getText(this, tr("Where should be the folder /system be located?"), tr("Path:"), QLineEdit::Normal, projectPath.path(), &ok);
        if (!QDir(Dir).exists()){
            InvalidDir(Dir);
            return; }
        if(ok)
            CreateNewFolder (FolderName,Dir);
    }
    if (ui->New_ComboBox->currentIndex() == 2){
        bool ok;
        QString FolderName = "/constant";
        QString Dir = QInputDialog::getText(this, tr("Where should be the folder /constant be located?"), tr("Path:"), QLineEdit::Normal, projectPath.path(), &ok);
        if (!QDir(Dir).exists()){
            InvalidDir(Dir);
            return; }
        if(ok)
            CreateNewFolder (FolderName,Dir);
    }
    if (ui->New_ComboBox->currentIndex() == 3){
        bool ok;
        QString FolderName = "/0";
        QString Dir = QInputDialog::getText(this, tr("Where should be the folder /0 be located?"), tr("Path:"), QLineEdit::Normal, projectPath.path(), &ok);
        if (!QDir(Dir).exists()){
            InvalidDir(Dir);
            return; }
        if(ok)
            CreateNewFolder (FolderName,Dir);
    }
}

//GEOMETRY TAB//

void MainWindow::on_PolyMesh_TreeView_doubleClicked(const QModelIndex &index)
{
    QStringList args;
    args << projectPath.path() + "/constant/" + ui->PolyMesh_TreeView->model()->data(index.sibling(index.row(),0)).toString();
    if(QDir(projectPath.path()+"/constant").exists(ui->PolyMesh_TreeView->model()->data(index.sibling(index.row(),0)).toString()))
    {
        QProcess *openProcess = new QProcess();
        openProcess->start("pluma",args);
        openProcess->waitForFinished();
    }
}

void MainWindow::on_OpenMesh_Button_clicked()
{
    if(!QDir(projectPath.path()+"/constant").exists()){
        QMessageBox msgBox;
        msgBox.setText("/constant is missing. Please choose a valid project folder first.");
        msgBox.exec();
        return;
    }
    QStringList args;
    args << projectPath.path() + "/constant/" + ui->PolyMesh_TreeView->model()->data(ui->PolyMesh_TreeView->currentIndex().sibling(ui->System_TreeView->currentIndex().row(),0)).toString();
    qDebug() << 1;
    qDebug() << args;
    qDebug() << 2;
    qDebug() << ui->PolyMesh_TreeView->model()->data(ui->PolyMesh_TreeView->currentIndex().sibling(ui->System_TreeView->currentIndex().row(),0)).toString();
    if(QDir(projectPath.path()+"/constant").exists(ui->PolyMesh_TreeView->model()->data(ui->PolyMesh_TreeView->currentIndex().sibling(ui->System_TreeView->currentIndex().row(),0)).toString()))
    {
        QProcess *openProcess = new QProcess();
        openProcess->start("pluma",args);
        openProcess->waitForFinished();
    }
}

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
        QProcess *openProcess = new QProcess();
        openProcess->start("pluma",args);
        openProcess->waitForFinished();
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

//SOLVE TAB//
void MainWindow::on_plotResidual_Button_clicked()
{
    QFile file(projectPath.path()+"/postprocessing/residuals/0/residuals.dat");
    //double n_time;

    if(file.exists()){
        if (!file.open(QFile::ReadOnly | QFile::Text)){
            QMessageBox msgBox;
            msgBox.setText("Could not read Residuals, make sure"+projectPath.path()+"/postprocessing/residuals.dat exists!");
            msgBox.exec();
        }
        QString first_line = file.readLine();
        first_line = file.readLine();
        first_line.remove(0,2);
        QStringList residual_properties = first_line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        int n_residuals = residual_properties.size();
        /*QMessageBox msgBox;

        msgBox.setText("number of variables : " + QString::number(n_residuals) + list_first[0]+list_first[1]+list_first[2]+list_first[3]);

        msgBox.exec();*/

        QLineSeries* residuals [n_residuals];
        for(int i=1;i<n_residuals;i++){
            residuals[i] = new QLineSeries();
            residuals[i]->setName(residual_properties[i]);
        }
        while (!file.atEnd()) {
                QString line = file.readLine();
                QStringList l = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                for(int i=1;i<n_residuals;i++){
                    residuals[i]->append(l[0].toDouble(),l[i].toDouble());
                    //n_time = l[0].toDouble();
                }
        }
        QChart* chart = new QChart();
        chart->setTitle("Residuals");
        chart->legend()->setAlignment(Qt::AlignBottom);
        chart->legend()->setBrush(QBrush(QColor(128, 128, 128)));
        chart->legend()->setPen(QPen(QColor(192, 192, 192)));
        chart->legend()->setGeometry(QRectF(6,6,6,6));
        chart->legend()->update();
        QPushButton *hide_show[n_residuals];
        for(int i=1;i<n_residuals;i++){
            chart->addSeries(residuals[i]);
            hide_show[i] = new QPushButton("Hide/Show "+residual_properties[i],this);
            ui->Layout_Plot->addWidget(hide_show[i]);
        }
        chart->createDefaultAxes();
        ui->Residual_Chart->setChart(chart);
        ui->Residual_Chart->setRenderHint(QPainter::Antialiasing);
    }
    else{
        QMessageBox msgBox;
        msgBox.setText("Could not read Residuals, make sure"+projectPath.path()+"/postprocessing/residuals.dat exists!");
        msgBox.exec();
    }
}

void MainWindow::on_hide_show_clicked(){
    QMessageBox msgBox;
    msgBox.setText("lol this works?");
    msgBox.exec();
}
void MainWindow::on_Solve_Button_clicked()
{
    ui->Solver_Output->clear();
    ui->Solver_Error->clear();
    /*QDir solvePath(projectPath.path()+"/system");
    if(solvePath.exists())
    {

    }
    else{

    }*/
    QStringList args;
    args << "-case" << projectPath.path();
    QProcess *solveProcess = new QProcess();
    solveProcess->setCurrentReadChannel(QProcess::StandardError);
    //solveProcess->start((ui->comboBox->currentText()),args);
    solveProcess->start("pisoFoam",args);
    //solveProcess->start("simpleFoam > log&",args); //vielleicht sogar mit mkdir logfiles -> mkdir solver,mesh etc
    solveProcess->waitForFinished();
    QString output(solveProcess->readAllStandardOutput());
    QString error(solveProcess->readAllStandardError());
    if(error == "")
    {
        ui->Solver_Error->hide();
        ui->SolverError_Label->hide();
    }
    else
    {
        ui->Solver_Error->show();
        ui->SolverError_Label->show();
    }
    ui->Solver_Output->append(output);
    ui->Solver_Error->append(error);
}

void MainWindow::on_paraFoam_Button_clicked()
{
    QProcess *paraFoam = new QProcess();
    QStringList args;
    args << "-case" << projectPath.path();
    paraFoam->setCurrentReadChannel(QProcess::StandardError);
    paraFoam->start("paraFoam",args);
    paraFoam->waitForFinished();
}
