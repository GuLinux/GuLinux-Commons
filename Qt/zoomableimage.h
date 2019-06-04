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

#ifndef ZOOMABLEIMAGE_H
#define ZOOMABLEIMAGE_H

#include <QWidget>
#include <QRect>
#include <memory>
#include "c++/dptr.h"
#ifdef HAVE_QT5_OPENGL
#include <QtOpenGL/QGLWidget>
#endif

class QGraphicsScene;
class QLabel;
class QImage;
class QRubberBand;
class QToolBar;
class ZoomableImage : public QWidget
{
Q_OBJECT
public:

    enum class SelectionMode { None, Rect, Point };

   ~ZoomableImage();
   ZoomableImage(bool embed_toolbar = true, QWidget* parent = 0);

   /// When adding items to the scene, account for the current image zoom level (e.g. by using 'getImgTransform()')
   QGraphicsScene *scene() const;

   /// Returns the current transform used for displaying the image
   /** Meant to be applied to points & rects obtained from 'selectedRect' and 'selectedPoint' signals. */
   const QTransform &getImgTransform() const;

  QToolBar *toolbar() const;
  enum Actions {ZoomIn, ZoomOut, ZoomFit, ZoomRealSize};
  QMap<Actions, QAction*> actions() const;
  void setTransformationMode(Qt::TransformationMode mode);
  double zoomLevel() const;
#ifdef HAVE_QT5_OPENGL
  void setOpenGL();
#endif

public slots:
  void setImage(const QImage &image);

  /// Changes the current image scale by 'factor'
  void scale(double factor);

  void absoluteScale(double factor);
  void fitToWindow();
  void normalSize();
  void startSelectionMode(SelectionMode mode);

protected:
    virtual void resizeEvent(QResizeEvent * e);
signals:
  void selectedRect(const QRectF &); ///< Passes a rectangle in image space
  void zoomLevelChanged(double);
  void selectedPoint(const QPointF &); ///< Passes a point in image space
private:
  DPTR
};

#endif // ZOOMABLEIMAGE_H

