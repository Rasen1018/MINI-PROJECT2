#ifndef SHOPPINGITEM_H
#define SHOPPINGITEM_H

#include <QTreeWidgetItem>

class ShoppingItem : public QTreeWidgetItem     //QTreeWidgetItem을 상속
{
public:
    explicit ShoppingItem(int =0, int =0, int=0, QString="", int=0, int=0);

    int id() const;                  // 쇼핑 목록 ID getter 함수
    int getCID() const;              // 고객 ID getter, setter 함수
    void setClient(int clinetId);
    int getPID() const;              // 제품 ID getter, setter 함수
    void setProduct(int productId);
    QString getTime() const;         // 주문 일자 getter, setter 함수
    void setTime(QString sellTime);
    int getAmount() const;           // 주문 수량 getter, setter 함수
    void setAmount(int sellAmount);
    int getTotalPrice() const;       // 주문 총액 getter, setter 함수
    void setTotalPrice(int totalPrice);
    bool operator==(const ShoppingItem& other) const;     // 오퍼레이터 연산자 오버로딩
};

#endif // SHOPPINGITEM_H
