
#include <QDebug>
#include <QPainter>

#include "pacman.h"

const int PIE_START_RIGHT     = 0;
const int PIE_ENDS_RIGHT      = 5000;
const int PIE_START_LEFT      = 3500;
const int PIE_ENDS_LEFT       = 5000;
const int PIE_START_UP        = 2000;
const int PIE_ENDS_UP         = 5000;
const int PIE_START_DOWN      = 5000;
const int PIE_ENDS_DOWN       = 5000;

const int EYE_SIZE            = 5;
const int EYE_ADJUSTMENT      = 5;
const int KEY_UP              = 16777235;
const int KEY_DOWN            = 16777237;
const int KEY_LEFT            = 16777234;
const int KEY_RIGHT           = 16777236;

pacman::pacman(const int x,
       const int y,
       QVector<QVector<gameBoard::boardElement>> *board,
       const int &pixelWidth,
       const int &pixelHeight
       ) : mMouthOpen(true),
           mScore(0),
           mLastDirectionCommand(NO_COMMAND_RECEIVED),
           mProcessingCommand(false),
           mChasing(false),
           mMessageToBoard(NO_MESSAGE_FOR_THE_BOARD),
           mDead(false),
           mMintsAndPills(0)
         {
          mX = x;
          mY = y;
          mBoard = board;
          mPixelWidth = pixelWidth;
          mPixelHeight = pixelHeight;
         }

void pacman::drawInBoard(QPainter &painter, int &x, int &y)
{
    if (!mProcessingCommand){
        makeNextMove(mLastDirectionCommand);
    }

    // clear the space
    painter.setPen(QPen(Qt::black));
    painter.setBrush(QBrush(Qt::black));
    painter.drawRect((mX*mPixelWidth), (mY*mPixelHeight), mPixelWidth, mPixelHeight);

    if(!mDead){
        qDebug() << "pacman : Drawing in x=" << mX << ", y=" << mY;

        painter.setPen(QPen(Qt::black));
        painter.setBrush(QBrush(Qt::yellow));

        if(!mMouthOpen){

            switch(mLastDirectionCommand){
            case KEY_UP:
                painter.drawPie((mX*mPixelWidth), (mY*mPixelHeight), mPixelWidth, mPixelHeight, PIE_START_UP , PIE_ENDS_UP);
                break;
            case KEY_LEFT:
                painter.drawPie((mX*mPixelWidth), (mY*mPixelHeight), mPixelWidth, mPixelHeight, PIE_START_LEFT , PIE_ENDS_LEFT);
                break;
            case KEY_DOWN:
                painter.drawPie((mX*mPixelWidth), (mY*mPixelHeight), mPixelWidth, mPixelHeight, PIE_START_DOWN , PIE_ENDS_DOWN);
                break;
            case KEY_RIGHT:
            default:
                painter.drawPie((mX*mPixelWidth), (mY*mPixelHeight), mPixelWidth, mPixelHeight, PIE_START_RIGHT , PIE_ENDS_RIGHT);
                break;
            };
        }
        else{
            painter.drawEllipse((mX*mPixelWidth), (mY*mPixelHeight), mPixelWidth, mPixelHeight);
        }

        painter.setBrush(QBrush(Qt::black));

        switch(mLastDirectionCommand){
        case KEY_UP:
            painter.drawEllipse((mX*mPixelWidth) + (mPixelWidth - (mPixelWidth/3)), (mY*mPixelHeight) + (mPixelHeight/4), EYE_SIZE, EYE_SIZE);
            break;
        case KEY_LEFT:
            painter.drawEllipse((mX*mPixelWidth) + ((mPixelWidth/3)) + EYE_ADJUSTMENT, (mY*mPixelHeight) + (mPixelHeight/4), EYE_SIZE, EYE_SIZE);
            break;
        case KEY_DOWN:
            painter.drawEllipse((mX*mPixelWidth) + ((mPixelWidth/3)) - EYE_ADJUSTMENT, (mY*mPixelHeight) + (mPixelHeight/4) + EYE_ADJUSTMENT, EYE_SIZE, EYE_SIZE);
            break;
        case KEY_RIGHT:
        default:
            painter.drawEllipse((mX*mPixelWidth) + (mPixelWidth - (mPixelWidth/3)), (mY*mPixelHeight) + (mPixelHeight/4), EYE_SIZE, EYE_SIZE);
            break;
        };
    }

    mMouthOpen = !mMouthOpen;

    mProcessingCommand = false;

    x=mX;
    y=mY;
}

void pacman::makeNextMove(const int &keyCode)
{
    int lX = mX;
    int lY = mY;
    int tmDirectionCommand = NO_COMMAND_RECEIVED;

    if(mDead)
        return;

    if (mProcessingCommand)
        return;
    else
        mProcessingCommand = true;

    std::function<void()> makeMovement = [&]() -> void{

        if((*mBoard)[lY][lX].dynamicContent==GOST){
            if(mChasing){
                qDebug() << "pacman : killing a gost!";
                mScore+=GOST_SCORE;
            }
            else{
                qDebug() << "pacman : a gost killed me!";
                mDead=true;
                mMessageToBoard = GAME_OVER;
            }
        }

        switch((*mBoard)[lY][lX].hardContent)
        {
        case PILL:
            mMessageToBoard = (!mChasing ? CHANGE_CHASING_STATUS : EXTEND_CHASING_TIME);
            mScore+=PILL_SCORE;
            mMintsAndPills++;
            (*mBoard)[mY][mX].dynamicContent = NO_DYNAMIC_CONTENT;
            (*mBoard)[lY][lX].hardContent = EMPTY;
            (*mBoard)[lY][lX].dynamicContent = PACMAN;
            mX = lX;
            mY = lY;
            break;
        case MINT:
            mScore+=MINT_SCORE;
            mMintsAndPills++;
            (*mBoard)[mY][mX].dynamicContent = NO_DYNAMIC_CONTENT;
            (*mBoard)[lY][lX].hardContent=EMPTY;
            (*mBoard)[lY][lX].dynamicContent = PACMAN;
            mX = lX;
            mY = lY;
            break;
        case EMPTY:
            (*mBoard)[mY][mX].dynamicContent = NO_DYNAMIC_CONTENT;
            (*mBoard)[lY][lX].dynamicContent = PACMAN;
            mX = lX;
            mY = lY;
            break;
        case WALL:
            mProcessingCommand = false;
            mLastDirectionCommand = tmDirectionCommand;
            break;
        };
    };

    qDebug() << "Total pills pacman << " << mMintsAndPills;

    // save last command in case we need to invaliate the current one
    tmDirectionCommand = mLastDirectionCommand;
    mLastDirectionCommand = keyCode;

    switch(keyCode){
    case KEY_UP:
        lY--;
        break;
    case KEY_DOWN:
        lY++;
        break;
    case KEY_LEFT:
        lX--;
        break;
    case KEY_RIGHT:
        lX++;
        break;
    };

    if( lY>=0 && lY<BOARD_PIXELS_Y && lX>=0 && lX < BOARD_PIXELS_X ){
        makeMovement();
    }
    else{
        // Tunnels
        if(lX <= 0)
            lX = BOARD_PIXELS_X-1;
        else if(lY <= 0)
            lY = BOARD_PIXELS_Y-1;
        else if(lX > BOARD_PIXELS_X-1)
            lX = 0;
        else if(lY > BOARD_PIXELS_Y-1)
            lY = 0;

        makeMovement();
    }
}
