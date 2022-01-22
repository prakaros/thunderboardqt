#include "dataplotter.hpp"
#include <QtCore\QDebug>
#include <QtCore\QPoint>
#include <QtWidgets\QGraphicsView>
#include <QtWidgets\QGraphicsItem>
#include <QtWidgets\QGraphicsScene>
#include <QtWidgets\QGraphicsSceneMouseEvent>
#include <QtWidgets\QToolTip>
#include <QtCore\QMath.h>
#include <algorithm>
#include "capturethread.hpp"

namespace sensemon
{

    DataPlotterScene *DataPlotterScene::mDataPlotterInstanceScene = nullptr;

    DataPlotterScene::DataPlotterScene(QObject *parent) :
         mGraphicView(0)
        , mSceneWidth(0)
        , mSceneHeight(0)
        , mXAxis(0)
        , mYAxis(0)
        , mMinSignalValue(0)
        , mMaxSignalValue(0)
        , mScaleFactor(1.0)
        , mXCoord(4)
        , mSavedSignalVec(6, std::vector<double>(1000))
        , mOffset(0)
    {
        qRegisterMetaType<sensemon::VectorPlot>("sensemon::VectorPlot");
    }

    DataPlotterScene::~DataPlotterScene()
    {

    }

    void DataPlotterScene::Initialize(QGraphicsView *qview, QGraphicsView *xqview, QGraphicsView *yqview)
    {
        mGraphicView = qview;

        mSceneWidth = mGraphicView->rect().width();
        mSceneHeight = mGraphicView->rect().height();

        qDebug() << "Scene Width " << mSceneWidth;
        qDebug() << "Scene Height " << mSceneHeight;

        mGraphicView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        mGraphicView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        mCurveRectWidth = 1;

        this->setSceneRect(0, -mGraphicView->rect().height(), mGraphicView->rect().width(), mGraphicView->rect().height());
        mGraphicView->setScene(this);
        clearPlot();

        mXAxis = new DrawAxis(true, gridSize);
        mXAxis->Initialize(xqview);


        mYAxis = new DrawAxis(false, gridSize);
        mYAxis->Initialize(yqview);

        mCursor = new QCursor();
        mGraphicView->setCursor(*mCursor);
        mGraphicView->setMouseTracking(true);


        mColorVec.push_back(Qt::red);
        mColorVec.push_back(Qt::blue);
        mColorVec.push_back(Qt::green);
        mColorVec.push_back(Qt::cyan);
        mColorVec.push_back(Qt::magenta);
        mColorVec.push_back(Qt::yellow);
    }

    DataPlotterScene*DataPlotterScene::getDataPlotterSceneInstance()
    {
        if (mDataPlotterInstanceScene == nullptr)
        {
            mDataPlotterInstanceScene = new DataPlotterScene();
        }
        return mDataPlotterInstanceScene;
    }


    void DataPlotterScene::plotData(int data)
    {
        static int x_coord = 4;
        static int y_prev = 0;
        static int y_offsetPrev = 0;
        static int offset = 300;

        qDebug() << "Plot Data X =" << x_coord << "Y= " << data;
#if 0

        mGraphicsScene->addRect(mCurveRectWidth*x_coord, -mCurveRectWidth*data, mCurveRectWidth, mCurveRectWidth, QPen(Qt::black), QBrush(Qt::blue));
#endif
        this->addLine(x_coord, -y_prev, x_coord + 1, -data, QPen(Qt::red, 1, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));

        this->addLine(x_coord, -y_offsetPrev, x_coord + 1, -(data + offset), QPen(Qt::blue, 1, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));


        y_prev = data;
        y_offsetPrev = data + offset;
        x_coord++;

        if (x_coord > ((mSceneWidth / mCurveRectWidth) - 2))
        {
            qDebug() << "Clear Scence ";
            clearPlot();
        }
    }

