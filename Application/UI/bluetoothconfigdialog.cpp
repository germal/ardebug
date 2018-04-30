/* bluetoothconfigdialog.cpp
 *
 * This class encapsulates a small dialog for adding a new tag ID -> robot ID mapping.
 *
 * Charlotte Arndt Feb 2018
 */

#include "bluetoothconfigdialog.h"
#include <QFormLayout>
#include <QPushButton>
#include <QDebug>

/* constructor
 *
 */
BluetoothConfigDialog::BluetoothConfigDialog(Bluetoothconfig* btConfig)
{
    qDebug()<<"dialog constructor";
    this->setModal(true);
    this->setGeometry(200, 200, 600, 400);
    this->setWindowTitle("Bluetooth configuration");
    infoDialog = NULL;

    //create UI elements for the lists of devices and buttons to modify them
    currentList = new QListView();
    currentList->setEditTriggers(QListView::NoEditTriggers);
    scanList = new QListView();
    scanList->setEditTriggers(QListView::NoEditTriggers);

    QPushButton* moveButton = new QPushButton("<<");
    QPushButton* deleteButton = new QPushButton("Delete Entry");
    QPushButton* toggleActiveStatusButton = new QPushButton("(de)activate");
    scanButton = new QPushButton("Scan");

    connect(scanButton, SIGNAL(clicked()), this, SLOT(startScan()));
    connect(moveButton, SIGNAL(clicked(bool)), this, SLOT(addScannedEntry()));
    connect(toggleActiveStatusButton, SIGNAL(clicked(bool)), this, SLOT(toggleStatus()));
    connect(deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteEntry()));

    //setup the discovery agent and its signals
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),this, SLOT(addDevice(QBluetoothDeviceInfo)));
    connect(discoveryAgent, SIGNAL(finished()), this, SLOT(scanFinished()));

    // Layout the boxes and labels on one row
    QHBoxLayout* currentListButtonsBox = new QHBoxLayout();
    currentListButtonsBox->addWidget(toggleActiveStatusButton);
    currentListButtonsBox->addWidget(deleteButton);

    QVBoxLayout*  currentListBox = new QVBoxLayout();
    currentListBox->addWidget(currentList);
    currentListBox->addLayout(currentListButtonsBox);

    QVBoxLayout* scanListBox = new QVBoxLayout();
    scanListBox->addWidget(scanList);
    scanListBox->addWidget(scanButton);


    // Layout the boxes and labels on one row
    QHBoxLayout* listsBox = new QHBoxLayout();
    listsBox->addLayout(currentListBox);
    listsBox->addWidget(moveButton);
    listsBox->addLayout(scanListBox);

    // UI elements to manually add devices
    QPushButton* manualAdditionButton = new QPushButton("Add");
    manualAdditionName = new QLineEdit();
    manualAdditionName->setPlaceholderText("enter name");
    manualAdditionAddress = new QLineEdit();
    manualAdditionAddress->setPlaceholderText("enter MAC Address");
    QObject::connect(manualAdditionButton, SIGNAL(clicked(bool)), this, SLOT(addEntry(void)));

    // Layout the UI elemts for manual entries in one row
    QHBoxLayout* manualEntryBox = new QHBoxLayout();
    manualEntryBox->addWidget(manualAdditionAddress);
    manualEntryBox->addWidget(manualAdditionName);
    manualEntryBox->addWidget(manualAdditionButton);

    // Buttons to cancel or apply
    QPushButton* applyButton = new QPushButton("Apply");
    QPushButton* cancelButton = new QPushButton("Cancel");
    QObject::connect(applyButton, SIGNAL(clicked(bool)), this, SLOT(accept(void)));
    QObject::connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject(void)));

    // Layout the buttons on another row
    QHBoxLayout* buttonBox = new QHBoxLayout();
    buttonBox->addWidget(applyButton);
    buttonBox->addWidget(cancelButton);


    // Arrange the rows vertically
    QVBoxLayout* mainbox = new QVBoxLayout();
    mainbox->addLayout(listsBox);
    mainbox->addLayout(manualEntryBox);
    mainbox->addLayout(buttonBox);

    this->btConfig = btConfig;
    deviceList = btConfig->getDeviceList();
    deviceListModel = new QStandardItemModel();

    for(size_t i = 0; i < deviceList->size(); i++) {

        BluetoothDeviceListItem* item = (BluetoothDeviceListItem*)deviceList->at(i);

        QStandardItem *list_item = new QStandardItem(QString("%0 : %1").arg(item->getName()).arg(item->getBTAddress()));
        if( item->getState())
        {
           list_item->setBackground(Qt::green);
        }
        else
        {
           list_item->setBackground(Qt::red);
        }

        deviceListModel->setItem(i, 0, list_item);
    }

    currentList->setModel(deviceListModel);
    currentList->setSelectionBehavior(QListView::SelectRows);
    currentList->setSelectionMode(QListView::ExtendedSelection);

    scanListModel = new QStandardItemModel();
    scanList->setModel(scanListModel);
    scanList->setSelectionBehavior(QListView::SelectRows);
    scanList->setSelectionMode(QListView::ExtendedSelection);

    // Set layout
    this->setLayout(mainbox);
}

/* deconstructor
 * frees memory
 */
BluetoothConfigDialog::~BluetoothConfigDialog()
{
    delete(deviceList);
    delete( discoveryAgent);

}

/* toggleStatus()
 * slot to toggle the active/inactive status of selected device
 */
