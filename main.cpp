#include <QApplication>
#include <QQmlApplicationEngine>
#include "backend.h"
#include <QtQuick/QQuickView>
#include <QtQml/QQmlContext>
int main(int argc, char *argv[])
{
   // QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QQuickView viewer;

   BackEnd backend(&viewer);

    viewer.rootContext()->setContextProperty("backend", &backend);
#ifdef Q_OS_WIN
    QString extraImportPath(QStringLiteral("%1/../../../../%2"));
#else
    QString extraImportPath(QStringLiteral("%1/../../../%2"));
#endif
   // viewer.engine()->addImportPath(extraImportPath.arg(QGuiApplication::applicationDirPath(),QString::fromLatin1("qml")));
    QObject::connect(viewer.engine(), &QQmlEngine::quit, &viewer, &QWindow::close);

    viewer.setTitle(QStringLiteral("Lynx Logger"));

    //qmlRegisterType<BackEnd>("backend", 1, 1, "BackEnd");

    viewer.setSource(QUrl("qrc:/main.qml"));
    viewer.setMinimumSize(QSize(200,200));

   // viewer.setResizeMode(QQuickView::SizeRootObjectToView);
    //viewer.setColor(QColor("#404040"));
    viewer.show();


//    QQmlApplicationEngine engine;
//    const QUrl url(QStringLiteral("qrc:/main.qml"));
//    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
//                     &app, [url](QObject *obj, const QUrl &objUrl) {
//        if (!obj && url == objUrl)
//            QCoreApplication::exit(-1);
//    }, Qt::QueuedConnection);
//    engine.load(url);

    return app.exec();
}
