#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QtCore>
#include <QMessageBox>
#include <QDesktopServices>
#include <stdio.h>
#include "myMd5.h"
#include <QCryptographicHash>
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
        if(fn.contains(".bin")||fn.contains(".txt"))
        {
            if(ui->changeVercomboBox->currentIndex()==0){
                fn.replace(fn.length()-3,3,"cl");
                fn.insert(fn.length()-3,"_"+ui->sVersionEdit->text());
            }else{
                fn.replace(fn.length()-3,3,"leg");
                fn.insert(fn.length()-4,"_"+ui->sVersionEdit->text());
            }
        }
        else
        {
            return ;
        }
        ui->fileEdit->setText(fn);
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
        if(fn.contains(".bin")||fn.contains(".txt"))
        {
            if(ui->changeVercomboBox->currentIndex()==0){
                fn.replace(fn.length()-3,3,"cl");
                fn.insert(fn.length()-3,"_"+ui->sVersionEdit->text());
            }else{
                fn.replace(fn.length()-3,3,"leg");
                fn.insert(fn.length()-4,"_"+ui->sVersionEdit->text());
            }
        }
        else
        {
            return ;
        }
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
    res.append(crc & 0xFF);
    res.append(crc >> 8);

    return res;
}

/**
 * @brief MyMd5
 * @param InBuf,OutBuf
 * @return void
 */
void MainWindow::MyMd5(unsigned char *InBuf,char *OutBuf , int len)
{
    char i;
    unsigned char decrypt[16]={0x00};
    MD5_CTX md5;
    char ch[2]={0,0};

    MD5Init(&md5);
    MD5Update(&md5,InBuf,len);
    MD5Final(&md5,decrypt);

    for(i=0;i<16;i++)
    {
        sprintf(ch,"%02x",decrypt[i]);
        OutBuf[2*i] = ch[0];
        OutBuf[2*i+1] = ch[1];
    }
}

void MainWindow::appendCrc(QByteArray &array)
{
    //QByteArray crcs;
    QByteArray md5Str;
    for(int i=0; i<array.size(); ) {
        QByteArray temp;
        int k=0;
        for(; (k<1024) && (i<array.size()); k++) {
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
    QString code = "241";
    if( ui->changeVercomboBox->currentIndex()==0 )code = "241";//0xf1
    else if( ui->changeVercomboBox->currentIndex()==1 )code = "ff";
    else if( ui->changeVercomboBox->currentIndex()==2 )code = "250";//0xfa
    QString endByte = QString("&%1&%2&").arg(code).arg(ui->sVersionEdit->text());//241:可降级 ff:不可降级
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
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata.append(hexdata);
    }
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

    str = ui->sVersionEdit->text();
    if(str.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("版本号不能为空"));
        return false;
    }
    if(!isDigitStr(str))
    {
        QMessageBox::information(this, tr("提示"), tr("版本号不能为非数字"));
        return false;
    }

    return true;
}

/***
  *判断一个字符串是否为纯数字
  */
bool MainWindow::isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1();//QString 转换为 char*
    const char *s = ba.data();

    while(*s && *s>='0' && *s<='9') s++;

    if (*s)
    { //不是纯数字
        return false;
    }
    else
    { //纯数字
        return true;
    }
}

void MainWindow::on_startBtn_clicked()
{
    bool ret = inputCheck();
    if(ret) {
        QByteArray array;
        ret = readFile(array);
//        array.remove(array.size() - 776, 776);//配合测试不同情况
        if(ret) {
            //CRC32_Init();
            writeFile(array);
            QMessageBox::information(this, tr("提示"), tr("转换成功"));

            QString fn = ui->sFileEdit->text();
            int index = fn.lastIndexOf('/');
            QDesktopServices::openUrl(QUrl("file:///"+fn.left(index), QUrl::TolerantMode));
        }
    }
}


