
#include "productitem.h"
#include <QLabel>
#include <iostream>

using namespace std;

ProductItem::ProductItem(int id, QString productName, int price, int stock, QString category)
{
    setText(0, QString::number(id));        // 1번째 index에 id 입력
    setText(1, productName);
    setText(2, QString::number(price));
    setText(3, QString::number(stock));
    setText(4, category);
}

int ProductItem::id() const
{
    return text(0).toInt();
}

QString ProductItem::getName() const
{
    return text(1);
}

void ProductItem::setName(QString productName)
{
    setText(1, productName);
}

int ProductItem::getPrice() const
{
    return text(2).toInt();
}

void ProductItem::setPrice(int price)
{
    setText(2, QString::number(price));
}

int ProductItem::getStock() const {
    return text(3).toInt();
}

void ProductItem::setStock(int stock)
{
    setText(3, QString::number(stock));
}

QString ProductItem::getCategory() const
{
    return text(4);
}

void ProductItem::setCategory(QString category)
{
    setText(4, category);
}

// ProductItem끼리 비교할 때 고객의 이름으로 비교
bool ProductItem::operator==(const ProductItem& other) const
{
    return (this->text(1) == other.text(1));
}
