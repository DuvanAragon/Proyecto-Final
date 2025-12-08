#include "MenuWindow.h"
#include "nivel2window.h"
#include "ui_menuwindow.h"

#include "mainwindow.h"
#include <QApplication>
#include <QPixmap>
#include <QMessageBox>

MenuWindow::MenuWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MenuWindow)
    , gameWindow(nullptr)
{
    ui->setupUi(this);



    //  Fondo del menu
    this->setStyleSheet(
        "QMainWindow {"
        "    border-image: url(:/Menu/FondoMenu) 0 0 0 0 stretch stretch;"
        "}"
        );

    // Estilo base de botones transparentes
    const char *buttonStyle =
        "QPushButton {"
        "    border: none;"
        "    background-color: transparent;"
        "}"
        "QPushButton:pressed {"
        "    padding-left: 2px;"
        "    padding-top: 2px;"
        "}";

    //  Boton INICIAR
    {
        QPixmap pixIniciar(":/Menu/BotonIniciar");
        if (!pixIniciar.isNull()) {
            QPixmap scaled = pixIniciar.scaled(
                pixIniciar.width() * 0.85,
                pixIniciar.height() * 0.85,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );

            ui->btnIniciar->setIcon(scaled);
            ui->btnIniciar->setIconSize(scaled.size());
            ui->btnIniciar->setFixedSize(scaled.size());
        }

        ui->btnIniciar->setFlat(true);
        ui->btnIniciar->setStyleSheet(buttonStyle);
        ui->btnIniciar->setCursor(Qt::PointingHandCursor);
        ui->btnIniciar->setText("");
    }

    //  Boton SALIR
    {
        QPixmap pixSalir(":/Menu/BotonSalir");
        if (!pixSalir.isNull()) {
            QPixmap scaled = pixSalir.scaled(
                pixSalir.width() * 0.85,
                pixSalir.height() * 0.85,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );

            ui->btnSalir->setIcon(scaled);
            ui->btnSalir->setIconSize(scaled.size());
            ui->btnSalir->setFixedSize(scaled.size());
        }

        ui->btnSalir->setFlat(true);
        ui->btnSalir->setStyleSheet(buttonStyle);
        ui->btnSalir->setCursor(Qt::PointingHandCursor);
        ui->btnSalir->setText("");
    }
}

MenuWindow::~MenuWindow()
{
    delete ui;
}

//   BOTON INICIAR
void MenuWindow::on_btnIniciar_clicked()
{
    if (!gameWindow) {
        gameWindow = new MainWindow();
    }

    gameWindow->setWindowOpacity(0.0);
    gameWindow->show();

    QPropertyAnimation *animIn = new QPropertyAnimation(gameWindow, "windowOpacity");
    animIn->setDuration(450);
    animIn->setStartValue(0.0);
    animIn->setEndValue(1.0);
    animIn->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *animOut = new QPropertyAnimation(this, "windowOpacity");
    animOut->setDuration(450);
    animOut->setStartValue(1.0);
    animOut->setEndValue(0.0);

    connect(animOut, &QPropertyAnimation::finished, this, [this]() {
        this->hide();
        this->setWindowOpacity(1.0);
    });

    animOut->start(QAbstractAnimation::DeleteWhenStopped);
}


//  BOTON SALIR
void MenuWindow::on_btnSalir_clicked()
{
    QApplication::quit();
}
