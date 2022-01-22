#pragma once
#include <thread>
#include <memory>
#include "cmdresponse.hpp"
#include <memory>
#include <QtCore\QObject>

//#include <qobject.h>
#include "applicationtypes.hpp"





namespace sensemon
{
    class VCOMDevice;
    class CaptureThread : public QObject
    {
        Q_OBJECT
    public:
        CaptureThread(VCOMDevice *dev);
        ~CaptureThread() {}
        void workerThread();
        void workerSimThread();
        void startThread(int signalindex);
        bool stopLog(int signalIndex);
        void stopThread();
        void  startSimThread(int signalindex);
        void stopSimThread(int signalIndex);
        void sendStopCommand();
    signals:
        void plotsignal(int data);
        void plotsignaldata(const sensemon::VectorPlot&);
    private:
        std::thread *mThreadPtr;
        std::thread *mSimThreadPtr;
        VCOMDevice *mDev;
        bool mStartStop;
        bool mStartStopSimThread;
        uint8_t mBuffer[4096];
        PeripheralCaptureRsp mCapRsp;
        VectorPlot mPlotData;
        uint8_t mPeripheralId;
    };
}