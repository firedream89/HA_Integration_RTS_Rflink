#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QDebug>
#include <QList>
#include <QVBoxLayout>
#include <QPlainTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class Window; }
QT_END_NAMESPACE

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    ~Window();

public slots:
    void Start();
    void Stop();
    void SerialReadyRead();
    void SerialError(QSerialPort::SerialPortError serialPortError);
    void CreateConfig();

private:
    Ui::Window *ui;
    QSerialPort *serial;
};
#endif // WINDOW_H
