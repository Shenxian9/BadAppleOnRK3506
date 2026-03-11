#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QStringList>

class QMediaPlayer;
class QMouseEvent;
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

private:
    bool loadVideoList();
    void playCurrentVideo();
    void playNextVideo();
    void playPreviousVideo();
    void togglePlayback();
    void setSystemVolume(int volumePercent);

    QMediaPlayer *m_player;
    QVideoWidget *m_videoWidget;
    QStringList m_videoFiles;
    int m_currentIndex;
    int m_volumePercent;
    QPoint m_pressPos;
    QPoint m_lastMovePos;
    bool m_isDragging;
    bool m_volumeGestureTriggered;
};

#endif // MAINWINDOW_H
