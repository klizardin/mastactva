#ifndef QUIZIMAGE_H
#define QUIZIMAGE_H


#include <QtQuick/QQuickFramebufferObject>
#include <QQuickItem>


class QuizImageFB : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    QuizImageFB();

    Q_INVOKABLE void updateState();

    virtual Renderer *createRenderer() const override;
};


class QuizImage : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    QuizImage(QQuickItem *parent_ = nullptr);
    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;
};


#endif // QUIZIMAGE_H
