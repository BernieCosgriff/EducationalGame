#include "registerpane.h"
#include "ui_registerpane.h"

RegisterPane::RegisterPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterPane)
{
    ui->setupUi(this);

    connect(ui->registerButton, &QPushButton::clicked,
            this, &RegisterPane::attemptRegistration);
    connect(ui->adminCheck, &QCheckBox::stateChanged, this, &RegisterPane::adminBoxChanged);
    connect(ui->returnToLoginButton, &QPushButton::clicked, this, &RegisterPane::returnToLogin);
}

void RegisterPane::attemptRegistration()
{
    if (ui->usernameField->text().size() == 0) {
        displayErrorMessage("Username cannot be blank.");
    } else if (ui->usernameField->text().size() > 20) {
        displayErrorMessage("Username is too long.");
    } else if (ui->passwordField->text().size() == 0) {
        displayErrorMessage("Password cannot be blank.");
    } else if (ui->usernameField->text().size() > 20) {
        displayErrorMessage("Password is too long.");
    } else if (ui->passwordField->text().toStdString() != ui->confirmPasswordField->text().toStdString()) {
        displayErrorMessage("Passwords do not match.");
    } else {
        std::string username = ui->usernameField->text().toStdString();
        std::string password = ui->passwordField->text().toStdString();
        UserType type = ui->adminCheck->isChecked() ? UserType::ADMIN : UserType::STUDENT;
        LoginResult result;
        LoqueClient client;
        Status returnStatus = client.createAccount(username, password, type, result);
        if(returnStatus == Status::NETWORK_ERR)
        {
            displayErrorMessage("Cannot connect to the server");
        }
        else if (returnStatus == Status::DB_ERR)
        {
            displayErrorMessage("Username already in use.");
        }
        else
        {
            emit onRegistration(UserInfo(username, result.userId, result.userType));
        }
    }
}

void RegisterPane::displayErrorMessage(const char *errorMessage)
{
    ui->errorLabel->setText(QString(errorMessage));
    ui->errorLabel->setStyleSheet("QLabel { color : red }");
    ui->errorLabel->setMaximumSize(10000, 10000);
}

void RegisterPane::adminBoxChanged(int state)
{
    int width, height;
    width = height = 0;
    if (state == Qt::Unchecked)
    {
        width = 0;
        height = 0;
    }
    else if (state == Qt::Checked)
    {
        width = 10000;
        height = 10000;
    }
    ui->adminKeyLabel->setMaximumSize(width, height);
    ui->adminKeyField->setMaximumSize(width, height);
}

void RegisterPane::returnToLogin()
{
    ui->errorLabel->setMaximumSize(0, 0);
    ui->errorLabel->clear();
    ui->usernameField->clear();
    ui->passwordField->clear();
    ui->confirmPasswordField->clear();
    ui->adminCheck->setChecked(false);
    ui->adminKeyLabel->setMaximumSize(0, 0);
    ui->adminKeyField->setMaximumSize(0, 0);
    emit onLoginRequested();
}

RegisterPane::~RegisterPane()
{
    delete ui;
}
