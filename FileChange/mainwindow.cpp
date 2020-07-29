#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QtCore>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_sFileBtn_clicked()
{
    QFileDialog dlg(this,tr("文件选择"));
    dlg.setFileMode(QFileDialog::ExistingFile);
    dlg.setDirectory("D:");
    if(dlg.exec() == QDialog::Accepted) {
        QString fn = dlg.selectedFiles().at(0);
        ui->sFileEdit->setText(fn);
    }
}

void MainWindow::on_fileBtn_clicked()
{
    QFileDialog dlg(this,tr("文件选择"));
    dlg.setFileMode(QFileDialog::AnyFile);
    QString fn = ui->sFileEdit->text();
    dlg.selectFile(fn);

    if(dlg.exec() == QDialog::Accepted) {
        QString fn = dlg.selectedFiles().at(0);
        ui->fileEdit->setText(fn);
    }
}

bool MainWindow::readFile(QByteArray &array)
{
    QFile file(ui->sFileEdit->text());
    bool ret = file.exists();
    ret = file.open(QIODevice::ReadOnly);
    if(ret) {
        array = file.readAll();
        file.close();
    } else {
        qDebug() << "open file err";
    }

    return ret;
}

void MainWindow::getXorNumber(QByteArray &array)
{
    uchar xorsum = 0x00;
    for(int i=0; i<array.size(); i++)
        xorsum ^= array.at(i);
    array.append(xorsum);
}

ushort MainWindow::calccrc (ushort crc, uchar crcbuf)
{
    uchar x, kkk=0;
    crc = crc^crcbuf;
    for(x=0;x<8;x++)
    {
        kkk = crc&1;
        crc >>= 1;
        crc &= 0x7FFF;
        if(kkk == 1)
            crc = crc^0xa001;
        crc=crc&0xffff;
    }
    return crc;
}

/**
  * 功　能：CRC校验
  * 入口参数：buf -> 缓冲区  len -> 长度
  * 返回值：CRC
  */
QByteArray MainWindow::rtu_crc(QByteArray &array)
{
    ushort crc = 0xffff;
    for(int i=0; i<array.size(); i++)
        crc = calccrc(crc, array.at(i));

    QByteArray res;
    res.append(crc >> 8);
    res.append(crc & 0xFF);

    return res;
}

void MainWindow::appendCrc(QByteArray &array)
{
    QByteArray crcs;
    for(int i=0; i<array.size(); ) {
        QByteArray temp;
        for(int k=0; (k<1024) && (i<array.size()); k++) {
             temp.append(array.at(i++));
        }
        crcs.append(rtu_crc(temp));
    }
    array.append(rtu_crc(crcs));
}

bool MainWindow::writeFile(QByteArray &array)
{
    QFile file(ui->fileEdit->text());
    bool ret = file.open(QIODevice::WriteOnly);
    if(ret) {
        array.append(ui->suffixEdit->text());
        appendCrc(array);
        //getXorNumber(array);
        file.write(array);
        file.close();
    }

    return ret;
}

bool MainWindow::inputCheck()
{
    QString str = ui->sFileEdit->text();
    if(str.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("原始文件不能为空"));
        return false;
    }

    str = ui->fileEdit->text();
    if(str.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("保存文件不能为空"));
        return false;
    }

    return true;
}

void MainWindow::on_startBtn_clicked()
{
    bool ret = inputCheck();
    if(ret) {
        QByteArray array;
        ret = readFile(array);
        if(ret) {
            writeFile(array);
            QMessageBox::information(this, tr("提示"), tr("转换成功"));
        }
    }
}
