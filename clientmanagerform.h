#ifndef CLIENTMANAGERFORM_H
#define CLIENTMANAGERFORM_H

#include <QWidget>
#include <QHash>

#include "clientitem.h"

class QMenu;
class QTreeWidgetItem;

namespace Ui {
class ClientManagerForm;
}

class ClientManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ClientManagerForm(QWidget *parent = nullptr);
    ~ClientManagerForm();

signals:
    // shoppingManagerForm에서 id나 이름을 가져오면 그에 맞는 item을 전달해주는 시그널
    void clientDataSent(ClientItem*);
    // 고객 리스트의 id와 이름을 QStringList에 저장해서 전달해주는 시그널
    void getAllClient(QStringList);

private slots:
    /* QTreeWidget을 위한 슬롯 */
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void showContextMenu(const QPoint &);
    void removeItem();
    void receiveData(QString);
    void receiveData(int);
    /* QAction을 위한 슬롯 */
    void on_addPushButton_clicked();
    void on_modifyPushButton_clicked();
    void on_searchPushButton_clicked();
    void on_clearPushButton_clicked();
    /*채팅 서버에 전체 고객 리스트 가져오기*/
    void receiveAllClient();

private:
    int makeId();   //lastKey()를 활용한 key 생성
    QMap<int, ClientItem*> clientList;
    Ui::ClientManagerForm *ui;
    QMenu* menu;
};

#endif // CLIENTMANAGERFORM_H
