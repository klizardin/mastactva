#ifndef QUIZIMAGE_H
#define QUIZIMAGE_H


#include <QtQuick/QQuickFramebufferObject>


class QuizImageFB : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    QuizImageFB();

    Q_INVOKABLE void updateState();

    virtual Renderer *createRenderer() const override;
};

#endif // QUIZIMAGE_H
