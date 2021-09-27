#ifndef EFFECTS_EXCHANGE_H
#define EFFECTS_EXCHANGE_H


#include <QObject>
#include <QtQuick/QQuickItem>


class EffectModel;


class EffectsExchange : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit EffectsExchange(QObject *parent_ = nullptr);
    virtual ~EffectsExchange() override;

    Q_PROPERTY(QString savePath READ savePath WRITE setSavePath NOTIFY savePathChanged)
    Q_PROPERTY(QString saveArchive READ saveArchive WRITE setSaveArchive NOTIFY saveArchiveChanged)
    Q_INVOKABLE void download();
    Q_INVOKABLE void upload();
    Q_INVOKABLE void merge();

private:
    QString savePath() const;
    void setSavePath(const QString &url_);
    QString saveArchive() const;
    void setSaveArchive(const QString &url_);
    void initSavePath(const QString &path_);
    void free();
    void create();
    void downloadStep();
    void archiveResults();
    qreal stepProgress();

public:
signals:
    void savePathChanged();
    void saveArchiveChanged();
    void downloaded();
    void progress(qreal p_, const QString &status_);

protected slots:
    void listReloadedSlot();

private:
    QString m_path;
    QString m_archiveName;
    QString m_oldPathServerFiles;

    EffectModel *m_effectModel = nullptr;
    int m_step = 0;
    int c_downloadStepsCount = 1;
};


#endif // EFFECTS_EXCHANGE_H
