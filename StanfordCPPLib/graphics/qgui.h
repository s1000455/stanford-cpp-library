/*
 * File: qgui.h
 * ------------
 *
 * @version 2018/07/03
 * - initial version
 */

#ifndef _qgui_h
#define _qgui_h

#include <functional>
#include <string>
#include <QApplication>
#include <QMutex>
#include <QObject>
#include <QThread>
#include "queue.h"

// function pointer (no params / no return)
typedef std::function<void()> QGThunk;
typedef std::function<int()> QGThunkInt;

void __initializeStanfordCppLibraryQt(int argc, char** argv, int (* mainFunc)(void));

class QGui;   // forward declaration


class QGStudentThread : public QThread {
public:
    QGStudentThread(QGThunkInt mainFunc);
    int getResult() const;

protected:
    void run();

private:
    QGThunkInt _mainFunc;
    int _result;
};


class QGuiEventQueue : public QObject {
    Q_OBJECT
signals:
    void mySignal();

private:
    QGuiEventQueue();

    QGThunk dequeue();
    static QGuiEventQueue* instance();
    bool isEmpty() const;
    QGThunk peek();
    void runOnQtGuiThreadAsync(QGThunk thunk);
    void runOnQtGuiThreadSync(QGThunk thunk);

    static QGuiEventQueue* _instance;
    Queue<QGThunk> _functionQueue;
    QMutex _queueMutex;

    friend class QGui;
};


/*
 * ...
 */
class QGui : public QObject {
    Q_OBJECT

public:
    void ensureThatThisIsTheQtGuiThread(const std::string& message = "");
    void exitGraphics(int exitCode = 0);
    QThread* getCurrentThread();
    QThread* getQtMainThread();
    QThread* getStudentThread();
    static bool iAmRunningOnTheQtGuiThread();
    static bool iAmRunningOnTheStudentThread();
    void initializeQt();
    static QGui* instance();
    void runOnQtGuiThread(QGThunk func);
    void runOnQtGuiThreadAsync(QGThunk func);
    void startBackgroundEventLoop(QGThunkInt mainFunc);
    void startEventLoop();

public slots:
    void mySlot();

signals:
    void mySignal();

private:
    QGui();   // forbid construction

    static QApplication* _app;
    static QThread* _qtMainThread;
    static QGStudentThread* _studentThread;
    static int _argc;
    static char** _argv;
    static QGui* _instance;

    friend void __initializeStanfordCppLibraryQt(int argc, char** argv, int (* mainFunc)(void));
    friend class QGStudentThread;
    friend class QGuiEventQueue;
};

#include "private/init.h"   // ensure that Stanford C++ lib is initialized

#endif // _qgui_h