#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "zgddatabase.h"

#include <QDebug>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QHeaderView>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->showMaximized();

    this->setWindowIcon(QIcon(":/image/zgd.jpg"));

    // table wiget 0 init
    ui->lEBarCode->setFocus();
    ui->tEReceipt->setReadOnly(true);
    ui->outPrice->setDigitCount(6);
    ui->outPrice->setMode(QLCDNumber::Dec);

    //  table wiget 1 init
    QStringList commodityClass;
    commodityClass << "   饮料 " << "   香烟 " << "   酒 " << "   零食 " << "   文具 " << "   玩具 " << "   粮油 " << "   生活用品" << "   其他 ";
    ui->cBCommodityClass->addItems(commodityClass);
    ui->lETotalSales->setReadOnly(true);

    //  table wiget 2 init
    ui->lESearch->setPlaceholderText(" 搜索...");
    ui->tWWarehouse->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tWWarehouse->setColumnCount(8);
    QStringList columnHeaders;
    columnHeaders << "条形码" << "商品名" << "商品大类" << "售价(元)" << "进价(元)" << "库存(件)" << "折扣" << "总销量";
    ui->tWWarehouse->setHorizontalHeaderLabels(columnHeaders);
    ui->tWWarehouse->setAlternatingRowColors(true);
    ui->tWWarehouse->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tWWarehouse->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tWWarehouse->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    //ui->tWWarehouse->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    //ui->tWWarehouse->setColumnWidth(0,300);

    //  table wiget 3 init
    ui->tLWebView->setText(tr("<a href = https://www.taobao.com/ >点击打开订购页面</a>"));
    ui->tLWebView->setOpenExternalLinks(true);
    ui->tLviewYC->setText(tr("<a href = http://www.xinshangmeng.com/xsm2/?Version=2019121400 >中烟新商盟</a>"));
    ui->tLviewYC->setOpenExternalLinks(true);

    database = new ZgdDatabase("zgd");

    connect(ui->actionkfxx, &QAction::triggered, [=](){
                QMessageBox::information(this," 开发者信息","掌柜的软件是一款针对中小超市及其他售卖为主的个体经营户的仓储和出售软件，配合扫码枪（条形码）使用。"
                                                       "\n当前版本： v1.1.0"
                                                       "\n联系方式：1970336264@qq.com");
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pBDelete_clicked()  //从数据库中删除指定数据
{
    if(ui->tWWarehouse->currentRow() >= 0){
        QTableWidgetItem *item = ui->tWWarehouse->item(ui->tWWarehouse->currentRow(), 0);
        database->deleteData(item->text());
        emit ui->pBRefresh->clicked();
    }else{
        qDebug() << "Noting delete!";
    }
}

void MainWindow::on_pBAdd_clicked() //页面跳转至 -> 入库
{
    if (ui->tWWarehouse->currentRow() >= 0) {
        QTableWidgetItem *item = ui->tWWarehouse->item(ui->tWWarehouse->currentRow(), 0);
        if(item){
            ui->lEBarCodeIn->setText(item->text());
        }
        emit ui->lEBarCodeIn->returnPressed();
    } else {
        ui->lEBarCodeIn->setText("");
        ui->lECommodityName->setText("");
        ui->cBCommodityClass->setCurrentIndex(0);
        ui->lEPrice->setText("");
        ui->lEPriceIn->setText("");
        ui->sBCurrentCount->setValue(0);
        ui->lEDiscount->setText("");
        ui->lETotalSales->setText("");
    }
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_pBAddCommodity_clicked()    //收集入库界面信息，插入数据库
{
    ZgdData inData;
    inData.barCode = ui->lEBarCodeIn->text();
    inData.CommodityName = ui->lECommodityName->text();
    inData.CommodityClass = ui->cBCommodityClass->currentText();
    inData.priceOut = ui->lEPrice->text().toDouble();
    inData.priceIn = ui->lEPriceIn->text().toDouble();
    inData.CommodityCount = ui->sBCurrentCount->value() + ui->sBAddCount->value() - ui->sBMinusCount->value();
    inData.discount = ui->lEDiscount->text().toDouble();
    inData.totalSales = 0;
    inData.IMGPath = "";

    if (inData.barCode == "") {
        return;
    }

    if (database->insertData(inData)) {
        qDebug() << "insertData OK";
    } else {
        qDebug() << "insertData ERROR";
        QMessageBox::critical(this,"插入商品错误","请重新添加，如多次添加失败，请确认商品种类是否已经在库存，如已经在库存，更新商品信息即可！");
    }
    ui->tabWidget->setCurrentIndex(2);
    emit ui->pBRefresh->clicked();
}

static bool ClearTableWidgetItem(QTableWidget *tWWarehouse){
    int column = tWWarehouse->columnCount();
    int row = tWWarehouse->rowCount();
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            QTableWidgetItem *item = tWWarehouse->item(i, j);
            delete item;
        }
    }
    return true;
}

void MainWindow::on_pBRefresh_clicked()
{
    QList<ZgdData> dataList;
    if (database->getDataList(dataList)) {
        ClearTableWidgetItem(ui->tWWarehouse);
        ui->tWWarehouse->setRowCount(dataList.count());
        for (int i = 0; i < dataList.count(); i++) {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(dataList.value(i).barCode);
            ui->tWWarehouse->setItem(i,0,item);
            QTableWidgetItem *item1 = new QTableWidgetItem;
            item1->setText(dataList.value(i).CommodityName);
            ui->tWWarehouse->setItem(i,1,item1);
            QTableWidgetItem *item2 = new QTableWidgetItem;
            item2->setText(dataList.value(i).CommodityClass);
            ui->tWWarehouse->setItem(i,2,item2);
            QTableWidgetItem *item3 = new QTableWidgetItem;
            item3->setText(QString::number(dataList.value(i).priceOut));
            ui->tWWarehouse->setItem(i,3,item3);
            QTableWidgetItem *item4 = new QTableWidgetItem;
            item4->setText(QString::number(dataList.value(i).priceIn));
            ui->tWWarehouse->setItem(i,4,item4);
            QTableWidgetItem *item5 = new QTableWidgetItem;
            item5->setText(QString::number(dataList.value(i).CommodityCount));
            ui->tWWarehouse->setItem(i,5,item5);
            QTableWidgetItem *item6 = new QTableWidgetItem;
            item6->setText(QString::number(dataList.value(i).discount));
            ui->tWWarehouse->setItem(i,6,item6);
            QTableWidgetItem *item7 = new QTableWidgetItem;
            item7->setText(QString::number(dataList.value(i).totalSales));
            ui->tWWarehouse->setItem(i,7,item7);
        }
        ui->tWWarehouse->show();
    } else {
        qDebug() << "getDataList ERROR";
    }

}
//#include "httplib.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue> // json 解析value为数字型时有BUG 传入的json串key:value都应为字符串
void MainWindow::on_lEBarCodeIn_returnPressed() // 捕捉enter键进行商品信息查询
{
    ZgdData outData;
    QString barcode = ui->lEBarCodeIn->text();
    if (database->getDataByBarcode(barcode, outData)) {
        ui->lECommodityName->setText(outData.CommodityName);
        ui->cBCommodityClass->setCurrentText(outData.CommodityClass);
        ui->lEPrice->setText(QString::number(outData.priceOut));
        ui->lEPriceIn->setText(QString::number(outData.priceIn));
        ui->sBCurrentCount->setValue(outData.CommodityCount);
        ui->sBAddCount->setValue(0);
        ui->sBMinusCount->setValue(0);
        ui->lEDiscount->setText(QString::number(outData.discount));
        ui->lETotalSales->setText(QString::number(outData.totalSales));
        if (outData.IMGPath == "") {   // == test
            qDebug() << outData.IMGPath;

            QPixmap pixmap("./img/test.jpg");
            QSize sz(500, 500);
            pixmap = pixmap.scaled(sz, Qt::KeepAspectRatio);
            ui->lPicture->setPixmap(pixmap);

//            QImage *img=new QImage; //新建一个image对象
//            qDebug() << img->load("./img/test.jpg"); //将图像资源载入对象img，注意路径，可点进图片右键复制路径
//            ui->lPicture->setPixmap(QPixmap::fromImage(*img));
        }

    } else {
        qDebug() << "getDataByBarcode Not Found";
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        QNetworkRequest request;
//        QString url_base = "http://192.168.1.101:5215/spinfo?barcode=%1";
        QString url_base = "http://124.70.144.62:5215/spinfo?barcode=%1";
        QString url = url_base.arg(barcode);
        request.setUrl(QUrl(url));

        QNetworkReply *reply = manager->get(request);
        connect(manager,&QNetworkAccessManager::finished, [=](){
                    qDebug() << "QNetworkAccessManager::finished";
                    QString rep_json = reply->readAll();
                    qDebug() << qPrintable(rep_json.toUtf8());
                    QJsonParseError parseJsonErr;
                    QJsonDocument document = QJsonDocument::fromJson(rep_json.toUtf8(),&parseJsonErr);
                    if(!(parseJsonErr.error == QJsonParseError::NoError)){
                        qDebug()<<tr("解析json文件错误！");
                        return;
                    }
                    QJsonObject jsonObject = document.object();
                    qDebug()<<"jsonObject[barcode]=="<<jsonObject["barcode"].toString();
                    if(jsonObject.contains("ErrorCode")){
                        int ErrorCode = jsonObject["ErrorCode"].toInt();
                        if(ErrorCode){
                            if(ErrorCode == 1){
                                qDebug()<<tr("barcode 格式错误");
                            }
                            return;
                        }
                    }

                    if(jsonObject.contains("CommodityName")){
                        QString barcode_str = jsonObject["CommodityName"].toString();
                        ui->lECommodityName->setText(barcode_str);
                    }
                    ui->cBCommodityClass->setCurrentIndex(-1);
                    ui->lEPrice->setText(QString::number(0));
                    ui->lEPriceIn->setText(QString::number(0));
                    ui->sBCurrentCount->setValue(0);
                    ui->sBAddCount->setValue(0);
                    ui->sBMinusCount->setValue(0);
                    ui->lEDiscount->setText(QString::number(0));
                    ui->lETotalSales->setText(QString::number(0));
                });

    }
}

void MainWindow::on_pBUpdate_clicked()
{
    ZgdData inData;
    if (database->getDataByBarcode(ui->lEBarCodeIn->text(), inData)) {
        inData.barCode = ui->lEBarCodeIn->text();
        inData.CommodityName = ui->lECommodityName->text();
        inData.CommodityClass = ui->cBCommodityClass->currentText();
        inData.priceOut = ui->lEPrice->text().toDouble();
        inData.priceIn = ui->lEPriceIn->text().toDouble();
        inData.CommodityCount = ui->sBCurrentCount->value() + ui->sBAddCount->value() - ui->sBMinusCount->value();
        inData.discount = ui->lEDiscount->text().toDouble();
        inData.totalSales = ui->lETotalSales->text().toInt();

        if(database->updateTable(inData)){
            emit ui->pBRefresh->clicked();
            emit ui->lEBarCodeIn->returnPressed();
        }else{
            QMessageBox::critical(this,"未知商品","请确认二维码，若多次失败，请确认商品是否入库！");
        }
    } else {
        QMessageBox::critical(this,"未知商品","请确认二维码，若多次失败，请确认商品是否入库！");

    }
}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    if (index == 0) {
        ui->lEBarCode->setFocus();
    } else if (index == 1) {
        emit ui->pBAdd->clicked();
        ui->sBAddCount->setFocus();
    } else if (index == 2) {
        emit ui->pBRefresh->clicked();
    } else if (index == 3) {

    }
}

