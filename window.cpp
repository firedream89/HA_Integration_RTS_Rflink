#include "window.h"
#include "ui_window.h"

Window::Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Window)
{
    ui->setupUi(this);
    serial = new QSerialPort("usb");

    _counter = 0;

    //Reading available serial port
    const auto serialPortInfos = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo &serialPortInfo : serialPortInfos)
        ui->usb->addItem(serialPortInfo.systemLocation());

    connect(ui->start, &QPushButton::clicked, this, &Window::Start);
    connect(ui->createConfig, &QPushButton::clicked, this, &Window::CreateConfig);
    connect(serial, &QSerialPort::readyRead, this, &Window::SerialReadyRead);
    connect(serial, &QSerialPort::errorOccurred, this, &Window::SerialError);
    connect(ui->addingRow, &QPushButton::clicked, this, &Window::AddingRow);
}

Window::~Window()
{
    delete ui;
    delete serial;
}

void Window::Start()
{
    ui->start->setEnabled(false);

    serial->setPortName(ui->usb->currentText());
    serial->setBaudRate(59000);

     if(!serial->open(QIODevice::ReadOnly)) {
         QMessageBox::warning(this,"Error","Fail to reading port !");
         Stop();
     }

}

void Window::Stop()
{
    serial->close();
    ui->start->setEnabled(true);
}

//When new device discovered
void Window::SerialReadyRead()
{
    QByteArray array = serial->readLine();

    ui->lastRX->setText(array);
    QStringList list = QString(array).split(";");


    //not a device
    if(list.count() != 7) return;

    QString id = list.at(3).split("=").last();
    QString protocol = list.at(2);

    //item exist
    if(ui->table->findItems(id,Qt::MatchExactly).count() != 0) return;

    //insert new item in table
    ui->table->insertRow(0);
    ui->table->setItem(0,0,new QTableWidgetItem());
    ui->table->setItem(0,1,new QTableWidgetItem(id));
    ui->table->setItem(0,2,new QTableWidgetItem(protocol));
}

void Window::SerialError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        QMessageBox::warning(this,"Error","Reading error !");
        Stop();
    }
}

///Adding rows
void Window::AddingRow()
{
    for(int i=0;i<ui->rowNb->text().toInt();i++) {
        int id = ui->table->rowCount();
        ui->table->insertRow(id);
        ui->table->setItem(id,0,new QTableWidgetItem());
        ui->table->setItem(id,1,new QTableWidgetItem(QString::number(_counter) + "_0"));
        ui->table->setItem(id,2,new QTableWidgetItem("rts"));
        _counter++;
    }
}

///Creating configuration for configuration.yaml
void Window::CreateConfig()
{
    Stop();

    const QString enableRflink = "rflink:\n    port: {USB_PORT}\n\n";
    const QString base = "cover:\n    platform: rflink\n    devices:\n";
    const QString newDevice = "        rts_%1:\n            name: %2\n";

    QString result = enableRflink + base;

    for(int i=0;i<ui->table->rowCount();i++) {
        result += newDevice.arg(ui->table->item(i,1)->text(),ui->table->item(i,0)->text());
    }

    QDialog dialog(this);
    QVBoxLayout l;
    QPlainTextEdit tEdit;
    l.insertWidget(0,&tEdit);
    tEdit.setPlainText(result);

    dialog.setLayout(&l);

    dialog.exec();
}

















