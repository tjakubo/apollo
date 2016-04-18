#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include <QWidget>

#ifndef HARDWARE_H
#include "hardware.h"
#define HARDWARE_H
#endif

namespace Ui {
class InputWindow;
}

class InputWindow : public QWidget
{
    Q_OBJECT
    meas actMeas;
public:
    explicit InputWindow(QWidget *parent = 0);
    ~InputWindow();
    void paintEvent(QPaintEvent *);

public slots:
    void setMeas(meas newMeas);
    void updateBars(meas newMeas);
private:
    Ui::InputWindow *ui;
};

#endif // INPUTWINDOW_H