void MainWindow::on_lEBarCode_returnPressed()
{
    if ((ui->lEBarCode->text() == "") && (commodityList.count() > 0)) {
        emit ui->pBSell->clicked();
        return;
    }
    ZgdData outData;
    QString barcode = ui->lEBarCode->text();
    if (barcode.startsWith("+")) {  // startsWith(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const
        totalMoney += barcode.toDouble();
        ui->tEReceipt->append(QString("商品名：未入库商品 \n商品价格：%2  折扣 无").arg(barcode.toDouble()));
        ui->outPrice->display(QString::number(totalMoney));
        ui->lEBarCode->clear();
        return;
    }
    if (database->getDataByBarcode(barcode, outData)) {
        if((outData.discount > 1) || (outData.discount <= 0)){
            outData.discount = 1;
        }
        totalMoney += outData.priceOut * outData.discount;
        commodityList.append(outData);
        ui->outPrice->display(QString::number(totalMoney));
        QString receipt = QString("商品名：%1 \n商品单价：%2  小计 %3").arg(outData.CommodityName)
                                                                    .arg(outData.priceOut)
                                                                    .arg(outData.priceOut * outData.discount);
        ui->tEReceipt->append(receipt);
        ui->lEBarCode->clear();
    } else {
        QMessageBox::critical(this,"找不到商品","请重新扫码，若多次失败，请确认商品是否入库！");
        ui->lEBarCode->clear();
    }

}

