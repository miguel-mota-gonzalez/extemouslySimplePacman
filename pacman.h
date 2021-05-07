#ifndef PACMAN_H
#define PACMAN_H

#include "commonConst.h"
#include "gameActiveElement.h"
#include "gameBoard.h"

class pacman : gameActiveElement{

protected:
    int mX;
    int mY;
    int mPixelHeight;
    int mPixelWidth;
    QVector<QVector<gameBoard::boardElement>> *mBoard;
    bool mMouthOpen;
    int mScore;
    int mLastDirectionCommand;
    bool mProcessingCommand;
    bool mChasing;
    int mMessageToBoard;
    bool mDead;
    int mMintsAndPills;

public:
    pacman(const int x,
           const int y,
           QVector<QVector<gameBoard::boardElement>> *board,
           const int &pixelWidth,
           const int &pixelHeight
           );

    void makeNextMove(const int &keyCode);
    void drawInBoard(QPainter &painter, int &x, int &y);
    int getScore(int &mintsAndPills) {
        mintsAndPills=mMintsAndPills;
        return mScore;
    }
    void changeChasingStatus() { mChasing=(!mChasing); }
    int getMessage() {
        int tmpRet = mMessageToBoard;
        mMessageToBoard = NO_MESSAGE_FOR_THE_BOARD;
        return tmpRet;
    }
    void sendMessage(int, int &, int&){}
};

#endif // PACMAN_H
