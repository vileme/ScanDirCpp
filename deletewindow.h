#ifndef DELETEWINDOW_H
#define DELETEWINDOW_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class DeleteWindow;
}

class DeleteWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteWindow(int number,QWidget *parent = nullptr);
    ~DeleteWindow();
signals:
    void ok();
private slots:

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::DeleteWindow *ui;
    int a;
};

#endif // DELETEWINDOW_H
