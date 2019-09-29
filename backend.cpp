#include "backend.h"
#include <QtCharts/QXYSeries>
#include <QtCharts/QAreaSeries>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QtCore/QDebug>

#include <QtCore/QtMath>
#include <QTime>
BackEnd::BackEnd(QObject *parent) :
    QObject(parent),
    _lynx(0x25),
    _uart(_lynx),
    _controlDatagram(_lynx, 0x22,"Control Datagram"),
    _feedbackDatagram(_lynx, 0x23,"Feedback Datagram"),
    m_index(-1),
    m_loggingIndex(0)

{
   //  _uart.open(4, 115200);
    newDataTimer = new QTimer(this);
    connect(newDataTimer, SIGNAL(timeout()), this, SLOT(newData()));
    newDataTimer->start(10);
    connect(_uart.portPointer(), SIGNAL(readyRead()), this, SLOT(readData()));


    qRegisterMetaType<QAbstractSeries*>();
    qRegisterMetaType<QAbstractAxis*>();
    QVector<QPointF> points;
    points.reserve(20000000);

    logger.append(points);
    logger.append(points);
    logger.append(points);
    logger.append(points);
    logger.append(points);

 //   generateData(0, 5, 1024);
}

void BackEnd::newData()
{
    emit rollChanged();
    emit pitchChanged();
    emit yawChanged();

    QDateTime momentInTime = QDateTime::currentDateTime();

    logger[0].append(QPointF(momentInTime.toMSecsSinceEpoch(), 10* qSin(momentInTime.toMSecsSinceEpoch()/1000.0*6.0/10.0)+double(roll())));
    logger[1].append(QPointF(momentInTime.toMSecsSinceEpoch(), 10*qCos(momentInTime.toMSecsSinceEpoch()/1000.0*6/10)+pitch()));
    logger[2].append(QPointF(momentInTime.toMSecsSinceEpoch(), 10*qSin(momentInTime.toMSecsSinceEpoch()/1000*6/10)+yaw()));
    logger[3].append(QPointF(momentInTime.toMSecsSinceEpoch(), 10*qSin(momentInTime.toMSecsSinceEpoch()/1000*6/10)+yaw()));
    logger[4].append(QPointF(momentInTime.toMSecsSinceEpoch(), 10*qSin(momentInTime.toMSecsSinceEpoch()/1000*6/10)+yaw()));

    m_loggingIndex--;

    //qDebug()<<"time: "<<time.msecsSinceStartOfDay();
    //qDebug()<<time;


}
void BackEnd::removeSignal()
{

}
void BackEnd::addSignal()
{
    QVector<QPointF> points;
    points.reserve(20000000);
    logger.append(points);
    loggerInfo info;
    info.name = "Test";
    info.unit = "m/s";

    info.index = logger.count();

    loggerInformation.append(info);
    qDebug()<<"new append: ";
}

void BackEnd::sendData()
{
    _uart.send(_controlDatagram.lynxId());
}

void BackEnd::readData()
{
    _receiveInfo = _uart.update();


    if(_receiveInfo.state != eNoChange)
    {


        if(_receiveInfo.state == eNewDataReceived)
        {
            //qDebug()<<_feedbackDatagram.sta;
            //qDebug()<<"XYZ: "<<_feedbackDatagram.feedbackX<<" : "<<_feedbackDatagram.feedbackY<<" : "<<_feedbackDatagram.feedbackZ;
            //qDebug()<<"RPY: "<<_feedbackDatagram.feedbackRoll<<" : "<<_feedbackDatagram.feedbackPitch<<" : "<<_feedbackDatagram.feedbackYaw;
            //qDebug()<<"roll: "<<_feedbackDatagram.imuRoll<<" Pitch: "<<_feedbackDatagram.imuPitch<<" Yaw:"<<_feedbackDatagram.imuYaw;
            emit rollChanged();
            emit pitchChanged();
            emit yawChanged();
            if(updateDial || _feedbackDatagram.sta & (1<<3))
            {
                this->getData(_feedbackDatagram.feedbackX,Ex);
                this->getData(_feedbackDatagram.feedbackY,Ey);
                this->getData(_feedbackDatagram.feedbackZ,Ez);
                this->getData(_feedbackDatagram.feedbackRoll,Eroll);
                this->getData(_feedbackDatagram.feedbackPitch,Epitch);
                this->getData(_feedbackDatagram.feedbackYaw,Eyaw);
                updateDial=false;
            }

        }
    }
}

void BackEnd::enableYawButtonClicked(bool state)
{


    if(state)
        _controlDatagram.command = 12;
    else {
        _controlDatagram.command = 13;
    }
     _uart.send(_controlDatagram.command.lynxId());
}
void BackEnd::gyroConnectButtonClicked(bool state)
{
    qDebug()<<"state is: "<<state;

    if(state)
        _controlDatagram.command = 10;
    else {
        _controlDatagram.command = 11;
    }
     _uart.send(_controlDatagram.command.lynxId());
}
void BackEnd::refreshPortList()
{
    this->clearPortList();
    _portList = QSerialPortInfo::availablePorts();

    QString tempName;
    for (int i = 0; i < _portList.count(); i++)
    {
        tempName = _portList.at(i).portName();
        tempName += " - ";
        tempName += _portList.at(i).description();

        if (!_portList.at(i).isNull())
            this->addPort(tempName);
    }
}

void BackEnd::portSelected(int index)
{
    qDebug() << "Port selsected";
    qDebug() << "Index: " << index;
    if ((index < 0) || (index >= _portList.count()))
        return;

    _selectedPort = _portList.at(index);
}

void BackEnd::connectButtonClicked()
{
    // qDebug() << "Button clicked";
    if (_uart.opened())
    {
        qDebug() << "Closing port";
        _uart.close();
    }
    else
    {
        qDebug() << "Attempting to open";
        qDebug() << _selectedPort.portName();
        qDebug() << _selectedPort.description();
        if (_uart.open(_selectedPort, _baudrate))
            qDebug() << "Opened successfully";
        else
            qDebug() << "Open failed";
    }

}


void BackEnd::update(QAbstractSeries *series,int index)
{

    if (series) {
        QXYSeries *xySeries = static_cast<QXYSeries *>(series);

        m_index++;
        if (m_index > logger.count() - 1)
            m_index = 0;

        QVector<QPointF> points = logger.at(index);
        if(points.last().y()>max)
        {
            max=points.last().y();

        }
        if(points.last().y()<min)
        {
            min=points.last().y();

        }
        // Use replace instead of clear + append, it's optimized for performance
        xySeries->replace(points);


    }
}
