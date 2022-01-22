#pragma once

#include <QtWidgets\QMainWindow>
#include <QtCore\QStringListModel>
#include <QtWidgets\QMenuBar>
#include <memory>
#include "ui_qt_thuderboard_sense.h"
#include "ConnectionManager.hpp"
#include "dataplotter.hpp"
#include "sensemonstringlistmodel.hpp"

using namespace sensemon;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
    virtual ~MainWindow();
    public slots:
    void onOpenPortButtonClicked();
    void onConnectButtonClicked();
    void onDisconnectButtonClicked();
    void onDisplayDevicesButtonClicked();
    void onDisplayPeripheralsButtonClicked();
    void onDeviceDescrpButtonClicked();
    void onStartLogButtonClicked();
    void onStopLogButtonClicked();
    void InitializeMenuBar();
    void onPeripheralSignalClicked(const QModelIndex&);


private:
    std::shared_ptr<Ui::MainWindow> mMainWindow;
    std::shared_ptr<ConnectionManager> mConnManager;
#if 0
    QStringListModel mStringlistModel;
    QStringListModel mPeripheralListModel;
    QStringListModel mPeripheralSignalListModel;
#endif
    SenseMonStringListModel mStringlistModel;
    SenseMonStringListModel mPeripheralListModel;
    SenseMonStringListModel mPeripheralSignalListModel;
    QModelIndex mPeripheralModelIndex;


    QMenu *mFileMenu;
    QMenu *mEditMenu;
    QMenu *mViewMenu;
    QMenu *mHelpMenu;
};
