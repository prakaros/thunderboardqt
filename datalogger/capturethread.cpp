#include <QtCore\QDebug>
#include "messageformat.hpp"
#include "capturethread.hpp"
#include "vcomdevice.hpp"
#include "dataplotter.hpp"
#include <Windows.h>

namespace sensemon
{
    CaptureThread::CaptureThread(VCOMDevice *dev) :
        mThreadPtr(nullptr)
        , mSimThreadPtr(nullptr)
        , mDev(dev)
        , mStartStop(false)
        , mStartStopSimThread(false)
        , mPeripheralId((uint8_t)PeripheralId::P_PERIPHERALID_END)
    {
    
    }

    void CaptureThread::workerThread()
    {
        int len = 0;
        bool result = false;
        Frame receiveFrame;
        int sendCount = 5;
        qDebug() << "Worker Thread Starting";

        while (mStartStop)
        {
            memset(mBuffer, 0, 4096);
            result = false;
            len = 0;
            len = mDev->readData(mBuffer, 50);

            if (len > 0)
            {

                //Create frame from raw data and check validity of data
                result = receiveFrame.createFrame(mBuffer, len);
            }
            if (result)
            {
                if (static_cast<CommandId>(receiveFrame.getCmd()) == CommandId::CMD_PERIPHERALCAPTURERSP)
                {   
         
                    mCapRsp.copyResponseData((const uint8_t*)receiveFrame.getMessagePointer());
#if 1
                    qDebug() << "Recived Data Capture";
                    qDebug() << "Peripheral Id " << (int)mCapRsp.getPeripheralId();
                    qDebug() << "Capture Len " << (int)mCapRsp.getCaptureDataLen();
#endif
                    const  uint8_t *capDataPtr = mCapRsp.getCaptureDataPtr();
                    mPeripheralId = mCapRsp.getPeripheralId();

                    if ((PeripheralId)mCapRsp.getPeripheralId() == PeripheralId::P_RHTEMP_SI7021)
                    {
                        uint32_t rhData = *(uint32_t*)(capDataPtr);
                        int32_t tempData = *(int32_t*)(capDataPtr + 4);
                        double rhVal = rhData / 1000.0;
                        double tempVal = tempData / 1000.0;
#if 0
                        qDebug() << "rh Val " << rhVal;
                        qDebug() << "Temp Val " << tempVal;
#endif

                        mPlotData[0].signalvalue = rhVal;
                        mPlotData[1].signalvalue = tempVal;

                        emit plotsignaldata(mPlotData);
                    }
                    else if ((PeripheralId)mCapRsp.getPeripheralId() == PeripheralId::P_UVALS_SI1133)
                    {
                       
                       uint16_t uvData = *(uint16_t*)(capDataPtr);
                       uint32_t ambData = *(uint32_t*)(capDataPtr + 2);
                       double uvVal = uvData / 100.0;
                       double ambVal = ambData / 1000.0;
#if 0
                       qDebug() << "uv Val " << uvVal;
                       qDebug() << "Amb Val " << ambVal;
#endif

                       mPlotData[0].signalvalue = uvVal;
                       mPlotData[1].signalvalue = ambVal;
                       emit plotsignaldata(mPlotData);
                    }
                    else if ((PeripheralId)mCapRsp.getPeripheralId() == PeripheralId::P_AIRQ_CCS811)
                    {
                        uint16_t eCO2Data = *(uint16_t*)(capDataPtr);
                        uint16_t tvocData = *(uint16_t*)(capDataPtr + 2);
                        double eCO2Val = eCO2Data / 1.0;
                        double tvocVal = tvocData / 1.0;
#if 0
                        qDebug() << "eCO2 Val " << eCO2Val;
                        qDebug() << "tvoc Val " << tvocVal;
#endif

                        mPlotData[0].signalvalue = eCO2Val;
                        mPlotData[1].signalvalue = tvocVal;
                        emit plotsignaldata(mPlotData);

                    }
                    else if ((PeripheralId)mCapRsp.getPeripheralId() == PeripheralId::P_HGBAR_BMP280)
                    {
                        int32_t tmpData = *(int32_t*)(capDataPtr);
                        int32_t presData = *(int32_t*)(capDataPtr + 4);
                        double tmpVal = tmpData / 1000.0;
                        double presVal = presData / 1000.0;
#if 0
                        qDebug() << "temp Val " << tmpVal;
                        qDebug() << "pres Val " << presVal;
#endif

                        mPlotData[0].signalvalue = tmpVal;
                        mPlotData[1].signalvalue = presVal;
                        emit plotsignaldata(mPlotData);

                    }
                    else if ((PeripheralId)mCapRsp.getPeripheralId() == PeripheralId::P_IMU_ICM20648)
                    {
                        int16_t accelXData = *(int16_t*)(capDataPtr);
                        int16_t accelYData = *(int16_t*)(capDataPtr + 2);
                        int16_t accelZData = *(int16_t*)(capDataPtr + 4);

                        int16_t orientXData = *(int16_t*)(capDataPtr + 6);
                        int16_t orientYData = *(int16_t*)(capDataPtr + 8);
                        int16_t orientZData = *(int16_t*)(capDataPtr + 10);

                        double accelXVal = accelXData / 100.0;
                        double accelYVal = accelYData / 100.0;
                        double accelZVal = accelZData / 100.0;

                        double orientXVal = orientXData / 100.0;
                        double orientYVal = orientYData / 100.0;
                        double orientZVal = orientZData / 100.0;


                        mPlotData[0].signalvalue = accelXVal;
                        mPlotData[1].signalvalue = accelYVal;
                        mPlotData[2].signalvalue = accelZVal;
                        emit plotsignaldata(mPlotData);
                    }
                }
            }
           
        }
        qDebug() << "Capture Thread Dead";
    }
    void CaptureThread::startThread(int signalIndex)
    {
        if (!mStartStop)
        {
            mStartStop = true;
            const SignalDescpList_t sigDec = DataPlotterScene::getDataPlotterSceneInstance()->getSignalList();
            mPlotData.clear();
            for (auto itr : sigDec)
            {
                plotingdata_t pdata;
                pdata.plotsignal = false;
                pdata.signalid = itr.signalid;
                pdata.signalvalue = 0;
                mPlotData.push_back(pdata);
            }
            //   connect(this, SIGNAL(plotsignal(int)), DataPlotter::getDataPlotterInstance(), SLOT(plotData(int)));
         //   connect(this, SIGNAL(plotsignal(int)), DataPlotterScene::getDataPlotterSceneInstance(), SLOT(plotData(int)));
            connect(this, SIGNAL(plotsignaldata(const sensemon::VectorPlot&)), DataPlotterScene::getDataPlotterSceneInstance(), SLOT(plotsignaldata(const sensemon::VectorPlot&)));
            if (mThreadPtr == nullptr)
            {
                mThreadPtr = new std::thread(&CaptureThread::workerThread, this);
            }
        }
        mPlotData[signalIndex].plotsignal = true;
    }

