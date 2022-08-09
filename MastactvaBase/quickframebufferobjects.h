#ifndef QUICKFRAMEBUFFEROBJECTS_H
#define QUICKFRAMEBUFFEROBJECTS_H

#include <QtQuick/QQuickItem>
#include <QSGTextureProvider>
#include <QSGSimpleTextureNode>


class QOpenGLFramebufferObject;
class QQuickFramebufferObjectsPrivate;
class QQuickFramebufferObjects;
class QQuickFramebufferObjectsRenderer;

class QSGFramebufferMultiObjectsNode : public QSGTextureProvider, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    QSGFramebufferMultiObjectsNode();
    ~QSGFramebufferMultiObjectsNode() override;

    void scheduleRender();
    QSGTexture *texture() const override;

public Q_SLOTS:
    void render();
    void handleScreenChange();

public:
    QQuickWindow *window;
    QOpenGLFramebufferObject *fbo;
    QOpenGLFramebufferObject *msDisplayFbo;
    QQuickFramebufferObjectsRenderer *renderer;
    QQuickFramebufferObjects *quickFbo;

    bool renderPending;
    bool invalidatePending;

    qreal devicePixelRatio;
};


// ### Qt 6: Consider what to do here. QQuickFbo supports both direct OpenGL and
// OpenGL via QRhi, but it cannot function when running with another rhi backend.

class Q_QUICK_EXPORT QQuickFramebufferObjectsRenderer {
protected:
    QQuickFramebufferObjectsRenderer();
    virtual ~QQuickFramebufferObjectsRenderer();
    virtual void render() = 0;
    virtual QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    virtual void synchronize(QQuickFramebufferObjects *);
    QOpenGLFramebufferObject *framebufferObject() const;
    void update();
    void invalidateFramebufferObject();
private:
    friend class QSGFramebufferMultiObjectsNode;
    friend class QQuickFramebufferObjects;
    friend class QQuickFramebufferObjects;
    void *data;
};

class Q_QUICK_EXPORT QQuickFramebufferObjects : public QQuickItem
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QQuickFramebufferObjects)

    Q_PROPERTY(bool textureFollowsItemSize READ textureFollowsItemSize WRITE setTextureFollowsItemSize NOTIFY textureFollowsItemSizeChanged)
    Q_PROPERTY(bool mirrorVertically READ mirrorVertically WRITE setMirrorVertically NOTIFY mirrorVerticallyChanged)

public:

    QQuickFramebufferObjects(QQuickItem *parent = nullptr);

    bool textureFollowsItemSize() const;
    void setTextureFollowsItemSize(bool follows);

    bool mirrorVertically() const;
    void setMirrorVertically(bool enable);

    virtual QQuickFramebufferObjectsRenderer *createRenderer() const = 0;

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


#endif // QUICKFRAMEBUFFEROBJECTS_H
