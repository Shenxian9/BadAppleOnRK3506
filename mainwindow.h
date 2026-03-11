#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QStringList>

class QLabel;
class QMediaPlayer;
class QMouseEvent;
class QResizeEvent;
class QTimer;
class QVideoWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    bool loadVideoList();
    void playCurrentVideo();
    void playNextVideo();
    void playPreviousVideo();
    void togglePlayback();
    void setSystemVolume(int volumePercent);
    void showPlaybackInfoHint();
    void showTransientHint(const QString &text);
    void updateHintGeometry();

    QMediaPlayer *m_player;
    QVideoWidget *m_videoWidget;
    QLabel *m_hintLabel;
    QTimer *m_hintHideTimer;
    QStringList m_videoFiles;
    int m_currentIndex;
    int m_volumePercent;
    QPoint m_pressPos;
    QPoint m_lastMovePos;
    bool m_isDragging;
    bool m_volumeGestureTriggered;
};

#endif // MAINWINDOW_H
