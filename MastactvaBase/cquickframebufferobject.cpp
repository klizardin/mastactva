#include "cquickframebufferobject.h"

#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOpenGLFunctions>
#include <QWindow>
#include <QQuickWindow>
#include <QThread>
#include <QQuickItem>
#include <qsgtextureprovider.h>

#include <QSGSimpleTextureNode>
#include <QSGRendererInterface>

QT_BEGIN_NAMESPACE

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
    mutable QSGFramebufferObjectsNode *node;
};

QQuickFramebufferObjects::QQuickFramebufferObjects(QQuickItem *parent) :
    QQuickItem(/* *new QQuickFramebufferObjectsPrivate,*/ parent)
{
    setFlag(ItemHasContents);
}

void QQuickFramebufferObjects::setTextureFollowsItemSize(bool follows)
{
    Q_D(QQuickFramebufferObjects);
    if (d->followsItemSize == follows)
        return;
    d->followsItemSize = follows;
    emit textureFollowsItemSizeChanged(d->followsItemSize);
}

bool QQuickFramebufferObjects::textureFollowsItemSize() const
{
    Q_D(const QQuickFramebufferObjects);
    return d->followsItemSize;
}

void QQuickFramebufferObjects::setMirrorVertically(bool enable)
{
    Q_D(QQuickFramebufferObjects);
    if (d->mirrorVertically == enable)
        return;
    d->mirrorVertically = enable;
    emit mirrorVerticallyChanged(d->mirrorVertically);
    update();
}

bool QQuickFramebufferObjects::mirrorVertically() const
{
    Q_D(const QQuickFramebufferObjects);
    return d->mirrorVertically;
}

void QQuickFramebufferObjects::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);

    Q_D(QQuickFramebufferObjects);
    if (newGeometry.size() != oldGeometry.size() && d->followsItemSize)
        update();
}

class QSGFramebufferObjectsNode : public QSGTextureProvider, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    QSGFramebufferObjectsNode()
        : window(nullptr)
        , fbo(nullptr)
        , msDisplayFbo(nullptr)
        , renderer(nullptr)
        , renderPending(true)
        , invalidatePending(false)
        , devicePixelRatio(1)
    {
        qsgnode_set_description(this, QStringLiteral("fbonode"));
    }

    ~QSGFramebufferObjectsNode()
    {
        delete renderer;
        delete texture();
        delete fbo;
        delete msDisplayFbo;
    }

    void scheduleRender()
    {
        renderPending = true;
        window->update();
    }

    QSGTexture *texture() const override
    {
        return QSGSimpleTextureNode::texture();
    }

public Q_SLOTS:
    void render()
    {
        if (renderPending) {
            renderPending = false;

            const bool needsWrap = QSGRendererInterface::isApiRhiBased(window->rendererInterface()->graphicsApi());
            if (needsWrap) {
                window->beginExternalCommands();
                window->resetOpenGLState();
            }

            fbo->bind();
            QOpenGLContext::currentContext()->functions()->glViewport(0, 0, fbo->width(), fbo->height());
            renderer->render();
            fbo->bindDefault();

            if (msDisplayFbo)
                QOpenGLFramebufferObject::blitFramebuffer(msDisplayFbo, fbo);

            if (needsWrap)
                window->endExternalCommands();

            markDirty(QSGNode::DirtyMaterial);
            emit textureChanged();
        }
    }

    void handleScreenChange()
    {
        if (window->effectiveDevicePixelRatio() != devicePixelRatio) {
            renderer->invalidateFramebufferObject();
            quickFbo->update();
        }
    }

public:
    QQuickWindow *window;
    QOpenGLFramebufferObject *fbo;
    QOpenGLFramebufferObject *msDisplayFbo;
    QQuickFramebufferObjects::Renderer *renderer;
    QQuickFramebufferObjects *quickFbo;

    bool renderPending;
    bool invalidatePending;

    qreal devicePixelRatio;
};

static inline bool isOpenGL(QSGRenderContext *rc)
{
    QSGRendererInterface *rif = rc->sceneGraphContext()->rendererInterface(rc);
    return rif && (rif->graphicsApi() == QSGRendererInterface::OpenGL
                   || rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);
}

