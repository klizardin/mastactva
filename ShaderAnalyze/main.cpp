#include <QCoreApplication>
#include <QFile>
#include "shadercomments.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QStringList arguments = app.arguments();
    arguments.takeFirst();      // remove the first argument, which is the program's name

    ShaderComments *comments = new ShaderComments(&app);
    comments->append(arguments);

    QObject::connect(comments, &ShaderComments::finished,
                     &app, &QCoreApplication::quit);

    return app.exec();
}
