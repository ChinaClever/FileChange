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
    void getXorNumber(QByteArray &array);
    ushort calccrc (ushort crc, uchar crcbuf);
    QByteArray rtu_crc(QByteArray &array);
    void appendCrc(QByteArray &array);
    void CRC32_Init();
    void CRC32_Update(unsigned char *data, size_t len);
    QByteArray CRC32_Final();
    QByteArray Md5(QByteArray ba , QString str);

private slots:
    void on_sFileBtn_clicked();
    void on_fileBtn_clicked();
    void on_startBtn_clicked();

private:
    Ui::MainWindow *ui;
    unsigned long mCrc;
};
#endif // MAINWINDOW_H
