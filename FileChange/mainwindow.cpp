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

bool MainWindow::writeFile(QByteArray &array)
{
    QFile file(ui->fileEdit->text());
    bool ret = file.open(QIODevice::WriteOnly);
    if(ret) {
        array.append(ui->suffixEdit->text());
        file.write(array);
        file.close();
    }

    return ret;
}

bool MainWindow::inputCheck()
{
    QString str = ui->sFileEdit->text();
    if(str.isEmpty()) {
        QMessageBox::information(this, "Title", tr("原始文件不能为空"));
        return false;
    }

    str = ui->fileEdit->text();
    if(str.isEmpty()) {
        QMessageBox::information(this, "Title", tr("保存文件不能为空"));
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
            QMessageBox::information(this, "Title", tr("转换成功"));
        }
    }
}
