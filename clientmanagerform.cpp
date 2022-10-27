#include "clientmanagerform.h"
#include "ui_clientmanagerform.h"
#include "clientitem.h"

#include <QFile>
#include <QMenu>

ClientManagerForm::ClientManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientManagerForm)
{
    ui->setupUi(this);

    QList<int> sizes;
    sizes << 640 << 480;
    ui->splitter->setSizes(sizes);

    QAction* removeAction = new QAction(tr("&Remove"));
    connect(removeAction, SIGNAL(triggered()), SLOT(removeItem()));

    menu = new QMenu;
    menu->addAction(removeAction);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));

    // TreeWidgetItem의 내용에 맞춰서 header 사이즈 조절
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->searchTreeWidget->header()->
            setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->searchLineEdit, SIGNAL(returnPressed()),    // 검색할 때 엔터 누르면 검색 버튼 클릭
            this, SLOT(on_searchPushButton_clicked()));

    // 추가 버튼 누르면 ChatServer로 고객 리스트 전달
    connect(ui->addPushButton, SIGNAL(clicked(bool)), this, SLOT(receiveAllClient()));

    QFile file("clientlist.txt");   // 고객 정보 txt 파일 불러오기
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))  // 파일이 읽기 전용이나 text가 아니면
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");  // ',' 기준으로 고객 정보 불러오기
        if(row.size()) {
            int id = row[0].toInt();    //string -> int 변환
            int age = row[3].toInt();
            ClientItem* c = new ClientItem(id, row[1], row[2], age, row[4], row[5]);
                                         //id, name, gender, age, phoneNumber, address
            ui->treeWidget->addTopLevelItem(c);     // 하나씩 TreeWidget에 추가
            clientList.insert(id, c);    // Map에 저장
        }
    }
    file.close( );  // 파일 닫기
}

ClientManagerForm::~ClientManagerForm()
{
    delete ui;

    QFile file("clientlist.txt");       // 파일 저장
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : clientList) {      // Map에 있던 Item들 하나씩 저장
        ClientItem* c = v;
        out << c->id() << ", " << c->getName() << ", ";     // 정보는 ','로 나눔
        out << c->getGender() << ", ";
        out << c->getAge() << ", ";
        out << c->getPhoneNumber() << ", ";
        out << c->getAddress() << "\n";     // Item의 정보 다 저장하면 줄바꿈
    }
    file.close( );  // 파일 닫기
}

int ClientManagerForm::makeId( )    // 고객 ID 생성 함수
{
    if(clientList.size( ) == 0) {   // 고객 리스트가 비었으면
        return 1001;                // ID 1001번부터 시작
    }
    else {                          // 고객 리스트가 있으면 맨 마지막 고객 리스트 키값에 +1
        auto id = clientList.lastKey();
        return ++id;
    }
}

void ClientManagerForm::removeItem()    // TreeWidget에 있는 고객 리스트 삭제
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();  // 우클릭으로 선택한 Item 가져옴
    if(item != nullptr) {
        clientList.remove(item->text(0).toInt());   //item에서 id 가져와서 key로 삭제
        // 제거 후 업데이트
        ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(item));    // 아이템 삭제
        ui->treeWidget->update();
    }
}

void ClientManagerForm::showContextMenu(const QPoint &pos)  // 우클릭 메뉴 표시
{
    QPoint globalPos = ui->treeWidget->mapToGlobal(pos);    // 우클릭한 위치에 메뉴 띄움
    menu->exec(globalPos);
}

void ClientManagerForm::on_searchPushButton_clicked()   // 검색 버튼 사용자 슬롯
{
    ui->searchTreeWidget->clear();
    int i = ui->searchComboBox->currentIndex();     // 콤보박스의 현재 index 가져옴
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains   // ID일 땐 text가 정확히 일치하면 검색
                   : Qt::MatchCaseSensitive;                    // 나머지 항목은 일부 글자만 포함되도 검색
    {
        auto items = ui->treeWidget->findItems(ui->searchLineEdit->text(), flag, i);
                                              // TreeWidget에 i번째 column에 있는 내용과 검색하고 싶은 text를 비교
                                              // (검색 방식은 flag 메소드로)
        foreach(auto i, items) {
            // 결과 검색
            ClientItem* c = static_cast<ClientItem*>(i);
            int id = c->id();
            QString name = c->getName();
            QString gender = c->getGender();
            int age = c->getAge();
            QString p_number = c->getPhoneNumber();
            QString address = c->getAddress();
            ClientItem* item = new ClientItem(id, name, gender, age, p_number, address);
            ui->searchTreeWidget->addTopLevelItem(item);    // 검색 결과 출력
        }
    }
}

