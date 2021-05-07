#ifndef GAMEELEMENT_H
#define GAMEELEMENT_H

#include <QPainter>

class gameActiveElement{

public:
  virtual void makeNextMove(const int &key)=0;
  virtual void drawInBoard(QPainter &painter, int &x, int &y)=0;
  virtual int getScore(int &mintsAndPills)=0;
  virtual void changeChasingStatus()=0;
  virtual int getMessage()=0;
  virtual void sendMessage(int gameStatus, int &x, int &Y)=0;
};

#endif // GAMEELEMENT_H
