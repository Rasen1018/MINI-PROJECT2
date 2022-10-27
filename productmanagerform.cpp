#include "productmanagerform.h"
#include "ui_productmanagerform.h"
#include "productitem.h"

#include <QFile>
#include <QMenu>
#include <QMessageBox>

ProductManagerForm::ProductManagerForm(QWidget* parent) :
    QWidget(parent), ui(new Ui::ProductManagerForm)
{
    ui->setupUi(this);

    QList<int> sizes;
    sizes << 640 << 480;            // 위젯 사이즈 설정
    ui->splitter->setSizes(sizes);

    // 삭제 Action 연결
    QAction* removeAction = new QAction(tr("&Remove"));
    connect(removeAction, SIGNAL(triggered()), this, SLOT(removeItem()));

    // 삭제 기능 우클릭 메뉴에 추가
    menu = new QMenu;
    menu->addAction(removeAction);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)),
        this, SLOT(showContextMenu(QPoint)));

    // TreeWidgetItem의 header 사이즈 조절
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->searchTreeWidget->header()->
            setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->searchLineEdit, SIGNAL(returnPressed()),    // 엔터 누르면 버튼 클릭
        this, SLOT(on_searchPushButton_clicked()));

    QFile file("productlist.txt");      // 제품 정보 txt 파일 불러오기
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");  // ',' 기준으로 고객 정보 불러오기
        if (row.size()) {
            int id = row[0].toInt();    //string -> int 변환
            int price = row[2].toInt();
            int stock = row[3].toInt();
            ProductItem* p = new ProductItem(id, row[1], price, stock, row[4]);
                                           //id, productName, price, stock, category
            ui->treeWidget->addTopLevelItem(p);     // 하나씩 TreeWidget에 추가
            productList.insert(id, p);     // Map에 저장
        }
    }
    file.close( );  // 파일 닫기
}

ProductManagerForm::~ProductManagerForm() {

    delete ui;
    QFile file("productlist.txt");      // 파일 저장
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : productList) {     // Map에 있던 Item들 하나씩 저장
        ProductItem* p = v;
        out << p->id() << ", " << p->getName() << ", ";     // 정보는 ','로 나눔
        out << p->getPrice() << ", ";
        out << p->getStock() << ", ";
        out << p->getCategory() << "\n";     // Item의 정보를 다 저장하면 줄바꿈
    }
    file.close( );
}

int ProductManagerForm::makeId() {    // 고객 ID 생성 함수
    if (productList.size() == 0) {    // 고객 리스트가 비었으면
        return 3001;                  // ID 3001번부터 시작
    }
    else {                            // 고객 리스트가 있으면 맨 마지막 고객 리스트 키값에 +1
        auto id = productList.lastKey();
        return ++id;
    }
}

void ProductManagerForm::showContextMenu(const QPoint& pos) {      // 우클릭 메뉴 표시

    QPoint globalPos = ui->treeWidget->mapToGlobal(pos);           // 우클릭한 위치에 메뉴 띄움
    menu->exec(globalPos);
}

void ProductManagerForm::removeItem() {     // TreeWidget에 있는 고객 리스트 삭제

    QTreeWidgetItem* item = ui->treeWidget->currentItem();   // 우클릭으로 선택한 Item 가져옴
    if (item != nullptr) {
        productList.remove(item->text(0).toInt());    // item에서 id 가져와서 key로 삭제
        // 제거후 업데이트
        ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(item));
        ui->treeWidget->update();
    }
}

void ProductManagerForm::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
// TreeWidget에서 아이템 클릭시
{
    Q_UNUSED(column);
    // 입력 창에 현재 Item 정보 가져오기
    ui->idLineEdit->setText(item->text(0));
    ui->productNameLineEdit->setText(item->text(1));
    ui->priceLlineEdit->setText(item->text(2));
    ui->stockLlineEdit->setText(item->text(3));
    ui->categoryLineEdit->setText(item->text(4));
}

void ProductManagerForm::on_clearPushButton_clicked()   // 클리어 버튼 클릭시
{   // 입력창 내용 클리어
    ui->idLineEdit->clear();
    ui->productNameLineEdit->clear();
    ui->priceLlineEdit->clear();
    ui->stockLlineEdit->clear();
    ui->categoryLineEdit->clear();
}

void ProductManagerForm::on_addPushButton_clicked()     // 추가 버튼 클릭시
{
    QString name, category;
    int price, stock;
    int id = makeId();
    // 입력창의 내용을 변수로 저장
    name= ui->productNameLineEdit->text();
    price = ui->priceLlineEdit->text().toInt();
    stock = ui->stockLlineEdit->text().toInt();
    category = ui->categoryLineEdit->text();

    if (name.length()) {
        ProductItem* p = new ProductItem(id, name, price, stock, category);
        productList.insert(id, p);            // 고객 리스트에 추가
        ui->treeWidget->addTopLevelItem(p);   // TreeWidget에 추가
    }
}

