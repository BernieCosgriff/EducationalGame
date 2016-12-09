#include <iostream>
#include <QHeaderView>
#include "classtab.h"
#include <QToolButton>

ClassTab::ClassTab(ClassStats classStats, int teacherID, QWidget *parent) :
    QWidget(parent), teacherID(teacherID), classID(classStats.classId), currClassDialog(nullptr)
{
    LoqueClient client;
    client.getEnabledClassLevels(classID, enabledLevels);
    client.getAllLevels(allLevels);

    initWidgets();
    setSummaryBox(classStats);
    setUserTable(classStats);
    setMapTable(classStats);
}

ClassTab::~ClassTab()
{
//    delete classSummaryLabel;
//    delete classNameLabel;
//    delete classPointsLabel;
//    delete classTimeLabel;
//    delete enabledLevelsLabel;
//    delete userStatsLabel;
//    delete mapStatsLabel;
//    delete ui;
//    delete userStatsTable;
//    delete levelStatsTable;
//    delete summaryBox;
//    delete levelArea;
//    delete mainLayout;
//    delete summaryLayout;
}

void ClassTab::initWidgets(){
    classSummaryLabel = new QLabel("Class Summary");
    classNameLabel = new QLabel;
    classPointsLabel = new QLabel;
    classTimeLabel = new QLabel;
    userStatsLabel = new QLabel("Student Stats");
    levelStatsLabel = new QLabel("Level Stats");
    classIdLabel = new QLabel;
    QToolButton *levelButton = new QToolButton;
    QIcon lvl(QPixmap(":/assets/levelView.png"));
    levelButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    levelButton->setIcon(lvl);
    levelButton->setText("Level Settings");
    levelButton->setIconSize(QSize(75,75));
    levelButton->setFixedSize(QSize(100, 100));
    QToolButton *addClassButton = new QToolButton;
    QIcon plus(QPixmap(":/assets/plus.png"));
    addClassButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addClassButton->setIcon(plus);
    addClassButton->setText("Add Class");
    addClassButton->setIconSize(QSize(75,75));
    addClassButton->setFixedSize(QSize(100, 100));

    summaryBox = new QGroupBox;
    summaryLayout = new QVBoxLayout;
    userStatsTable = new QTableWidget;
    levelStatsTable = new QTableWidget;
    mainLayout = new QVBoxLayout;
    topLayout = new QHBoxLayout;

    summaryLayout->addWidget(classIdLabel, 0, Qt::AlignCenter);
    summaryLayout->addWidget(classNameLabel, 0, Qt::AlignCenter);
    summaryLayout->addWidget(classPointsLabel, 0, Qt::AlignCenter);
    summaryLayout->addWidget(classTimeLabel, 0, Qt::AlignCenter);
    summaryBox->setLayout(summaryLayout);
    summaryBox->setFixedSize(200, 100);

    topLayout->addWidget(levelButton);
    topLayout->addWidget(summaryBox);
    topLayout->addWidget(addClassButton);
    mainLayout->addLayout(topLayout);

    mainLayout->addWidget(userStatsLabel);
    mainLayout->addWidget(userStatsTable);
    mainLayout->addWidget(levelStatsLabel);
    mainLayout->addWidget(levelStatsTable);
    setLayout(mainLayout);

    connect(addClassButton, &QToolButton::clicked, this, [this]{
        currClassDialog.reset(new AddClassDialog(teacherID));
        currClassDialog->show();
        connect(currClassDialog.get(), &AddClassDialog::classCreated, this, &ClassTab::classCreated);
    });

    connect(levelButton, &QToolButton::clicked, this, [this]{
        LevelSettingsDialog *d = new LevelSettingsDialog(enabledLevels, allLevels);
        d->show();
    });
}



void ClassTab::setSummaryBox(const ClassStats& classStats){
    int totalPoints = 0;
    int totalTime = 0;
    for(auto& user : classStats.studentStats){
        totalPoints += user.totalScore;
        totalTime += user.totalSecPlayed;
    }
    classIdLabel->setText("Class ID: " + QString::number(classStats.classId));
    classNameLabel->setText("Class Name: " + QString::fromStdString(classStats.className));
    classPointsLabel->setText("Total Points: " + QString::number(totalPoints));
    classTimeLabel->setText("Total Time Played: " + getFormattedTime(totalTime));
}

QString ClassTab::getFormattedTime(int seconds){
    QString hourString;
    int hours = seconds / 3600;
    if(hours == 0){
        hourString = "";
    }
    else if(hours < 10){
        hourString = QString::number(0) + QString::number(seconds / 3600) + ":";
    } else{
        hourString = QString::number(seconds / 3600) + ":";
    }
    QString minuteString;
    int minutes = (seconds % 3600) / 60;
    if(minutes < 10){
        minuteString = QString::number(0) + QString::number((seconds % 3600) / 60) + ":";
    } else{
        minuteString = QString::number((seconds % 3600) / 60) + ":";
    }

    QString secString;
    int secs = (seconds % 3600) % 60;
    if(secs < 10){
        secString = QString::number(0) + QString::number((seconds % 3600) % 60);
    } else{
        secString = QString::number((seconds % 3600) % 60);
    }
    return  hourString + minuteString + secString;
}

