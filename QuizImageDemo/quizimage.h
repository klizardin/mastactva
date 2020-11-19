#ifndef QUIZIMAGE_H
#define QUIZIMAGE_H


#include <QtQuick/QQuickFramebufferObject>
#include <QQuickItem>


class QuizImage : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    QuizImage(QQuickItem *parent_ = nullptr);

    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

    Q_INVOKABLE void updateState();

public:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

public:
    qreal t() const;
    void setT(const qreal &t_);

signals:
    void tChanged();

private:
    qreal m_t = 0.0;
};


#endif // QUIZIMAGE_H
