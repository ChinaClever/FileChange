#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool readFile(QByteArray &array);
    bool writeFile(QByteArray &array);
    bool inputCheck();

private slots:
    void on_sFileBtn_clicked();
    void on_fileBtn_clicked();
    void on_startBtn_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
