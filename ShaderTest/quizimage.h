#ifndef QUIZIMAGE_H
#define QUIZIMAGE_H


#include <QQuickItem>
#include <QtQuick/QQuickFramebufferObject>
#include "../MastactvaModels/effect.h"
#include "../MastactvaModels/effectargset.h"


class QuizImage : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    QuizImage(QQuickItem *parent_ = nullptr);

    Q_PROPERTY(QString fromImage READ fromImage WRITE setFromImage NOTIFY fromImageChanged)
    Q_PROPERTY(QString toImage READ toImage WRITE setToImage NOTIFY toImageChanged)
    Q_PROPERTY(QString fragmentShaderFilename READ fragmentShaderFilename WRITE setFragmentShaderFilename NOTIFY fragmentShaderFilenameChanged)
    Q_PROPERTY(QString vertexShaderFilename READ vertexShaderFilename WRITE setVertexShaderFilename NOTIFY vertexShaderFilenameChanged)
    Q_PROPERTY(QString fragmentShader READ fragmentShader WRITE setFragmentShader NOTIFY fragmentShaderChanged)
    Q_PROPERTY(QString vertexShader READ vertexShader WRITE setVertexShader NOTIFY vertexShaderChanged)
    Q_PROPERTY(int paintedWidth READ paintedWidth NOTIFY paintedWidthChanged)
    Q_PROPERTY(int paintedHeight READ paintedHeight NOTIFY paintedHeightChanged)
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
    Q_INVOKABLE void updateState();
    Q_INVOKABLE void updateFragmentShader();
    Q_INVOKABLE void updateVertexShader();

public:
    QString fromImage() const;
    void setFromImage(const QString &fromImage_);
    QString toImage() const;
    void setToImage(const QString &toImage_);
    QString fragmentShaderFilename();
    void setFragmentShaderFilename(const QString &fragmentShaderFilename_);
    QString vertexShaderFilename();
    void setVertexShaderFilename(const QString &vertextShaderFilename_);
    QString fragmentShader();
    void setFragmentShader(const QString &fragmentShader_);
    QString vertexShader();
    void setVertexShader(const QString &vertexShader_);
    qreal t() const;
    void setT(qreal t_);
    int paintedWidth() const;
    int paintedHeight() const;
    bool shadersUpdated() const;
    void clearShadersUpdated();

protected:
    QString loadFile(const QString &filename_);
    void saveFile(const QString &filename_, const QString& data_);

public:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

signals:
    void fromImageChanged();
    void toImageChanged();
    void tChanged();
    void paintedWidthChanged();
    void paintedHeightChanged();
    void fragmentShaderFilenameChanged();
    void vertexShaderFilenameChanged();
    void fragmentShaderChanged();
    void vertexShaderChanged();

private:
    QString m_fromImage;
    QString m_toImage;
    QString m_fragmentShaderFilename;
    QString m_fragmentShaderHash;
    QString m_vertexShaderFilename;
    QString m_vertexShaderHash;
    QString m_fragmentShader;
    QString m_vertexShader;
    QImage *m_image = nullptr;
    QSize m_imageSize = QSize(0,0);
    qreal m_t = 0.0;
    bool m_shadersUpdated = false;
};


#endif // QUIZIMAGE_H
