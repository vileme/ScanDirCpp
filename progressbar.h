#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include<QMainWindow>
#include <QProgressBar>

class ProgressBar : public QProgressBar {
Q_OBJECT

public:
explicit ProgressBar(QWidget *parent = 0);

public slots:
};

#endif // PROGRESSBAR_H
