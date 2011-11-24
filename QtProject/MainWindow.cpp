/*
 * The information in this file is
 * Copyright (C) 2010-2011, Sven De Smet <sven@cubiccarrot.com>
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from
 * http://www.gnu.org/licenses/lgpl.html
 */


#include "MainWindow.h"

#include "../Convolution.h"

#include <QLabel>

#include "../tests/Test.h"

class QImageData : public Data2D<float> {
private:
    QImage* image;
    int band;
public:
    QImageData(QImage* iImage, int iBand) : image(iImage), band(iBand) { }

    int getWidth() { return image->width(); }
    int getHeight() { return image->height(); }

    float getPixel(int x, int y) { return (image->pixel(x, y) >> (8*band)) & 0xFF; }

    void setPixel(int x, int y, float value) { //qDebug("[%i]", (int) value);
        int c = image->pixel(x, y) & ~(0xFF << (8*band));
        int b = ((int) value << (8*band));

        image->setPixel(x, y, b | c);
    }
};


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    performTests();
    //QApplication::instance()->exit();

    return;

    std::string n = "../QtProject/";
    n = "/Volumes/ITACHI/conv2D_opticks/QtProject/";
    QImage imageToConvolve = QImage((n + "testImages/itc-1.png").c_str());
    QImage imgConvolutionKernel = QImage((n + "testImages/ck-2.png").c_str());
    QImage imgResult = QImage(imageToConvolve.width(), imageToConvolve.height(), QImage::Format_RGB32);

    qDebug("%ix%i", imageToConvolve.width(), imageToConvolve.height());
    qDebug("%ix%i", imgConvolutionKernel.width(), imgConvolutionKernel.height());

    qDebug("Complex<float> size = %i", (int) sizeof(Complex<float>));

    for (int b = 0; b < 3; ++b) {
        QImageData dataToConvolve(&imageToConvolve, b);
        QImageData convolutionKernel(&imgConvolutionKernel, b);
        QImageData result(&imgResult, b);

        Convolution2D_FFT<float> convolution(&dataToConvolve, &convolutionKernel, &result);
        convolution.convolve();
    }
/*
    QPicture *picture = new QPicture(imgResult);
    QLabel *label = new QLabel(this);
    label->setPicture(imgResult);
    setCentralWidget(label);*/
    qDebug("done");
//    imageToConvolve.save((n + "testImages/result-1-1b.png").c_str());
    imgResult.save((n + "testImages/result-1-1.png").c_str());
    qDebug("saved");
}

MainWindow::~MainWindow()
{

}
