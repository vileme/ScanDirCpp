#include "mainwindow.h"
#include "ui_mainwindow.h"

#include"scanworker.h"
#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QProgressBar>
#include <QStatusBar>
#include <QtWidgets>
#include "deletewindow.h"

main_window::main_window(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));

    ui->WidgetForDirs->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->treeWidget->header()->setSectionResizeMode(1,QHeaderView::Interactive);



    QCommonStyle style;
    setWindowTitle("File Scanning Application");

    statusBar()->showMessage("© Copyright Kozyrev Vlad 2018");


    ui->actionAdd_Directory->setIcon(QIcon(":/img/257832.png"));
    ui->actionDelete_Files->setIcon(QIcon(":/img/trash-512.png"));
    ui->actionScan_Directories->setIcon(QIcon(":/img/264660.png"));
    ui->action_Cancel->setIcon(QIcon(":/img/iconfinder_error_1646012.png"));
    ui->action_Cancel->setDisabled(true);
    ui->actionDelete_Files->setDisabled(true);
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));
    ui->actionAbout->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    connect(ui->actionAdd_Directory, &QAction::triggered, this, &main_window::select_directory);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &main_window::show_about_dialog);
    connect(ui->actionScan_Directories, &QAction::triggered, this, &main_window::onButtonClicked);
    connect(ui->actionDelete_Files,&QAction::triggered,this,&main_window::deleteWindow);

}
main_window::~main_window()
{
    delete ui;
}
void main_window::select_directory()
{
    QString directory = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
        QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    add_directory(directory);
}
void main_window::onButtonClicked(){

    ui->treeWidget->clear();
    QList<QString> allDirs;
    for (int i = 0; i < ui->WidgetForDirs->topLevelItemCount(); ++i) {
        allDirs.push_back(ui->WidgetForDirs->topLevelItem(i)->text(0));
    }
    if(allDirs.size()==0){
            notification("You must choose directories first!", "Notification");
            return;
    }
    createThread();
    makeProgressBar();
    changeButtons();

    worker = new ScanWorker();

    qRegisterMetaType<QMap<QString,QList<QString>>>("myMap");

    qRegisterMetaType<QList<QString>>("myList");

    connect(ui->action_Cancel,&QAction::triggered,thread,&QThread::requestInterruption);

    connect(worker,SIGNAL(update(int)),this,SLOT(setProgressValue(int)));

    connect(this,SIGNAL(startOperation(myList)),worker,SLOT(scan_directories(myList)));

    connect(worker,SIGNAL(requestCancel()),this,SLOT(getCancel()));

    connect(worker,&ScanWorker::callNotification,this,&main_window::getNotification);

    connect(worker,SIGNAL(callOutput(myMap)),this,SLOT(output(myMap)));

    connect(worker,SIGNAL(callButtonUnblock()),this,SLOT(buttonUnblock()));

    connect(worker,SIGNAL(sendTroubles(QString const&)),this,SLOT(getTroubles(QString const&)));

    worker->moveToThread(thread);
    emit startOperation(allDirs);
    worker->deleteLater();
}

void main_window::createThread(){
    thread = new QThread(this);
    connect(this,SIGNAL(destroyed()),thread,SLOT(quit()));
    thread->start();
}
void main_window::makeProgressBar(){
    progress = new QProgressBar(this);
    statusBar()->clearMessage();
    progress->setTextVisible(true);
    progress->setRange(0,100);
    statusBar()->addWidget(progress,1);
}
void main_window::setProgressValue(int value){
    progress->setValue(value);
}
void main_window::buttonUnblock(){
    ui->actionScan_Directories->setEnabled(true);
    ui->action_Cancel->setDisabled(true);
}

