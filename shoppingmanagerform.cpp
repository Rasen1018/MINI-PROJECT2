#include "shoppingmanagerform.h"
#include "ui_shoppingmanagerform.h"
#include "clientitem.h"
#include "productitem.h"
#include "shoppingitem.h"

#include <QFile>
#include <QMenu>
#include <QMessageBox>

ShoppingManagerForm::ShoppingManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShoppingManagerForm)
{
    ui->setupUi(this);

    QList<int> sizes;
    sizes << 640 << 480;            // 위젯 사이즈 설정
    ui->splitter->setSizes(sizes);

    // 주문 수량이 바뀔때마다 자동으로 총액을 구해서 입력해주는 lambda식
    connect(ui->orderLineEdit, &QLineEdit::textChanged, this, [=](){
        QString order = ui->orderLineEdit->text();

        // 제품 정보를 보여주는 TreeWidget이 없을 경우 주문 목록 TreeWidget에서 가격과 주문 총액을 계산
        if(ui->productTreeWidget->currentItem()==nullptr) {
            int price =  // 가격 = 주문 수량/주문 총액
                    (ui->shopTreeWidget->currentItem()->text(5).toInt())/(ui->shopTreeWidget->currentItem()->text(4).toInt());

            int totalAmount = price * order.toInt();    //주문 총액
            ui->totalLineEdit->setText(QString::number(totalAmount));
        }
        // 제품 정보를 보여주는 TreeWidget이 있을 경우
        else {
            QString price = ui->productTreeWidget->currentItem()->text(2);  // 가격 가져옴
            int amount = price.toInt() * order.toInt();
            ui->totalLineEdit->setText(QString::number(amount));    // 주문 총액 계산
        }
    });

    // 주운 목록 삭제 Action
    QAction* removeAction = new QAction(tr("&Remove"));
    connect(removeAction, SIGNAL(triggered()), this, SLOT(removeItem()));

    // 삭제 Action 우클릭 메뉴에 추가
    menu = new QMenu;
    menu->addAction(removeAction);
    ui->shopTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->shopTreeWidget, SIGNAL(customContextMenuRequested(QPoint)),
        this, SLOT(showContextMenu(QPoint)));

    // header 사이즈 설정
    ui->shopTreeWidget->header()->
            setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->clientTreeWidget->header()->
            setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->productTreeWidget->header()->
            setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->searchTreeWidget->header()->
            setSectionResizeMode(QHeaderView::ResizeToContents);

    // 엔터 입력시 검색 버튼 클릭
    connect(ui->searchLineEdit, SIGNAL(returnPressed()),
        this, SLOT(on_searchPushButton_clicked()));

    QFile file("shoplist.txt");     // 파일 불러오기
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");    // ',' 기준으로 쇼핑 목록 정보 불러오기
        if (row.size()) {
            int id = row[0].toInt();        // string -> int 변환
            int CID = row[1].toInt();
            int PID = row[2].toInt();
            int order = row[4].toInt();
            int totalPrice = row[5].toInt();
            ShoppingItem* s = new ShoppingItem(id, CID, PID, row[3], order, totalPrice);
                                            // id, clientId, productId, sellTime, sellAmount, totalPrice
            ui->shopTreeWidget->addTopLevelItem(s);
            shopList.insert(id, s);
        }
    }
    file.close( );  // 파일 닫기
}

ShoppingManagerForm::~ShoppingManagerForm()
{
    delete ui;
    QFile file("shoplist.txt");     // 파일 저장
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : shopList) {    // Map에 있던 Item들 하나씩 저장
        ShoppingItem* s = v;
        out << s->id() << ", " << s->getCID() << ", ";    //','로 정보 파싱
        out << s->getPID() << ", ";
        out << s->getTime() << ", ";
        out << s->getAmount() << ", ";
        out << s->getTotalPrice() << "\n";     // Item의 정보를 다 저장하면 줄바꿈
    }
    file.close( );    // 파일 닫기
}

