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
    m_loggingIndex(0),
    haltChartRefreshs(false),
    haltLogging(false),
    historicMode(false)

{

    connect(_uart.portPointer(), SIGNAL(readyRead()), this, SLOT(readData()),Qt::UniqueConnection
            );
   //  _uart.open(4, 115200);
    newDataTimer = new QTimer(this);
    connect(newDataTimer, SIGNAL(timeout()), this, SLOT(newDataRecived()),Qt::UniqueConnection);
    newDataTimer->start(10);

    // SETUP LOGGING

    loggerInformation.append(loggerInfo{0,"Voltage",""});
    loggerInformation.append(loggerInfo{0,"Current",""});
    loggerInformation.append(loggerInfo{0,"Power",""});
    loggerInformation.append(loggerInfo{0,"Flux",""});
    loggerInformation.append(loggerInfo{0,"Pressure",""});
//    loggerInformation[0].name = "Voltage";
//    loggerInformation[0].index = 0;

//    loggerInformation[1].name = "Current";
//    loggerInformation[1].index = 1;

//    loggerInformation[2].name = "Power";
//    loggerInformation[2].index = 2;

    QVector<QPointF> points;
    points.reserve(20);

    logger.append(points);
    logger.append(points);
    logger.append(points);
    logger.append(points);
    logger.append(points);

 //   generateData(0, 5, 1024);
}


void BackEnd::newDataRecived()
{
    QDateTime momentInTime = QDateTime::currentDateTime();
    if(!haltLogging)
    {
        logger[0].append(QPointF(momentInTime.toMSecsSinceEpoch(), 10* qSin(momentInTime.toMSecsSinceEpoch()/1000.0*6.0/10.0)+double(1*QRandomGenerator::global()->generateDouble())));
        logger[1].append(QPointF(momentInTime.toMSecsSinceEpoch(), 10*qCos(momentInTime.toMSecsSinceEpoch()/1000.0*6/10)+1*QRandomGenerator::global()->generateDouble()));
        logger[2].append(QPointF(momentInTime.toMSecsSinceEpoch(), 7*qSin(momentInTime.toMSecsSinceEpoch()/1000.0*6/9)+0.2*QRandomGenerator::global()->generateDouble()));
        logger[3].append(QPointF(momentInTime.toMSecsSinceEpoch(), 5*qCos(momentInTime.toMSecsSinceEpoch()/1000.0*6/7)+0.1*QRandomGenerator::global()->generateDouble()));
        logger[4].append(QPointF(momentInTime.toMSecsSinceEpoch(), 4*qSin(qCos(momentInTime.toMSecsSinceEpoch()/1000.0*6/3)+0.4*QRandomGenerator::global()->generateDouble())));
    }

    if(!haltChartRefreshs)
    {
        emit refreshChart();
   }

}
void BackEnd::readFromCSV(QString filepath)
{
    // Open csv-file
    pauseLogging();
    pauseChartviewRefresh();
    QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QDir dir;
    //QFile file(path + "\\file.csv");
    std::string x =filepath.toStdString().substr(filepath.toStdString().find("///") + 3);

    QFile file(QString::fromUtf8(x.c_str()));
    if (!file.open(QIODevice::ReadOnly)) {
                    qDebug() << file.errorString();
                    return ;
                }
    QStringList wordList;
    logger.clear();
    int colums = file.readLine().split(',').count()-1;

    QVector<QPointF> points;

    for (int c=0;c<colums;c++)
    {
        logger.append(points);
    }
    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        QByteArray xpoint = line.split(',') .first();
        QDateTime Date = QDateTime::fromString(xpoint,Qt::ISODateWithMs);
        //qDebug()<<line;
        for (int n=0;n<colums-1;n++)
        {
            logger[n].append(QPointF(qint64(Date.toMSecsSinceEpoch()),line.split(',').at(n+1).toDouble()));
        }
    }

    historicMode=true;
    emit refreshChart();
    emit reScale();
}

void BackEnd::writeToCSV(QString filepath)
{
    // Open csv-file
    pauseLogging();
    QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QDir dir;
    QFile file(path + "\\file.csv");
    file.remove();
    if ( !file.open(QIODevice::ReadWrite | QIODevice::Text) )
    {
        qDebug() << file.errorString();
        return ;
    }

    // Write data to file
    QTextStream stream(&file);
    QString separator(",");
    stream << QString("Time");
    for (int n = 1; n < logger.size(); ++n)
    {
        stream<<separator + QString("Data ")+QString::number(n);
    }
    stream << separator<<endl;

    for (int i = 0; i < logger.at(0).size()-1; ++i)
    {
        stream << QDateTime::fromMSecsSinceEpoch(qint64(logger.at(0).at(i).x())).toString(Qt::ISODateWithMs);
        for (int n = 0; n < logger.size()-1; ++n)
        {
            stream << separator << QString::number(logger.at(n).at(i).y());
        }
        stream << endl;
    }
    stream.flush();
    file.close();
    resumeLogging();
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
