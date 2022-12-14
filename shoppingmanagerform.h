#ifndef SHOPPINGMANAGERFORM_H
#define SHOPPINGMANAGERFORM_H

#include <QTreeWidgetItem>
#include <QWidget>

#include "shoppingitem.h"

class QMenu;
class ClientItem;
class ProductItem;
class ProductManagerForm;

namespace Ui {
class ShoppingManagerForm;
}

class ShoppingManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ShoppingManagerForm(QWidget *parent = nullptr);
    ~ShoppingManagerForm();

signals:
    void clientDataSent(QString);
    void dataSent(QString);
    void clientIdSent(int);
    void productIdSent(int);
    void inventorySent(int, int);
    void categoryDataSent(QString);

private slots:
    /* QTreeWidget을 위한 슬롯 */
    void showContextMenu(const QPoint&);
    void removeItem();
    void receiveData(ClientItem*);
    void shopReceiveData(ProductItem*);
    void on_showLineEdit_returnPressed();
    void on_clientTreeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void on_productTreeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void on_shopTreeWidget_itemClicked(QTreeWidgetItem *item, int column);
    /* QAction을 위한 슬롯 */
    void on_addPushButton_clicked();
    void on_modifyPushButton_clicked();
    void on_clearPushButton_clicked();
    void on_searchPushButton_clicked();

private:
    int makeId();   //lastKey()를 활용한 key 생성
    QMap<int, ShoppingItem*> shopList;
    QMenu* menu;
    Ui::ShoppingManagerForm *ui;
};

#endif // SHOPPINGMANAGERFORM_H
