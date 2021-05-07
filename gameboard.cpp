#include "commonConst.h"
#include "gameBoard.h"
#include "pacman.h"
#include "gost.h"

#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QDebug>

const int GAME_SPEED          = 250;
const int SECS_FACTOR         = 4;
const int PILL_EFFECT_TIME    = 7;
const int BOARD_SIZE_X        = 640;
const int BOARD_SIZE_Y        = 480;
const int SCORE_BORARD_HEIGHT = 30;
const int MINT_HEIGHT         = 3;
const int PILL_HEIGHT         = 10;
const int SCORE_OFFSET        = 25;
const int GAME_OVER_TEXT_Y_POS= 160;
const int GAME_OVER_TEXT_X_POS= 60;
const int START_NEW_GAME_KEY  = 32;
const int GAME_OVER_SIZE      = 17;
const int SCORE_SIZE          = 20;

const QString GAME_OVER_TEXT = QStringLiteral("GAME OVER!");
const QString GAME_OVER_FONT = QStringLiteral("Arial");
const QString SCORE_FONT     = QStringLiteral("Arial");
const QString SCORE_TEXT     = QStringLiteral("SCORE  ");


const QVector<QVector<int>> boardTemplate = {
    {WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  EMPTY, WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  WALL},
    {WALL,  PILL,  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, PILL, WALL},
    {WALL,  EMPTY, WALL,  WALL,  WALL,  EMPTY, WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  EMPTY, WALL,  WALL,  WALL,  EMPTY, WALL},
    {WALL,  EMPTY, WALL,  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL,  EMPTY, WALL},
    {WALL,  EMPTY, WALL,  EMPTY, WALL,  EMPTY, WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  EMPTY, WALL,  EMPTY, WALL,  EMPTY, WALL},
    {WALL,  EMPTY, WALL,  EMPTY, EMPTY, EMPTY, WALL,  DSBLD, DSBLD, DSBLD, DSBLD, WALL,  EMPTY, EMPTY, EMPTY, WALL,  EMPTY, WALL},
    {WALL,  EMPTY, EMPTY, EMPTY, WALL,  EMPTY, WALL,  DSBLD, DSBLD, DSBLD, DSBLD, WALL,  EMPTY, WALL,  EMPTY, EMPTY, EMPTY, WALL},
    {WALL,  EMPTY, WALL,  EMPTY, EMPTY, EMPTY, WALL,  DSBLD, DSBLD, DSBLD, DSBLD, WALL,  EMPTY, EMPTY, EMPTY, WALL,  EMPTY, WALL},
    {WALL,  EMPTY, WALL,  WALL,  WALL,  EMPTY, WALL,  DSBLD, DSBLD, DSBLD, DSBLD, WALL,  EMPTY, WALL,  WALL,  WALL,  EMPTY, WALL},
    {EMPTY, EMPTY, WALL,  EMPTY, EMPTY, EMPTY, WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  EMPTY, EMPTY, EMPTY, WALL,  EMPTY, EMPTY},
    {WALL,  EMPTY, WALL,  EMPTY, WALL,  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL,  EMPTY, WALL,  EMPTY, WALL},
    {WALL,  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL,  EMPTY, WALL,  WALL,  EMPTY, WALL,  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL},
    {WALL,  EMPTY, WALL,  EMPTY, WALL,  EMPTY, WALL,  EMPTY, WALL,  WALL,  EMPTY, WALL,  EMPTY, WALL,  EMPTY, WALL,  EMPTY, WALL},
    {WALL,  EMPTY, WALL,  EMPTY, WALL,  EMPTY, WALL,  EMPTY, WALL,  WALL,  EMPTY, WALL,  EMPTY, WALL,  EMPTY, WALL,  EMPTY, WALL},
    {WALL,  EMPTY, WALL,  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL,  EMPTY, WALL},
    {WALL,  EMPTY, WALL,  WALL,  WALL,  EMPTY, WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  EMPTY, WALL,  WALL,  WALL,  EMPTY, WALL},
    {WALL,  PILL,  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, PILL,  WALL},
    {WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  EMPTY, WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  WALL}
                                           };

gameBoard *gameBoard::mInstance = nullptr;

gameBoard *gameBoard::singleInstance(QWidget *parent)
{
    if(mInstance == nullptr){
        mInstance = new gameBoard(parent);
    }
    return mInstance;
}

gameBoard *gameBoard::singleInstance()
{
    return mInstance;
}

