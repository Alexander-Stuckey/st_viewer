/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef IMAGETEXTUREGL_H
#define IMAGETEXTUREGL_H

#include "GraphicItemGL.h"

#include <QFuture>

class QGLPainter;
class QImage;
class QGLTexture2D;
class QByteArray;

class ImageTextureGL : public GraphicItemGL
{
    Q_OBJECT

public:

    explicit ImageTextureGL(QObject *parent = 0);
    virtual ~ImageTextureGL();

    //will split the image into small textures of fixed size
    QFuture<void> createTexture(const QByteArray& imageByteArray);

    //will remove and destroy all textures
    void clearData();

    //return the total size of the image as a QRectF
    const QRectF boundingRect() const override;

signals:

    void imageTexturesCreated();

public slots:

    void setIntensity(qreal intensity);

protected:

    void draw(QGLPainter *painter) override;
    void setSelectionArea(const SelectionEvent *) override;

private:

    //internal functions to create textures from images
    void createTiles(QByteArray imageByteArray);
    void addTexture(const QImage &image, const int x = 0, const int y = 0);

    void clearTextures();
    void clearNodes();

    QList<QGLTexture2D *> m_textures;
    QList<QGLSceneNode *> m_nodes;
    qreal m_intensity;
    QRectF m_bounds;

    Q_DISABLE_COPY(ImageTextureGL)
};

#endif // IMAGETEXTUREGL_H