void BluetoothConfigDialog::toggleStatus()
{

    QModelIndexList indexes = currentList->selectionModel()->selectedRows();

    for( QModelIndexList::const_iterator it = indexes.constEnd() -1; it>=indexes.constBegin(); it--)
    {
        BluetoothDeviceListItem* btDevice = deviceList->at(it->row());
        btDevice->setState(!btDevice->getState());

        if( btDevice->getState())
        {
           deviceListModel->item(it->row())->setBackground(Qt::green);
        }
        else
        {
           deviceListModel->item(it->row())->setBackground(Qt::red);
        }

    }
    currentList->clearSelection();

}


/* deleteEntry()
 * slot
 * deletes selected entries from device list
 */
void BluetoothConfigDialog::deleteEntry()
{
    QModelIndexList indexes = currentList->selectionModel()->selectedRows();

    for( QModelIndexList::const_iterator it = indexes.constEnd() -1; it>=indexes.constBegin(); it--)
    {
        deviceList->erase(deviceList->begin()+it->row() );
        deviceListModel->removeRow(it->row());

    }
}


/* accept
 * accepts user changes and applies them
 */
void BluetoothConfigDialog::accept()
{
    qDebug()<<"accepted";
    btConfig->setDeviceList(*deviceList);
    QDialog::accept();


}

/* reject
 * cancels user changes and discards changes
 */
void BluetoothConfigDialog::reject()
{
    QDialog::reject();

}

/* addEntry
 * slot, adds the new entry from the lineEdits
 * to the lists if valid
 */
void BluetoothConfigDialog::addEntry()
{
    QString address = manualAdditionAddress->text();
    QString name = manualAdditionName->text();


    BluetoothDeviceListItem* newDevice = new BluetoothDeviceListItem(address, name, false);
    //check if valid address was used
    if( QBluetoothAddress(newDevice->getBTAddress()).isNull())
    {
        if (infoDialog!= NULL)
        {
            delete(infoDialog);
            infoDialog = NULL;
        }

        infoDialog = new QMessageBox(QMessageBox::Warning, "Invalid Entry", "The entered device adress is not valid. Please change it and try again.", QMessageBox::Ok);
        infoDialog->show();
    }
    else {
        //make sure every adress is in the list only once
        QString label = QString("%0 : %1").arg(name).arg(newDevice->getBTAddress());
        QList<QStandardItem *> items =  deviceListModel->findItems(newDevice->getBTAddress(), Qt::MatchEndsWith);
        if (items.isEmpty())
        {
            deviceList->push_back(newDevice);
            QStandardItem *list_item = new QStandardItem(label);
            list_item->setBackground(Qt::red);
            deviceListModel->setItem(deviceListModel->rowCount() , 0, list_item);
        }
        else
        {
            if (infoDialog!= NULL)
            {
                delete(infoDialog);
                infoDialog = NULL;
            }

            infoDialog = new QMessageBox(QMessageBox::Warning, "Device doubled", "The entered device adress is already in the list.", QMessageBox::Ok);
            infoDialog->show();

        }
    }
}


/* startScan
 * slot, starts the scan for bt devices
 * to user interface
 */
void BluetoothConfigDialog::startScan()
{
    discoveryAgent->start();
    scanButton->setEnabled(false);

}



/* scanFinished
 * slot, enables the button after end of scan
 */
void BluetoothConfigDialog::scanFinished()
{
    scanButton->setEnabled(true);

}


/* addDevice
 * slot, adds a new Bluetooth Device from the discovery agent
 * to user interface
 */
void BluetoothConfigDialog::addDevice(const QBluetoothDeviceInfo &info)
{
    QString label = QString("%0 : %1").arg(info.name()).arg(info.address().toString());
    QList<QStandardItem *> items =  scanListModel->findItems(label, Qt::MatchExactly);
    if (items.empty()) {
        QStandardItem *list_item = new QStandardItem(label);
        scanListModel->setItem(scanListModel->rowCount() , 0, list_item);
    }
}


/* addScannedEntry
 * slot, adds a new entry from the scan list
 * to the device list
 */
void BluetoothConfigDialog::addScannedEntry()
{
    QModelIndexList indexes = scanList->selectionModel()->selectedRows();
    QString address;
    QString name;

    for( QModelIndexList::const_iterator it = indexes.constEnd() -1; it>=indexes.constBegin(); it--)
    {
        QStandardItem* currentItem = scanListModel->item(it->row());


        address = currentItem->text().right(17);
        name = currentItem->text().left(currentItem->text().length()-19);

         BluetoothDeviceListItem* newDevice = new BluetoothDeviceListItem(address, name, false);

        //make sure every adress is in the list only once
        QList<QStandardItem *> items =  deviceListModel->findItems(address, Qt::MatchEndsWith);
        if (items.isEmpty())
        {
            deviceList->push_back(newDevice);
            QStandardItem *list_item = new QStandardItem(currentItem->text());
            list_item->setBackground(Qt::red);
            deviceListModel->setItem(deviceListModel->rowCount() , 0, list_item);
        }
        else
        {
            if (infoDialog!= NULL)
            {
                delete(infoDialog);
                infoDialog = NULL;
            }

            infoDialog = new QMessageBox(QMessageBox::Warning, "Device doubled", "The entered device adress is already in the list.", QMessageBox::Ok);
            infoDialog->show();

        }

    }
    scanList->clearSelection();
}
