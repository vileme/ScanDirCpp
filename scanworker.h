#ifndef SCANWORKER_H
#define SCANWORKER_H

#include <QMainWindow>
#include <QMap>
#include <QTreeWidgetItem>
#include <QSet>
#include <QList>
#include <QString>

class ScanWorker: public QObject{

    Q_OBJECT

public:~ScanWorker();

    explicit ScanWorker(qint64 size,QObject *parent=0);

signals:
    void update(int value);
    void callNotification(QString const& message,
                          const char* window_title);
    void callOutput(QMap<QString, QList<QString>>hashesAndFile);
    void callButtonUnblock();
    void requestCancel();
    void sendTroubles(QString const& file);

public slots:

    void scan_directories(QList<QString> allDirs);



private:
    qint64 size;

    void hash(QString const& fileName,
QMap<QString, QList<QString>> &hashesAndFile);
};



#endif // SCANWORKER_H
