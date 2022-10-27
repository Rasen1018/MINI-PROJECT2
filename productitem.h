#ifndef PRODUCTITEM_H
#define PRODUCTITEM_H

#include <QTreeWidgetItem>

class ProductItem : public QTreeWidgetItem      // QTreeWidgetItem을 상속
{
public:
    explicit ProductItem(int =0, QString="", int=0, int=0, QString="");

    int id() const;                 // 제품 id getter 함수
    QString getName() const;        // 제품 이름 getter, setter 함수
    void setName(QString);
    int getPrice() const;           // 제품 가격 getter, setter 함수
    void setPrice(int);
    int getStock() const;           // 재고 수량 getter, setter 함수
    void setStock(int);
    QString getCategory() const;    // 품목 getter, setter 함수
    void setCategory(QString);
    bool operator==(const ProductItem& other) const;    // 오퍼레이터 연산자 오버로딩
};

#endif // PRODUCTITEM_H
