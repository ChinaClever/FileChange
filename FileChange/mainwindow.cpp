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
//        char str[40];
//        MyMd5((unsigned char*)(temp+md5Str).data(),str , temp.size()+md5Str.size());
//        md5Str.clear();
//        for(size_t j = 0 ; j < 32 ; j++)
//            md5Str.append(str[j]);
        QByteArray test;
        test.append(temp+md5Str);
        QCryptographicHash hash(QCryptographicHash::Sha256);
        hash.addData(test); //将btArray作为参数加密
        md5Str=hash.result();
        qDebug()<<temp.size() << "temp size"<<md5Str.size()<<" sha256 "<<md5Str<<"   string "<<md5Str.toHex()<<endl;
        //CRC32_Update((unsigned char*)temp.data(),k);
        //crcs.append(rtu_crc(temp));
    }

//    unsigned char data1[21]={0xF0,0xE6,0x01,0xFF,0x1D,0x84,0x00,0x08,0xCD,0x23,0xFF,0x08,0xD3,0xF1,0x00,0x08,0x0D,0x1F,0x01,0x08,0x5D};
//    QByteArray test;
//    test.append((char*)data1);
//    QCryptographicHash hash(QCryptographicHash::Sha256);
//    hash.addData(test); //将btArray作为参数加密
//    QByteArray resultArray=hash.result();//得到hash加密后的结果

   //QString md5=resultArray.toHex(); //将字节数组内容转换为字符串
//    qDebug()<<"Sha256"<<resultArray.size()<<endl;




    //QByteArray ret = CRC32_Final();
    char strtemp[40];
    char str1[100];
    char FixedBuf[11]="@PLD?FDFQ5";
    qDebug()<<md5Str.size()<<endl;
    for(int i = 0 ; i < 32 ; i++)
    {
        str1[i] = md5Str.data()[i];
    }
    strncpy(&str1[32],FixedBuf,11);
//    MyMd5((unsigned char*)str1,strtemp , 42);
//    md5Str.clear();


//    QByteArray testlast;
//    testlast.append(str1);
//    qDebug()<<testlast.size()<<" testlast "<<testlast<<"  testlast string "<<testlast.toHex()<<endl;
    QCryptographicHash hashlast(QCryptographicHash::Sha256);
    hashlast.addData(str1 , 42); //将btArray作为参数加密
    md5Str=hashlast.result();

    qDebug()<<md5Str.size()<<" last sha256 "<<md5Str<<"   string "<<md5Str.toHex()<<endl;
    QString rsaStr = rsaSign(QString(md5Str.toHex()).toStdString());
    //std::cout<<"QString(md5Str).toStdString()"<<QString(rsaStr).toStdString()<<"len"<<QString(rsaStr).toStdString().length()<<std::endl;
    qDebug()<<"rsaStr"<<rsaStr<<endl;
    array.append(rsaStr);
    QString endByte = QString("&%1&%2&").arg(ui->changeVercomboBox->currentIndex()==0?"241":"ff").arg(ui->sVersionEdit->text());//241:可降级 ff:不可降级
    if( ui->chipVercomboBox->currentIndex() == 0 ){
        endByte+="0";
    }else if(ui->chipVercomboBox->currentIndex() == 1){
        endByte+="1";
    }
    array.append(endByte);
    //qDebug()<<rsaStr.size()<<"Last md5Str" << md5Str<<endl;
}

