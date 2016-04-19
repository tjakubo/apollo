#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include <QWidget>
#include <qstring.h>
#include <QPainter>

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
    meas measOffset;

public:
Hardware *_HWlink;
   explicit InputWindow(Hardware *HWlink, QWidget *parent = 0);
    ~InputWindow();
    void paintEvent(QPaintEvent *);

public slots:
    void setMeas(meas newMeas);
    void updateBars(meas newMeas);
private slots:
    void on_resetOffset_clicked();

    void on_compensateOffset_clicked();

    void on_applyOffset_clicked();

    void updateOffsetTextboxes();

    void updateRawDataTextbox(QString newRaw);

private:
    Ui::InputWindow *ui;

};

#endif // INPUTWINDOW_H
