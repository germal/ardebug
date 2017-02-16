#ifndef VISUALISER_H
#define VISUALISER_H

#include "datamodel.h"

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include <opencv2/opencv.hpp>

class Visualiser : public QWidget
{
    Q_OBJECT
public:
    Visualiser(QWidget *parent = 0);
    Visualiser(DataModel* dataModelRef);

    QSize minimumSizeHint () const {return QSize(200, 200); }

    void checkFrameSize(void);

public slots:
    void showImage(const cv::Mat& image);

signals:
    void frameSizeChanged(int width, int height);

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);

    QImage _qimage;
    cv::Mat _tmp;
    DataModel* dataModelRef;
};

#endif // VISUALISER_H
