#ifndef QUIZIMAGE_H
#define QUIZIMAGE_H


#include <QtQuick/QQuickFramebufferObject>


class QuizImage : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    QuizImage();

    virtual Renderer *createRenderer() const override;
};

#endif // QUIZIMAGE_H
