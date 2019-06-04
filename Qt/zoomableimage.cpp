/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  Marco Gulino <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "zoomableimage.h"
#include <QLabel>
#include <QImage>
#include <QDragMoveEvent>
#include <QDrag>
#include <QPaintEngine>
#include <QScrollBar>
#include <QApplication>
#include <QDebug>
#include <QMimeData>
#include <QRubberBand>
#include <QGraphicsRectItem>
#include <QGridLayout>
#include <QGraphicsView>
#include <QToolBar>
#include <QIcon>
#include <QAction>
#include <functional>

using namespace std;

class ZoomableImage::Private {
public:
  class GraphicsView : public QGraphicsView {
  public:
    GraphicsView(ZoomableImage *q, QWidget *parent = nullptr);
    enum ZoomMode {FreeZoom, RealSize, FitToWindow} zoomMode = RealSize;
    SelectionMode selectionMode = SelectionMode::None;

    /// Selection rectangle in image space
    QRectF selectionRect;

  protected:
    virtual void mouseReleaseEvent(QMouseEvent*e);

  private:
    ZoomableImage *q;
  };
  Private(ZoomableImage *q) : q(q) {}
  GraphicsView *view;
  ZoomableImage *q;
  QGraphicsScene scene;
  QToolBar* toolbar;
  QMap<Actions, QAction*> actions;
  Qt::TransformationMode transformation_mode = Qt::SmoothTransformation;
  double current_zoom() const;
  void set_zoom_level(double ratio, GraphicsView::ZoomMode zoom_mode);
  QGraphicsPixmapItem *imageItem = nullptr;
  QTransform imgTransform;

    /// Updates scene rectangle to reflect the current image zoom level
    void updateSceneRect();
};


void ZoomableImage::Private::updateSceneRect()
{
    const auto imgDisplaySize = imgTransform.mapRect(imageItem->boundingRect()).size();
    scene.setSceneRect(0, 0, imgDisplaySize.width(), imgDisplaySize.height());
}


ZoomableImage::Private::GraphicsView::GraphicsView(ZoomableImage *q, QWidget* parent) : QGraphicsView(parent), q{q}
{
}


ZoomableImage::~ZoomableImage()
{

}

ZoomableImage::ZoomableImage(bool embed_toolbar, QWidget* parent) : QWidget(parent), dptr(this)
{
  setLayout(new QGridLayout(this));
  layout()->setSpacing(0);
  d->toolbar = new QToolBar(tr("Image Controls"));
  d->toolbar->setObjectName("zoomable-image-controls");
  if(embed_toolbar)
    layout()->addWidget(d->toolbar);
  auto zoomInAction = d->toolbar->addAction(QIcon::fromTheme("zoom-in"), tr("Zoom In"));
  auto zoomOutAction =d->toolbar->addAction(QIcon::fromTheme("zoom-out"), tr("Zoom Out"));
  auto zoomBestFitAction = d->toolbar->addAction(QIcon::fromTheme("zoom-fit-best"), tr("Fit Window"));
  auto zoomOriginalAction =d->toolbar->addAction(QIcon::fromTheme("zoom-original"), tr("Original Size"));
  d->actions = {{ZoomIn, zoomInAction}, {ZoomOut, zoomOutAction}, {ZoomFit, zoomBestFitAction}, {ZoomRealSize, zoomOriginalAction}};
  connect(zoomInAction, &QAction::triggered, bind(&ZoomableImage::scale, this, 1.2));
  connect(zoomOutAction, &QAction::triggered, bind(&ZoomableImage::scale, this, 0.8));
  connect(zoomBestFitAction, &QAction::triggered, bind(&ZoomableImage::fitToWindow, this));
  connect(zoomOriginalAction, &QAction::triggered, bind(&ZoomableImage::normalSize, this));
  d->toolbar->setEnabled(false);
  layout()->addWidget(d->view = new Private::GraphicsView(this, this));
  d->view->setScene(&d->scene);
  d->view->setDragMode(QGraphicsView::ScrollHandDrag);
  connect(d->view, &QGraphicsView::rubberBandChanged, [=](QRect a,const QPointF &sceneStart, const QPointF &sceneEnd){
    if(d->view->selectionMode != SelectionMode::Rect || a.isEmpty())
      return;
    const QTransform imgInvT = d->imgTransform.inverted();
    d->view->selectionRect = QRectF(imgInvT.map(sceneStart),
                                    imgInvT.map(sceneEnd));
  });
  d->view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform );
}

