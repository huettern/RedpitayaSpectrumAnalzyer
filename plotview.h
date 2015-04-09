#ifndef PLOTVIEW_H
#define PLOTVIEW_H

#include <QMainWindow>

namespace Ui {
class PlotView;
}

class PlotView : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotView(QWidget *parent = 0);
    ~PlotView();

private slots:
    void on_quitButton_clicked();

private:
    Ui::PlotView *ui;
};

#endif // PLOTVIEW_H