QString MainWindow::rsaSign(std::string message)
{
    //MD5
    //    std::string text="clever";
    //    std::string digest;
    //    CryptoPP::Weak1::MD5 md5;
    //    CryptoPP::HashFilter hashfilter(md5);
    //    hashfilter.Attach(new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest), false));
    //    hashfilter.Put(reinterpret_cast<const unsigned char*>(text.c_str()), text.length());
    //    hashfilter.MessageEnd();
    //    QString tmp= QString::fromStdString(digest);
    //    qDebug()<<"md5:"<<tmp;
    //    QByteArray privateKeydecBase64 = QByteArray::fromBase64(
    //                QString("MIIEogIBAAKCAQEAsWTKM5vgDgOmsILmdF+PkPBW76slU9Z/VDN98Zg0ve1B5WKX \
    //                fbxgdHTtA4nXg0eBUtvhyyhNMbUA8HEPm5ZU8e9XDTkpLW5PG3QWevIrDjVUrDOY \
    //                h1AiqbpEZP/wPPX98Ckr4Tuf0+1lhuN6hTbbpxK5Nb2k3HyhAzsiJmWGjl4JhXEN \
    //                gw6+GOhpwuN1i45YKMGn7CBHvksBombo8BCitNfhk51fT3nVwoJbOF0GMIQ6mptN \
    //                PeqtxK4oZ+Ih3/KXeOAVOuOKaD6nwhIZ9ww9pnZytFVsSQ+0g2pdFEQy/qldoBSF \
    //                7GiWjgF9dmSBLveH17XY0fMZ0apE7eN6mjFRFQIDAQABAoIBAEIXXCJoQ8zU3WjM \
    //                DFJ1PAbzFGDM8eZPdzcGzDiTzaqpV7C/z9M/uOwKmDeGRq4ZmPwnAfPv+vq3BjeO \
    //                2BLOqq1Mz7EUh9vuSjjfQnTh/tqXUgmqW3ymhZOY21yJ4pbDy62LmVacIUsTGo1r \
    //                +TzimGmh1zFHi8G+CTsr7+hAUSZ/EcjrdrcoaWOGkttqcm9FBG0cE+iIN60Ao7x4 \
    //                FeZrQrWEWRaIKrripwMwU/TjAHdfnLOpe/JRzmOGn2CAQBvTBOnFqXSnqGWZcumJ \
    //                7uZFGydF4lHZo6TE6pXoLjk5U7oIhg3aDi2466c2ENAnha+0imj+e4h/TJNKfTGT \
    //                3Eh1q+ECgYEA3GmGe3aHFFxkMgt0KnD7svvMHbSN/VW2+0Mzfa4v0pyLuR0FM1rz \
    //                8cqXwZ0J9gERgB5hPi7j8FZ6aSxwee17jb4FhKsnFJeNHR+LD+Z2TlcSieNI6wfK \
    //                SM1SanF9QK5BF10TAvLkNaWMWUf5MlITe8XlEZ7L0g3Ql31GXIIwoY0CgYEAzgkl \
    //                fJoEVBbUkRuEPjNKYPZPA7HPE92nwmuzQ1kZgv6Gp7xgZuNTDV+VZBdzbNfpibq+ \
    //                cLFBbL0A5fCOnkpFdaYmvh+v6OPvJ1C6o7En3TW7+8Z9y5NGab34SIKYLPtH8yhd \
    //                F+jIZeWz/uK6YkG9I1GP+ngfSlaJyoCSEX+fF6kCgYBwaf2l4HjaYJjVkVPpBYDJ \
    //                83RSPjEg4OOXWNndIsbPQvpcKjnpzFyYmDw4CjD3X+oeDuCHx4CHpoN+OqUO+plG \
    //                XIER/fzLmtPk456YOHFkWTJiUyJwhKOd5+I92a4JHBakpNq6aUkNN2bSbkvhZ0z6 \
    //                lp7TEstfUZ5udP0610eYTQKBgAMtqvFgIt71NzRB22lmOyiBxDhHXu1shvD93lcU \
    //                ocseiA9AXbBYo2haaCCSKAquzgB87AErCtpVlBT4AAs7FBU7ie9256zND1xo52gt \
    //                DHNiJ9M7fadNT1HER3c+5IAVJEgBAGalBapkv8EX2E45u2Q3FuSDQ+BOs3N3RkmU \
    //                0Wx5AoGAXPWfHIVWnCcb+PoXC75ytXkWJt33lvaWgFbagad/9I+qjeimwh0N68Ir \
    //                yXPc+5hOsJ3r6QAWf5xRZWCsSVaTBhyrOgGHuDUnRSJLEwPT1wEjO+4v0WMAG3zb \
    //                mRyA4h68fb9cerOpmqWpkzHywsd2Y7RKTKHM2KOSRPmmHiB+b5U=").toLatin1());//pkcs1
    QByteArray privateKeydecBase64 = QByteArray::fromBase64(
                QString("MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQC5S3A9QSmiFwTD \
                        Lf+xoFXGvry7tKJu/qqYliH418iVFxj+7ewDJdiY45fJhJLd52QtqzXKEfKnIsUc \
                        qC0ZdA7qlt/eGS7x1aJVt+yPTjspPSlB1mlmXmq/5B4gxA2YQcvdLV2yQH5NAOPH \
                        cR7gvHhKelcC35qqdqJzBFNX9Q47WCxbn/mdm5g7uPELlcWI35w7TGuysg4jhoOB \
                        PIKq9sLaXTwa9WRuxbBZMJ0xEwNa4DQGkvpwg822JUXsxnL0XgA4X/i/xk6IGMH7 \
                        8Iot5jIKP91FCzMnlZeIvyWlaEIzFhlpzXqdVqlWJXOHR/WctnyeSGunO+g6i9/p \
                        tkaTPfKrAgMBAAECggEBAJf/ShL1YTGVsFJraVRMMhDWZfuUMarbPRXuEfAPEns6 \
                        d0an5PKwGP1RtTGZjuB477SGvZkFIAf6fbcEPfqjOZk2Km4GOnwmwwON3H+xrNYn \
                        VbCsMcnHqcm/VveJ250KXDETBckzHmgMUs1H/gxcjlCKCtZQlo8+wgOXiMaePw5t \
                        4FZiDBv4gZ/7sEhs/LaBy1uhmgX/QPZvFqZij6wx4KqKCYkjneRT7ljpmK2NdepJ \
                        yJbyFdWY/nBFGfk4wqdpU0UFB7wI3UR0uli6GKHniZRv4FsSZhNkwRmwgntLGsFn \
                        wxjtsqt0pW4def8I9JWCnJI9Du9f4d8+f6HrmglsQAECgYEA4t+E2VqyctA+UCpH \
                        R2V9jmd9ddmsoQm/8rdHPCw1xR2Xw4kP74g0ZBcvWSOVIyJPsOnWzEbG6QKKT7n1 \
                        cWzM5eACyfHa6Mx/9LIvlRX6cx77fPrvwU+RB7fef01bH7VSwUgm2marg26teAKz \
                        ZQ1drCsduA2TOs7hwsd04nIt0xECgYEA0RVjzMQbmIDTPJgGPVwTaqO/KXf8jhBn \
                        auVrQ/yc98/EnQzOG99SH4yOvnTt2/5bPcaDSAf9tlVqwrOkOcHfJbSXVOUAnDjE \
                        sX2eSMUGW5X3BR0Qxcc0T9LGTl0q5Xq0FbCoGCuCiMlIjar3XdMLxpkj1r2z3xnV \
                        jDewCfjN8fsCgYEAga8vK2eS1dp/pCdms0MqcDpsFu+kpDEEjeOCTr0ex1xRIi64 \
                        8z0yZXxzTkrutzSnxQMGC6LxlMQk98neqUV+InDYZ0Ne1PAAvIkWdewoS6IMM7bl \
                        UeT/jB/oiXBYhFmLwnxR6UoDBJBBjHZOwZbQUtgaltj7rgQQPNzyzXZjB2ECgYAU \
                        E566qdYQhcZC2kI3ICFJKyIPBFN+/LpBfHgZVidocE3r2q/r4zzKhL+sMCQq7Lis \
                        LOSsRQAqwvb2bTOGzEP2JeomH7s4RQc2F97cE/cxUiwfFJRouWEfjRi3q6CT2n1y \
                        W704XO0y30kVOWZ5XuuRjWDwnNSpxpxH040W2ApGgwKBgEUUOkh1Afpj+hgto2SI \
                        AG/JgwDzgttUnZJTQyuZDMuOUsjzxrgazzvM4QWfMAlmoczQTXdhb3jhsf7KPoqm \
                        QM7hmVQ5HoCmMWdx6dvg/ZC2ozAiyT/8JJB9dNfqkbyxDuw6OOfamgQ5yjOzVaTy \
                        68h8JPWcH619enP88iZxUMfU").toLatin1());


    QByteArray publicKeydecBase64 = QByteArray::fromBase64(
                QString("MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsWTKM5vgDgOmsILmdF+P \
                        kPBW76slU9Z/VDN98Zg0ve1B5WKXfbxgdHTtA4nXg0eBUtvhyyhNMbUA8HEPm5ZU \
                        8e9XDTkpLW5PG3QWevIrDjVUrDOYh1AiqbpEZP/wPPX98Ckr4Tuf0+1lhuN6hTbb \
                        pxK5Nb2k3HyhAzsiJmWGjl4JhXENgw6+GOhpwuN1i45YKMGn7CBHvksBombo8BCi \
                        tNfhk51fT3nVwoJbOF0GMIQ6mptNPeqtxK4oZ+Ih3/KXeOAVOuOKaD6nwhIZ9ww9 \
                        pnZytFVsSQ+0g2pdFEQy/qldoBSF7GiWjgF9dmSBLveH17XY0fMZ0apE7eN6mjFR \
                        FQIDAQAB").toLatin1());

    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::ByteQueue Privatequeue;
    CryptoPP::HexDecoder encoder(new CryptoPP::Redirector(Privatequeue));
    CryptoPP::Weak::RSASSA_PKCS1v15_MD5_Signer signer;
    std::string dek = QString(privateKeydecBase64.toHex()).toStdString();
    encoder.Put((const unsigned char*)dek.data(), dek.size());
    encoder.MessageEnd();
    signer.AccessKey().Load(Privatequeue);


    // Sign message
    std::string signedMessage = "";
    CryptoPP::StringSource s1(message, true, new CryptoPP::SignerFilter(rng, signer, new CryptoPP::HexEncoder(new CryptoPP::StringSink(signedMessage))));

    QByteArray bb;
    qDebug()<<QString::fromStdString(signedMessage.c_str()).toLower()<<endl;
    //StringToHex(QString::fromStdString(signedMessage.c_str()).toLower() , bb);
    bb.append(QString::fromStdString(signedMessage.c_str()).toLower());


    //    CryptoPP::ByteQueue Publicqueue;
    //    CryptoPP::Weak::RSASSA_PKCS1v15_MD5_Verifier verifier;
    //    CryptoPP::HexDecoder decoder(new CryptoPP::Redirector(Publicqueue));
    //    std::string dec = QString(publicKeydecBase64.toHex()).toStdString();
    //    //std::cout<<"dec"<<dec<<std::endl;
    //    decoder.Put((const unsigned char*)dec.data(), dec.size());
    //    decoder.MessageEnd();
    //    verifier.AccessKey().Load(Publicqueue);

    //    CryptoPP::StringSource signatureFile( signedMessage, true, new CryptoPP::HexDecoder);
    //    if (signatureFile.MaxRetrievable() != verifier.SignatureLength())
    //    { throw std::string( "Signature Size Problem" ); }

    //    CryptoPP::SecByteBlock signature1(verifier.SignatureLength());
    //    signatureFile.Get(signature1, signature1.size());

    //    // Verify
    //    CryptoPP::SignatureVerificationFilter *verifierFilter = new CryptoPP::SignatureVerificationFilter(verifier);
    //    verifierFilter->Put(signature1, verifier.SignatureLength());
    //    CryptoPP::StringSource s(message, true, verifierFilter);

    //    // Result
    //    qDebug()<<"verifierFilter->GetLastResult()"<<verifierFilter->GetLastResult()<<endl;

    //    // Result
    //    if(true == verifierFilter->GetLastResult()) {
    //        qDebug() << "Signature on message verified" << endl;
    //    } else {
    //        qDebug() << "Message verification failed" << endl;
    //    }
    return bb;
}


int MainWindow::ConvertHexChar(unsigned char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}


void MainWindow::StringToHex(QString str, QByteArray &senddata)
{
    int hexdata,lowhexdata;
    int len = str.length();
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
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
