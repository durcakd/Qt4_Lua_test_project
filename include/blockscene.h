#ifndef BLOCKSCENE_H
#define BLOCKSCENE_H

#include <QGraphicsScene>

#include "scene/item.h"
#include "scene/layout.h"

class QGraphicsLinearLayout;
class QGraphicsWidget;

class BlockScene : public QGraphicsScene
{
    Q_OBJECT

public:
    static BlockScene *instance( QObject *parent = 0);
    void addItem(QGraphicsItem *item);
    void test();

public slots:


    Item* addParserItem( Item *item);
    Layout* addParserLayout( Layout *layout);
    void updateTreeNeighbors();

private:
    static BlockScene *inst;

    BlockScene( QObject *parent = 0);

    QGraphicsLinearLayout *_vLayout;
    QGraphicsWidget *_form;
    Layout *_root;

};

#endif // MAINWINDOW_H
