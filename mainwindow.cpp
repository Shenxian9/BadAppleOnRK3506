#include "mainwindow.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTimer>
#include <QUrl>
#include <QVideoWidget>

namespace {
// 固定播放文件：可按实际部署目录修改。
const char kVideoFilePath[] = "/opt/badapple/videos/xxx.mp4";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_player(new QMediaPlayer(this))
    , m_videoWidget(new QVideoWidget(this))
{
    setCentralWidget(m_videoWidget);
    m_player->setVideoOutput(m_videoWidget);

    const QFileInfo videoInfo(QString::fromUtf8(kVideoFilePath));
    if (!videoInfo.exists() || !videoInfo.isFile()) {
        QMessageBox::critical(this,
                              QStringLiteral("视频不存在"),
                              QStringLiteral("未找到指定视频文件：\n%1").arg(videoInfo.absoluteFilePath()));
        QTimer::singleShot(0, qApp, &QCoreApplication::quit);
        return;
    }

    m_player->setSource(QUrl::fromLocalFile(videoInfo.absoluteFilePath()));
    connect(m_player, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error, const QString &errorString) {
        QMessageBox::critical(this, QStringLiteral("播放错误"), errorString);
    });

    showFullScreen();
    m_player->play();
}

MainWindow::~MainWindow() = default;

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        togglePlayback();
        event->accept();
        return;
    }

    QMainWindow::mousePressEvent(event);
}

void MainWindow::togglePlayback()
{
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        m_player->pause();
    } else {
        m_player->play();
    }
}
