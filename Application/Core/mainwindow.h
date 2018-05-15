#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QThread>
#include <QItemSelection>

#include "../Networking/Bluetooth/bluetoothconfig.h"
#include "../Visualiser/visualiser.h"
#include "../DataModel/datamodel.h"
#include "../Core/appconfig.h"

#include <QtCharts/QChartView>

#include "Application/Tracking/aruco.h"

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
    QThread networkThread;
    QThread bluetoothThread;
    Visualiser* visualiser;
    DataModel* dataModel;

   // IRDataView* irDataView;
    Bluetoothconfig * btConfig;


    std::map<int, QString> arucoNameMapping;

    ARCameraThread* cameraThread;

    ArUco* arucoTracker;

    QDialog* addIDMappingDialog;
    QDialog* bluetoothConfigDialog;
    QDialog* chartDialog;
    bool chartReset = false;

    QtCharts::QChart *chart;
    QString chartEntry="";
    ValueType chartType = ValueType::Unknown;

    QColor colourmap[NR_OF_COLOURS];

    int sockfd;

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

    void dataModelUpdate(bool listChanged);

    void visConfigUpdate(void);

    void robotListSelectionChanged(const QItemSelection &selection);

    void robotSelectedInVisualiser(QString id);

    void socketOpened(const int &sockfd) { this->sockfd = sockfd; }

    void idMappingUpdate(void);

    void resettingChart();

    void updateChart(bool listChanged, QString robotId, std::vector<QString> changedData);

private slots:
    void on_actionExit_triggered();

    void on_actionEnable_Video_changed();

    void on_videoEnChbx_stateChanged();

    void on_networkListenButton_clicked();

    void on_networkPortBox_textChanged(const QString &arg1);

    void on_imageXDimEdit_textChanged(const QString &arg1);

    void on_imageYDimEdit_textChanged(const QString &arg1);

    void on_visSettingsList_itemClicked(QListWidgetItem *item);

    void on_visSettingsList_itemDoubleClicked(QListWidgetItem *item);

    void on_robotColoursCheckBox_stateChanged();

    void on_logFileButton_clicked();

    void on_loggingButton_clicked();

    void on_tagMappingDeleteButton_clicked();

    void on_tagMappingAddButton_clicked();

    void on_flipImageCheckBox_stateChanged(int arg1);

    void on_robotList_doubleClicked(const QModelIndex &index);

    void on_averagePositionCheckBox_stateChanged(int arg1);

    void on_bluetoothListenButton_clicked();

    void on_bluetoothDisconnectAllButton_clicked();

    void on_bluetoothlist_doubleClicked(const QModelIndex &index);

    void on_angleCorrectionEdit_textChanged(const QString &arg1);

    void on_bluetoothConfigButton_clicked();

    void on_customDataTable_itemDoubleClicked(QTableWidgetItem *item);



private:
    Ui::MainWindow *ui;

    void setVideo(bool enabled);
    void updateOverviewTab(void);
    void updateCustomData();
    void redrawChart();
    void idMappingTableSetup(void);
};

#endif // MAINWINDOW_H