void ProductManagerForm::on_modifyPushButton_clicked()    // 수정 버튼 클릭시
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();  // 현재 클릭한 Item에서
    if (item != nullptr) {
        int key = item->text(0).toInt();
        ProductItem* p = productList[key];
        int price, stock;
        QString name, category;
        // 입력창의 내용을 변수로 저장
        name = ui->productNameLineEdit->text();
        price = ui->priceLlineEdit->text().toInt();
        stock = ui->stockLlineEdit->text().toInt();
        category = ui->categoryLineEdit->text();
        // 내용 변경
        p->setName(name);
        p->setPrice(price);
        p->setStock(stock);
        p->setCategory(category);
        // 새로 업데이트
        productList[key] = p;
    }
}

void ProductManagerForm::on_searchPushButton_clicked()    // 검색 버튼 클릭시
{
    ui->searchTreeWidget->clear();
    int i = ui->searchComboBox->currentIndex();     // 콤보박스의 현재 index 가져옴
    auto flag = (i) ? Qt::MatchCaseSensitive | Qt::MatchContains    // ID일 땐 text가 정확히 일치하면 검색
        : Qt::MatchCaseSensitive;                                   // 나머지 항목은 일부 글자만 포함되도 검색
    {
        auto items = ui->treeWidget->findItems(ui->searchLineEdit->text(), flag, i);
                                              // TreeWidget에 i번째 column에 있는 내용과 검색하고 싶은 text를 비교
                                              // (검색 방식은 flag 메소드로)
        foreach(auto i, items) {
            // 결과 검색
            ProductItem* p = static_cast<ProductItem*>(i);
            int id = p->id();
            QString name = p->getName();
            int price = p->getPrice();
            int stock = p->getStock();
            QString category = p->getCategory();
            ProductItem* item = new ProductItem(id, name, price, stock, category);
            ui->searchTreeWidget->addTopLevelItem(item);    // 검색 결과 출력
        }
    }
}

void ProductManagerForm::receiveName(QString string) {
        // ShoppingManagerForm에서 이름을 가져오는 경우
        auto items = ui->treeWidget->findItems(string, Qt::MatchContains, 1);
        if (string=="") return;     // 이름이 공백일 땐 반환 X

        foreach(auto i, items) {    // 이름으로 검색

            ProductItem* p = static_cast<ProductItem*>(i);
            int id = p->id();
            QString name = p->getName();
            int price = p->getPrice();
            int stock = p->getStock();
            QString category = p->getCategory();
            ProductItem* item = new ProductItem(id, name, price, stock, category);
            emit producdataSent(item);  // 검색한 item을 가지는 시그널을 발생시킴

        }
}

void ProductManagerForm::receiveName(int id) {
        // ShoppingManagerFrom에서 id를 가져오는 경우
        auto items = ui->treeWidget->findItems(QString::number(id),
                                               Qt::MatchCaseSensitive|Qt::MatchContains, 0);

        //if (string=="") return;
        foreach(auto i, items) {     // ID로 검색

            ProductItem* p = static_cast<ProductItem*>(i);
            int id = p->id();
            QString name = p->getName();
            int price = p->getPrice();
            int stock = p->getStock();
            QString category = p->getCategory();
            ProductItem* item = new ProductItem(id, name, price, stock, category);
            emit producdataSent(item);  // 검색한 item을 가지고 시그널 발생시킴
        }
}

void ProductManagerForm::receiveCategory(QString string)
{       // ShoppingManagerForm에서 품목을 가져오는 경우
        auto items = ui->treeWidget->findItems(string, Qt::MatchContains, 4);
        if (string=="") return;

        foreach(auto i, items) {    // 품목으로 검색

            ProductItem* p = static_cast<ProductItem*>(i);
            int id = p->id();
            QString name = p->getName();
            int price = p->getPrice();
            int stock = p->getStock();
            QString category = p->getCategory();
            ProductItem* item = new ProductItem(id, name, price, stock, category);
            emit producdataSent(item);  // 검색한 item을 가지고 시그널 발생시킴
        }
}

void ProductManagerForm::inventoryChanged(int id, int order) {
    // ShoppingManagerForm에서 주문하는 경우 id, 주문량(order)을 가져옴
    ProductItem* p = productList[id];
    if (p->getStock() > order) {    // 재고가 주문량보다 많을 때만
        int stock = p->getStock() - order;      // 재고 감소 or 재고 증가
        p->setStock(stock);
        productList[id] = p;        // 재고 변경
    }
}



