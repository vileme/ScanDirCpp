#include "deletewindow.h"
#include "ui_deletewindow.h"
#include <QPixmap>
DeleteWindow::DeleteWindow(int number,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteWindow),
    a(number)

{
    ui->setupUi(this);
    ui->label->clear();
    QPixmap p = QPixmap::fromImage(QImage(":/img/icons8-warning-shield-48.png"));
    ui->label->setPixmap(p);
    ui->label_2->setText(QString("Are you sure you want to delete %1 file(s)?").arg(a));
    setWindowTitle("Warning");

}

DeleteWindow::~DeleteWindow()
{
    delete ui;
}


void DeleteWindow::on_buttonBox_accepted()
{
    emit(ok());
}

void DeleteWindow::on_buttonBox_rejected()
{
    delete ui;
}