void ZoomableImage::fitToWindow()
{
  d->view->zoomMode = Private::GraphicsView::FitToWindow;

  auto viewRect = d->view->rect();

  // Adjustment as performed in QGraphicsView::fitInView(), to make sure it really fits
  constexpr int MARGIN = 2;
  viewRect.adjust(MARGIN, MARGIN, -MARGIN, -MARGIN);

  const double scaleFitWidth = (double)viewRect.width() / d->imageItem->boundingRect().width();
  const double scaleFitHeight = (double)viewRect.height() / d->imageItem->boundingRect().height();
  const double scale = std::min(scaleFitWidth, scaleFitHeight);

  d->imgTransform = QTransform{ scale, 0, 0, scale, 0, 0 };
  d->imageItem->setTransform(d->imgTransform);

  d->updateSceneRect();

  emit zoomLevelChanged(zoomLevel());
}


double ZoomableImage::zoomLevel() const
{
    return d->current_zoom();
}


void ZoomableImage::normalSize()
{
  d->set_zoom_level(1, Private::GraphicsView::RealSize);;
}



void ZoomableImage::scale(double factor)
{
  d->set_zoom_level(factor * d->current_zoom(), Private::GraphicsView::FreeZoom);
}

void ZoomableImage::absoluteScale(double factor)
{
  d->set_zoom_level(factor, Private::GraphicsView::FreeZoom);
}


QToolBar* ZoomableImage::toolbar() const
{
  return d->toolbar;
}


void ZoomableImage::startSelectionMode(SelectionMode mode)
{
    d->view->selectionMode = mode;
    switch (mode)
    {
    case SelectionMode::Rect:
        d->view->setDragMode(QGraphicsView::RubberBandDrag);
        break;

    case SelectionMode::Point:
        d->view->setCursor(QCursor{Qt::CrossCursor});
        d->view->setDragMode(QGraphicsView::NoDrag);
        break;
    }
}

void ZoomableImage::Private::GraphicsView::mouseReleaseEvent(QMouseEvent* e)
{
    switch (selectionMode)
    {
    case SelectionMode::Rect:
        selectionMode = SelectionMode::None;
        qDebug() << "rect: " << selectionRect;
        q->selectedRect(selectionRect);
        break;

    case SelectionMode::Point:
        selectionMode = SelectionMode::None;
        const QPointF imgPoint = q->d->imgTransform.inverted().map(mapToScene(e->pos()));
        qDebug() << "point: " << imgPoint;
        q->selectedPoint(imgPoint);
        break;
    }

    QGraphicsView::mouseReleaseEvent(e);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void ZoomableImage::setImage(const QImage& image)
{
  d->toolbar->setEnabled(!image.isNull());
  if(d->imageItem) {
    d->scene.removeItem(d->imageItem);
    delete d->imageItem;
  }
  if(image.isNull()) {
    d->imageItem = nullptr;
    return;
  }

  d->imageItem = d->scene.addPixmap(QPixmap::fromImage(image));
  d->imageItem->setTransformationMode(d->transformation_mode);
  d->imageItem->setTransform(d->imgTransform);
  d->updateSceneRect();
  d->imageItem->setZValue(0);
}

void ZoomableImage::resizeEvent(QResizeEvent* e)
{
  if(d->view->zoomMode == Private::GraphicsView::FitToWindow)
    fitToWindow();
  QWidget::resizeEvent(e);
}

QGraphicsScene* ZoomableImage::scene() const
{
  return &d->scene;
}


const QTransform &ZoomableImage::getImgTransform() const
{
    return d->imgTransform;
}


QMap< ZoomableImage::Actions, QAction* > ZoomableImage::actions() const
{
  return d->actions;
}

double ZoomableImage::Private::current_zoom() const
{
  return std::max(imgTransform.m11(), imgTransform.m21());
}

void ZoomableImage::Private::set_zoom_level(double ratio, GraphicsView::ZoomMode zoom_mode)
{
  view->zoomMode = zoom_mode;
  imgTransform = QTransform{ ratio, 0, 0, ratio, 0, 0 };
  if (imageItem)
  {
      imageItem->setTransform(imgTransform);
      updateSceneRect();
  }

  emit q->zoomLevelChanged(q->zoomLevel());
}

void ZoomableImage::setTransformationMode(Qt::TransformationMode mode)
{
  d->transformation_mode = mode;
}

#ifdef HAVE_QT5_OPENGL
void ZoomableImage::setOpenGL() {
  d->view->setViewport(new QGLWidget);
}
#endif
