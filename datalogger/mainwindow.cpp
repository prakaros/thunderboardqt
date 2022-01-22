#include <QtWidgets\QMessageBox>
#include <QtCore\QDebug>
#include <QtSerialPort\QSerialPort>
#include <QtCore\QStringList>

#include "applicationTypes.hpp"
#include "MainWindow.hpp"
#include "ConnectionManagerVCOM.hpp"

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags)
    , mMainWindow(new Ui::MainWindow())
    , mStringlistModel()
    , mPeripheralListModel()

{
    mMainWindow->setupUi(this);
    this->setWindowTitle("Sensors-Monitor");
    this->setStyleSheet("background-color: #2f4f4f;");  //Slate gray
  //  this->setStyleSheet("background-color: #003333;");  //Teal

    mMainWindow->connectButton->setStyleSheet("background-color: #555555; color: #ffffff;");
    mMainWindow->disconnectButton->setStyleSheet("background-color: #555555; color: #ffffff;");
    mMainWindow->openPortButton->setStyleSheet("background-color: #555555; color: #ffffff;");
    mMainWindow->displayPeripheralButton->setStyleSheet("background-color: #555555; color: #ffffff;");
    mMainWindow->displayDevicesButton->setStyleSheet("background-color: #555555; color: #ffffff;");
    mMainWindow->deviceDescpButton->setStyleSheet("background-color: #555555; color: #ffffff;");
    mMainWindow->startLogButton->setStyleSheet("background-color: #555555; color: #ffffff;");
    mMainWindow->stopLogButton->setStyleSheet("background-color: #555555; color: #ffffff;");
    mMainWindow->groupBox->setStyleSheet("color: #ffffff;");
    mMainWindow->groupBox_2->setStyleSheet("color: #ffffff;");

    QString peripheralListstylesheet = "";
    // I want a colored background:
    peripheralListstylesheet += "QListView{ background-color: #555555; color: #ffffff;}";
    peripheralListstylesheet += "QListView::item:selected:active { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6a6ea9, stop: 1 #888dd9); }";

    mMainWindow->peripheralListView->setStyleSheet(peripheralListstylesheet);

    QString devListViewstylesheet = "";
    // I want a colored background:
    devListViewstylesheet += "QListView{ background-color: #555555; color: #ffffff;}";
    devListViewstylesheet += "QListView::item:!selected:hover { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6a6ea9, stop: 1 #888dd9); }";

    mMainWindow->deviceListView->setStyleSheet(devListViewstylesheet);   //Drak Gray and white
    mMainWindow->plotterGraphicsView->setStyleSheet("background-color: #778899;");   //Light gray
    mMainWindow->graphicsViewXAxis->setStyleSheet("QGraphicsView { border-style: none; }");
    mMainWindow->graphicsViewYAxis->setStyleSheet("QGraphicsView { border-style: none; }");
    InitializeMenuBar();

    mMainWindow->deviceListView->setModel(&mStringlistModel);
    mMainWindow->peripheralListView->setModel(&mPeripheralListModel);
    mMainWindow->peripheralSignalListView->setModel(&mPeripheralSignalListModel);
  //  mMainWindow->peripheralSignalListView->setStyleSheet(devListViewstylesheet);

    connect(mMainWindow->peripheralSignalListView, SIGNAL(clicked(const QModelIndex&)),
        this, SLOT(onPeripheralSignalClicked(const QModelIndex&)));

    DataPlotterScene::getDataPlotterSceneInstance()->Initialize(mMainWindow->plotterGraphicsView, mMainWindow->graphicsViewXAxis, 
                                                                mMainWindow->graphicsViewYAxis);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onOpenPortButtonClicked()
{
    QString message("Open Port Button Clicked");
    mMainWindow->debugTextDisplay->setText(message);
    qDebug() << "Open Port Button Clicked";
    QModelIndex modelIndex = mMainWindow->deviceListView->currentIndex();
    QString deviceName = mStringlistModel.stringList().at(modelIndex.row());
    qDebug() << "Device Name " << deviceName;
    mConnManager->openDevicePort(deviceName.toStdString());
}

void MainWindow::onConnectButtonClicked()
{
    QString message("Connect Button Clicked");
    mMainWindow->debugTextDisplay->setText(message);
    qDebug() << "Connect Button Clicked";
    QModelIndex modelIndex = mMainWindow->deviceListView->currentIndex();
    QString deviceName = mStringlistModel.stringList().at(modelIndex.row());
    qDebug() << "Device Name " << deviceName;
    mConnManager->connectToDevice(deviceName.toStdString());
}

void MainWindow::onDisconnectButtonClicked()
{
    QString message("Disconnect Button Clicked");
    mMainWindow->debugTextDisplay->setText(message);
    qDebug() << QString("Disconnect Button Clicked");
}

void MainWindow::onDisplayDevicesButtonClicked()
{
    if (!mConnManager)
    {
        if (mMainWindow->vCOMRadioButton->isChecked())
        {
            qDebug() << QString("Creating VCOM connection manager");
            mConnManager = std::make_shared<ConnectionManagerVCOM>();
        }
    }

    std::vector<std::string> deviceList;
    mConnManager->getDeviceList(deviceList);

    QStringList qStrList;
    for (auto&itr : deviceList)
    {
        qStrList << QString::fromStdString(itr);
    }

    for (auto & itr : qStrList)
    {
        qDebug() << itr;
    }

    mStringlistModel.setStringList(qStrList);
    mMainWindow->deviceListView->update();
}

void MainWindow::onDisplayPeripheralsButtonClicked()
{
    QModelIndex modelIndex = mMainWindow->deviceListView->currentIndex();
    QString deviceName = mStringlistModel.stringList().at(modelIndex.row());
    qDebug() << "Device Name " << deviceName;

    if (mConnManager->isConnected())
    {
        ConnectionManagerVCOM *conMgrVCom = static_cast<ConnectionManagerVCOM*>(mConnManager.get());
        qDebug() << "Display Peripheral Button Clicked";
        if (conMgrVCom->executeGetPeripheralList(deviceName.toStdString()))
        {
            PeripheralList pList = conMgrVCom->getPeripheralList();
            QStringList qStrList;
            for (auto&itr : pList)
            {
                qStrList << QString::fromStdString(itr.peripheralName);
            }

            for (auto & itr : qStrList)
            {
                qDebug() << itr;
            }

            mPeripheralListModel.setStringList(qStrList);
            mMainWindow->peripheralListView->update();
        }
    }
}

void MainWindow::onDeviceDescrpButtonClicked()
{
    if (mConnManager->isConnected())
    {
        QModelIndex modelIndex = mMainWindow->peripheralListView->currentIndex();
        QString peripheralName = mPeripheralListModel.stringList().at(modelIndex.row());
        qDebug() << "Peripheral Name " << peripheralName;
        
        ConnectionManagerVCOM *conMgrVCom = static_cast<ConnectionManagerVCOM*>(mConnManager.get());
        qDebug() << "Display Device Description Button Clicked";
        if (conMgrVCom->executePeripheralDecp(peripheralName.toStdString()))
        {
            SignalDescpList_t pList = conMgrVCom->getSignalDescList();
            QStringList qStrList;
            for (auto&itr : pList)
            {
                qStrList << QString::fromStdString(itr.signalname);
            }

            for (auto & itr : qStrList)
            {
                qDebug() << itr;
            }

            if (!mPeripheralListModel.setData(mPeripheralModelIndex, QBrush(QColor(255, 255, 255)), Qt::ForegroundRole))
            {
                exit(1);
            }

            if (!mPeripheralListModel.setData(mPeripheralModelIndex, QFont("MS Shell Dlg 2", 9, QFont::Normal, false), Qt::FontRole))
            {
                exit(1);
            }

            if (!mPeripheralListModel.setData(modelIndex, QBrush(QColor(238, 130, 238,127)), Qt::ForegroundRole))
            {
                exit(1);
            }

            if (!mPeripheralListModel.setData(modelIndex, QFont("MS Shell Dlg 2",9, QFont::Bold, false), Qt::FontRole))
            {
                exit(1);
            }

            mMainWindow->peripheralListView->setFocus();

            mPeripheralSignalListModel.setStringList(qStrList);
            mMainWindow->peripheralSignalListView->update();
            mMainWindow->peripheralSignalListView->selectAll();
            int numRow = mPeripheralSignalListModel.rowCount();
            for (int i = 0; i < numRow;i++)
            {
                if (!mPeripheralSignalListModel.setData(mPeripheralSignalListModel.index(i), QColor(Qt::transparent), Qt::DecorationRole))
                {
                    exit(1);
                }
            }
            mMainWindow->peripheralSignalListView->update();
            mMainWindow->peripheralSignalListView->setFocus();


            mPeripheralModelIndex = modelIndex;
        }
    }
}

//Working 
#if 0
void MainWindow::onStartLogButtonClicked()
{

    if (mConnManager->isConnected())
    {
        QModelIndex modelIndex = mMainWindow->peripheralListView->currentIndex();
        QString peripheralName = mPeripheralListModel.stringList().at(modelIndex.row());
        qDebug() << "Peripheral Name " << peripheralName;

        ConnectionManagerVCOM *conMgrVCom = static_cast<ConnectionManagerVCOM*>(mConnManager.get());
        qDebug() << "Start Log Button Clicked";
        conMgrVCom->executeStartLog(peripheralName.toStdString());
    }
    else
    {
        ConnectionManagerVCOM *conMgrVCom = static_cast<ConnectionManagerVCOM*>(mConnManager.get());
        qDebug() << "Start Log Button Clicked For SimPeripheral";
        conMgrVCom->executeStartLog("SimPeripheral");
    }
}
#endif
#if 1
void MainWindow::onStartLogButtonClicked()
{

    if (mConnManager->isConnected())
    {
        QModelIndex modelIndex = mMainWindow->peripheralSignalListView->currentIndex();
        QString signalName = mPeripheralSignalListModel.stringList().at(modelIndex.row());
        qDebug() << "Peripheral Name " << signalName;
        if (!mPeripheralSignalListModel.setData(modelIndex, QBrush(QColor(176, 255, 255)), Qt::ForegroundRole))
        {
            exit(1);
        }

        if (!mPeripheralSignalListModel.setData(modelIndex, QColor(DataPlotterScene::getDataPlotterSceneInstance()->getSignalColor(modelIndex.row())), Qt::DecorationRole))
        {
            exit(1);
        }

        mMainWindow->peripheralSignalListView->setFocus();
        ConnectionManagerVCOM *conMgrVCom = static_cast<ConnectionManagerVCOM*>(mConnManager.get());
        qDebug() << "Start Log Button Clicked";
        conMgrVCom->executeStartLog(modelIndex.row());
    }
}

#endif
void MainWindow::onStopLogButtonClicked()
{
    if (mConnManager->isConnected())
    {
        QModelIndex modelIndex = mMainWindow->peripheralSignalListView->currentIndex();
        QString signalName = mPeripheralSignalListModel.stringList().at(modelIndex.row());

        if (!mPeripheralSignalListModel.setData(modelIndex, QColor(Qt::transparent), Qt::DecorationRole))
        {
            exit(1);
        }

        if (!mPeripheralSignalListModel.setData(modelIndex, QBrush(Qt::black), Qt::ForegroundRole))
        {
            exit(1);
        }

#if 0
        if (!mPeripheralSignalListModel.setData(modelIndex, Qt::AlignRight, Qt::TextAlignmentRole))
        {
            exit(1);
        }
#endif

        mMainWindow->peripheralSignalListView->setFocus();
        qDebug() << "Peripheral Name " << signalName;
        ConnectionManagerVCOM *conMgrVCom = static_cast<ConnectionManagerVCOM*>(mConnManager.get());
        qDebug() << "Stop Log Button Clicked";
        conMgrVCom->executeStopLog(modelIndex.row());
//        mMainWindow->peripheralSignalListView->selectionModel()->select(modelIndex, QItemSelectionModel::SelectCurrent);
    }
}

void MainWindow::onPeripheralSignalClicked(const QModelIndex& index)
{
    QString selectedSignalName = mPeripheralSignalListModel.stringList().at(index.row());
    qDebug() << "PeripheralSignal Selected " << selectedSignalName;
    SignalDescpList_t signalList;

    ConnectionManagerVCOM *conMgrVCom = static_cast<ConnectionManagerVCOM*>(mConnManager.get());
    signalList = conMgrVCom->getSignalDescList();
    bool found = false;
    signaldescp_t signalDesc;
    for (auto itr : signalList)
    {
        if (itr.signalname == selectedSignalName.toStdString())
        {
            found = true;
            signalDesc = itr;
            break;
        }
    }

    if (found)
    {
        mMainWindow->minimumValText->setPlainText(QString::asprintf("%d", signalDesc.signalMinVal));
        mMainWindow->maximumValueText->setPlainText(QString::asprintf("%d", signalDesc.signalMaxVal));
        mMainWindow->sampleRateText->setPlainText(QString::asprintf("%d", signalDesc.signalsamplingrate));
    }
    

}

void MainWindow::InitializeMenuBar()    
{
    mFileMenu = menuBar()->addMenu(tr("&File"));
    menuBar()->addSeparator();
    mEditMenu = menuBar()->addMenu(tr("&Edit"));
    mViewMenu = menuBar()->addMenu(tr("&View"));
    mHelpMenu = menuBar()->addMenu(tr("&Help"));
    menuBar()->setStyleSheet("background-color: #555555");
    mFileMenu->setStyleSheet("foreground-color: #555555");
}

