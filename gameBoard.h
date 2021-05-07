#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QWidget>
#include <QDebug>
#include <QTimer>
#include <QSharedPointer>

#include "gameActiveElement.h"

class gameBoard : public QWidget
{
    Q_OBJECT

public:

    struct boardElement{
        int hardContent;
        int dynamicContent;
    };

    static gameBoard *singleInstance(QWidget *parent);
    static gameBoard *singleInstance();

    static void destroySingleton(){
                                    if(mInstance!=nullptr){
                                        delete mInstance;
                                    }
                                    mInstance=nullptr;
                                  };

protected:

    QTimer *mTimer;
    int mPixelHeight;
    int mPixelWidth;
    int mScore;
    int mTotalMints;
    int mPillEffectTimeRemaining;
    bool mGameActive;
    QVector<QVector<boardElement>> mBoardContent;
    QVector<QSharedPointer<gameActiveElement>> mGameActiveElements;
    int mPacmanPostionX;
    int mPacmanPostionY;

    // Singleton instance, No smart pointer here
    // 'cause the window wodget will destroy it
    static gameBoard *mInstance;

    // Events 
    void paintEvent(QPaintEvent *event) override;

    // Drawing methods
    void drawBoard(QPainter &painter);
    void drawBoardPixel(QPainter &painter, int x, int y);
    void drawMint(QPainter &painter, int x, int y);
    void drawPill(QPainter &painter, int x, int y);
    void drawScore(QPainter &painter);

    // Game actions
    void initGame();
    void restartGame();

    // Helpers
    void loadBoard();

    // Constructors - Destructors  TODO: Do I need a virtual destructor?
    gameBoard(QWidget *parent);
    ~gameBoard() { qDebug() << "game board destroyed!"; };

protected slots:

    // Time for updating screen events
    void updateScreenTimerSlot();

public:
    void setLastKeyPressed(int keyCode);

};

#endif // GAMEBOARD_H
