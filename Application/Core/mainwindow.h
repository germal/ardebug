#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QThread>
#include <QItemSelection>

#include "../Networking/Bluetooth/bluetoothconfig.h"
#include "../Visualiser/visualiser.h"
#include "../DataModel/datamodel.h"

#include <QtCharts/QChartView>

#include "Application/Tracking/aruco.h"
#include "Application/Networking/Wifi/datathread.h"

#ifdef CVB_CAMERA_PRESENT
#include "Application/Tracking/cvbcamerathread.h"
#else
#include "Application/Tracking/usbcamerathread.h"
#endif

#define NR_OF_COLOURS 10
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread bluetoothThread;
    Visualiser* visualiser = nullptr;
    DataModel* dataModel = nullptr;
    DataThread* dataThread = nullptr;

   // IRDataView* irDataView;
    Bluetoothconfig * btConfig = nullptr;


    std::map<int, QString> arucoNameMapping;

    ARCameraThread* cameraThread = nullptr;

    ArUco* arucoTracker = nullptr;

    QDialog* addIDMappingDialog = nullptr;
    QDialog* bluetoothConfigDialog = nullptr;
    QDialog* chartDialog = nullptr;
    bool chartReset = false;

    QtCharts::QChart* chart = nullptr;
    QString chartEntry="";
    ValueType chartType = ValueType::Unknown;

    QColor colourmap[NR_OF_COLOURS];
    QMap<QString, QColor> colourList;
    int colourCounter = 0;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void openUDPSocket(int port);    
    void closeUDPSocket(void);

    void connectBluetooth(void);
    void changeStateBluetoothDevice(int);
    void disconnectBluetooth(void);

    void startReadingCamera(void);
    void stopReadingCamera(void);
    void resetChart();

public slots:
    void robotDeleted(void);

    void dataModelUpdate(bool listChanged, QString robotId, std::vector<QString> changedData);

    void robotListSelectionChanged(const QItemSelection &selection);

    void robotSelectedInVisualiser(QString id);

    void resettingChart();

    void updateChart(bool listChanged, QString robotId, std::vector<QString> changedData);

private slots:

    void redrawChart();

    void updateBluetoothlist();

    void on_networkListenButton_clicked();

    void on_networkPortBox_textChanged(const QString &arg1);

    void on_robotList_doubleClicked(const QModelIndex &index);

    void on_bluetoothListenButton_clicked();

    void on_bluetoothDisconnectAllButton_clicked();

    void on_bluetoothlist_doubleClicked(const QModelIndex &index);

    void on_bluetoothConfigButton_clicked();

    void on_customDataTable_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::MainWindow* ui = nullptr;

    void setVideo(bool enabled);
    void updateCustomData();
    //void redrawChart();
    void idMappingTableSetup(void);
};

#endif // MAINWINDOW_H