int ShoppingManagerForm::makeId()       // key 생성
{
    if (shopList.size()==0)     // 주문 목록 리스트가 없을 때
    {
        return 110001;          // ID 생성
    }
    else {
        auto id = shopList.lastKey();   // 있을 때 마지막 키값에 +1
        return ++id;
    }
}

void ShoppingManagerForm::showContextMenu(const QPoint& pos) {

    QPoint globalPos = ui->shopTreeWidget->mapToGlobal(pos);    // 우클릭한 위치에 메뉴 표시
    menu->exec(globalPos);
}

void ShoppingManagerForm::removeItem() {    // 아이템 삭제 함수

    QTreeWidgetItem* item = ui->shopTreeWidget->currentItem();     // 현재 Item 삭제
    if (item != nullptr) {
        shopList.remove(item->text(0).toInt());
        ui->shopTreeWidget->takeTopLevelItem(ui->shopTreeWidget->indexOfTopLevelItem(item));
        // 제거후 업데이트
        ui->shopTreeWidget->update();
    }
}

void ShoppingManagerForm::receiveData(ClientItem *c)
    // ClientManagerForm에서 id, 고객 이름으로 검색된 Item을 가져오는 슬롯
{
    int id = c->id();
    QString name = c->getName();
    QString gender = c->getGender();
    int age = c->getAge();
    QString pNumber = c->getPhoneNumber();
    QString address = c->getAddress();
    ClientItem* item = new ClientItem(id, name, gender, age, pNumber, address);
    ui->clientTreeWidget->addTopLevelItem(item);     // 고객 정보 TreeWidget에 추가
}

void ShoppingManagerForm::shopReceiveData(ProductItem *p)
    // ProductManagerForm에서 id, 제품 이름, 품목으로 검색된 Item을 가져오는 슬롯
{

    int id = p->id();
    QString name = p->getName();
    int price = p->getPrice();
    int stock = p->getStock();
    QString category = p->getCategory();
    ProductItem* item = new ProductItem(id, name, price, stock, category);
    ui->productTreeWidget->addTopLevelItem(item);     // 제품 정보 TreeWidget에 추가

}

// ShoppingManagerForm에서 고객 리스트, 제품 리스트를 검색해서 띄워주기
void ShoppingManagerForm::on_showLineEdit_returnPressed()
{
    int i = ui->showComboBox->currentIndex();

    if(i==0) {      // 고객 이름으로 검색할 경우
        ui->clientTreeWidget->clear();
        QString name = ui->showLineEdit->text();
        emit clientDataSent(name);      // 고객 이름 전달해주는 시그널 발생
    }

    if(i==1) {      // 제품 이름으로 검색할 경우
        ui->productTreeWidget->clear();
    QString name = ui->showLineEdit->text();
        emit dataSent(name);            // 제품 이름 전달해주는 시그널 발생
    }

    if(i==2) {      // 제품 품목으로 검색할 경우
        ui->productTreeWidget->clear();
        QString name = ui->showLineEdit->text();
        emit categoryDataSent(name);    // 제품 품목 전달해주는 시그널 발생
    }
}

void ShoppingManagerForm::on_clientTreeWidget_itemClicked   // 고객 정보 TreeWidget 클릭시
(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    ui->clientNameLineEdit->setText(item->text(0));     // 고객 이름 입력
}


void ShoppingManagerForm::on_productTreeWidget_itemClicked   // 제품 정보 TreeWidget 클릭시
(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    ui->pdNameLlineEdit->setText(item->text(0));    // 제품 이름 입력
    QString order = ui->orderLineEdit->text();
    // 주문량이 없다면 주문 총액에 제품 가격 입력
    if(order=="") {
    ui->totalLineEdit->setText(item->text(2));
    }
    // 주문량이 있다면 총액 입력
    else {
        int amount = order.toInt()*(item->text(2).toInt());     // 총액 계산
        ui->totalLineEdit->setText(QString::number(amount));
    }
}

