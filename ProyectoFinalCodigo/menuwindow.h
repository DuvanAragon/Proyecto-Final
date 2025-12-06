#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QMainWindow>
#include <QPropertyAnimation>
#include <QAbstractAnimation>


namespace Ui {
class MenuWindow;
}

class MainWindow;   // Ventana del juego (la tuya)

class MenuWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MenuWindow(QWidget *parent = nullptr);
    ~MenuWindow();

private slots:
    void on_btnIniciar_clicked();
    void on_btnSalir_clicked();

private:
    Ui::MenuWindow *ui;
    MainWindow *gameWindow;   // puntero a la ventana del juego
};

#endif // MENUWINDOW_H