    bool CaptureThread::stopLog(int signalIndex)
    {
        mPlotData[signalIndex].plotsignal = false;
        bool keeprunning = false;
        for (auto itr : mPlotData)
        {
            keeprunning = keeprunning || itr.plotsignal;
        }
        return keeprunning;
    }

    void CaptureThread::sendStopCommand()
    {
        for (int i = 0; i < 5; i++)
        {
            qDebug() << "Sending Stop command send " << (int)mPeripheralId;

            PeripheralCaptureCmd pCmd;
            pCmd.setPeripheralCmdData((uint8_t)mPeripheralId, 0, static_cast<uint8_t>(PeripheralCapCmd::STOP_CAPTURE));
            mDev->sendCommand(pCmd);
            qDebug() << "Stop command send " << (int)mPeripheralId;
            ::Sleep(100);
        }
       
    }

    void CaptureThread::stopThread()
    {
        if (mStartStop)
        {
            mStartStop = false;
            mThreadPtr->join();
            if (mThreadPtr != nullptr)
            {
                delete mThreadPtr;
                mThreadPtr = nullptr;
            }
            mPlotData.clear();
        }
    }


    void  CaptureThread::startSimThread(int signalIndex)
    {
        qDebug() << "Start Log for  " << signalIndex;
        if (!mStartStopSimThread)
        {
            mStartStopSimThread = true;

            const SignalDescpList_t sigDec = DataPlotterScene::getDataPlotterSceneInstance()->getSignalList();
            qDebug() << "Size of Signal List " << sigDec.size();
            mPlotData.clear();
            for (auto itr : sigDec)
            {
                plotingdata_t pdata;
                pdata.plotsignal = false;
                pdata.signalid = itr.signalid;
                pdata.signalvalue = 0;
                mPlotData.push_back(pdata);
            }
          //  connect(this, SIGNAL(plotsignal(int)), DataPlotterScene::getDataPlotterSceneInstance(), SLOT(plotData(int)));
            connect(this, SIGNAL(plotsignaldata(const sensemon::VectorPlot&)), DataPlotterScene::getDataPlotterSceneInstance(), SLOT(plotsignaldata(const sensemon::VectorPlot&)));

            if (mSimThreadPtr == nullptr)
            {
                mSimThreadPtr = new std::thread(&CaptureThread::workerSimThread, this);
            }
        }
        mPlotData[signalIndex].plotsignal = true;
    }

    void CaptureThread::stopSimThread(int signalIndex)
    {
        mPlotData[signalIndex].plotsignal = false;
        bool keeprunning = false;
        for (auto itr : mPlotData)
        {
            keeprunning = keeprunning || itr.plotsignal;
        }
        if (!keeprunning)
        {
            if (mStartStopSimThread)
            {
                mStartStopSimThread = false;
                mSimThreadPtr->join();
                if (mSimThreadPtr != nullptr)
                {
                    delete mSimThreadPtr;
                    mSimThreadPtr = nullptr;
                }
                mPlotData.clear();
            }
        }
    }

    void CaptureThread::workerSimThread()
    {
        int len = 0;
        bool result = false;
        Frame receiveFrame;
        qDebug() << "Sim Worker Thread Starting";
        static int i = 0;
        bool rampUp = false;
        while (mStartStopSimThread)
        {
#if 0
            if (i == 0)
            {
                rampUp = true;
            }
            if (i == 100)
            {
                rampUp = false;
            }
            if (rampUp)
            {
                i = i + 5;
            }
            else
            {
                i = i -5;
            }
#endif
            for (size_t i = 0; i < mPlotData.size(); i++)
            {
                mPlotData[i].signalvalue = qrand() % 255 +i*88;
            }
           

        //    emit plotsignal(qrand()%255);
            emit plotsignaldata(mPlotData);
            ::Sleep(500);
        }
    }
}