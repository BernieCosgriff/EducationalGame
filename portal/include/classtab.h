#ifndef CLASSTAB_H
#define CLASSTAB_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include "../../shared/loqueclient.h"


namespace Ui {
class ClassTab;
}

class ClassTab : public QWidget
{
    Q_OBJECT

public:
    explicit ClassTab(int classID, int teacherID, QWidget *parent = 0);
    ~ClassTab();

private:
    int classID;
    int teacherID;

    Ui::ClassTab *ui;
    QTableWidget *userStatsTable;
    QTableWidget *levelStatsTable;
    QVBoxLayout *mainLayout;
    QGroupBox *summaryBox;
    QVBoxLayout *summaryLayout;

    QLabel *classSummaryLabel;
    QLabel *classNameLabel;
    QLabel *classPointsLabel;
    QLabel *classTimeLabel;
    QLabel *enabledLevelsLabel;
    QLabel *userStatsLabel;
    QLabel *userLabel;
    QLabel *totalScoreLabel;
    QLabel *totalTimeLabel;
    QLabel *lvl1Label;
    QLabel *lvl2Label;
    QLabel *lvl3Label;
    QLabel *mapStatsLabel;
    QLabel *mapUserLabel;
    QLabel *levelLabel;
    QLabel *levelNameLabel;
    QLabel *levelScoreLabel;
    QLabel *completionTimeLabel;

    void initWidgets();
    void initConnections();
    void setSummaryBox();
    void setUserTable();
    void setMapTable();
    QColor getLevelColor(UserStats user, int levelID);
};

#endif // CLASSTAB_H