gameBoard::gameBoard(QWidget *parent) : QWidget(parent),
    mPixelHeight(0),
    mPixelWidth(0),
    mScore(0),
    mTotalMints(0),
    mPillEffectTimeRemaining(0),
    mGameActive(true)
{
    mPixelWidth = BOARD_SIZE_X/BOARD_PIXELS_X;
    mPixelHeight = BOARD_SIZE_Y/BOARD_PIXELS_Y;

    setFixedSize(mPixelWidth*BOARD_PIXELS_X , (mPixelHeight*BOARD_PIXELS_Y) + SCORE_BORARD_HEIGHT);

    initGame();

    mTimer = new QTimer(this);

    // setup signal and slot
    connect(mTimer, SIGNAL(timeout()), this, SLOT(updateScreenTimerSlot()));

    // msec
    mTimer->start(GAME_SPEED);
}

void gameBoard::updateScreenTimerSlot()
{
    // repaint...
    update();

    // Check if pill is active
    if(mPillEffectTimeRemaining>0){
        mPillEffectTimeRemaining--;

        if(mPillEffectTimeRemaining==0)
            for (const QSharedPointer<gameActiveElement> &element2 : mGameActiveElements)
                element2->changeChasingStatus();
    }
}

void gameBoard::paintEvent(QPaintEvent*)
{
    QPainter painter;

    painter.begin(this);

    // draw active elements
    qDebug() << "---- gameborad : drawing active elements ----";
    for (const QSharedPointer<gameActiveElement> &element : mGameActiveElements){
        int tmpPosX=0;
        int tmpPosY=0;
        int tmpMintsAndPills=0;

        element->drawInBoard(painter, tmpPosX, tmpPosY);

        int lScore;
        if((lScore=element->getScore(tmpMintsAndPills))!=NO_SCORE){
            // it is pacman!
            mScore=lScore;
            mPacmanPostionX=tmpPosX;
            mPacmanPostionY=tmpPosY;

            // is the game completed?
            if(mTotalMints==tmpMintsAndPills){
                mGameActive = false;
                mTimer->stop();
            }
        }
        else{
            // it is a gost!
            element->sendMessage(mGameActive ? NO_MESSAGE_FOR_GOST : GAME_OVER, mPacmanPostionX, mPacmanPostionY);
        }

        // Mensajes
        switch(element->getMessage()){
        case CHANGE_CHASING_STATUS:
            for (const QSharedPointer<gameActiveElement> &element2 : mGameActiveElements)
                element2->changeChasingStatus();

            mPillEffectTimeRemaining = SECS_FACTOR*PILL_EFFECT_TIME;
            break;
        case EXTEND_CHASING_TIME:
            mPillEffectTimeRemaining += SECS_FACTOR*PILL_EFFECT_TIME;
            break;
        case GAME_OVER:
            mGameActive = false;
            mTimer->stop();
            break;
        case NO_MESSAGE_FOR_THE_BOARD:
        default:
            break;
        }
    }

    // Board
    drawBoard(painter);

    // Score
    drawScore(painter);


    if (!mGameActive){
        QFont gameOverFont(GAME_OVER_FONT, GAME_OVER_SIZE, QFont::Bold);
        painter.setFont(gameOverFont);
        painter.drawText((mPixelWidth*BOARD_PIXELS_X)/2 - GAME_OVER_TEXT_X_POS,
                        GAME_OVER_TEXT_Y_POS,
                        GAME_OVER_TEXT);
    }

    painter.end();
}

void gameBoard::drawScore(QPainter &painter)
{
    painter.setPen(QPen(Qt::black));
    painter.setBrush(QBrush(Qt::black));
    painter.drawRect((mPixelWidth*BOARD_PIXELS_X)/3,
                     (mPixelHeight*BOARD_PIXELS_Y)+1,
                     (mPixelWidth*BOARD_PIXELS_X)/3,
                     SCORE_BORARD_HEIGHT);
    QFont scoreFont(SCORE_FONT, SCORE_SIZE, QFont::Bold);
    painter.setPen(QPen(Qt::yellow));
    painter.setFont(scoreFont);
    painter.drawText((mPixelWidth*BOARD_PIXELS_X)/3,
                    (mPixelHeight*BOARD_PIXELS_Y) + SCORE_OFFSET,
                    SCORE_TEXT + QString::number(mScore));
}

void gameBoard::loadBoard(){
    for(int i=0; i<BOARD_PIXELS_X; i++){
        QVector<boardElement> currLine;

        for(int j=0; j<BOARD_PIXELS_Y; j++){
            boardElement be;
            be.hardContent=boardTemplate[i][j];
            be.dynamicContent=NO_DYNAMIC_CONTENT;

            currLine.append(be);
        }

        mBoardContent.append(currLine);
    }
}

void gameBoard::restartGame()
{
    mTimer->stop();
    update();

    mScore=0;
    mTotalMints=0;
    mPillEffectTimeRemaining=0;
    mGameActive=true;
    mBoardContent.clear();
    mGameActiveElements.clear();

    initGame();

    mTimer->start(GAME_SPEED);
}