void ClassTab::setUserTable(const ClassStats& classStats){
    QStringList headers;
    headers.append("Student");
    headers.append("Total Score");
    headers.append("Time Played");

    int numlvls = allLevels.size();

    for(int i = 0; i < numlvls; i++){
        headers.append("Level " + QString::number(i+1) + " Completed");
    }

    userStatsTable->setRowCount(classStats.studentStats.size());
    userStatsTable->setColumnCount(3 + numlvls);
    userStatsTable->setHorizontalHeaderLabels(headers);
    userStatsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int row = 0;
    int col = 3;
    for(auto& user : classStats.studentStats){
        QTableWidgetItem *nameCell = new QTableWidgetItem(QString::fromStdString(user.username));
        QTableWidgetItem *scoreCell = new QTableWidgetItem(QString::number(user.totalScore));
        QTableWidgetItem *timeCell = new QTableWidgetItem(QString::number(user.totalSecPlayed));
        nameCell->setTextAlignment(Qt::AlignCenter);
        scoreCell->setTextAlignment(Qt::AlignCenter);
        timeCell->setTextAlignment(Qt::AlignCenter);

        userStatsTable->setItem(row, 0, nameCell);
        userStatsTable->setItem(row, 1, scoreCell);
        userStatsTable->setItem(row, 2, timeCell);

        QTableWidgetItem *lvlCell = new QTableWidgetItem;
        for(int i = 0; i < numlvls; i++){
            lvlCell = new QTableWidgetItem;
            lvlCell->setBackgroundColor(getLevelColor(user, i));
            lvlCell->setTextAlignment(Qt::AlignCenter);
            userStatsTable->setItem(row, col++, lvlCell);
        }
        row++;
        col = 3;
    }
    userStatsTable->sortByColumn(0, Qt::AscendingOrder);
    userStatsTable->setSelectionMode(QAbstractItemView::NoSelection);
    userStatsTable->verticalHeader()->setVisible(false);
}

QColor ClassTab::getLevelColor(const UserStats& user, int levelID){
    QColor color;
    if(user.highScores.find(levelID) == user.highScores.end() ||user.highScores.at(levelID) == 0){
        color.setRgb(235, 16, 16);
    } else {
        color.setRgb(0, 186, 6);
    }
    return color;
}

void ClassTab::setMapTable(const ClassStats& classStats){
    QStringList headers;
    headers.append("Student");
    headers.append("Level ID");
    headers.append("Level Name");
    headers.append("Level Score");
    headers.append("Completion Time");
    levelStatsTable->setColumnCount(5);
    levelStatsTable->setHorizontalHeaderLabels(headers);
    levelStatsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    LoqueClient client;
    int row = levelStatsTable->rowCount();
    std::map<int, std::string> levelMap;

    for(LevelInfo info : allLevels){
        levelMap[info.id] = info.name;
    }

    for(int i = 0; i < classStats.studentStats.size(); i++){
        const UserStats& user = classStats.studentStats.at(i);
        for(auto& kv : user.highScores){
            levelStatsTable->insertRow(row);
            QTableWidgetItem *studentNameCell = new QTableWidgetItem(QString::fromStdString(user.username));
            QTableWidgetItem *levIDCell = new QTableWidgetItem(QString::number(kv.first));
            QTableWidgetItem *levelNameCell = new QTableWidgetItem(QString::fromStdString(levelMap.at(kv.first)));
            QTableWidgetItem *scoreCell = new QTableWidgetItem(QString::number(kv.second));
            QTableWidgetItem *timeCell = new QTableWidgetItem(getFormattedTime(user.completionTimes.at(kv.first)));
            studentNameCell->setTextAlignment(Qt::AlignCenter);
            levIDCell->setTextAlignment(Qt::AlignCenter);
            levelNameCell->setTextAlignment(Qt::AlignCenter);
            scoreCell->setTextAlignment(Qt::AlignCenter);
            timeCell->setTextAlignment(Qt::AlignCenter);
            levelStatsTable->setItem(row, 0, studentNameCell);
            levelStatsTable->setItem(row, 1, levIDCell);
            levelStatsTable->setItem(row, 2, levelNameCell);
            levelStatsTable->setItem(row, 3, scoreCell);
            levelStatsTable->setItem(row++, 4, timeCell);
        }
    }
    levelStatsTable->sortByColumn(0, Qt::AscendingOrder);
    levelStatsTable->setSelectionMode(QAbstractItemView::NoSelection);
    levelStatsTable->verticalHeader()->setVisible(false);
}
