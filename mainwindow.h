#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chunklifter.h"
#include "region.h"
#include <QElapsedTimer>
#include <QList>
#include <QQueue>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void complete();
    void regionComplete();
    void chunkComplete();

private slots:
    void on_dirEdit_editingFinished();
    void on_startButton_clicked();

private:
    Ui::MainWindow *ui;
    bool running;
    Region* region;
    QQueue<QString> regionFiles;
    QList<ChunkLifter*> chunkLifters;
    int nextThread;
    int threadsRunning;
    QElapsedTimer timer;
};

#endif // MAINWINDOW_H
