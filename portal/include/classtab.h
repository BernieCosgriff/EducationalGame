#ifndef CLASSTAB_H
#define CLASSTAB_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include "addclassdialog.h"
#include "levelsettingsdialog.h"
#include "../../shared/loqueclient.h"


namespace Ui {
class ClassTab;
}

class ClassTab : public QWidget
{
    Q_OBJECT

public:
    explicit ClassTab(ClassStats classStats, int teacherID, QWidget *parent = 0);
    ~ClassTab();

signals:
    void classCreated(ClassStats cstats);

private:

    QTableWidget *userStatsTable;
    QTableWidget *levelStatsTable;
    QVBoxLayout *mainLayout;
    QGroupBox *summaryBox;
    QVBoxLayout *summaryLayout;
    QHBoxLayout *topLayout;
    QScrollArea *tableArea;
    QVBoxLayout *scrollLayout;
    std::unique_ptr<AddClassDialog> currClassDialog;
    std::unique_ptr<LevelSettingsDialog> levelDialog;

    int teacherID;
    ClassStats cstats;
    std::vector<LevelInfo> allLevels;
    std::vector<int> enabledLevels;

    QLabel *classSummaryLabel;
    QLabel *classIdLabel;
    QLabel *classNameLabel;
    QLabel *classPointsLabel;
    QLabel *classTimeLabel;
    QLabel *enabledLevelsLabel;
    QLabel *userStatsLabel;
    QLabel *levelStatsLabel;

    void initWidgets();
    void setSummaryBox();
    void setUserTable();
    void setMapTable();
    void refresh();
    QColor getLevelColor(const UserStats& user, int levelID);
    QString getFormattedTime(int seconds);
};

#endif // CLASSTAB_H
