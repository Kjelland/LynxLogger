#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QtDebug>
#include "lynxstructure.h"
#include "lynxuartqt.h"
#include "stewartcontrol.h"
#include <QtCore/QRandomGenerator>
// #include "teststruct.h"

#include <QtCharts/QAbstractSeries>

QT_BEGIN_NAMESPACE
class QQuickView;
QT_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE


class BackEnd : public QObject
{
    Q_OBJECT


    Q_PROPERTY(float roll READ roll NOTIFY rollChanged)
    Q_PROPERTY(float pitch READ pitch  NOTIFY pitchChanged)
    Q_PROPERTY(float yaw READ yaw  NOTIFY yawChanged)
    Q_PROPERTY(int sta READ sta  NOTIFY yawChanged)





    LynxManager _lynx;
    LynxUartQt _uart;

    StewartControl _controlDatagram;
    StewartFeedback _feedbackDatagram;

    LynxInfo _receiveInfo;

    QList<QSerialPortInfo> _portList;
    QSerialPortInfo _selectedPort;

    unsigned long _baudrate = 115200;

    bool updateDial;

    QList<QVector<QPointF> > m_data;
    int m_index;

public:
    enum E_variable{
        Ex,
        Ey,
        Ez,
        Eroll,
        Epitch,
        Eyaw
    };
    Q_ENUM(E_variable)
    explicit BackEnd(QObject *parent = nullptr);
    ~BackEnd() { _uart.close(); }

    float roll() const { return  10*QRandomGenerator::global()->generateDouble();}//_feedbackDatagram.imuRoll; }
    float pitch() const { return 10*QRandomGenerator::global()->generateDouble();}//_feedbackDatagram.imuPitch; }
    float yaw() const { return 10*QRandomGenerator::global()->generateDouble();}//_feedbackDatagram.imuYaw; }

    int sta() const {return _feedbackDatagram.sta;}




signals:
    void clearPortList();
    void addPort(const QString & portName);

    void rollChanged();
    void pitchChanged();
    void yawChanged();

    void getData(float input,E_variable variable);

public slots:
    void generateData(int type, int rowCount, int colCount);
    void update(QAbstractSeries *series);
    void setData(float input,E_variable var)
    {
        qDebug()<<input << " as "<<var;
        updateDial=true;
        switch (var)
        {
        case Ex:
            _controlDatagram.setpointX = input;
            _uart.send(_controlDatagram.setpointX.lynxId());
            break;
        case Ey:
            _controlDatagram.setpointY = input;
            _uart.send(_controlDatagram.setpointY.lynxId());
            break;
        case Ez:
            _controlDatagram.setpointZ = input;
            _uart.send(_controlDatagram.setpointZ.lynxId());
            break;
        case Eroll:
            _controlDatagram.setpointRoll = input;
            _uart.send(_controlDatagram.setpointRoll.lynxId());
            break;
        case Epitch:
            _controlDatagram.setpointPitch = input;
            _uart.send(_controlDatagram.setpointPitch.lynxId());
            break;
        case Eyaw:
            _controlDatagram.setpointYaw = input;
            _uart.send(_controlDatagram.setpointYaw.lynxId());
            break;
        }

    }
    void sendData();
    void readData();
    void refreshPortList();
    void portSelected(int index);
    void connectButtonClicked();
    bool uartConnected() { return _uart.opened(); }
    void gyroConnectButtonClicked(bool state);
    void enableYawButtonClicked(bool state);
};

#endif // BACKEND_H
