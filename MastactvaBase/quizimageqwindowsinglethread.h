#ifndef QUIZIMAGEQWINDOWSINGLETHREAD_H
#define QUIZIMAGEQWINDOWSINGLETHREAD_H

#include <QWindow>
#include <QObject>
#include <QWidget>

class QuizImageQWindowSingleThread : public QWindow
{
    Q_OBJECT
public:
    QuizImageQWindowSingleThread();
};

#endif // QUIZIMAGEQWINDOWSINGLETHREAD_H
