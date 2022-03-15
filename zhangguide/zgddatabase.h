#ifndef STUDENTDATABASE_H
#define STUDENTDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QSqlError>

struct ZgdData
{
    QString barCode;
    QString CommodityName;
    QString CommodityClass;
    QString IMGPath;
    double priceOut;
    double priceIn;
    int CommodityCount;
    double discount;
    int totalSales;
};

class ZgdDatabase : public QObject
{
    Q_OBJECT
public:
    explicit ZgdDatabase(QString name, QObject *parent = nullptr);
    ~ZgdDatabase();
public:
    bool insertData(ZgdData &data);
    bool deleteData(QString name);
    bool getDataByBarcode(QString barCode, ZgdData &data);
    bool getDataList(QList<ZgdData> &list);
    bool updateTable(ZgdData &data);

private:
    bool initTable();
    bool isExistTable(QString table);
    bool createTable(QString table);

private:
    QSqlDatabase m_DataBase;

signals:

public slots:
};

#endif // STUDENTDATABASE_H
