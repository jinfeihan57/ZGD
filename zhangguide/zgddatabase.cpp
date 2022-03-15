#include "zgddatabase.h"
#include <QCoreApplication>
#include <QSqlRecord>
#include <QVariant>
#include <QDebug>
#include <QSqlRecord>

ZgdDatabase::ZgdDatabase(QString name, QObject *parent) : QObject(parent)
{
    name = name + ".db";    // real database file name;
    if (QSqlDatabase::contains(name)) {
        m_DataBase = QSqlDatabase::database(name);
    } else {
        m_DataBase = QSqlDatabase::addDatabase("QSQLITE");
        m_DataBase.setDatabaseName(QCoreApplication::applicationDirPath()+"/"+ name);
    }
    this->initTable();
}

ZgdDatabase::~ZgdDatabase()
{

}

bool ZgdDatabase::initTable()
{
    if (!m_DataBase.open()) {
        return false;
    }

    if (!isExistTable("productinfo")) {
        qDebug() << "createTable(productinfo)";
        createTable("productinfo");
    }
    return true;
}

bool ZgdDatabase::isExistTable(QString table)
{
    bool bRet = false;
    if (!m_DataBase.open()) {
        return bRet;
    }
    QSqlQuery query(m_DataBase);
    query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg(table));    //关键的判断
    if (query.next())
    {
        if (query.value(0).toInt() > 0)
        {
            bRet = true;
        }
    }
    return bRet;
}

bool ZgdDatabase::createTable(QString table)
{
    if (!m_DataBase.open()) {
        return false;
    }
    QSqlQuery query(m_DataBase);

    bool success = query.exec(QString("CREATE TABLE IF NOT EXISTS %1 ("
                                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                "barCode VARCHAR(128) NOT NULL UNIQUE, "
                                "CommodityName VARCHAR(256) NOT NULL, "
                                "CommodityClass VARCHAR(128) NOT NULL, "
                                "priceOut DOUBLE NOT NULL, "
                                "priceIn DOUBLE NOT NULL, "
                                "CommodityCount INTEGER NOT NULL, "
                                "discount DOUBLE NOT NULL, "
                                "totalSales INTEGER NOT NULL , "
                                "IMGPath VARCHAR(256) NOT NULL)").arg(table));
    if (success) {
        qDebug() << "新建数据库成功";
        return true;   //新建数据库成功
    }
    else {
        QSqlError lastError = query.lastError();
        QString errStr = lastError.driverText();
        qDebug() << errStr;
        return false;
    }

}

bool ZgdDatabase::insertData(ZgdData &data)
{
    if (!m_DataBase.open()) {
        return false;
    }
    QSqlQuery query(m_DataBase);
    bool success = query.exec(QString("INSERT INTO productinfo (barCode,"
                      "CommodityName, CommodityClass, priceOut, priceIn, CommodityCount, "
                      "discount, totalSales, IMGPath) VALUES ('%1', '%2', '%3', '%4', '%5', '%6', "
                      "'%7', '%8', '%9')").arg(data.barCode).arg(data.CommodityName)
                                    .arg(data.CommodityClass).arg(data.priceOut)
                                    .arg(data.priceIn).arg(data.CommodityCount)
                                    .arg(data.discount).arg(data.totalSales).arg(data.IMGPath));

    if (!success) {
        QSqlError lastError = query.lastError();
        QString errStr = lastError.driverText();
        qDebug() << errStr;
        return false;
    }
    return true;
}

bool ZgdDatabase::deleteData(QString barCode)
{
    if (!m_DataBase.open()) {
        return false;
    }
    QSqlQuery query(m_DataBase);
    query.prepare(QString("DELETE FROM productinfo WHERE barCode='%1'").arg(barCode));
    query.exec();
    return true;
}



bool ZgdDatabase::getDataByBarcode(QString barCode, ZgdData &data)
{
    if (!m_DataBase.open()) {
        return false;
    }

    QSqlQuery query(m_DataBase);
    query.prepare(QString("SELECT * FROM productinfo WHERE barCode='%1'").arg(barCode));
    query.exec();
    QSqlRecord rec = query.record();
    if (query.next())
    {
        data.barCode = query.value(1).toString();
        data.CommodityName = query.value(2).toString();
        data.CommodityClass = query.value(3).toString();
        data.priceOut = query.value(4).toDouble();
        data.priceIn = query.value(5).toDouble();
        data.CommodityCount = query.value(6).toInt();
        data.discount = query.value(7).toDouble();
        data.totalSales = query.value(8).toInt();
        data.IMGPath = query.value(9).toString();
        return true;
    }
    return false;
}

bool ZgdDatabase::getDataList(QList<ZgdData> &list)
{
    if (!m_DataBase.open()) {
        return false;
    }

    QSqlQuery query(m_DataBase);
    query.prepare(QString("SELECT * FROM productinfo"));
    query.exec();
    QSqlRecord rec = query.record();
    while (query.next())
    {
        ZgdData data;
        data.barCode = query.value(1).toString();
        data.CommodityName = query.value(2).toString();
        data.CommodityClass = query.value(3).toString();
        data.priceOut = query.value(4).toDouble();
        data.priceIn = query.value(5).toDouble();
        data.CommodityCount = query.value(6).toInt();
        data.discount = query.value(7).toDouble();
        data.totalSales = query.value(8).toInt();
        list.append(data);
    }
    return true;
}

bool ZgdDatabase::updateTable(ZgdData &data)
{
    if (!m_DataBase.open()) {
        return false;
    }
    if (data.barCode == "") {
        return false;
    }
    QSqlQuery query(m_DataBase);
    query.prepare(QString("UPDATE productinfo SET CommodityName = '%1',"
                          "CommodityClass = '%2',"
                          "priceOut = %3,"
                          "priceIn = %4,"
                          "CommodityCount = %5,"
                          "discount = %6,"
                          "totalSales = %7 WHERE barCode = '%8'").arg(data.CommodityName).arg(data.CommodityClass).arg(data.priceOut)
                                                                 .arg(data.priceIn).arg(data.CommodityCount).arg(data.discount)
                                                                 .arg(data.totalSales).arg(data.barCode));
    return query.exec();
}
