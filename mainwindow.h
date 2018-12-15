#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "deletewindow.h"
#include "scanworker.h"
#include "progressbar.h"
#include <memory>
#include <QMap>
#include <QPair>
#include <QTreeWidgetItem>
#include <QSet>
#include <QThread>
#include <QProgressBar>
namespace Ui {
class MainWindow;
}
class main_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit main_window(QWidget *parent = 0);

    Ui::MainWindow *ui;
    void notification(QString const& message,
            const char* window_title);
    ~main_window();

public slots:

    void setProgressValue(int value);

    void delete_files();

    void output(QMap<QString, QList<QString>>hashesAndFile);

    void select_directory();

    void getNotification(QString const& message,
                         const char* window_title);

    void onButtonClicked();

    void file_to_delete_choice(QTreeWidgetItem* check);

    void buttonUnblock();

    void deleteWindow();

    void getCancel();

    void getTroubles(QString const & file);

signals:
    void startOperation(QList<QString> allDirs);

private:
    qint64 countSize(QString const &path);

    void add_directory(QString const& dir);

    void changeButtons();

    void makeProgressBar();

    void show_about_dialog();

    void checkForMatch(QString const& absolutePath);

    void createThread();

    QSet<QPair<QString, QTreeWidgetItem*>> files_to_remove;


    QThread *thread;

    ScanWorker *worker;

    QProgressBar *progress;

    DeleteWindow *window;

//    ProgressBar *progress;

};

#endif // MAINWINDOW_H