void main_window::changeButtons(){
    ui->actionScan_Directories->setDisabled(true);
    ui->action_Cancel->setEnabled(true);
    ui->actionDelete_Files->setDisabled(true);

}
void main_window::getCancel(){
    thread->quit();
    buttonUnblock();
    delete progress;
}
void main_window::add_directory(QString const& currentDirectory)
{
    checkForMatch(currentDirectory);
}
void main_window::checkForMatch(QString const& absolutePath)
{
    if(absolutePath==""){
        notification("I think,you need to choose directory","Notification");
        return;
    }

    int size = ui->WidgetForDirs->topLevelItemCount();

    bool check = false;

    QList<QTreeWidgetItem*> coincidecncies;

    if (size == 0) {
        QTreeWidgetItem* directoriesColumn = new QTreeWidgetItem(ui->WidgetForDirs);
        directoriesColumn->setText(0, absolutePath);
        ui->WidgetForDirs->addTopLevelItem(directoriesColumn);
        return;
    }
    for (int i = 0; i < size; ++i) {
        QTreeWidgetItem* curr = ui->WidgetForDirs->topLevelItem(i);
        QString text = curr->text(0);
        if (absolutePath == text||absolutePath.contains(text)) {
            check = true;
        }
        else if (text.contains(absolutePath)) {
            coincidecncies.append(curr);
        }
    }

    if(check) notification("You've already chosen this directory or the parent of it!","Notification");

    if(coincidecncies.size() != 0) notification("You've added the parent directory of already chosen,every subdirectory is replaced.","Notification");

    if (!check || coincidecncies.size() != 0) {
        for (auto item : coincidecncies) {
            delete item;
        }
        coincidecncies.clear();
        QTreeWidgetItem* directoriesColumn = new QTreeWidgetItem(ui->WidgetForDirs);
        directoriesColumn->setText(0, absolutePath);
        ui->WidgetForDirs->addTopLevelItem(directoriesColumn);
    }
}

void main_window:: output(QMap<QString, QList<QString>> hashesAndFile){
    bool coincidencies = false;
    for (auto i: hashesAndFile) {
            if (i.size() > 1) {
                coincidencies = true;
                QTreeWidgetItem* parent = new QTreeWidgetItem();
                QString item_string = (*i.begin());
                parent->setText(0, item_string);
                ui->treeWidget->insertTopLevelItem(0, parent);
                for (auto k: i) {
                    QTreeWidgetItem* item = new QTreeWidgetItem();
                    item->setText(0, k);
                    item->setCheckState(0, Qt::Unchecked);
                    parent->insertChild(0, item);
                    connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
                        this, SLOT(file_to_delete_choice(QTreeWidgetItem*)));
                }
            }
        }

    if(!coincidencies){
        notification("Similar files not found!","Notification");
    }
    buttonUnblock();
    delete progress;
}

void main_window::deleteWindow(){
    if (files_to_remove.size() == 0) {
       notification("You need to choose files to delete","Warning");
        return;
    }
    window = new DeleteWindow(files_to_remove.size(),this);
    connect(window,&DeleteWindow::ok,this,&main_window::delete_files);
    window->show();
}

void main_window::file_to_delete_choice(QTreeWidgetItem* chosenFileToDelete) {
    if(files_to_remove.size()>0){
        statusBar()->showMessage(QString("%1 files are chosen").arg(files_to_remove.size()));
        ui->actionDelete_Files->setEnabled(true);
    }
    else {
        statusBar()->clearMessage();
        ui->actionDelete_Files->setDisabled(true);
    }
    QString file_name = chosenFileToDelete->text(0);
    if (chosenFileToDelete->checkState(0) == Qt::Checked) {
        files_to_remove.insert(qMakePair(file_name, chosenFileToDelete));
    } else  {
        files_to_remove.erase(files_to_remove.find({file_name, chosenFileToDelete}));
    }
}

void main_window:: delete_files() {
        for (auto i = files_to_remove.begin(); i != files_to_remove.end(); ) {
            QFile file(i->first);
            if(!file.remove()){
                getTroubles(i->first);
            }
             QTreeWidgetItem* parent_item = i->second->parent();
             parent_item->removeChild(i->second);
             if (parent_item->childCount() == 0) {
                   ui->treeWidget->invisibleRootItem()->removeChild(parent_item);
             }
             i = files_to_remove.erase(i);

  }
        statusBar()->clearMessage();
}

void main_window::getNotification(QString const& message,
                                  const char* window_title)
{
    notification(message,window_title);
}

void main_window::notification(QString const& message,
    const char* window_title)
{
    QMessageBox* msgbox = new QMessageBox(QMessageBox::Information,
        QString(window_title), QString(message),
        QMessageBox::StandardButtons(QMessageBox::Ok), this);
    msgbox->open();
}

void main_window::getTroubles(QString const &file){
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
    item->setText(1,file);
    ui->treeWidget->addTopLevelItem(item);
}

void main_window::show_about_dialog()
{
    QMessageBox::aboutQt(this);
}
