#include <QInputDialog>
#include <QProcess>
#include <QScrollBar>
#include <iostream>
#include "studentplaypane.h"
#include "launchgame.h"

StudentPlayPane::StudentPlayPane(UserInfo user, QWidget *parent) :
    QWidget(parent),
    user(user), 
    levelInfo(nullptr),
    activeClass(nullptr),
    activeLevelRecord(nullptr),
    ui(new Ui::StudentPlayPane)
{
    ui->setupUi(this);

    QPalette palette;
    palette.setColor(QPalette::Highlight, Qt::transparent);
    palette.setColor(QPalette::HighlightedText, ui->levelList->palette().color(QPalette::Text));
    ui->levelList->setPalette(palette);

    ui->classList->setAttribute(Qt::WA_MacShowFocusRect, 0);

    connect(ui->classList, &QListWidget::currentRowChanged,
            this, &StudentPlayPane::classClicked);

    connect(ui->levelList, &QListWidget::currentRowChanged,
           this, &StudentPlayPane::levelThumbnailClicked);

    connect(ui->playButton, &QPushButton::clicked,
            this, &StudentPlayPane::playButtonClicked);

    connect(ui->firstClassButton, &QPushButton::clicked,
            this, &StudentPlayPane::showAddClassDialog);

    updateLevelInfo();
    updateDisplay();
}

StudentPlayPane::~StudentPlayPane() {
    delete ui;
}

void StudentPlayPane::updateLevelInfo() {
    LoqueClient client;
    UserLevelInfo* newInfo = new UserLevelInfo();
    auto status = client.getUserLevelInfo(user.userId, *newInfo);
    if (status != Status::OK) {
        // TODO: show error
        std::cerr << "ERROR: Unable to update level info. Status returned was " <<
            status << std::endl;
        return;
    }
    if ( levelInfo ) {
        delete levelInfo;
    }
    levelInfo = newInfo;
}

void StudentPlayPane::updateDisplay() {
    activeClass = nullptr;
    activeLevelRecord = nullptr;
    ui->classList->clear();
    clearLevelDescription();
    if (levelInfo == nullptr || levelInfo->size() == 0) {
        ui->stackedWidget->setCurrentWidget(ui->noClassesPane);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->mainPane);
        for (auto& classInfo : *levelInfo) {
            ui->classList->addItem(QString::fromStdString(classInfo.className));
        }
        ui->classList->item(0)->setSelected(true);
        classClicked(0);
    }
}

void StudentPlayPane::refreshDisplay() {
    activeClass = nullptr;
    activeLevelRecord = nullptr;
    clearLevelDescription();
    if (levelInfo == nullptr || levelInfo->size() == 0) {
        ui->stackedWidget->setCurrentWidget(ui->noClassesPane);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->mainPane);
        ui->classList->item(0)->setSelected(true);
        classClicked(0);
    }
}

void StudentPlayPane::classClicked(int col) {
    ui->levelList->clear();
    clearLevelDescription();
    activeClass = &levelInfo->at(col);
    for (auto& record : activeClass->levelRecords) {
        std::string path = Maps[record.level.id].thumbnail;
        QIcon thumbnail(QString::fromStdString(path));
        ui->levelList->addItem(new QListWidgetItem(thumbnail, NULL));
    }
    if (activeClass->levelRecords.size() > 0) {
        ui->levelList->item(0)->setSelected(true);
        levelThumbnailClicked(0);
    } else {
        ui->levelDescriptionStack->setCurrentWidget(ui->noActiveLevelPage);
    }
}

void StudentPlayPane::levelThumbnailClicked(int col) {
    if (col == -1) return;
    ui->levelDescriptionStack->setCurrentWidget(ui->levelDescriptionPage);
    activeLevelRecord = &activeClass->levelRecords.at(col);
    std::string thumbnailPath = Maps[activeLevelRecord->level.id].thumbnail;
    QPixmap thumbnail(QString::fromStdString(thumbnailPath));
    int width = ui->activeLevelThumbnail->width();
    int height = ui->activeLevelThumbnail->height();
    ui->activeLevelThumbnail->setPixmap(thumbnail.scaled(width, height, Qt::KeepAspectRatio));
    ui->levelNameLabel->setText(QString::fromStdString(activeLevelRecord->level.name));
    ui->levelDescLabel->setText(QString::fromStdString(activeLevelRecord->level.description));
    if (activeLevelRecord->hasCompleted()) {
        ui->highScoreLabel->setText("High Score: " + QString::number(activeLevelRecord->highScore));
    } else {
        ui->highScoreLabel->setText("Looks like you haven't played this level yet!");
    }
}

void StudentPlayPane::playButtonClicked() {
    if (activeLevelRecord != nullptr) {
        launchGame(activeLevelRecord->level.id, user.userId); 
    }
}

void StudentPlayPane::showAddClassDialog() {
    bool ok = false;
    int classId = QInputDialog::getInt(this, tr("Add a Class"), tr("Enter the Class ID"), 0, 0, INT_MAX, 1, &ok);
    if (!ok) return;
    LoqueClient client;
    auto status = client.addClassroom(user.userId, classId);
    if (status != Status::OK) {
        // TODO: SHOW ERROR
        std::cerr << "ERROR: Unable to add classroom. " <<
                     "Server returned status " << status << std::endl;
        return;
    }
    UserLevelInfo info;
    status = client.getUserLevelInfo(user.userId, info);
    if (status != Status::OK) {
        // TODO: SHOW ERROR
        std::cerr << "ERROR: Unable to get user level info. " <<
                     "Server returned status " << status << std::endl;
        return;
    }
    for (auto& classInfo : info) {
       if (classInfo.classId == classId) {
           levelInfo->push_back(classInfo);
           ui->classList->addItem(QString::fromStdString(classInfo.className));
           if (activeClass == nullptr) {
               ui->stackedWidget->setCurrentWidget(ui->mainPane);
               classClicked(0);
           }
       }
    }
}

void StudentPlayPane::updateInfo() {
    updateLevelInfo();
    refreshDisplay();
}

void StudentPlayPane::clearLevelDescription() {
    ui->activeLevelThumbnail->clear();
    ui->levelNameLabel->clear();
    ui->levelDescLabel->clear();
    ui->highScoreLabel->clear();
}
