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
#include <QQuickStyle>
#include <QTranslator>
#include "netapiv0.h"
#include "galleryeditviewmodel.h"
#include "qmlmainobjects.h"
#include "mastactvaapi.h"
#include "appconsts.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/utils.h"


void switchTranslator(QGuiApplication &app, QTranslator& translator, const QString& filename)
{
    app.removeTranslator(&translator);
    if(translator.load(filename, "."))
    {
        app.installTranslator(&translator);
    }
}

void switchLanguage(const QString & lang_, QTranslator& translator_, QTranslator& translatorQt_, QGuiApplication &app_)
{
    QString lang;
    if(g_belarusLanguage == lang_)
    {
        lang = "be_BY";
    }
    else if(g_englishLanguage == lang_)
    {
        lang = "en_US";
    }
    else
    {
        return;
    }

    QLocale locale = QLocale(lang);
    QLocale::setDefault(locale);

    switchTranslator(app_, translator_, QString("MastactvaQuizEditor_%1.qm").arg(lang));
    switchTranslator(app_, translatorQt_, QString("qt_%1.qm").arg(lang));
}


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QTranslator translator(&app);
    QTranslator translatorQt(&app);
    switchLanguage(AppConsts::getInstance()->getLanguage(), translator, translatorQt, app);

    app.setOrganizationName("Mastactva");
    app.setOrganizationDomain("mastactva.by");
    app.setApplicationName("Mastactva Quiz Editor Application");

    NetAPIV0::createSingelton(&app);
    QMLMainObjects::createSingelton(&app);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/Login.qml"));
    engine.addImportPath("qrc:/Mastactva");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if(url == objUrl)
        {
            if (nullptr == obj)
            {
                QCoreApplication::exit(-1);
            }
            else
            {
                QMLMainObjects *mainObjects = QMLMainObjects::getSingelton();
                Q_ASSERT(nullptr != mainObjects);
                if(nullptr != mainObjects)
                {
                    mainObjects->setRootQMLObject(obj);
                }
                QMLObjects::getInstance().setRoot(obj);
                //QMLObjects::getInstance().setInitialized();
            }
        }
    }, Qt::QueuedConnection);

    QMLMainObjects *mainObjects = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != mainObjects);
    if(nullptr != mainObjects)
    {
        mainObjects->setApp(&app);
    }

    engine.load(url);

    const bool res = app.exec();
    QMLObjects::getInstance().setRoot(nullptr);
    return res;
}
