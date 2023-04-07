/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCommandLineParser>
#include "../MastactvaBase/quizimageqwindowsinglethread.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QCommandLineParser commandLineParser;
    commandLineParser.setApplicationDescription("Graphics engine test application");
    commandLineParser.addHelpOption();
    commandLineParser.addVersionOption();

    QCommandLineOption commandLineRunTestByTestOption(QStringList{} << "t" << "test-by-test", "run multiple tests");
    commandLineParser.addOption(commandLineRunTestByTestOption);

    commandLineParser.process(app);

    qDebug() << "command line:";
    qDebug() << "\t" << commandLineRunTestByTestOption.names() << " :" << commandLineParser.isSet(commandLineRunTestByTestOption);

    /*QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.addImportPath("qrc:/Mastactva");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);*/


    // TODO:
    // use of the window
    // (standard steps for QWindow):
    // create
    QuizImageQWindowSingleThread window(QStringLiteral("qrc:/main3.qml"), commandLineParser.isSet(commandLineRunTestByTestOption));
    // set default size
    window.resize(800, 600);
    // show
    window.show();


    return app.exec();
}
