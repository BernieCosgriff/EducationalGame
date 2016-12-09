#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QMenuBar>
#include <QPushButton>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    paneContainer(new QStackedWidget),
    loginPane(new LoginPane),
    registerPane(new RegisterPane),
    studentPlayPane(nullptr),
    adminPane(nullptr),
    adminPlayPane(nullptr)
{
    ui->setupUi(this);

    QAction *logoutAction = new QAction(tr("Log Out"), this);
    connect(logoutAction, &QAction::triggered, this, &MainWindow::logout);
    ui->menuFile->addAction(logoutAction);

    paneContainer->addWidget(loginPane);
    paneContainer->addWidget(registerPane);
    setCentralWidget(paneContainer);
    menuBar()->setHidden(true);
    connect(loginPane, &LoginPane::onLogin,
            this, &MainWindow::handleLogin);

    connect(loginPane, &LoginPane::onSignupRequested,
            this, [this] {
        paneContainer->setCurrentWidget(registerPane);
    });

    connect(registerPane, &RegisterPane::onRegistration,
            this, &MainWindow::handleLogin);

    connect(registerPane, &RegisterPane::onLoginRequested,
            this, [this] {
        paneContainer->setCurrentWidget(loginPane);
    });

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::handleLogin(UserInfo user) {
    if (user.type == UserType::ADMIN) {
        adminPane = new AdminPane(user);
        paneContainer->addWidget(adminPane);
        paneContainer->setCurrentWidget(adminPane); 
    } else {
        menuBar()->setHidden(false);
        QList<QMenu*> menus = menuBar()->findChildren<QMenu*>();
        for(int i = 0; i < menus.size(); i++){
            if(menus.at(i)->title() == "File"){
                menus.at(i)->addAction("Add Class");
                break;
            }
        }
        studentPlayPane = new StudentPlayPane(user);
        paneContainer->addWidget(studentPlayPane);
        paneContainer->setCurrentWidget(studentPlayPane);

        addClassAction = new QAction(tr("Add Class"));
        connect(addClassAction, &QAction::triggered,
                studentPlayPane, &StudentPlayPane::showAddClassDialog);
        ui->menuFile->addAction(addClassAction);
    }
}

void MainWindow::logout() {
    paneContainer->setCurrentWidget(loginPane);
    if (adminPane != nullptr) {
        paneContainer->removeWidget(adminPane);
        delete adminPane;
    } else {
        ui->menuFile->removeAction(addClassAction);
        delete addClassAction;
        paneContainer->removeWidget(studentPlayPane);
        delete studentPlayPane;
    }
}
