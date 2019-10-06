#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QtDebug>
#include "lynxstructure.h"
#include "lynxuartqt.h"
#include "stewartcontrol.h"
#include <QtCore/QRandomGenerator>
#include <QTimer>
#include <QFile>
#include <QList>
#include <QDataStream>
#include <QStandardPaths>
#include <QDir>
// #include "teststruct.h"

#include <QtCharts/QAbstractSeries>

QT_BEGIN_NAMESPACE
class QQuickView;
QT_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE


class BackEnd : public QObject
{
    Q_OBJECT


    QTimer* newDataTimer;
    QTimer* newDataTimer2;


    LynxManager _lynx;
    LynxUartQt _uart;

    StewartControl _controlDatagram;
    StewartFeedback _feedbackDatagram;

    LynxInfo _receiveInfo;

    QList<QSerialPortInfo> _portList;
    QSerialPortInfo _selectedPort;

    unsigned long _baudrate = 115200;

    bool updateDial;
    QList<QVector<QPointF> > logger;
    QList<QVector<QPointF> > m_data;

    struct loggerInfo
    {
        int index;
        QString name;
        QString unit;
    };

    QList<loggerInfo> loggerInformation;
    int m_index;
    int m_loggingIndex;

    bool haltChartRefreshs,haltLogging,historicMode;
    qint64 xFirstPause;
    qint64 xLastPause;
public:
    void addSignal();
    void removeSignal();
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


    int sta() const {return _feedbackDatagram.sta;}
    double max;
    double min;




signals:
    void reScale();
    void refreshChart();
    void clearPortList();
    void addPort(const QString & portName);


    void getData(float input,E_variable variable);

public slots:

    // Updates the logger list function
    void newDataRecived();

    // Starts the chartview refresh emit
    void resumeChartviewRefresh(){ haltChartRefreshs = false; }

    // Stops the chartview refresh emit and store the current x axis values
    void pauseChartviewRefresh()
    {
        xLastPause=getLastX();
        xFirstPause=getFirstX();
        haltChartRefreshs = true;
    }
    // Enable the logger append
    void resumeLogging(){haltLogging = false;historicMode=false;}

    // Disable the logger append
    void pauseLogging(){haltLogging = true;}

    // Returns the max Y axis
    double getMaxY() { return max; }

    // Returns the min Y axis
    double getMinY() { return min; }

    // Resets the min and max Y axis
    void resetY() { max = 0; min = 0; }

    // Get the first Xaxis data. Dependant on realtime or history logging
    qint64 getFirstX()
    {
        if(haltChartRefreshs && !historicMode)
            return xFirstPause;
        else
            return qint64(logger.at(0).first().x());
    }

    // Get the last Xaxis data. Dependant on realtime or history logging
    qint64 getLastX()
    {
        if(haltChartRefreshs && !historicMode)
            return xLastPause;
        else
            return qint64(logger.at(0).last().x());
    }

    // Get signalText
    QString getSignalText(int index) { return loggerInformation.at(index).name; }

    // Actual update function from QML on chartview
    void update(QAbstractSeries *series,int index);



    void writeToCSV(QString filepath);
    void readFromCSV(QString filepath);

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
