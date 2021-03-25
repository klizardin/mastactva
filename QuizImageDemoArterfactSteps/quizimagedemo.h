#ifndef QUIZIMAGEDEMO_H
#define QUIZIMAGEDEMO_H


#include <QtQuick/QQuickFramebufferObject>
#include <QQuickItem>
#include <QColor>



class QuizImageDemo : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    QuizImageDemo(QQuickItem *parent_ = nullptr);

    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(QString fromImage READ fromImage WRITE setFromImage NOTIFY fromImageChanged)
    Q_PROPERTY(QString toImage READ toImage WRITE setToImage NOTIFY toImageChanged)
    Q_PROPERTY(QString projectFilename READ project WRITE setProject NOTIFY projectChanged)

    Q_INVOKABLE void updateState();

public:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

public:
    qreal t() const;
    void setT(const qreal &t_);
    QString fromImage() const;
    void setFromImage(const QString &fromImage_);
    QString toImage() const;
    void setToImage(const QString &toImage_);
    QString project() const;
    void setProject(const QString &project_);

signals:
    void tChanged();
    void fromImageChanged();
    void toImageChanged();
    void projectChanged();

private:
    qreal m_t = 0.0;
    QString m_fromImage;
    QString m_toImage;
    QString m_project;
};


#endif // QUIZIMAGEDEMO_H
