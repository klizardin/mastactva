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
    Q_PROPERTY(QString fromImage READ fromImage WRITE setFromImage NOTIFY fromImageChanged)
    Q_PROPERTY(QString toImage READ toImage WRITE setToImage NOTIFY toImageChanged)

    Q_INVOKABLE void updateState();

public:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

public:
    qreal t() const;
    void setT(const qreal &t_);
    QColor color() const;
    void setColor(const QColor &clr_);
    QString fromImage() const;
    void setFromImage(const QString &fromImage_);
    QString toImage() const;
    void setToImage(const QString &toImage_);

signals:
    void tChanged();
    void colorChanged();
    void fromImageChanged();
    void toImageChanged();

private:
    qreal m_t = 0.0;
    QColor m_color = QColor("blue");
    QString m_fromImage;
    QString m_toImage;
};


#endif // QUIZIMAGE_H