static unsigned long crc32_tbl[256] =
{
    0x00000000L, 0x77073096L, 0xEE0E612CL, 0x990951BAL, 0x076DC419L,
    0x706AF48FL, 0xE963A535L, 0x9E6495A3L, 0x0EDB8832L, 0x79DCB8A4L,
    0xE0D5E91EL, 0x97D2D988L, 0x09B64C2BL, 0x7EB17CBDL, 0xE7B82D07L,
    0x90BF1D91L, 0x1DB71064L, 0x6AB020F2L, 0xF3B97148L, 0x84BE41DEL,
    0x1ADAD47DL, 0x6DDDE4EBL, 0xF4D4B551L, 0x83D385C7L, 0x136C9856L,
    0x646BA8C0L, 0xFD62F97AL, 0x8A65C9ECL, 0x14015C4FL, 0x63066CD9L,
    0xFA0F3D63L, 0x8D080DF5L, 0x3B6E20C8L, 0x4C69105EL, 0xD56041E4L,
    0xA2677172L, 0x3C03E4D1L, 0x4B04D447L, 0xD20D85FDL, 0xA50AB56BL,
    0x35B5A8FAL, 0x42B2986CL, 0xDBBBC9D6L, 0xACBCF940L, 0x32D86CE3L,
    0x45DF5C75L, 0xDCD60DCFL, 0xABD13D59L, 0x26D930ACL, 0x51DE003AL,
    0xC8D75180L, 0xBFD06116L, 0x21B4F4B5L, 0x56B3C423L, 0xCFBA9599L,
    0xB8BDA50FL, 0x2802B89EL, 0x5F058808L, 0xC60CD9B2L, 0xB10BE924L,
    0x2F6F7C87L, 0x58684C11L, 0xC1611DABL, 0xB6662D3DL, 0x76DC4190L,
    0x01DB7106L, 0x98D220BCL, 0xEFD5102AL, 0x71B18589L, 0x06B6B51FL,
    0x9FBFE4A5L, 0xE8B8D433L, 0x7807C9A2L, 0x0F00F934L, 0x9609A88EL,
    0xE10E9818L, 0x7F6A0DBBL, 0x086D3D2DL, 0x91646C97L, 0xE6635C01L,
    0x6B6B51F4L, 0x1C6C6162L, 0x856530D8L, 0xF262004EL, 0x6C0695EDL,
    0x1B01A57BL, 0x8208F4C1L, 0xF50FC457L, 0x65B0D9C6L, 0x12B7E950L,
    0x8BBEB8EAL, 0xFCB9887CL, 0x62DD1DDFL, 0x15DA2D49L, 0x8CD37CF3L,
    0xFBD44C65L, 0x4DB26158L, 0x3AB551CEL, 0xA3BC0074L, 0xD4BB30E2L,
    0x4ADFA541L, 0x3DD895D7L, 0xA4D1C46DL, 0xD3D6F4FBL, 0x4369E96AL,
    0x346ED9FCL, 0xAD678846L, 0xDA60B8D0L, 0x44042D73L, 0x33031DE5L,
    0xAA0A4C5FL, 0xDD0D7CC9L, 0x5005713CL, 0x270241AAL, 0xBE0B1010L,
    0xC90C2086L, 0x5768B525L, 0x206F85B3L, 0xB966D409L, 0xCE61E49FL,
    0x5EDEF90EL, 0x29D9C998L, 0xB0D09822L, 0xC7D7A8B4L, 0x59B33D17L,
    0x2EB40D81L, 0xB7BD5C3BL, 0xC0BA6CADL, 0xEDB88320L, 0x9ABFB3B6L,
    0x03B6E20CL, 0x74B1D29AL, 0xEAD54739L, 0x9DD277AFL, 0x04DB2615L,
    0x73DC1683L, 0xE3630B12L, 0x94643B84L, 0x0D6D6A3EL, 0x7A6A5AA8L,
    0xE40ECF0BL, 0x9309FF9DL, 0x0A00AE27L, 0x7D079EB1L, 0xF00F9344L,
    0x8708A3D2L, 0x1E01F268L, 0x6906C2FEL, 0xF762575DL, 0x806567CBL,
    0x196C3671L, 0x6E6B06E7L, 0xFED41B76L, 0x89D32BE0L, 0x10DA7A5AL,
    0x67DD4ACCL, 0xF9B9DF6FL, 0x8EBEEFF9L, 0x17B7BE43L, 0x60B08ED5L,
    0xD6D6A3E8L, 0xA1D1937EL, 0x38D8C2C4L, 0x4FDFF252L, 0xD1BB67F1L,
    0xA6BC5767L, 0x3FB506DDL, 0x48B2364BL, 0xD80D2BDAL, 0xAF0A1B4CL,
    0x36034AF6L, 0x41047A60L, 0xDF60EFC3L, 0xA867DF55L, 0x316E8EEFL,
    0x4669BE79L, 0xCB61B38CL, 0xBC66831AL, 0x256FD2A0L, 0x5268E236L,
    0xCC0C7795L, 0xBB0B4703L, 0x220216B9L, 0x5505262FL, 0xC5BA3BBEL,
    0xB2BD0B28L, 0x2BB45A92L, 0x5CB36A04L, 0xC2D7FFA7L, 0xB5D0CF31L,
    0x2CD99E8BL, 0x5BDEAE1DL, 0x9B64C2B0L, 0xEC63F226L, 0x756AA39CL,
    0x026D930AL, 0x9C0906A9L, 0xEB0E363FL, 0x72076785L, 0x05005713L,
    0x95BF4A82L, 0xE2B87A14L, 0x7BB12BAEL, 0x0CB61B38L, 0x92D28E9BL,
    0xE5D5BE0DL, 0x7CDCEFB7L, 0x0BDBDF21L, 0x86D3D2D4L, 0xF1D4E242L,
    0x68DDB3F8L, 0x1FDA836EL, 0x81BE16CDL, 0xF6B9265BL, 0x6FB077E1L,
    0x18B74777L, 0x88085AE6L, 0xFF0F6A70L, 0x66063BCAL, 0x11010B5CL,
    0x8F659EFFL, 0xF862AE69L, 0x616BFFD3L, 0x166CCF45L, 0xA00AE278L,
    0xD70DD2EEL, 0x4E048354L, 0x3903B3C2L, 0xA7672661L, 0xD06016F7L,
    0x4969474DL, 0x3E6E77DBL, 0xAED16A4AL, 0xD9D65ADCL, 0x40DF0B66L,
    0x37D83BF0L, 0xA9BCAE53L, 0xDEBB9EC5L, 0x47B2CF7FL, 0x30B5FFE9L,
    0xBDBDF21CL, 0xCABAC28AL, 0x53B39330L, 0x24B4A3A6L, 0xBAD03605L,
    0xCDD70693L, 0x54DE5729L, 0x23D967BFL, 0xB3667A2EL, 0xC4614AB8L,
    0x5D681B02L, 0x2A6F2B94L, 0xB40BBE37L, 0xC30C8EA1L, 0x5A05DF1BL,
    0x2D02EF8DL
};

void MainWindow::CRC32_Init()
{
    mCrc = 0xFFFFFFFFL;
}

void MainWindow::CRC32_Update(unsigned char *data, size_t len)
{
    unsigned long i;
    for (i = 0; i < len; i++)
    {
        mCrc = (mCrc >> 8) ^ crc32_tbl[(mCrc & 0xFF) ^  *data++];
    }
}

QByteArray MainWindow::CRC32_Final()
{
    mCrc ^= 0xFFFFFFFFUL;
    unsigned char temp[8];
    QByteArray ret;
    temp[0] = (mCrc & 0xFF000000UL) >> 24;
    ret.append(temp[0]);
    temp[1] = (mCrc & 0x00FF0000UL) >> 16;
    ret.append(temp[1]);
    temp[2] = (mCrc & 0x0000FF00UL) >> 8;
    ret.append(temp[2]);
    temp[3] = (mCrc & 0x000000FFUL);
    ret.append(temp[3]);

    return ret;
}