void ShoppingManagerForm::on_shopTreeWidget_itemClicked     // 주문 목록 Item 클릭시
(QTreeWidgetItem *item, int column)
{
    ui->clientTreeWidget->clear();
    ui->productTreeWidget->clear();
    Q_UNUSED(column);
    ui->idLineEdit->setText(item->text(0));         // TreeWidget의 아이템의 내용을 LineEdit에 입력
    ui->clientNameLineEdit->setText(item->text(1));
    ui->pdNameLlineEdit->setText(item->text(2));
    ui->timeLlineEdit->setText(item->text(3));
    ui->orderLineEdit->setText(item->text(4));
    ui->totalLineEdit->setText(item->text(5));
    emit clientIdSent(item->text(1).toInt());       // 고객 ID 전달하는 시그널 발생
    emit productIdSent(item->text(2).toInt());      // 제품 ID 전달하는 시그널 발생
}

void ShoppingManagerForm::on_addPushButton_clicked()    // 추가 버튼 클릭시
{
    QString time;
    int CID, PID, order, totalPrice;
    int id = makeId( );
    CID= ui->clientNameLineEdit->text().toInt();
    PID = ui->pdNameLlineEdit->text().toInt();
    time = ui->timeLlineEdit->text();
    totalPrice = ui->totalLineEdit->text().toInt();

    // 기존의 주문 목록을 이용하여 주문하는 경우(트리 위젯 X)
    if (ui->productTreeWidget->currentItem() == nullptr) {
        int stock = ui->productTreeWidget->topLevelItem(0)->text(3).toInt();
        qDebug() << stock;
        order = ui->orderLineEdit->text().toInt();

        if (stock > order) {       // 재고가 주문보다 많을 경우 정상 주문
            emit(inventorySent(PID, order));    // 제품 ID와 주문량을 전달해주는 시그널 발생
            ShoppingItem* s = new ShoppingItem(id, CID, PID, time, order, totalPrice);
            shopList.insert(id, s);
            ui->shopTreeWidget->addTopLevelItem(s);
        }

        else if (stock < order) {       // 재고가 부족할 경우
            QMessageBox::warning(this, tr("ERROR"), tr("Inventory Overflow"));      // 오류 메세지 표시
            return;
        }
    }

    else {      // 제품 정보 보여주는 트리 위젯이 있을 경우
        int stock = ui->productTreeWidget->currentItem()->text(3).toInt();
        order = ui->orderLineEdit->text().toInt();

        if (stock < order) {        // 재고가 부족할 경우
            QMessageBox::warning(this, tr("ERROR"), tr("Inventory Overflow"));      // 오류 메세지 표시
            return;
        }

        else if (stock > order) {         // 재고가 주문보다 많을 경우 정상 주문
            emit(inventorySent(PID, order));    // 제품 ID와 주문량을 전달해주는 시그널 발생
            ShoppingItem* s = new ShoppingItem(id, CID, PID, time, order, totalPrice);
            shopList.insert(id, s);
            ui->shopTreeWidget->addTopLevelItem(s);
        }
    }
}