void MainWindow::on_pBClear_clicked()   // 清空显示界面
{
    ui->outPrice->display(0.0);
    ui->tEReceipt->clear();
    ui->lEBarCode->setText("");
    totalMoney = 0;
    ui->lEBarCode->setFocus();
    commodityList.clear();
}

static bool CommodityINCountList(ZgdData &commodityList, QList<CommodityCountList> &countList){
    for (int i = 0; i < countList.count(); i++) {
        if (commodityList.barCode == countList.value(i).data.barCode) {
            return true;
        }
    }
    return false;
}

void MainWindow::on_pBSell_clicked()   // 对sql进行更新
{
    ui->outPrice->display(0.0);
    ui->tEReceipt->clear();
    ui->lEBarCode->setText("");

    QList<CommodityCountList> countList;
    for (int i = 0; i < commodityList.count(); i++) {
        if (CommodityINCountList(commodityList[i], countList)) {   //判断countlist中已有commodity
            for (int j = 0; j < countList.count(); j++) {
                if (countList.value(j).data.barCode == commodityList.value(i).barCode) {
                    countList[j].count++;
                }
            }
        } else {                                                   //没有则添加
            CommodityCountList tmpCountList;
            tmpCountList.data = commodityList[i];
            tmpCountList.count = 1;
            countList.append(tmpCountList);
        }
    }
    for (int i = 0; i < countList.count(); i++) {
        ZgdData tmpdata = countList.value(i).data;
        tmpdata.CommodityCount = tmpdata.CommodityCount - countList.value(i).count;
        if (tmpdata.CommodityCount <= 0) {
            tmpdata.CommodityCount = 0;
        }
        tmpdata.totalSales += countList.value(i).count;
        database->updateTable(tmpdata);
    }
    totalMoney = 0;
    ui->lEBarCode->setFocus();
    commodityList.clear();
}

void MainWindow::on_lESearch_textChanged(const QString &arg1)
{
    if (ui->tWWarehouse->rowCount() <= 0) {
        return;
    }
    if (ui->lESearch->text() == "") {
        for(int i = 0; i < ui->tWWarehouse->rowCount(); i++){
            ui->tWWarehouse->showRow(i);
        }
        return;
    } else {
        for (int i = 0; i < ui->tWWarehouse->rowCount(); i++) {
            for (int j = 0; j < ui->tWWarehouse->columnCount(); j++) {
                QTableWidgetItem *item = ui->tWWarehouse->item(i, j);
                if (item->text().contains(arg1)) {
                    ui->tWWarehouse->showRow(i);
                    break;
                } else {
                    ui->tWWarehouse->hideRow(i);
                }
            }
        }
    }
}

void MainWindow::keyPressEvent( QKeyEvent * ev )
{
    if (ui->tabWidget->currentIndex() == 0) {
        if (ev->key() == Qt::Key_Escape) {
            if ((ui->lEBarCode->text() == "") && (commodityList.count() > 0)) {
                emit ui->pBClear->clicked();
                return;
            }
        }
    }

}
