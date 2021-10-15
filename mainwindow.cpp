#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->Mesh_Error->hide();
    ui->MeshError_Label->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Next_Button_clicked()
{
    ui->tabWidget->setCurrentIndex(ui->tabWidget->currentIndex()+1);
}

void MainWindow::on_Previous_Button_clicked()
{
    ui->tabWidget->setCurrentIndex(ui->tabWidget->currentIndex()-1);
}


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
        QDir meshPath(projectPath.path()+"/constant");
        if(meshPath.exists())
        {
            QFileSystemModel *meshModel = new QFileSystemModel;
            meshModel->setRootPath(meshPath.path());
            ui->PolyMesh_TreeView->setModel(meshModel);
            ui->PolyMesh_TreeView->setRootIndex(meshModel->index(meshPath.path()));
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Your selected path is not a OpenFOAM project");
            msgBox.exec();
        }
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
    solveProcess->start("simpleFoam",args);
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