QSGNode *QQuickFramebufferObjects::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
{
    QSGFramebufferObjectsNode *n = static_cast<QSGFramebufferObjectsNode *>(node);

    // We only abort if we never had a node before. This is so that we
    // don't recreate the renderer object if the thing becomes tiny. In
    // terms of API it would be horrible if the renderer would go away
    // that easily so with this logic, the renderer only goes away when
    // the scenegraph is invalidated or it is removed from the scene.
    if (!n && (width() <= 0 || height() <= 0))
        return nullptr;

    Q_D(QQuickFramebufferObjects);

    if (!n) {
        if (!isOpenGL(d->sceneGraphRenderContext()))
            return nullptr;
        if (!d->node)
            d->node = new QSGFramebufferObjectsNode;
        n = d->node;
    }

    if (!n->renderer) {
        n->window = window();
        n->renderer = createRenderer();
        n->renderer->data = n;
        n->quickFbo = this;
        connect(window(), SIGNAL(beforeRendering()), n, SLOT(render()));
        connect(window(), SIGNAL(screenChanged(QScreen*)), n, SLOT(handleScreenChange()));
    }

    n->renderer->synchronize(this);

    QSize minFboSize = d->sceneGraphContext()->minimumFBOSize();
    QSize desiredFboSize(qMax<int>(minFboSize.width(), width()),
                         qMax<int>(minFboSize.height(), height()));

    n->devicePixelRatio = window()->effectiveDevicePixelRatio();
    desiredFboSize *= n->devicePixelRatio;

    if (n->fbo && ((d->followsItemSize && n->fbo->size() != desiredFboSize) || n->invalidatePending)) {
        delete n->texture();
        delete n->fbo;
        n->fbo = nullptr;
        delete n->msDisplayFbo;
        n->msDisplayFbo = nullptr;
        n->invalidatePending = false;
    }

    if (!n->fbo) {
        n->fbo = n->renderer->createFramebufferObject(desiredFboSize);

        GLuint displayTexture = n->fbo->texture();

        if (n->fbo->format().samples() > 0) {
            n->msDisplayFbo = new QOpenGLFramebufferObject(n->fbo->size());
            displayTexture = n->msDisplayFbo->texture();
        }

        QSGTexture *wrapper = window()->createTextureFromNativeObject(QQuickWindow::NativeObjectTexture,
                                                                      &displayTexture, 0,
                                                                      n->fbo->size(),
                                                                      QQuickWindow::TextureHasAlphaChannel);
        n->setTexture(wrapper);
    }

    n->setTextureCoordinatesTransform(d->mirrorVertically ? QSGSimpleTextureNode::MirrorVertically : QSGSimpleTextureNode::NoTransform);
    n->setFiltering(d->smooth ? QSGTexture::Linear : QSGTexture::Nearest);
    n->setRect(0, 0, width(), height());

    n->scheduleRender();

    return n;
}

bool QQuickFramebufferObjects::isTextureProvider() const
{
    return true;
}

QSGTextureProvider *QQuickFramebufferObjects::textureProvider() const
{
    // When Item::layer::enabled == true, QQuickItem will be a texture
    // provider. In this case we should prefer to return the layer rather
    // than the fbo texture.
    if (QQuickItem::isTextureProvider())
        return QQuickItem::textureProvider();

    Q_D(const QQuickFramebufferObjects);
    QQuickWindow *w = window();
    if (!w || !w->openglContext() || QThread::currentThread() != w->openglContext()->thread()) {
        qWarning("QQuickFramebufferObjects::textureProvider: can only be queried on the rendering thread of an exposed window");
        return nullptr;
    }
    if (!isOpenGL(d->sceneGraphRenderContext()))
        return nullptr;
    if (!d->node)
        d->node = new QSGFramebufferObjectsNode;
    return d->node;
}

void QQuickFramebufferObjects::releaseResources()
{
    // When release resources is called on the GUI thread, we only need to
    // forget about the node. Since it is the node we returned from updatePaintNode
    // it will be managed by the scene graph.
    Q_D(QQuickFramebufferObjects);
    d->node = nullptr;
}

void QQuickFramebufferObjects::invalidateSceneGraph()
{
    Q_D(QQuickFramebufferObjects);
    d->node = nullptr;
}

QQuickFramebufferObjects::Renderer::Renderer()
    : data(nullptr)
{
}

QQuickFramebufferObjects::Renderer::~Renderer()
{
}

QOpenGLFramebufferObject *QQuickFramebufferObjects::Renderer::framebufferObject() const
{
    return data ? ((QSGFramebufferObjectsNode *) data)->fbo : nullptr;
}

void QQuickFramebufferObjects::Renderer::synchronize(QQuickFramebufferObjects *item)
{
    Q_UNUSED(item);
}

void QQuickFramebufferObjects::Renderer::invalidateFramebufferObject()
{
    if (data)
        ((QSGFramebufferObjectsNode *) data)->invalidatePending = true;
}

QOpenGLFramebufferObject *QQuickFramebufferObjects::Renderer::createFramebufferObject(const QSize &size)
{
    return new QOpenGLFramebufferObject(size);
}

void QQuickFramebufferObjects::Renderer::update()
{
    if (data)
        ((QSGFramebufferObjectsNode *) data)->scheduleRender();
}

QT_END_NAMESPACE
