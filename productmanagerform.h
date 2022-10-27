#ifndef PRODUCTMANAGERFORM_H
#define PRODUCTMANAGERFORM_H

#include <QWidget>
#include <QHash>

#include "productitem.h"

class QMenu;
class QTreeWidgetItem;

namespace Ui {
    class ProductManagerForm;
}

class ProductManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProductManagerForm(QWidget* parent = nullptr);
    ~ProductManagerForm();

signals:
    // shoppingManagerForm에서 id나 제품 이름, 품목을 가져오면
    // 그에 맞는 item을 전달해주는 시그널
    void producdataSent(ProductItem*);

private slots:
    /* QTreeWidget을 위한 슬롯 */
    void showContextMenu(const QPoint&);
    void removeItem();
    void receiveName(QString);
    void receiveName(int);
    void receiveCategory(QString);
    void inventoryChanged(int, int);
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    /* QAction을 위한 슬롯 */
    void on_clearPushButton_clicked();
    void on_addPushButton_clicked();
    void on_modifyPushButton_clicked();
    void on_searchPushButton_clicked();

private:
    int makeId();   //lastKey()를 활용한 key 생성
    QMap<int, ProductItem*> productList;
    Ui::ProductManagerForm* ui;
    QMenu* menu;
};

#endif // PRODUCTMANAGERFORM_H

