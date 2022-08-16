/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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


class QQuickFramebufferObjectsPrivate //: public QQuickItemPrivate
{
    //Q_DECLARE_PUBLIC(QQuickFramebufferObjects)
public:
    QQuickFramebufferObjectsPrivate()
        : followsItemSize(true)
        , mirrorVertically(false)
        , node(nullptr)
    {
    }

    bool followsItemSize;
    bool mirrorVertically;
    mutable QSGFramebufferMultiObjectsNode *node;
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
