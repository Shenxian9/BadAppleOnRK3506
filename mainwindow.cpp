#include "mainwindow.h"

#include <QCoreApplication>
#include <QDir>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTimer>
#include <QUrl>
#include <QVideoWidget>

namespace {
const char kVideoDirectory[] = "/mnt/udisk";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_player(new QMediaPlayer(this))
    , m_videoWidget(new QVideoWidget(this))
    , m_currentIndex(0)
{
    setCentralWidget(m_videoWidget);
    m_player->setVideoOutput(m_videoWidget);

    connect(m_player, static_cast<void (QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error), this,
            [this](QMediaPlayer::Error) {
                QMessageBox::critical(this, QStringLiteral("播放错误"), m_player->errorString());
            });

    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            playNextVideo();
        }
    });

    if (!loadVideoList()) {
        QTimer::singleShot(0, qApp, &QCoreApplication::quit);
        return;
    }

    showFullScreen();
    playCurrentVideo();
}

MainWindow::~MainWindow() = default;

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        const int thirdWidth = width() / 3;
        const int x = event->pos().x();

        if (x < thirdWidth) {
            playPreviousVideo();
        } else if (x < thirdWidth * 2) {
            togglePlayback();
        } else {
            playNextVideo();
        }

        event->accept();
        return;
    }

    QMainWindow::mousePressEvent(event);
}

bool MainWindow::loadVideoList()
{
    const QDir videoDir(QString::fromUtf8(kVideoDirectory));
    const QStringList nameFilters = {
        QStringLiteral("*.mp4"),
        QStringLiteral("*.MP4"),
        QStringLiteral("*.mkv"),
        QStringLiteral("*.MKV"),
        QStringLiteral("*.avi"),
        QStringLiteral("*.AVI")
    };

    m_videoFiles = videoDir.entryList(nameFilters, QDir::Files, QDir::Name);
    for (QString &fileName : m_videoFiles) {
        fileName = videoDir.absoluteFilePath(fileName);
    }

    if (m_videoFiles.isEmpty()) {
        QMessageBox::critical(this,
                              QStringLiteral("未找到视频"),
                              QStringLiteral("目录 %1 下没有可播放视频文件。").arg(videoDir.absolutePath()));
        return false;
    }

    m_currentIndex = 0;
    return true;
}

void MainWindow::playCurrentVideo()
{
    if (m_videoFiles.isEmpty()) {
        return;
    }

    m_player->setMedia(QUrl::fromLocalFile(m_videoFiles.at(m_currentIndex)));
    m_player->play();
}

void MainWindow::playNextVideo()
{
    if (m_videoFiles.isEmpty()) {
        return;
    }

    m_currentIndex = (m_currentIndex + 1) % m_videoFiles.size();
    playCurrentVideo();
}

void MainWindow::playPreviousVideo()
{
    if (m_videoFiles.isEmpty()) {
        return;
    }

    m_currentIndex = (m_currentIndex - 1 + m_videoFiles.size()) % m_videoFiles.size();
    playCurrentVideo();
}

void MainWindow::togglePlayback()
{
    if (m_player->state() == QMediaPlayer::PlayingState) {
        m_player->pause();
    } else {
        m_player->play();
    }
}
