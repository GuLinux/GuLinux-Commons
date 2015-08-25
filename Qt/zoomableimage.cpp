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

using namespace std;

class ZoomableImage::Private {
public:
  class GraphicsView : public QGraphicsView {
  public:
    GraphicsView(QWidget *parent = 0);
    enum {FreeZoom, RealSize, FitToWindow} zoomMode = RealSize;
    bool selectionMode = false;
    QRectF selectionRect;
    QGraphicsRectItem *selection = 0;
  protected:
    virtual void mouseReleaseEvent(QMouseEvent*e);
  };
  Private(ZoomableImage *q) : q(q) {}
  GraphicsView *view;
  ZoomableImage *q;
  QGraphicsScene scene;
  QRect imageSize;
    QToolBar* toolbar;
  
};

ZoomableImage::Private::GraphicsView::GraphicsView(QWidget* parent) : QGraphicsView(parent)
{
}


ZoomableImage::~ZoomableImage()
{

}

ZoomableImage::ZoomableImage(bool embed_toolbar, QWidget* parent) : QWidget(parent), d(new Private{this})
{
  setLayout(new QGridLayout(this));
  layout()->setSpacing(0);
  d->toolbar = new QToolBar(tr("Image Controls"));
  if(embed_toolbar)
    layout()->addWidget(d->toolbar);
  connect(d->toolbar->addAction(QIcon::fromTheme("zoom-in"), tr("Zoom In")), &QAction::triggered, bind(&ZoomableImage::scale, this, 1.2));
  connect(d->toolbar->addAction(QIcon::fromTheme("zoom-out"), tr("Zoom Out")), &QAction::triggered, bind(&ZoomableImage::scale, this, 0.8));
  d->toolbar->addSeparator();
  connect(d->toolbar->addAction(QIcon::fromTheme("zoom-fit-best"), tr("Fit Window")), &QAction::triggered, bind(&ZoomableImage::fitToWindow, this));
  connect(d->toolbar->addAction(QIcon::fromTheme("zoom-original"), tr("Original Size")), &QAction::triggered, bind(&ZoomableImage::normalSize, this));
  d->toolbar->setEnabled(false);
  layout()->addWidget(d->view = new Private::GraphicsView(this));
  d->view->setScene(&d->scene);
  d->view->setDragMode(QGraphicsView::ScrollHandDrag);
  connect(d->view, &QGraphicsView::rubberBandChanged, [=](QRect a,const QPointF &sceneStart, const QPointF &sceneEnd){
    if(!d->view->selectionMode || a.isEmpty())
      return;
    d->view->selectionRect = QRectF(sceneStart, sceneEnd);
  });
}

void ZoomableImage::fitToWindow()
{
  d->view->zoomMode = Private::GraphicsView::FitToWindow;
  d->view->fitInView(d->imageSize, Qt::KeepAspectRatio);
}




void ZoomableImage::normalSize()
{
  d->view->zoomMode = Private::GraphicsView::RealSize;
  d->view->setTransform({});
}



void ZoomableImage::scale(double factor)
{
  d->view->zoomMode = Private::Private::GraphicsView::FreeZoom;
  d->view->scale(factor, factor);
}

QToolBar* ZoomableImage::toolbar() const
{
  return d->toolbar;
}





void ZoomableImage::startSelectionMode()
{
  clearROI();
  d->view->selectionMode = true;
  d->view->setDragMode(QGraphicsView::RubberBandDrag);
}

void ZoomableImage::Private::GraphicsView::mouseReleaseEvent(QMouseEvent* e)
{
  if(selectionMode) {
    selectionMode = false;
    selection = scene()->addRect(selectionRect, {Qt::green}, {QColor{0, 250, 250, 20}});
    qDebug() << "rect: " << selectionRect << ", " << selection->rect();
  }
  QGraphicsView::mouseReleaseEvent(e);
  setDragMode(QGraphicsView::ScrollHandDrag);
}



void ZoomableImage::setImage(const QImage& image)
{
  d->toolbar->setEnabled(!image.isNull());
  if(d->view->selection)
    d->scene.removeItem(d->view->selection);
  d->scene.clear();
  if(image.isNull())
    return;
  d->imageSize = image.rect();
  d->scene.addPixmap(QPixmap::fromImage(image));
  if(d->view->selection)
    d->scene.addItem(d->view->selection);
}

QRect ZoomableImage::roi() const
{
  return d->view->selectionRect.toRect();
}

void ZoomableImage::clearROI()
{
  d->scene.removeItem(d->view->selection);
  delete d->view->selection;
  d->view->selection = 0;
  d->view->selectionRect = {};
}

void ZoomableImage::resizeEvent(QResizeEvent* e)
{
  if(d->view->zoomMode == Private::GraphicsView::FitToWindow)
    fitToWindow();
  QWidget::resizeEvent(e);
}
