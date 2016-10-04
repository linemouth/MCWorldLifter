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
    if(!chunkLifters.empty()) // This should be empty, but if it isn't, make sure it gets cleaned up
    {
        for(ChunkLifter* cl : chunkLifters)
        {
            delete cl;
        }
        chunkLifters.clear();
    }

    if(!running) // We were aborted, discard the last region
    {
        if(region != nullptr)
        {
            delete region;
        }
        regionFiles.clear();
        complete();
        return;
    }

    if(region != nullptr) // Clean up memory
    {
        region->save();
        delete region;
        region = nullptr;
    }

    if(regionFiles.empty()) // We finished the last region file
    {
        complete();
        return;
    }

    qDebug() << "Getting next region";
    QString nextFilename = regionFiles.dequeue(); // Get next region file
    QFileInfo fi(nextFilename);
    ui->statusBar->showMessage(fi.fileName()); // Show the filename being processed
    region = new Region(nextFilename); // Create the new file object

    if(region->size() == 0)
    {
        delete region; // If the region is invalid, get rid of it
    }
    else
    {
        region->load(); // Load the region
        for(int i=0; i<1024; ++i) // Add the new region's chunks to the queue to be lifted
        {
            if(region->chunks[i])
            {
                qDebug() << "Adding thread";
                ChunkLifter* newThread = new ChunkLifter(region->chunks[i]); // Create a new lifter thread for each existing chunk
                connect(newThread, SIGNAL(complete()), this, SLOT(chunkComplete())); // Allow the thread to send a signal to the gui
                chunkLifters.append(newThread); // Attach the thread to a chunk
            }
        }

        qDebug() << chunkLifters.size();
    }

    // Start n threads
    threadsRunning = 0;
    for(nextThread = 0; nextThread < QThread::idealThreadCount() && nextThread < chunkLifters.size(); ++nextThread)
    {
        qDebug() << "Starting thread";
        chunkLifters[nextThread]->start();
        ++threadsRunning;
    }
}

void MainWindow::chunkComplete()
{
    --threadsRunning;
    ui->progressBar->setValue(ui->progressBar->value() + 1);

    if(nextThread == chunkLifters.size() || !running) // There's no more threads to run
    {
        if(threadsRunning == 0) // We were the last thread
        {
            regionComplete();
        }
    }
    else
    {
        if(threadsRunning < QThread::idealThreadCount() && nextThread < chunkLifters.size()) // We're clear to start a new thread
        {
            chunkLifters[nextThread]->start();
            ++nextThread;
            ++threadsRunning;
        }
    }
}

void MainWindow::on_dirEdit_editingFinished()
{
    QDir dir(ui->dirEdit->text());
    if(dir.exists())
    {
        regionFiles.clear();
        QDirIterator iter(dir.absolutePath(), QStringList() << "*.mca", QDir::Files, QDirIterator::NoIteratorFlags);
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

        ui->statusBar->showMessage(QString("Ready to lift %1 map files (%2 chunks)").arg(regionFiles.size()).arg(chunks));
        ui->startButton->setEnabled(true);
        ui->progressBar->setMaximum(1024*regionFiles.size());
        ui->progressBar->setTextVisible(true);
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
        ChunkLifter::configure(ui->bottomSpinBox->value(), ui->amountSpinBox->value());

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