void ClientManagerForm::on_modifyPushButton_clicked()   // 수정 버튼 사용자 슬롯
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();  // 현재 클릭한 Item에서
    if(item != nullptr) {
        int key = item->text(0).toInt();
        ClientItem* c = clientList[key];
        int age;
        QString name, gender, p_number, address;
        // 입력창의 내용을 변수로 저장
        name = ui->nameLineEdit->text();
        gender = ui->genderLineEdit->text();
        age = ui->ageLineEdit->text().toInt();
        p_number = ui->phoneNumberLineEdit->text();
        address = ui->addressLineEdit->text();
        // 내용 변경
        c->setName(name);
        c->setGender(gender);
        c->setAge(age);
        c->setPhoneNumber(p_number);
        c->setAddress(address);
        // 새로 업데이트
        clientList[key] = c;
    }
}

void ClientManagerForm::on_addPushButton_clicked()  // 추가 버튼 사용자 슬롯
{
    QString name, gender, p_number, address;
    int age;
    int id = makeId( );
    // 입력창의 내용을 변수로 저장
    name = ui->nameLineEdit->text();
    gender = ui->genderLineEdit->text();
    age = ui->ageLineEdit->text().toInt();
    p_number = ui->phoneNumberLineEdit->text();
    address = ui->addressLineEdit->text();
    if(name.length()) {
        ClientItem* c = new ClientItem(id, name, gender, age, p_number, address);   // 아이템 생성
        clientList.insert(id, c);           // 고객리스트에 추가
        ui->treeWidget->addTopLevelItem(c); // TreeWidget에 추가
    }
}

void ClientManagerForm::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
// TreeWidget에서 아이템 클릭시 사용자 정의 슬롯
{
    Q_UNUSED(column);
    // 입력 창에 현재 Item 정보 가져오기
    ui->idLineEdit->setText(item->text(0));
    ui->nameLineEdit->setText(item->text(1));
    ui->genderLineEdit->setText(item->text(2));
    ui->ageLineEdit->setText(item->text(3));
    ui->phoneNumberLineEdit->setText(item->text(4));
    ui->addressLineEdit->setText(item->text(5));
}


void ClientManagerForm::on_clearPushButton_clicked()    // 클리어 버튼 사용자 정의 슬롯
{
    // 입력창 내용 클리어
    ui->idLineEdit->clear();
    ui->nameLineEdit->clear();
    ui->genderLineEdit->clear();
    ui->ageLineEdit->clear();
    ui->phoneNumberLineEdit->clear();
    ui->addressLineEdit->clear();
}

void ClientManagerForm::receiveData(QString string) {
    // ShoppingManagerForm에서 이름을 가져오는 경우
        auto items = ui->treeWidget->findItems(string, Qt::MatchContains, 1);
        if (string=="") return;     // 이름이 공백일 땐 반환 X

        foreach(auto i, items) {    // 이름으로 검색

            ClientItem* c = static_cast<ClientItem*>(i);
            int id = c->id();
            QString name = c->getName();
            QString gender = c->getGender();
            int age = c->getAge();
            QString pNumber = c->getPhoneNumber();
            QString address = c->getAddress();
            ClientItem* item = new ClientItem(id, name, gender, age, pNumber, address);
            emit clientDataSent(item);  // 검색한 item을 가지고 시그널 발생시킴
        }
}

void ClientManagerForm::receiveData(int id) {
    // ShoppingManagerForm에서 id를 가져오는 경우
        auto items = ui->treeWidget->findItems(QString::number(id),
                                               Qt::MatchContains|Qt::MatchCaseSensitive, 0);

        //if (string=="") return;
        foreach(auto i, items) {    // ID로 검색

            ClientItem* c = static_cast<ClientItem*>(i);
            int id = c->id();
            QString name = c->getName();
            QString gender = c->getGender();
            int age = c->getAge();
            QString pNumber = c->getPhoneNumber();
            QString address = c->getAddress();
            ClientItem* item = new ClientItem(id, name, gender, age, pNumber, address);
            emit clientDataSent(item);  // 검색한 item을 가지고 시그널 발생시킴
        }
}

void ClientManagerForm::receiveAllClient() {
    // ChatServerForm에 모든 고객 리스트 전달해주는 슬롯
    for(const auto &v : clientList) {
        ClientItem* c = v;
        int id = c->id();
        QString name = c->getName();
        QStringList list;
        list << QString::number(id) << name;
        qDebug() << list[0] << list[1];
        emit getAllClient(list);    // for문 돌면서 고객 리스트 하나씩 ChatServer에 전달
    }
}
