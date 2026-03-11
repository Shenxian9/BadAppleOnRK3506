#include "mainwindow.h"

#include <QCoreApplication>
#include <QDir>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QMouseEvent>
#include <QProcess>
#include <QTimer>
#include <QUrl>
#include <QVideoWidget>

namespace {
const char kVideoDirectory[] = "/mnt/udisk";
constexpr int kSwipeStepPixels = 12;
constexpr int kVolumeStepPercent = 2;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_player(new QMediaPlayer(this))
    , m_videoWidget(new QVideoWidget(this))
    , m_currentIndex(0)
    , m_volumePercent(50)
    , m_isDragging(false)
    , m_volumeGestureTriggered(false)
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
        m_pressPos = event->pos();
        m_lastMovePos = event->pos();
        m_isDragging = true;
        m_volumeGestureTriggered = false;
        event->accept();
        return;
    }

    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_isDragging || !(event->buttons() & Qt::LeftButton)) {
        QMainWindow::mouseMoveEvent(event);
        return;
    }

    const bool isRightHalf = m_pressPos.x() >= width() / 2;
    if (!isRightHalf) {
        m_lastMovePos = event->pos();
        event->accept();
        return;
    }

    const int deltaY = m_lastMovePos.y() - event->pos().y();
    const int steps = deltaY / kSwipeStepPixels;
    if (steps != 0) {
        setSystemVolume(m_volumePercent + steps * kVolumeStepPercent);
        m_lastMovePos.setY(m_lastMovePos.y() - steps * kSwipeStepPixels);
        m_volumeGestureTriggered = true;
    }

    event->accept();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isDragging) {
        const int thirdWidth = width() / 3;
        const int releaseX = event->pos().x();

        if (!m_volumeGestureTriggered) {
            if (releaseX < thirdWidth) {
                playPreviousVideo();
            } else if (releaseX >= thirdWidth * 2) {
                playNextVideo();
            }
        }

        m_isDragging = false;
        event->accept();
        return;
    }

    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        const int thirdWidth = width() / 3;
        const int x = event->pos().x();

        if (x >= thirdWidth && x < thirdWidth * 2) {
            togglePlayback();
            event->accept();
            return;
        }
    }

    QMainWindow::mouseDoubleClickEvent(event);
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

void MainWindow::setSystemVolume(int volumePercent)
{
    m_volumePercent = qBound(0, volumePercent, 100);
    QProcess::execute(QStringLiteral("amixer"),
                      {QStringLiteral("-c"), QStringLiteral("0"), QStringLiteral("set"), QStringLiteral("Master"),
                       QStringLiteral("%1%").arg(m_volumePercent)});
}
