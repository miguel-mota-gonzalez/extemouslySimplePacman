#ifndef GOST_H
#define GOST_H

#include "commonConst.h"
#include "gameActiveElement.h"
#include "gameBoard.h"

class gost : gameActiveElement{

protected:
    Qt::GlobalColor mColor;
    int mX;
    int mY;
    int mPixelHeight;
    int mPixelWidth;
    bool mDead;
    bool mChasing;
    int mMessageToBoard;
    QVector<QVector<gameBoard::boardElement>> *mBoard;
    int mContentBeforeMe;
    bool mCanMove;
    int mPacmanPostionX;
    int mPacmanPostionY;

    void calculateNextMove();

public:
    gost(Qt::GlobalColor color,
         const int x,
         const int y,
         QVector<QVector<gameBoard::boardElement>> *board,
         const int &pixelWidth,
         const int &pixelHeight
         );

    void makeNextMove(const int &keyCode);
    void drawInBoard(QPainter &painter, int &, int &);
    int getScore(int &) { return NO_SCORE; }
    void changeChasingStatus() { mChasing=(!mChasing); }
    int getMessage() {
        int tmpRet = mMessageToBoard;
        mMessageToBoard = NO_MESSAGE_FOR_THE_BOARD;
        return tmpRet;
    }
    void sendMessage(int gameStatus, int &x, int &y){
        if(gameStatus==GAME_OVER)
            mCanMove=false;

        mPacmanPostionX=x;
        mPacmanPostionY=y;
    }
};

#endif // GOST_H
