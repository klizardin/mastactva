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
#include <QTranslator>
#include <QFile>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


void switchTranslator(QGuiApplication &app,
                      QTranslator& translator,
                      const QString& filename)
{
    app.removeTranslator(&translator);
    if(translator.load(filename, "."))
    {
        app.installTranslator(&translator);
    }
}

void switchLanguage(const QString & lang_,
                    QTranslator& translator_,
                    QTranslator& translatorQt_,
                    QGuiApplication &app_)
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

    switchTranslator(app_, translator_, QString("MastactvaQuiz_%1.qm").arg(lang));
    switchTranslator(app_, translatorQt_, QString("qt_%1.qm").arg(lang));
}

void copyAssets()
{
#if defined(ASSETS_ROOT)
    QFile dfile1("assets:/mastactva_ro.db3");
    if (dfile1.exists())
    {
         dfile1.copy("./mastactva_ro.db3");
         QFile::setPermissions("./mastactva_ro.db3", QFile::ReadOwner);
    }
    QFile dfile2("assets:/mastactva_rw.db3");
    if (dfile2.exists())
    {
         dfile2.copy("./mastactva_rw.db3");
         QFile::setPermissions("./mastactva_rw.db3", QFile::WriteOwner | QFile::ReadOwner);
    }
    QFile dfile3("assets:/MastactvaQuiz_be_BY.qm");
    if (dfile3.exists())
    {
         dfile3.copy("./MastactvaQuiz_be_BY.qm");
         QFile::setPermissions("./MastactvaQuiz_be_BY.qm", QFile::ReadOwner);
    }
#endif
}

int main(int argc, char *argv[])
{
#if defined(TRACE_THREADS)
    qDebug() << "main()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    copyAssets();

    QGuiApplication app(argc, argv);
    QTranslator translator(&app);
    QTranslator translatorQt(&app);
    switchLanguage(g_belarusLanguage, translator, translatorQt, app);

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);

    // Request OpenGL 3.3 core or OpenGL ES 3.0.
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        qDebug("Requesting 3.3 core context");
        fmt.setVersion(3, 3);
        fmt.setProfile(QSurfaceFormat::CoreProfile);
    } else {
        qDebug("Requesting 3.0 context");
        fmt.setVersion(3, 0);
    }

    QSurfaceFormat::setDefaultFormat(fmt);

    app.setOrganizationName("Mastactva");
    app.setOrganizationDomain("mastactva.by");
    app.setApplicationName("Mastactva Quiz Application");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.addImportPath("qrc:/Mastactva");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl)
    {
        if(url == objUrl)
        {
            if (!obj)
            {
                QCoreApplication::exit(-1);
            }
            else
            {
                QMLObjectsBase &mainObjects = QMLObjectsBase::getInstance();
                mainObjects.setRoot(obj);
                mainObjects.setInitialized();
            }
        }

    }, Qt::QueuedConnection);
    engine.load(url);

    const bool res = app.exec();
    QMLObjectsBase::getInstance().setRoot(nullptr);
    return res;
}
