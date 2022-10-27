#ifndef CLIENTITEM_H
#define CLIENTITEM_H

#include <QTreeWidgetItem>

class ClientItem : public QTreeWidgetItem       //QTreeWidgetItem을 상속
{
public:
    explicit ClientItem(int id = 0, QString = "", QString ="", int age =0, QString = "", QString = "");

    QString getName() const;        //고객 이름 getter, setter 함수
    void setName(QString&);
    QString getGender() const;      //고객 성별 getter, setter 함수
    void setGender(QString&);
    int getAge() const;             //고객 나이 getter, setter 함수
    void setAge(int);
    QString getPhoneNumber() const; //고객 전화번호 getter, setter 함수
    void setPhoneNumber(QString&);
    QString getAddress() const;     //고객 주소 getter, setter 함수
    void setAddress(QString&);
    int id() const;                 //고객 id getter 함수
    bool operator==(const ClientItem &other) const;     //오퍼레이터 연산자 오버로딩
};

#endif // CLIENTITEM_H
