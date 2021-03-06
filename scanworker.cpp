#include "scanworker.h"
#include <QDir>
#include <QCryptographicHash>
#include <QThread>


ScanWorker::~ScanWorker(){
}
ScanWorker::ScanWorker(QObject *parent): QObject (parent){

}

void ScanWorker::scan_directories(QList<QString> allDirs)
{

    QMap<QString, QList<QString>> hashesAndFile;
    QMap<long long,QPair<QString,bool>> sizeAndFiles;

    qint64  size = 0;
    for (int i = 0; i < allDirs.size(); ++i) {
        size += countSize(allDirs[i]);
    }
    qint64 currentSize = 0;
    while (!allDirs.empty()) {

        QDir directory = allDirs.front();
        directory.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot|QDir::NoSymLinks);
        QFileInfoList list = directory.entryInfoList();

        for (QFileInfo file_info : list) {
            if(QThread::currentThread()->isInterruptionRequested()){
                emit(callNotification("You've cancelled the process","Notification"));
                emit(requestCancel());
                return;
            }
            if (file_info.isDir()) {
                allDirs.push_back(file_info.absoluteFilePath());
            }
            else {
                currentSize+=file_info.size();
                double updateSize = double(currentSize*100)/size;
                emit(update((updateSize)));
                if (sizeAndFiles.find(file_info.size()) == sizeAndFiles.end()) {
                    sizeAndFiles.insert(file_info.size(), qMakePair(file_info.absoluteFilePath(), false));
                }
                else {
                    QString path = file_info.absoluteFilePath();
                    if (sizeAndFiles[file_info.size()].second == false) {
                        hash(file_info.absoluteFilePath(),hashesAndFile);
                        hash(sizeAndFiles[file_info.size()].first,hashesAndFile);
                        sizeAndFiles[file_info.size()].second = true;
                    }
                    else {
                        hash(file_info.absoluteFilePath(),hashesAndFile);
                    }
                }
            }
        }
        allDirs.pop_front();
    }
    emit callOutput(hashesAndFile);
}


qint64 ScanWorker::countSize(QString const &path){
    qint64 size = 0;
        QDir directory = QDir(path);
        directory.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot|QDir::NoSymLinks);
        QFileInfoList list = directory.entryInfoList();
        for(QFileInfo file_info : list){
            QString name = file_info.fileName();
            if(file_info.isDir()){
                size += countSize(path + "/" + name);
            }
            else size+=file_info.size();
        }
        return size;
}

void ScanWorker::hash(QString const& fileName,QMap<QString, QList<QString>> &hashesAndFile)
{
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QCryptographicHash hasher(QCryptographicHash::Sha256);
        if (hasher.addData(&file)) {
            hashesAndFile[QString(hasher.result().toHex())].push_back(fileName);
        }
        else {
            emit(sendTroubles(fileName));
            emit callNotification("Can't get hash of the file:(","Notification");
        }
    }
    else{
        emit(sendTroubles(fileName));
    }
    return;
}


