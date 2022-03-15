#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "zgddatabase.h"

struct CommodityCountList
{
    ZgdData data;
    int count = 0;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
     virtual void keyPressEvent( QKeyEvent * ev );

private slots:
    void on_pBDelete_clicked();

    void on_pBAdd_clicked();

    void on_pBAddCommodity_clicked();

    void on_pBRefresh_clicked();

    void on_lEBarCodeIn_returnPressed();

    void on_pBUpdate_clicked();

    void on_tabWidget_tabBarClicked(int index);

    void on_lEBarCode_returnPressed();

    void on_pBClear_clicked();

    void on_pBSell_clicked();

    void on_lESearch_textChanged(const QString &arg1);

    //void on_tabWidget_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    ZgdDatabase *database;
    QList<ZgdData> commodityList;
    double totalMoney;
};
#endif // MAINWINDOW_H
