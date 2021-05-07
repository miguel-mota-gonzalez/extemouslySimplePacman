
#include <QDebug>

#include "gost.h"

const int   EYE_SIZE            = 6;
const float GOST_RED_PCT        = 0.75;
const int   SCARED_MOUTH_W      = 15;
const int   SCARED_MOUTH_H      = 8;
const int GOST_TO_THE_RIGHT     = 1;
const int GOST_TO_THE_LEFT      = 2;
const int GOST_UP               = 3;
const int GOST_DOWN             = 4;

gost::gost(
     Qt::GlobalColor color,
     const int x,
     const int y,
     QVector<QVector<gameBoard::boardElement>> *board,
     const int &pixelWidth,
     const int &pixelHeight
     ) : mDead(false),
         mChasing(true),
         mMessageToBoard(NO_MESSAGE_FOR_THE_BOARD),
         mContentBeforeMe(MINT),
         mCanMove(false),
         mPacmanPostionX(8),
         mPacmanPostionY(10){
    mColor = color;
    mX = x;
    mY = y;
    mBoard = board;
    mPixelWidth = pixelWidth;
    mPixelHeight = pixelHeight;
}

void gost::calculateNextMove(){
    QVector<int> availableMoves;

    if(!mCanMove)
        return;

    qDebug() << "pacman is in : x=" << mPacmanPostionX << ", y=" << mPacmanPostionY;

    if (mX+1 < BOARD_PIXELS_X && (((*mBoard)[mY][mX+1].hardContent==EMPTY ||
                                  (*mBoard)[mY][mX+1].hardContent==MINT ||
                                  (*mBoard)[mY][mX+1].hardContent==PILL) && (*mBoard)[mY][mX+1].dynamicContent!=GOST
                                  )
        )
        availableMoves.push_back(GOST_TO_THE_RIGHT);

    if (mX-1 >= 0 && (((*mBoard)[mY][mX-1].hardContent==EMPTY ||
                      (*mBoard)[mY][mX-1].hardContent==MINT ||
                      (*mBoard)[mY][mX-1].hardContent==PILL) && (*mBoard)[mY][mX-1].dynamicContent!=GOST
                      )
            )
        availableMoves.push_back(GOST_TO_THE_LEFT);

    if (mY+1 < BOARD_PIXELS_Y && (((*mBoard)[mY+1][mX].hardContent==EMPTY ||
                                  (*mBoard)[mY+1][mX].hardContent==MINT ||
                                  (*mBoard)[mY+1][mX].hardContent==PILL) && (*mBoard)[mY+1][mX].dynamicContent!=GOST
                                  )
            )
        availableMoves.push_back(GOST_DOWN);

    if (mY-1 >= 0 && (((*mBoard)[mY-1][mX].hardContent==EMPTY ||
                      (*mBoard)[mY-1][mX].hardContent==MINT ||
                      (*mBoard)[mY-1][mX].hardContent==PILL) && (*mBoard)[mY-1][mX].dynamicContent!=GOST
                      )
            )
        availableMoves.push_back(GOST_UP);

    bool moveMade=false;
    int lX=mX;
    int lY=mY;

    for(const int &move : availableMoves){
        switch(move){
        case GOST_TO_THE_RIGHT:
            if((mChasing && mX<mPacmanPostionX) || (!mChasing && mX>mPacmanPostionX)){
                mX++;
                moveMade=true;
            }
            break;
        case GOST_TO_THE_LEFT:
            if((mChasing && mX>mPacmanPostionX) || (!mChasing && mX<mPacmanPostionX)){
                mX--;
                moveMade=true;
            }
            break;
        case GOST_UP:
            if((mChasing && mY>mPacmanPostionY) || (!mChasing && mY<mPacmanPostionY)){
                mY--;
                moveMade=true;
            }
            break;
        case GOST_DOWN:
            if((mChasing && mY<mPacmanPostionY) || (!mChasing && mY>mPacmanPostionY)){
                mY++;
                moveMade=true;
            }
            break;
        };

        if(moveMade)
            break;
    }

    // No decision made? just move...
    if(!moveMade && availableMoves.size()>0){
        switch(availableMoves[0]){
        case GOST_TO_THE_RIGHT:
                mX++;
            break;
        case GOST_TO_THE_LEFT:
                mX--;
            break;
        case GOST_UP:
                mY--;
            break;
        case GOST_DOWN:
                mY++;
            break;
        };
    }

    // is pacman? the he is dead...
    if((*mBoard)[mY][mX].dynamicContent==PACMAN){
        if(mChasing){
            mMessageToBoard = GAME_OVER;
            qDebug() << "I KILLED PACMAN!!!";
        }
        else{
            qDebug() << "gost : I'm dead!";
            mDead=true;
        }
    }

    (*mBoard)[lY][lX].dynamicContent=NO_DYNAMIC_CONTENT;
    (*mBoard)[mY][mX].dynamicContent=GOST;
}

void gost::makeNextMove(const int &keyPessed){

    if(mDead)
        return;

    if(keyPessed!=NO_COMMAND_RECEIVED){
        mCanMove = true;
        return;
    }

    if(!mChasing && (*mBoard)[mY][mX].dynamicContent==PACMAN/*(tmp==PACMAN || tmp==EMPTY)*/){
        qDebug() << "gost : I'm dead!";
        mDead=true;
    }
    else{
        calculateNextMove();
    }
}

void gost::drawInBoard(QPainter &painter, int &, int &){

    makeNextMove(NO_COMMAND_RECEIVED);

    if(!mDead){
        qDebug() << "gost : Drawing in x=" << mX << ", y=" << mY;

        // clear the space
        painter.setPen(QPen(Qt::black));
        painter.setBrush(QBrush(Qt::black));
        painter.drawRect((mX*mPixelWidth), (mY*mPixelHeight), mPixelWidth, mPixelHeight);

        // draw the body
        painter.setPen(QPen(mChasing ? mColor : Qt::cyan));
        painter.setBrush(QBrush(mChasing ? mColor : Qt::cyan));

        double tmpWidthDbl = (((mPixelWidth*100)*GOST_RED_PCT)/100);
        int tmpWidth = tmpWidthDbl;
        painter.drawEllipse((mX*mPixelWidth), (mY*mPixelHeight), tmpWidth, mPixelHeight);
        painter.drawRect((mX*mPixelWidth), (mY*mPixelHeight) + (mPixelHeight/2), tmpWidth, (mPixelHeight/2));

        // the eyes!
        painter.setPen(QPen(Qt::black));
        painter.setBrush(QBrush(Qt::black));
        painter.drawEllipse((mX*mPixelWidth) + tmpWidth/4, (mY*mPixelHeight) + (mPixelHeight/3), EYE_SIZE, EYE_SIZE);
        painter.drawEllipse((mX*mPixelWidth) + (tmpWidth/4)*3, (mY*mPixelHeight) + (mPixelHeight/3), EYE_SIZE, EYE_SIZE);

        if(!mChasing){
            // scared mouth
            painter.drawEllipse((mX*mPixelWidth) + (tmpWidth/4),
                                (mY*mPixelHeight) + (mPixelHeight/3)*2,
                                SCARED_MOUTH_W,
                                SCARED_MOUTH_H);
        }
        else{
            // mouth
            painter.drawLine((mX*mPixelWidth) + (tmpWidth/4)*2,
                             (mY*mPixelHeight) + (mPixelHeight/4)*3,
                             (mX*mPixelWidth) + (tmpWidth/4)*3,
                             (mY*mPixelHeight) + (mPixelHeight/4)*3);
        }
    }
}
