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

//SOLVE TAB//

//This function uses the generated Residual Data to plot it in the UI
void MainWindow::on_plotResidual_Button_clicked()
{
    //Reading the data from residual.dat
    QFile file(projectPath.path()+"/postprocessing/Residuals/0/residuals.dat");
    double n_time = 0;
    double n_res = 0;
    if(file.exists()){
        if (!file.open(QFile::ReadOnly | QFile::Text)){
            QMessageBox msgBox;
            msgBox.setText("Could not read Residuals, make sure"+projectPath.path()+"/postprocessing/Residuals/0/residuals.dat exists");
            msgBox.exec();
        }
        QString first_line = file.readLine();
        first_line = file.readLine();
        first_line.remove(0,2);
        QStringList residual_properties = first_line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        int n_residuals = residual_properties.size();
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
                    n_time = l[0].toDouble();
                    if(n_res<l[i].toDouble()){
                    n_res = l[i].toDouble();
                    }
                }
        }
        //creating a Chart and linking the Data to it
        QChart* chart = new QChart();
        QPushButton *hide_show[n_residuals];
        for(int i=1;i<n_residuals;i++){
            chart->addSeries(residuals[i]);
            hide_show[i] = new QPushButton("Hide/Show "+residual_properties[i],this);
            ui->Layout_Plot->addWidget(hide_show[i]);
            //connect(hide_show[i], SIGNAL(clicked(bool)), this,SLOT(on_hide_show_clicked(bool)) ));
        }

        //Plotting the Chart on a ChartView
        chart->setTitle("Residuals");
        QValueAxis * axisX = new QValueAxis;
        QValueAxis * axisY = new QValueAxis;
        axisX->setRange(0, n_time);
        axisX->setTickCount(10);
        axisX->setLabelFormat("%.2f");
        axisY->setRange(0, n_res);
        axisX->setTickCount(10);
        axisY->setLabelFormat("%.2f");
        axisY->setTitleText("Residual");
        axisX->setTitleText("Time");
        chart->addAxis(axisY, Qt::AlignLeft);
        chart->addAxis(axisX, Qt::AlignBottom);
        chart->legend()->setAlignment(Qt::AlignBottom);
        chart->legend()->setBrush(QBrush(QColor(128, 128, 128)));
        chart->legend()->setPen(QPen(QColor(192, 192, 192)));
        chart->legend()->setGeometry(QRectF(6,6,6,6));
        chart->legend()->update();
        ui->Residual_Chart->setChart(chart);
        ui->Residual_Chart->setRenderHint(QPainter::Antialiasing);
    }
    else{
        QMessageBox msgBox;
        msgBox.setText("Could not read Residuals, make sure"+projectPath.path()+"/postprocessing/Residuals/0/residuals.dat");
        msgBox.exec();
    }
}

/*void MainWindow::on_hide_show_clicked(int ith){

}*/

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
    solveProcess->start((ui->LoadPreset_ComboBox->currentText()),args);
    //solveProcess->start("pisoFoam",args);
    solveProcess->waitForFinished();
    QString output(solveProcess->readAllStandardOutput());
    QString error(solveProcess->readAllStandardError());
    if(error == "")
    {
        ui->Solver_Error->hide();
        ui->SolverError_Label->hide();
    }
    else if(error == "" && output == ""){
        QMessageBox msgBox;
        msgBox.setText("There was no Output, the Solver was not applied");
        msgBox.exec();
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

//this function generates the Residual Data, it also overwrites old residual Data
void MainWindow::on_getResiduals_Button_clicked()
{
    bool inclresidual = false;
    ui->Solver_Output->clear();
    ui->Solver_Error->clear();
    QFile controlDict(projectPath.path()+"/system/controlDict");
    if(controlDict.exists()){
    if(controlDict.open(QIODevice::ReadOnly))
    {
        while (!controlDict.atEnd())
        {
                QString line = controlDict.readLine();
                QString str = line.simplified();
                if(str == "#includeFunc residuals")
                {
                   inclresidual = true;
               }
        }
    }
    controlDict.close();
    if(controlDict.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text) && inclresidual == false)
    {
        QTextStream stream(&controlDict);
        stream << '\n' << "functions \n {\n #includeFunc residuals \n } \n";
    }
    controlDict.close();
    QStringList args;
    args << "-case" << projectPath.path();
    QProcess *getResidual = new QProcess();
    getResidual->setCurrentReadChannel(QProcess::StandardError);
    getResidual->start("foamGet residuals",args);
    getResidual->waitForFinished();
    QString output(getResidual->readAllStandardOutput());
    QString error(getResidual->readAllStandardError());
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
    }else{

        QMessageBox msgBox;
        msgBox.setText("Could not generate Residuals, make sure "+projectPath.path()+"/system/controlDict exists.");
        msgBox.exec();
    }
}

//this function enables the User to simply Open the new-generated Residual configurations in the system directory to adjust the properties that should be calculated
void MainWindow::on_editResiduals_Button_clicked()
{
    QFile residuals_config(projectPath.path()+"/system/residuals");
    if(residuals_config.exists()){
        QStringList args;
        args << projectPath.path() + "/system/residuals";
        QProcess *openresidualsconf = new QProcess();
        openresidualsconf->start("pluma",args);
        openresidualsconf->waitForFinished();
        }
    else{
        QMessageBox msgBox;
        msgBox.setText(projectPath.path()+"/system/residuals does not exist! Make sure the Residuals were generated");
        msgBox.exec();
    }
}

