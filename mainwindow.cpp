#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDirIterator>
#include "region.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    running(false),
    threadsRunning(0)
{
    ui->setupUi(this);
    on_dirEdit_editingFinished();
}
MainWindow::~MainWindow()
{
    if(region != nullptr)
    {
        delete region;
    }
    delete ui;
}

void MainWindow::complete()
{
    running = false;
    ui->bottomSpinBox->setEnabled(true);
    ui->amountSpinBox->setEnabled(true);
    ui->startButton->setText("Lift it!");
    ui->startButton->setEnabled(true);
    int ms = timer.elapsed() % 1000;
    int s = (timer.elapsed() / 1000) % 60;
    int m = (timer.elapsed() / 60000) % 60;
    int h = timer.elapsed() / 3600000;
    ui->statusBar->showMessage(QString("Completed after %1:%2:%3.%4").arg(h, 1, 10, QChar('0')).arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0')).arg(ms, 3, 10, QChar('0')));
    ui->progressBar->setMaximum(1);
    ui->progressBar->setTextVisible(false);
    ui->dirEdit->setEnabled(true);
}
void MainWindow::regionComplete()
{
    if(region != nullptr) // Clean up memory
    {
        region->save();
        delete region;
        region = nullptr;
    }
    if(regionFiles.empty()) // We finished the last region file
    {
        complete();
    }
    else
    {
        ui->statusBar->showMessage(QFileInfo(regionFiles.first()).fileName()); // Show the filename being processed
        region = new Region(regionFiles.dequeue());
        region->load();
        threadsRunning = 0;
        for(int i=0; i<1024; ++i)
        {
            if(region->chunks[i] != nullptr)
            {
                connect(this, SIGNAL(lift()), region->chunks[i], SLOT(lift()));
                connect(region->chunks[i], SIGNAL(complete(int)), this, SLOT(chunkComplete()));
                ++threadsRunning;
            }
        }
        emit lift();
    }
}
void MainWindow::chunkComplete()
{
    --threadsRunning;
    ui->progressBar->setValue(ui->progressBar->value() + 1);

    if(threadsRunning == 0)
    {
        regionComplete();
    }
}
void MainWindow::on_dirEdit_editingFinished()
{
    QDir dir(ui->dirEdit->text());
    if(dir.exists())
    {
        regionFiles.clear();
        QDirIterator iter(dir.absolutePath(), QStringList() << "r.0.0.mca", QDir::Files, QDirIterator::NoIteratorFlags);
        while(iter.hasNext())
        {
            regionFiles.append(iter.next());
        }
    }

    if(regionFiles.size() > 0)
    {
        int chunks = 0;
        for(QString filename : regionFiles)
        {
            region = new Region(filename);
            chunks += region->size();
            delete region;
            region = nullptr;
        }

        if(chunks > 0)
        {
            ui->statusBar->showMessage(QString("Ready to lift %1 map files (%2 chunks)").arg(regionFiles.size()).arg(chunks));
            ui->startButton->setEnabled(true);
            ui->progressBar->setMaximum(chunks);
            ui->progressBar->setTextVisible(true);
        }
    }
    else
    {
        ui->statusBar->showMessage("Select a region folder");
        ui->startButton->setEnabled(false);
        ui->progressBar->setTextVisible(false);
    }
}
void MainWindow::on_startButton_clicked()
{
    if(!running)
    {
        on_dirEdit_editingFinished();
        ui->bottomSpinBox->setEnabled(false);
        ui->amountSpinBox->setEnabled(false);
        ui->startButton->setText("Stop");
        ui->dirEdit->setEnabled(false);

        running = true;
        timer.restart();
        regionComplete();
    }
    else
    {
        running = false;
        ui->startButton->setEnabled(false);
        ui->statusBar->showMessage("Waiting for threads to stop");
        ui->progressBar->setMaximum(0);
        ui->progressBar->setTextVisible(false);
    }
}
