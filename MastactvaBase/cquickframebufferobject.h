#ifndef CQUICKFRAMEBUFFEROBJECT_H
#define CQUICKFRAMEBUFFEROBJECT_H

#include <QtQuick/QQuickItem>

QT_BEGIN_NAMESPACE

class QOpenGLFramebufferObject;
class QQuickFramebufferObjectsPrivate;
class QSGFramebufferObjectsNode;

// ### Qt 6: Consider what to do here. QQuickFbo supports both direct OpenGL and
// OpenGL via QRhi, but it cannot function when running with another rhi backend.

class Q_QUICK_EXPORT QQuickFramebufferObjects : public QQuickItem
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QuickFramebufferObjects)

    Q_PROPERTY(bool textureFollowsItemSize READ textureFollowsItemSize WRITE setTextureFollowsItemSize NOTIFY textureFollowsItemSizeChanged)
    Q_PROPERTY(bool mirrorVertically READ mirrorVertically WRITE setMirrorVertically NOTIFY mirrorVerticallyChanged)

public:

    class Q_QUICK_EXPORT Renderer {
    protected:
        Renderer();
        virtual ~Renderer();
        virtual void render() = 0;
        virtual QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
        virtual void synchronize(QQuickFramebufferObjects *);
        QOpenGLFramebufferObject *framebufferObject() const;
        void update();
        void invalidateFramebufferObject();
    private:
        friend class QSGFramebufferObjectsNode;
        friend class QQuickFramebufferObjects;
        void *data;
    };

    QQuickFramebufferObjects(QQuickItem *parent = nullptr);

    bool textureFollowsItemSize() const;
    void setTextureFollowsItemSize(bool follows);

    bool mirrorVertically() const;
    void setMirrorVertically(bool enable);

    virtual Renderer *createRenderer() const = 0;

    bool isTextureProvider() const override;
    QSGTextureProvider *textureProvider() const override;
    void releaseResources() override;

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;

Q_SIGNALS:
    void textureFollowsItemSizeChanged(bool);
    void mirrorVerticallyChanged(bool);

private Q_SLOTS:
    void invalidateSceneGraph();
};

QT_END_NAMESPACE

#endif // CQUICKFRAMEBUFFEROBJECT_H
