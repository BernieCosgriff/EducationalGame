#include "loginpane.h"
#include "ui_loginpane.h"

LoginPane::LoginPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginPane)
{
    ui->setupUi(this);
    connect(ui->loginButton, &QPushButton::clicked,
            this, &LoginPane::attemptLogin);

    connect(ui->signupButton, &QPushButton::clicked,
            this, [this] {
        ui->usernameField->clear();
        ui->passwordField->clear();
        emit onSignupRequested();
    });
}

void LoginPane::attemptLogin() {
    UserInfo info;
    info.username = ui->usernameField->text().toStdString();
    info.userId = 12345;
    info.type = UserType::STUDENT;
    emit onLogin(info);
}

LoginPane::~LoginPane()
{
    delete ui;
}