    void DataPlotterScene::plotsignaldata(const VectorPlot& plotvec)
    {
       // static int x_coord = mXCoordInitial;
        static int y_prev = 0;
        static int y_offsetPrev = 0;
        static int offset = 300;
        double y_current = 0;
        static double signalAverage = 0;
        static int cAvg = 0;
      //  qDebug() << "Plot Data0 X =" << x_coord << "Y= " << plotvec[0].signalvalue;
      //  qDebug() << "Plot Data1 X =" << x_coord << "Y= " << plotvec[1].signalvalue;
 //      qDebug() << "Plot Data2 X =" << x_coord << "Y= " << plotvec[2].signalvalue;
        double originalSignal = 0;
        double maxSignal = 0;
        for (size_t i = 0; i < plotvec.size(); i++)
        {
            if (plotvec[i].plotsignal)
            {

                if (plotvec[i].signalvalue >= 0)
                {
                    y_current = plotvec[i].signalvalue*mScaleFactor + mSceneHeight*(1 - mHeighRatio) + mOffset;// +3);

                }
                else
                {
                    y_current = (plotvec[i].signalvalue*mScaleFactor + mSceneHeight*mHeighRatio + mOffset); // +3;

                }

          //    y_current = (plotvec[i].signalvalue);
                qDebug() << "Plot Data " << i << " X =" << mXCoord << "Act= " << plotvec[i].signalvalue << "Y_Cur= "<<y_current << "ScaleF=" <<mScaleFactor;
                originalSignal = plotvec[i].signalvalue;
                maxSignal = std::fmax(originalSignal, maxSignal);
                this->addLine(mXCoord, -mVectPlotData[i].signalvalue, mXCoord + 1, -y_current, QPen(mColorVec[i], 2, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));

            }
            else
            {
                y_current = 0;
            }

       //     this->addLine(mXCoord, -mVectPlotData[i].signalvalue, mXCoord + 1, -y_current, QPen(mColorVec[i], 2, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
            mVectPlotData[i].signalvalue = y_current;
            mSavedSignalVec[i][mXCoord] = originalSignal;
        }
        if (cAvg < 10)
        {
            cAvg++;
            signalAverage += maxSignal;
        }
        else
        {
            signalAverage = signalAverage / 10;
            if ((mVarMaxSignalValue - signalAverage) > 200)
            {
                mVarMaxSignalValue = signalAverage + 200;
                mScaleFactor = mSceneHeight / mVarMaxSignalValue;

                mYAxis->setAxisLimit(mMinSignalValue, mVarMaxSignalValue);
                mYAxis->setScaleFactor(mScaleFactor);
                mYAxis->update();
                
            }
            else if((signalAverage - mVarMaxSignalValue) > 100)
            {
                mVarMaxSignalValue = signalAverage + 100;
                mScaleFactor = mSceneHeight / (mVarMaxSignalValue);

                mYAxis->setAxisLimit(mVarMinSignalValue, mVarMaxSignalValue);
                mYAxis->setScaleFactor(mScaleFactor);
                mYAxis->update();
            }
            signalAverage = 0;
            cAvg = 0;
        }
#if 0
        if (plotvec[0].plotsignal)
        {
            this->addLine(x_coord, -y_prev, x_coord + 1, -plotvec[0].signalvalue, QPen(Qt::red, 1, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
            y_prev = plotvec[0].signalvalue;
        }
        else
        {
            y_prev = 0;
        }

        if (plotvec[1].plotsignal)
        {
            this->addLine(x_coord, -y_offsetPrev, x_coord + 1, -plotvec[1].signalvalue, QPen(Qt::blue, 1, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
            y_offsetPrev = plotvec[1].signalvalue;
        }
        else
        {
            y_offsetPrev = 0;
        }
#endif

        mXCoord++;

        if (mXCoord > ((mSceneWidth / mCurveRectWidth) - 1))
        {
            clearPlot();
            int minxlimit = 0, maxxlimit = 0;
            mXAxis->getAxisLimit(minxlimit, maxxlimit);
            mXAxis->setAxisLimit(maxxlimit, 2 * maxxlimit);
        }
    
    }
    void DataPlotterScene::drawBackground(QPainter *painter, const QRectF &rect)
    {
       

        qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
        qreal top = int(rect.top()) - (int(rect.top()) % gridSize);
        
        qDebug() << "Rect Left " << rect.left();
        qDebug() << "Rect Top " << rect.top();

        qDebug() << "Adjusted Left " << left;
        qDebug() << "Adjusted Top " << top;

        QVarLengthArray<QLineF, 100> lines;

        for (qreal x = left; x < rect.right(); x += gridSize)
        {
            lines.append(QLineF(x, rect.top(), x, rect.bottom()));
        }

    //    qDebug() << lines.size();
        painter->setPen(QPen(Qt::white, 0, Qt::DotLine));
        painter->drawLines(lines.data(), lines.size());
        lines.clear();
        for (qreal y = top; y < rect.bottom(); y += gridSize)
        {
            lines.append(QLineF(rect.left(), y, rect.right(), y));
        }

   //     qDebug() << lines.size();
        painter->setPen(QPen(Qt::white, 0, Qt::DotLine));
        painter->drawLines(lines.data(), lines.size());
    }




    DrawAxis::DrawAxis(bool axis, int grids) :
        mGraphicsView(0)
        , mSceneWidth(0)
        , mSceneHeight(0)
        , mAxisMin(0)
        , mAxisMax(100)
        , mDrawXAxis(axis)
        , mGridSize(grids)
        , mScaleFactor(1.0)
    {
    
    }

    void DrawAxis::Initialize(QGraphicsView *graphicsView)
    {
        mGraphicsView = graphicsView;
        mSceneWidth = mGraphicsView->rect().width();
        mSceneHeight = mGraphicsView->rect().height();
        mGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        mGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //    mGraphicsView->setHidden(true);
        qDebug() << "Axis Scene Height " << mSceneHeight << "Axis Scene Width " << mSceneWidth;

        this->setSceneRect(0, -mGraphicsView->rect().height(), mGraphicsView->rect().width(), mGraphicsView->rect().height());
        mGraphicsView->setScene(this);
    }

    void DrawAxis::drawBackground(QPainter *painter, const QRectF &rect)
    {
        const int smallSpace = 2;
        const int largeSpace = 5; //6;
        const int grideSize = 5;

        QVarLengthArray<QLineF, 100> lines;
        QVarLengthArray<QPoint, 50> endPoints;

//        qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
 //       qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

#if 0
        qDebug() << "Rect Top Left " << rect.topLeft().rx() << rect.topLeft().ry();
        qDebug() << "Rect Top Right " << rect.topRight().rx() << rect.topRight().ry();

        qDebug() << "Rect Bottom Left " << rect.bottomLeft().rx() << rect.bottomLeft().ry();
        qDebug() << "Rect Bottom Right " << rect.bottomRight().rx() << rect.bottomRight().ry();
#endif

        int logPoint = 0;
        if (mDrawXAxis)
        {
#if 0
            qDebug() << "Draw X Axis";
#endif
            for (int x = 0; x < rect.width(); x += 5)
            {
                if ((x % mGridSize) == 0)
                {
                    lines.append(QLineF(rect.topLeft().rx() + x, rect.topLeft().ry(), rect.topLeft().rx() + x, rect.topLeft().ry() + largeSpace));
                    if (logPoint % 2 == 0)
                    {
                        endPoints.append(QPoint(rect.topLeft().rx() + x, rect.topLeft().ry() + 2 * largeSpace));
                    }
                    logPoint++;
                }
                else
                {
                    lines.append(QLineF(rect.topLeft().rx() + x, rect.topLeft().ry(), rect.topLeft().rx() + x, rect.topLeft().ry() + smallSpace));
                   
                }

            }

            painter->setPen(QPen(Qt::white, 1, Qt::SolidLine));
            painter->drawLine(rect.topLeft().rx(), rect.topLeft().ry(), rect.topRight().rx(), rect.topRight().ry());
        }
        else
        {
#if 0
            qDebug() << "Draw Y Axis";
#endif
            for (int y = 0; y < rect.height(); y += 5)
            {
                if ((y % mGridSize) == 0)
                {
                    lines.append(QLineF(rect.bottomRight().rx(), rect.bottomRight().ry() - y, rect.bottomRight().rx() - largeSpace, rect.bottomRight().ry() - y));
                    if (logPoint % 2 == 0)
                    {
                        endPoints.append(QPoint(rect.bottomRight().rx() - 5 * largeSpace, rect.bottomRight().ry() - y));
                    }
                    logPoint++;
                    
                }
                else
                {
                    lines.append(QLineF(rect.bottomRight().rx(), rect.bottomRight().ry() - y, rect.bottomRight().rx() - smallSpace, rect.bottomRight().ry() - y));
                }
            }

            painter->setPen(QPen(Qt::white, 1, Qt::SolidLine));
            painter->drawLine(rect.topRight().rx()-1, rect.topRight().ry(), rect.bottomRight().rx()-1, rect.bottomRight().ry());
        }

        painter->setPen(QPen(Qt::white, 0, Qt::SolidLine));
        painter->drawLines(lines.data(), lines.size());
        int numPoints = (endPoints.size() > 0) ? endPoints.size() : 1;
       
     //   qreal resolution = (mAxisMax - mAxisMin) / numPoints;
      qreal resolution = (2*mGridSize) / mScaleFactor;

#if 0
        qDebug() << "NumPoints " << numPoints;

        qDebug() << "mAxisMax " << mAxisMax <<"mAxisMin " << mAxisMin << "resolution "<<resolution;
#endif

        struct axisText
        {
            QPoint textPoint;
            QString textData;
        };
        axisText axistextPoint;
        qreal axisPoint = mAxisMin;

        // If negative values then first draw 0 line by finding position of zero line
        if (mAxisMin < 0)
        {
            bool found = false;
            int i = 0;
            for (i = 0; i < endPoints.size(); i++)
            {
                if (i == endPoints.size() - 1)
                {
                    axisPoint = mAxisMax;
                }
                else
                {
                    qreal axisPointNew = axisPoint + resolution;
                    //Found sign switch over point
                    if (axisPoint < 0 && axisPointNew > 0)
                    {
                        found = true;
                        break;
                    }
                    axisPoint = axisPointNew;
                }
            }

            int i_zero = i;

            //Draw zero point
            QPoint zeroaxispoint = (endPoints[i + 1] + endPoints[i]) / 2;
            QString zerotextData = QString::asprintf("%0.2f", 0.00);
            painter->drawText(zeroaxispoint, zerotextData);
            axisPoint = 0;
            //Draw positive points
            axisPoint = axisPoint + resolution / 2;
            for (int j = i+1; j < endPoints.size(); j++)
            {
                axistextPoint.textPoint = endPoints[j];
                axistextPoint.textData = QString::asprintf("%0.2f", axisPoint);
                painter->drawText(axistextPoint.textPoint, axistextPoint.textData);
                if (j == endPoints.size() - 1)
                {
                    axisPoint = mAxisMax;
                }
                else
                {
                    qreal axisPointNew = axisPoint + resolution;
                    axisPoint = axisPointNew;
                }
            }

            //Draw negative points
            axisPoint = 0;
            axisPoint = axisPoint - resolution / 2;
            for (int j = i; j > 0; j--)
            {
                axistextPoint.textPoint = endPoints[j];
                axistextPoint.textData = QString::asprintf("%0.2f", axisPoint);
                painter->drawText(axistextPoint.textPoint, axistextPoint.textData);
                if (j == 0)
                {
                    axisPoint = mAxisMin;
                }
                else
                {
                    qreal axisPointNew = axisPoint - resolution;
                    axisPoint = axisPointNew;
                }
            }
        }
        else   //Only positive values
        {
            axisPoint = 0;
            for (int i = 0; i < endPoints.size(); i++)
            {
                axistextPoint.textPoint = endPoints[i];
                axistextPoint.textData = QString::asprintf("%0.2f", axisPoint);
                painter->drawText(axistextPoint.textPoint, axistextPoint.textData);
                if (i == endPoints.size() - 1)
                {
                    axisPoint = mAxisMax;
                }
                else
                {
                   // qreal axisPointNew = axisPoint + resolution;
                    qreal axisPointNew = axisPoint + (2*mGridSize)/mScaleFactor;
                   // qDebug() << "axis Point New " << ((2 * mGridSize) / mScaleFactor);
                    axisPoint = axisPointNew;
                }
            }
        }
    }

    void DataPlotterScene::setSignalList(const SignalDescpList_t& signallist)
    {
        mPeriSignalList = signallist;
        int minAxis = 0;
        int maxAxis = 0;
        clearPlot();
        mVectPlotData.resize(mPeriSignalList.size());
        mOffset = 0;
        for (size_t i = 0; i < mVectPlotData.size(); i++)
        {
            mVectPlotData[i].plotsignal = false;
            mVectPlotData[i].signalid = 0;
            mVectPlotData[i].signalvalue = 0;

            //Log only positive temperature values for now
            if (mPeriSignalList[i].signalname.find("Temperature") != std::string::npos)
            {
                minAxis = std::min(minAxis, 0);
                maxAxis = std::max(maxAxis, (int)mPeriSignalList[i].signalMaxVal);
                

            }
            else if ((mPeriSignalList[i].signalname.find("Accel") != std::string::npos) || 
                (mPeriSignalList[i].signalname.find("Gyro") != std::string::npos))
            {
                minAxis = std::min(minAxis, -90);
                maxAxis = std::max(maxAxis, 90);
                mOffset = 12.0;
            }
            else
            {
                minAxis = std::min(minAxis, (int)mPeriSignalList[i].signalMinVal);
                maxAxis = std::max(maxAxis, (int)mPeriSignalList[i].signalMaxVal);
            }
        }
        mVarMaxSignalValue = mMaxSignalValue = maxAxis;
        mVarMinSignalValue = mMinSignalValue = minAxis;
        qDebug() << "Axis Min Val" << minAxis;
        qDebug() << "Axis Max Val" << maxAxis;
        
        mHeighRatio = mVarMaxSignalValue / (mVarMaxSignalValue - mVarMinSignalValue);
        mScaleFactor = (mSceneHeight)/(mVarMaxSignalValue - mVarMinSignalValue);

        qDebug() << "Scene Height " << mSceneHeight;
        qDebug() << "Height Ratio " << mHeighRatio;
        qDebug() << "Scale Factor " << mScaleFactor;


        mYAxis->setAxisLimit(minAxis, maxAxis);
        mYAxis->setScaleFactor(mScaleFactor);
        mYAxis->update();
        qDebug() << "Dataplotter Set Signal List " << mPeriSignalList.size();
    }

    void DataPlotterScene::clearPlot()
    {
        this->clear();
        mXCoord = 0;
        for (int i = 0; i < mSavedSignalVec.size(); i++)
        {
            mSavedSignalVec[i].clear();
        }
    }

    void DataPlotterScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
    {
        qDebug() << "Mouse Pressed ";
     //   mCursor->setShape(Qt::CursorShape::CrossCursor);
        if (mGraphicView->cursor().shape() == Qt::ArrowCursor)
        {
            mGraphicView->setCursor(Qt::CrossCursor);

        }
        else if (mGraphicView->cursor().shape() == Qt::CrossCursor)
        {
            mGraphicView->setCursor(Qt::ArrowCursor);

        }
    
    }
    void DataPlotterScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
    {
        qDebug() << "Mouse Released ";
      //  mCursor->setShape(Qt::CursorShape::ArrowCursor);
       // mGraphicView->setCursor(Qt::ArrowCursor);
    }

    void DataPlotterScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
    {
     //   qDebug() << "Mouse Move ";
        bool showTooTip = false;
        double signalvalue = 0.0;
        if (mGraphicView->cursor().shape() == Qt::CrossCursor)
        {
            QPointF currPoint = event->scenePos();
            QList<QGraphicsItem *> itemsList = this->items(currPoint);
            if (itemsList.size() > 0)
            {
                qDebug() << "Contain Line at X: " << currPoint.x() <<"Y : "<< currPoint.y();
                for (auto itr : itemsList)
                {
                    QGraphicsLineItem  *lineitem = dynamic_cast<QGraphicsLineItem *>(itr);
                    if (lineitem != NULL)
                    {
                        showTooTip = true;
                        if (lineitem->pen().color() == Qt::red)
                        {
                            qDebug() << "Red Signal:: at X: " << currPoint.x() << "has Value : " << mSavedSignalVec[0][currPoint.x()];
                            signalvalue = mSavedSignalVec[0][currPoint.x()];
                        }
                        else if (lineitem->pen().color() == Qt::blue)
                        {
                            qDebug() << "Blue Signal:: X: " << currPoint.x() << "has Value : " << mSavedSignalVec[1][currPoint.x()];
                            signalvalue = mSavedSignalVec[1][currPoint.x()];
                        }
                        else if ((lineitem->pen().color() == Qt::green))
                        {
                            qDebug() << "Green Signal:: X: " << currPoint.x() << "has Value : " << mSavedSignalVec[2][currPoint.x()];
                            signalvalue = mSavedSignalVec[2][currPoint.x()];
                        }
                        else
                        {
                        }
                    }
                }
            }
            if (showTooTip)
            {
                QToolTip::showText(event->screenPos(), "Val: " + QString::number(signalvalue) + ", Time: " + QString::number(currPoint.x()));
            }
        }
        
    }


}