#include "widget.h"
#include "ui_widget.h"

int protocol_type = 0;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //socket_.readyRead(); //inner_signal

    QObject::connect(&socket_, &QAbstractSocket::connected, this, &Widget::doConnected);
    QObject::connect(&socket_, &QAbstractSocket::disconnected, this, &Widget::doDisconnected);
    QObject::connect(&socket_, &QAbstractSocket::readyRead, this, &Widget::doReadyRead);
    QObject::connect(&socket_, &QAbstractSocket::stateChanged, this, &Widget::changeEnabled);

    changeEnabled();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::changeEnabled() {
    ui->pbConnect->setEnabled(socket_.state() == QAbstractSocket::UnconnectedState);
    ui->pbDisconnect->setEnabled(socket_.state() == QAbstractSocket::ConnectedState);
    ui->pbSend->setEnabled(socket_.state() == QAbstractSocket::ConnectedState);
    ui->cbSslTcp->setEnabled(socket_.state() == QAbstractSocket::UnconnectedState);
    ui->pbClear->setEnabled(!ui->pteMessage->toPlainText().isEmpty());
}

void Widget::doConnected() {
    ui->pteMessage->insertPlainText("Connected\r\n");
    changeEnabled();
}


void Widget::doDisconnected() {
    ui->pteMessage->insertPlainText("Disconnected\r\n");
    changeEnabled();
}


void Widget::doReadyRead() {
    ui->pteMessage->insertPlainText(socket_.readAll());
}


void Widget::on_pbConnect_clicked()
{
    if (protocol_type == 0) socket_.connectToHost(ui->leHost->text(), ui->lePort->text().toUShort());
    else socket_.connectToHostEncrypted(ui->leHost->text(), ui->lePort->text().toUShort());

    changeEnabled();
}


void Widget::on_pbDisconnect_clicked()
{
    socket_.close();
    changeEnabled();
}


void Widget::on_pbSend_clicked()
{
    socket_.write(ui->pteSend->toPlainText().toUtf8());
    changeEnabled();
}


void Widget::on_pbClear_clicked()
{
    ui->pteMessage->clear();
    changeEnabled();
}


void Widget::on_cbSslTcp_stateChanged(int arg1)
{
    if (arg1 == 0) {
        protocol_type = 0;
        ui->lePort->setText("80");
    }
    else {
        protocol_type = 1;
        ui->lePort->setText("443");
    }
}
