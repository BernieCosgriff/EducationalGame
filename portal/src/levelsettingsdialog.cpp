#include "levelsettingsdialog.h"
#include "ui_levelsettingsdialog.h"
#include <QListView>
#include <iostream>

LevelSettingsDialog::LevelSettingsDialog(int classID, std::vector<int> enabledIDs, std::vector<LevelInfo> allLevels, int teacherID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LevelSettingsDialog),
    classID(classID),
    teacherID(teacherID)
{
    ui->setupUi(this);
    mapper = new QSignalMapper;
    connect(this, &LevelSettingsDialog::clicked, this, &LevelSettingsDialog::toggleLevel);
    connect(mapper, SIGNAL(mapped(int)), this, SIGNAL(clicked(int)));
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &LevelSettingsDialog::submitLevels);

    ui->scrollArea->setWidgetResizable(true);
    QFrame *frame = new QFrame(ui->scrollArea);
    ui->scrollArea->setWidget(frame);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    QGridLayout *layout = new QGridLayout(frame);

    int row = 0;
    int col = 0;
    for(size_t i = 0; i < allLevels.size(); i++){
        layout->addWidget(addLevel(allLevels[i], std::find(enabledIDs.begin(), enabledIDs.end(), allLevels[i].id) != enabledIDs.end()), row, col++);
        if(col > 1){
            row++;
            col = 0;
        }
    }

    frame->setLayout(layout);
}

QGroupBox* LevelSettingsDialog::addLevel(LevelInfo lvlInfo, bool enabled){
    QGroupBox *box = new QGroupBox;
    QHBoxLayout *hLayout = new QHBoxLayout;
    QVBoxLayout *vLayout = new QVBoxLayout;
    QPixmap img;
    for(size_t i=0;i<MapCount;i++ ) {
        if ( Maps[i].id == lvlInfo.id ) {
            img = QPixmap(Maps[i].thumbnail.c_str());
        }
    }
    QLabel *imgLabel = new QLabel;
    imgLabel->setPixmap(img.scaled(250, 250, Qt::KeepAspectRatio));
    QLabel *nameLabel = new QLabel(QString::fromStdString(lvlInfo.name));
    QLabel *descriptionLabel = new QLabel(QString::fromStdString(lvlInfo.description));
    descriptionLabel->setWordWrap(true);
    QPushButton *playButton = new QPushButton("Play");
    QPushButton *toggleButton = new QPushButton(enabled ? "Disable Level" : "Enable Level");
    btns[lvlInfo.id] = toggleButton;

    connect(toggleButton, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(toggleButton, lvlInfo.id);
    vLayout->addWidget(nameLabel);
    vLayout->addWidget(descriptionLabel);
    vLayout->addWidget(playButton);
    vLayout->addWidget(toggleButton);
    hLayout->addWidget(imgLabel);
    hLayout->addLayout(vLayout);
    box->setLayout(hLayout);
    box->setFixedSize(QSize(500, 250));
    return box;
}

void LevelSettingsDialog::toggleLevel(const int &levelID){
//    std::cout << levelID << std::endl;
//    if(std::find(enabledIDs.begin(), enabledIDs.end(), levelID) != enabledIDs.end()){

//    } else{

//    }
    QPushButton* btn = btns[levelID];
    if(btn->text() == "Disable Level"){
        btn->setText("Enable Level");
        enabledIDs.erase(std::remove(enabledIDs.begin(), enabledIDs.end(), levelID), enabledIDs.end());
        levelsToEnable.erase(std::remove(levelsToEnable.begin(), levelsToEnable.end(), levelID), levelsToEnable.end());
        levelsToDisable.push_back(levelID);
    } else{
        levelsToDisable.erase(std::remove(levelsToDisable.begin(), levelsToDisable.end(), levelID), levelsToDisable.end());
        levelsToEnable.push_back(levelID);
        enabledIDs.push_back(levelID);
        btn->setText("Disable Level");
    }
}

void LevelSettingsDialog::submitLevels(){
    LoqueClient client;
    for(int id : levelsToDisable){
        client.disableLevel(teacherID, classID, id);
    }
    for(int id : levelsToEnable){
        client.enableLevel(teacherID, classID, id);
    }
    emit refresh();
}

LevelSettingsDialog::~LevelSettingsDialog()
{
    delete ui;
}