void gameBoard::setLastKeyPressed(int keyCode)
{
    if (keyCode == START_NEW_GAME_KEY){
        restartGame();
        return;
    }

    for (const QSharedPointer<gameActiveElement> &element : mGameActiveElements){
        if (mGameActive){
            element->makeNextMove(keyCode);
        }
    }
}

void gameBoard::drawBoard(QPainter &painter)
{
    for(int i=0; i<BOARD_PIXELS_X; i++)
        for(int j=0; j<BOARD_PIXELS_Y; j++)
            drawBoardPixel(painter, i, j);

}

void gameBoard::drawMint(QPainter &painter, int x, int y)
{
    painter.setPen(QPen(Qt::black));
    painter.setBrush(QBrush(Qt::black));
    painter.drawRect((x*mPixelWidth), (y*mPixelHeight), mPixelWidth, mPixelHeight);

    painter.setPen(QPen(Qt::black));
    painter.setBrush(QBrush(Qt::yellow));
    painter.drawRect((x*mPixelWidth) + (mPixelWidth/3), (y*mPixelHeight) + (mPixelHeight/2), mPixelWidth/3, MINT_HEIGHT);
}

void gameBoard::drawPill(QPainter &painter, int x, int y)
{
    painter.setPen(QPen(Qt::black));
    painter.setBrush(QBrush(Qt::black));
    painter.drawRect((x*mPixelWidth), (y*mPixelHeight), mPixelWidth, mPixelHeight);

    painter.setPen(QPen(Qt::black));
    painter.setBrush(QBrush(Qt::green));
    painter.drawRect((x*mPixelWidth) + (mPixelWidth/3), (y*mPixelHeight) + (mPixelHeight/3), mPixelWidth/3, PILL_HEIGHT);
}

void gameBoard::drawBoardPixel(QPainter &painter, int x, int y)
{
    switch(mBoardContent[y][x].hardContent)
    {
    case WALL:
        painter.setPen(QPen(Qt::lightGray));
        painter.setBrush(QBrush(Qt::lightGray));
        painter.drawRect((x*mPixelWidth), (y*mPixelHeight), mPixelWidth, mPixelHeight);
        break;
    case MINT:
        if(mBoardContent[y][x].dynamicContent==NO_DYNAMIC_CONTENT)
            drawMint(painter, x, y);
        break;
    case PILL:
        if(mBoardContent[y][x].dynamicContent==NO_DYNAMIC_CONTENT)
            drawPill(painter, x, y);
        break;
    case EMPTY:
        if(mBoardContent[y][x].dynamicContent==NO_DYNAMIC_CONTENT){
            painter.setPen(QPen(Qt::black));
            painter.setBrush(QBrush(Qt::black));
            painter.drawRect((x*mPixelWidth), (y*mPixelHeight), mPixelWidth, mPixelHeight);
        }
        break;
    case DSBLD:
        painter.setPen(QPen(Qt::black));
        painter.setBrush(QBrush(Qt::black));
        painter.drawRect((x*mPixelWidth), (y*mPixelHeight), mPixelWidth, mPixelHeight);
        break;
    case PACMAN:
        break;
    case GOST:
        break;
    default:
        painter.setPen(QPen(Qt::green));
        painter.setBrush(QBrush(Qt::green));
        painter.drawRect((x*mPixelWidth), (y*mPixelHeight), mPixelWidth, mPixelHeight);
    }
}

void gameBoard::initGame()
{
    // Load the board
    loadBoard();

    // Load the game elements
    // Mr. PACMAN (nuestro héroe)
    mGameActiveElements.append(
                QSharedPointer<gameActiveElement>((gameActiveElement*)(new pacman(8,
                                                                                  10,
                                                                                  &mBoardContent,
                                                                                  mPixelWidth,
                                                                                  mPixelHeight)))
                );
    mBoardContent[8][10].dynamicContent=PACMAN;

    // The Enemies!
    Qt::GlobalColor gostColors[] = {Qt::blue, Qt::red, Qt::magenta, Qt::green};
    int xPos = 7;
    for (const auto &gostColor : gostColors){
        mGameActiveElements.append(
                    QSharedPointer<gameActiveElement>((gameActiveElement*)(new gost(gostColor,
                                                                                    xPos,
                                                                                    3,
                                                                                    &mBoardContent,
                                                                                    mPixelWidth,
                                                                                    mPixelHeight)))
                    );
        mBoardContent[3][xPos++].dynamicContent=GOST;
    }

    // set up content..
    for(int i=1; i<BOARD_PIXELS_X-1; i++){
        for(int j=1; j<BOARD_PIXELS_Y-1; j++){
            switch(mBoardContent[j][i].hardContent){
            case EMPTY:
                mBoardContent[j][i].hardContent=MINT;
                mTotalMints++;
                break;
            case PILL:
                mTotalMints++;
                break;
            };
        }
    }

    qDebug() << "Total mints " << mTotalMints;
}


