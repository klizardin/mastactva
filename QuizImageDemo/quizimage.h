#ifndef QUIZIMAGE_H
#define QUIZIMAGE_H


#include <QtQuick/QQuickFramebufferObject>
#include <QQuickItem>
#include <QColor>


class QuizImage : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    QuizImage(QQuickItem *parent_ = nullptr);

    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

    Q_INVOKABLE void updateState();

public:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

public:
    qreal t() const;
    void setT(const qreal &t_);
    QColor color() const;
    void setColor(const QColor &clr_);

signals:
    void tChanged();
    void colorChanged();

private:
    qreal m_t = 0.0;
    QColor m_color = QColor("blue");
};


#endif // QUIZIMAGE_H
