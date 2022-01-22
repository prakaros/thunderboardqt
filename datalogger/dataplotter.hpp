#ifndef DATAPLOTTER_HPP_
#define DATAPLOTTER_HPP_
#include <QtWidgets\QGraphicsView>
#include <QtWidgets\QGraphicsItem>
#include <QtWidgets\QGraphicsScene>
#include <QtCore\QMetaType>
#include "applicationtypes.hpp"

//Q_DECLARE_INTERFACE(sensemon::DataPlotter, "DataPlotter/1.0")

namespace sensemon
{


    class DrawAxis : public QGraphicsScene
    {
        Q_OBJECT
    public:
        DrawAxis(bool axis, int grids);
        ~DrawAxis() {}
        void Initialize(QGraphicsView *graphView);
        void drawBackground(QPainter *painter, const QRectF &rect);
        void setAxisLimit(int minVal, int maxVal) { mAxisMin = minVal; mAxisMax = maxVal; }
        void getAxisLimit(int& minVal, int& maxVal) { minVal = mAxisMin; maxVal = mAxisMax; }
        void setScaleFactor(double scalf) { mScaleFactor = scalf; }
    private:
        QGraphicsView *mGraphicsView;
        int mSceneWidth;
        int mSceneHeight;
        int mAxisMin;
        int mAxisMax;
        bool mDrawXAxis;
        int mGridSize;
        double mScaleFactor;
        double mOffset;
    };

    class DataPlotterScene : public QGraphicsScene
    {
        Q_OBJECT
        
    public:
        virtual ~DataPlotterScene();
        void Initialize(QGraphicsView *qview, QGraphicsView *xaxisview, QGraphicsView *yaxisview);

        static DataPlotterScene*getDataPlotterSceneInstance();

        void drawBackground(QPainter *painter, const QRectF &rect);
        void setXAxisLimit(int lower, int upper);
        void setYAxisLimit(int lower, int upper);
        void setSignalList(const SignalDescpList_t& signallist);
        const SignalDescpList_t& getSignalList() const {return mPeriSignalList; }
        public slots:
        void plotData(int data);
        void plotsignaldata(const sensemon::VectorPlot&);
        QColor getSignalColor(int index) { return mColorVec.at(index); }
    protected:
        virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);


    private:
        explicit DataPlotterScene(QObject *parent = 0);
        QGraphicsView *mGraphicView;
        DrawAxis *mXAxis;
        DrawAxis *mYAxis;
        QCursor *mCursor;
    private:
        void clearPlot();
        static DataPlotterScene *mDataPlotterInstanceScene;
        int mSceneWidth;
        int mSceneHeight;
        int mCurveRectWidth;
        SignalDescpList_t mPeriSignalList;
        VectorPlot mVectPlotData;
        std::vector<std::vector<double>> mSavedSignalVec;
        std::vector<Qt::GlobalColor> mColorVec;
        static const int gridSize = 25;
        double mMinSignalValue;
        double mMaxSignalValue;
        double mVarMaxSignalValue;
        double mVarMinSignalValue;
        double mScaleFactor;
        double mOffset;
        double mHeighRatio;
        int mXCoord;
    };



}



#endif
