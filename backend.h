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


    Q_PROPERTY(float roll READ roll NOTIFY rollChanged)
    Q_PROPERTY(float pitch READ pitch  NOTIFY pitchChanged)
    Q_PROPERTY(float yaw READ yaw  NOTIFY yawChanged)
    Q_PROPERTY(int sta READ sta  NOTIFY yawChanged)

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
        int *dataPointer;
    };
    void readFromCSV(QString filepath)
    {
        // Open csv-file
        QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QDir dir;
        QFile file(path + "\\file.csv");
        if (!file.open(QIODevice::ReadOnly)) {
                        qDebug() << file.errorString();
                        return ;
                    }
        QStringList wordList;
        logger.clear();
        int colums = file.readLine().split(',').count()-1;
        qDebug()<<colums;
        QVector<QPointF> points;

        for (int c=0;c<colums;c++) {
            logger.append(points);

        }

        int i=0;
            while (!file.atEnd())
            {
                QByteArray line = file.readLine();
                QByteArray xpoint = line.split(',') .first();
                QDateTime Date = QDateTime::fromString(xpoint,Qt::ISODateWithMs);

                for (int n=0;n<colums-1;n++)
                {
                    //qDebug()<<line.split(',').at(n+1).toDouble();
                    logger[n].append(QPointF(qint64(Date.toMSecsSinceEpoch()),line.split(',').at(n+1).toDouble()) );

                }
                i++;

            }

            //
            emit refreshChart();
            pauseChartviewRefresh();
            newDataTimer->start(10);
            emit reScale();

    }

    void WriteToCSV(const QList<QVector<QPointF>>& pixels)
    {
        // Open csv-file
        QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QDir dir;
        QFile file(path + "\\file.csv");
        file.remove();
        if ( file.open(QIODevice::ReadWrite | QIODevice::Text) )
            {
                qDebug()<<"file now exists";
            }

        // Write data to file
        QTextStream stream(&file);
        QString separator(",");
        stream << QString("Time");
        for (int n = 1; n < pixels.size(); ++n)
        {
            stream<<separator + QString("Data ")+QString::number(n);
        }
        stream << separator<<endl;

        for (int i = 0; i < pixels.at(0).size()-1; ++i)
        {
            stream << QDateTime::fromMSecsSinceEpoch(qint64(pixels.at(0).at(i).x())).toString(Qt::ISODateWithMs);
            for (int n = 1; n < pixels.size(); ++n)
            {
                stream << separator << QString::number(pixels.at(n).at(i).y());
            }
            stream << endl;
        }

        stream.flush();
        file.close();
    }
    QList<loggerInfo> loggerInformation;
    int m_index;
    int m_loggingIndex;

    bool haltChartRefreshs;
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

    float roll() const { return  1*QRandomGenerator::global()->generateDouble();}//_feedbackDatagram.imuRoll; }
    float pitch() const { return 1*QRandomGenerator::global()->generateDouble();}//_feedbackDatagram.imuPitch; }
    float yaw() const { return 1*QRandomGenerator::global()->generateDouble();}//_feedbackDatagram.imuYaw; }

    int sta() const {return _feedbackDatagram.sta;}
    double max;
    double min;




signals:
    void reScale();
    void refreshChart();
    void clearPortList();
    void addPort(const QString & portName);

    void rollChanged();
    void pitchChanged();
    void yawChanged();
    void getData(float input,E_variable variable);
private slots:
    void newDataRecived();
public slots:


    void resumeChartviewRefresh(){haltChartRefreshs = false;
                                 qDebug()<<"SET "<<haltChartRefreshs;}
    void pauseChartviewRefresh(){haltChartRefreshs = true;
                                qDebug()<<"set "<<haltChartRefreshs;}
    double getMax(){return max;}
    double getMin(){return min;}
    qint64 getFirst(){qDebug()<<QDateTime::fromMSecsSinceEpoch(logger.at(0).first().x());
        return logger.at(0).first().x();}
    qint64 getLast(){qDebug()<<QDateTime::fromMSecsSinceEpoch(logger.at(0).last().x());
                     return logger.at(0).last().x();}
    //void generateData(int type, int rowCount, int colCount);
    void update(QAbstractSeries *series,int index);
    void readFromTextfile(){ newDataTimer->stop();
                             qDebug()<<"Read to file..";
                                                  readFromCSV("");}
    void saveToTextfile();
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
