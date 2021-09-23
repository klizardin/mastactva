#ifndef EFFECTS_EXCHANGE_H
#define EFFECTS_EXCHANGE_H


#include <QObject>
#include <QtQuick/QQuickItem>


class EffectsExchange : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit EffectsExchange(QObject *parent_ = nullptr);
    virtual ~EffectsExchange() override;

    Q_PROPERTY(QString savePath READ savePath WRITE setSavePath NOTIFY savePathChanged)
    Q_INVOKABLE void download();
    Q_INVOKABLE void upload();

private:
    QString savePath() const;
    void setSavePath(const QString &path_);

public:
signals:
    void savePathChanged();

protected slots:

private:
    QString m_path;
};


#endif // EFFECTS_EXCHANGE_H