void ShoppingManagerForm::on_modifyPushButton_clicked()     // 수정버튼 클릭시
{
    QTreeWidgetItem* item = ui->shopTreeWidget->currentItem();
    if (item != nullptr) {
        int key = item->text(0).toInt();
        ShoppingItem* s = shopList[key];
        QString time;
        int CID, PID, order, totalPrice, prevOrder;
        CID= ui->clientNameLineEdit->text().toInt();
        PID = ui->pdNameLlineEdit->text().toInt();
        time = ui->timeLlineEdit->text();
        totalPrice = ui->totalLineEdit->text().toInt();
        prevOrder = item->text(4).toInt();      // 기존의 주문량 저장

        // 기존의 주문 목록을 이용하여 주문하는 경우(트리 위젯 X)
        if (ui->productTreeWidget->currentItem() == nullptr) {
            int stock = ui->productTreeWidget->topLevelItem(0)->text(3).toInt();
            qDebug() << stock;
            order = ui->orderLineEdit->text().toInt();

            if (stock > (order-prevOrder)) {    // 현재 주문량이 재고보다 적을 경우 정상 주문
                emit(inventorySent(PID, (order-prevOrder)));
                s->setClient(CID);
                s->setProduct(PID);
                s->setTime(time);
                s->setAmount(order);
                s->setTotalPrice(totalPrice);
                shopList[key] = s;
            }

            else if (stock < (order-prevOrder)) {     // 현재 주문량이 재고보다 많을 경우
                QMessageBox::warning(this, tr("ERROR"), tr("Inventory Overflow"));      // 에러메세지 표시
                return;
            }
        }

        else {      // 제품 정보 보여주는 트리 위젯이 있을 경우
            int stock = ui->productTreeWidget->currentItem()->text(3).toInt();
            order = ui->orderLineEdit->text().toInt();

            if (stock < (order-prevOrder)) {     // 재고가 부족하다면
                QMessageBox::warning(this, tr("ERROR"), tr("Inventory Overflow"));      // 에러메세지 표시
                return;
            }

            else if (stock > (order-prevOrder)) {      // 현재 주문량이 재고보다 적다면
                emit(inventorySent(PID, (order-prevOrder)));    // 현재 주문량을 전달하는 시그널 발생
                s->setClient(CID);
                s->setProduct(PID);
                s->setTime(time);
                s->setAmount(order);
                s->setTotalPrice(totalPrice);
                shopList[key] = s;     // 주문량 변경
            }
        }
    }
}

void ShoppingManagerForm::on_clearPushButton_clicked()      // 클리어 버튼 클릭시
{
    ui->idLineEdit->clear();
    ui->clientNameLineEdit->clear();
    ui->pdNameLlineEdit->clear();
    ui->timeLlineEdit->clear();
    ui->orderLineEdit->clear();
    ui->totalLineEdit->clear();
}


void ShoppingManagerForm::on_searchPushButton_clicked()     // 검색 버튼 클릭시
{
    ui->searchTreeWidget->clear();
    int i = ui->searchComboBox->currentIndex();

    if (i == 0 || i == 1 || i == 2) {       // 쇼핑 목록 ID, 고객 ID, 제품 ID는 정확히 일치할 때만 검색
        auto items = ui->shopTreeWidget->findItems(ui->searchLineEdit->text(),
                                                   Qt::MatchCaseSensitive, i);

        foreach(auto i, items) {
            ShoppingItem* s = static_cast<ShoppingItem*>(i);
            int id = s->id();
            int clientId = s->getCID();
            int productId = s->getPID();
            QString sellTime = s->getTime();
            int sellAmount = s->getAmount();
            int totalPrice = s->getTotalPrice();
            ShoppingItem* item = new ShoppingItem(id, clientId, productId,
                                                  sellTime, sellAmount, totalPrice);
            ui->searchTreeWidget->addTopLevelItem(item);
        }
    }

    else {      // 나머지 항목은 그냥 검색
        auto items = ui->shopTreeWidget->findItems(ui->searchLineEdit->text(),
                                               Qt::MatchContains | Qt::MatchCaseSensitive, i);

        foreach(auto i, items) {
            ShoppingItem* s = static_cast<ShoppingItem*>(i);
            int id = s->id();
            int clientId = s->getCID();
            int productId = s->getPID();
            QString sellTime = s->getTime();
            int sellAmount = s->getAmount();
            int totalPrice = s->getTotalPrice();
            ShoppingItem* item = new ShoppingItem(id, clientId, productId,
                                                  sellTime, sellAmount, totalPrice);
            ui->searchTreeWidget->addTopLevelItem(item);
        }
    }
}

