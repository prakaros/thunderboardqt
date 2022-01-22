#pragma once
#include <vector>
#include <list>
namespace sensemon
{
#define SIMULATED_DEVICE 

    enum class ConnectionType
    {
        CM_VCOM_SERIALPORT,
        CM_BLUETOOTH
    };

     enum class PeripheralId 
     {
        P_RHTEMP_SI7021 = 0,
        P_UVALS_SI1133,
        P_AIRQ_CCS811,
        P_HGBAR_BMP280,
        P_IMU_ICM20648,
        P_MEMS_MIC_SPV1840,
        P_SPIFLASG_MX25R8035F,
        P_PIC_EFM8SLEEPYBEE,
        P_PERIPHERALID_END
    };

    enum class PeripheralCapCmd
    {
        CAPTURE_IDLE,
        START_CAPTURE,
        STOP_CAPTURE
    };
    typedef struct peripheral
    {
        bool periAvailable;
        uint8_t periId;
        uint8_t periNameLen;
        std::string peripheralName;
    }peripheral_t;

    typedef std::list<peripheral_t> PeripheralList;

    typedef struct signaldescp
    {
        uint8_t signalid;
        int16_t signalMinVal;
        int16_t signalMaxVal;
        uint16_t signalsamplingrate;
        uint8_t operatingmode;
        uint8_t signalnamelen;
        std::string signalname;
        uint8_t signalunitnamelen;
        std::string signalunitname;
    }signaldescp_t;

    typedef std::vector<signaldescp_t> SignalDescpList_t;

    typedef struct plotingdata
    {
        uint8_t signalid;
        bool    plotsignal;
        double signalvalue;
    }plotingdata_t;

    typedef std::vector<plotingdata_t> VectorPlot;

}

