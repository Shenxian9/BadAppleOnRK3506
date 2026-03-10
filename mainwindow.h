#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

private:
    bool loadVideoList();
    void playCurrentVideo();
    void playNextVideo();
    void playPreviousVideo();
    void togglePlayback();

    QMediaPlayer *m_player;
    QVideoWidget *m_videoWidget;
    QStringList m_videoFiles;
    int m_currentIndex;
};

#endif // MAINWINDOW_H
