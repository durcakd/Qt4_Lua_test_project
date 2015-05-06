#include "item/layout.h"
#include <QDebug>

#include "style/style.h"
#include "scene/blockscene.h"
#include "item/state/elementstate.h"


Layout::Layout(QGraphicsLayoutItem *parent)
    : QObject(),
      QGraphicsLinearLayout( parent),
      QGraphicsItem(dynamic_cast<QGraphicsItem*>(parent)),
      AbstractElement(dynamic_cast<QGraphicsLinearLayout*>(parent))
{
    //qDebug() << "---------   "<< type << " " << style;
    setOrientation( Qt::Horizontal);

    //_childLayouts = new  QList<Layout*>();
    setInstantInvalidatePropagation(true);
    setVisible(true);
    setSpacing(0);
    //setOpacity(0.5);
}

// ----------------------------------------------------------
// -- reimplement from QGraphicsLinearLayout ----------------
// ----------------------------------------------------------

QSizeF Layout::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
    return  QGraphicsLinearLayout::sizeHint(which);
}

void Layout::setGeometry(const QRectF &geom) {
    prepareGeometryChange();
    QGraphicsLinearLayout::setGeometry( geom);
    setPos(0,0);
}


// ----------------------------------------------------------
// -- reimplement from QGraphicsItem ------------------------
// ----------------------------------------------------------

QRectF Layout::boundingRect() const {
    return QRectF(geometry().adjusted(0,0,5,22));
}

void Layout::paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/) {
    Q_UNUSED(widget);
    Q_UNUSED(option);


    if (state()->hasInvalidText()) {
        QRectF frame(geometry());
        frame.translate(3,20);
        frame.adjust(-5,-5,3,3);
        QBrush brush(Qt::red);
        painter->setBrush(brush);
        painter->drawRoundedRect(frame, 3.0, 3.0);
    }

    if (isPaintEnabled()) {
        //qDebug() << "paint " << toString();


        QRectF frame(geometry());
        frame.translate(3,20);
        frame.adjust(-5,-5,3,3);

        painter->drawRoundedRect(frame, 3.0, 3.0);
    }
}


// ----------------------------------------------------------
// -- reimplement from Abstract Element ---------------------
// ----------------------------------------------------------

QSizeF Layout::elementSizeHint(Qt::SizeHint which) const {
    return sizeHint(which);
}

bool Layout::isLayoutE() const {
    return true;
}

int Layout::textLength(bool length) const {
    if(length) {
        int sum = 0;
        for ( int i=0; i < count(); i++) {
            if (AbstractElement *element = dynamic_cast <AbstractElement*>(itemAt(i))) {
                sum += element->textLength(length);
            }
        }
        return sum;
    } else {
        return 0;
    }
}

QString Layout::textE() const {
    QString text;
    for ( int i=0; i < count(); i++) {
        if (AbstractElement *element = dynamic_cast <AbstractElement*>(itemAt(i))) {
            text += element->textE();
            if (orientation() == Qt::Vertical) {
                text += "\n";
            }
        }
    }
    return text;
}

QPixmap Layout::toPixmap() {
    BlockScene *scene = BlockScene::instance();
    QRectF r = boundingRect();
    r.adjust(0,0,5,0);

    QPixmap pixmap(r.width(), r.height());
    pixmap.fill(QColor(0, 0, 0, 10));
    QPainter painter(&pixmap);

    scene->render(&painter, QRectF(), sceneBoundingRect().translated(-3,5));
    painter.end();

    return pixmap.copy(0,10,r.width(),r.height()-13);
}

void Layout::setStyleE(Style *style) {
    AbstractElement::setStyleE(style);

    if (OrientationEnum::horizontal == _style->getOrientation()) {
        setOrientation( Qt::Horizontal);
    } else {
        setOrientation( Qt::Vertical);
    }
}

void Layout::setPaintEnable( bool enablePaint ) {
    _enablePaint = enablePaint;
    update();
}

// ----------------------------------------------------------
// -- class methods -----------------------------------------
// ----------------------------------------------------------

void Layout::childItemChanged() {
    this->updateGeometry();
}

void Layout::updateChildNeighbors() {
    //qDebug() << "  chids: " << count();
    AbstractElement *previous = NULL;
    for (int i=0; i < count(); i++) {
        if (AbstractElement *myItem = dynamic_cast <AbstractElement*>(itemAt(i))) {
            //if(NULL == previous) { qDebug() << "NULL           " << myItem;}
            //else { qDebug() << previous << "    " << myItem;}

            if( NULL != previous ) {
                previous->setNext(myItem);
            }
            myItem->setPrevius(previous);
            previous = myItem;

            // recursively update children
            if (Layout *layout = dynamic_cast <Layout*>(itemAt(i))) {
                layout->updateChildNeighbors();
            }
        }
    }

//    //qDebug() << " Neighbors controll:";

//    //qDebug() << "  chids: " << count();
//    for (int i=0; i < count(); i++) {
//        if (AbstractElement *myItem = dynamic_cast <AbstractElement*>(itemAt(i))) {
//            if (myItem->getPrevius() != NULL) {
//                qDebug() << "previous " << myItem->getPrevius() << "   " << myItem->getPrevius()->getType() << "  " << myItem->getPrevius()->isLayoutE();}
//            if (myItem->getNext() != NULL) {
//                qDebug() << "next     " << myItem->getNext() << "   " << myItem->getNext()->getType() << "  " << myItem->getNext()->isLayoutE();}

//        }
//    }
//    //qDebug() << "THIS  " << this << "   " << this->getType();
//    if( this->getLayoutParrent() != NULL){
//        //qDebug() << "THIS  " << this->getLayoutParrent();
//    }
//    qDebug() << "";
}

int Layout::indexOf(AbstractElement *element) {
    for ( int i=0; i < count(); i++) {
        if (dynamic_cast <AbstractElement*>(itemAt(i)) == element) {
            return i;
        }
    }
    return -1;
}

void Layout::insertBehind( AbstractElement *oldElement, AbstractElement *newElement) {
    int index = indexOf(oldElement);
    insertItem( index+1, dynamic_cast<QGraphicsLayoutItem*>(newElement));

    newElement->setLayoutParrent(this);
    newElement->setNext(oldElement->getNext());
    newElement->setPrevius(oldElement);

    if( oldElement->getNext()) {
        oldElement->getNext()->setPrevius(newElement);
    }
    oldElement->setNext(newElement);
}

void Layout::insertBefore( AbstractElement *oldElement, AbstractElement *newElement) {
    int index = indexOf(oldElement);
    insertItem( index, dynamic_cast<QGraphicsLayoutItem*>(newElement));

    newElement->setLayoutParrent(this);
    newElement->setPrevius(oldElement->getPrevius());
    newElement->setNext(oldElement);

    if( oldElement->getPrevius()) {
        oldElement->getPrevius()->setNext(newElement);
    }
    oldElement->setPrevius(newElement);
}

void Layout::removeElement( AbstractElement *element) {

    if( element->getPrevius()) {
        element->getPrevius()->setNext( element->getNext());
    }
    if( element->getNext()) {
        element->getNext()->setPrevius( element->getPrevius());
    }

    element->setNext(NULL);
    element->setPrevius(NULL);
    element->setLayoutParrent(NULL);
    removeItem( dynamic_cast<QGraphicsLayoutItem*>(element));

    if (count() == 0) {
        qDebug() << "WARNING!!!  RemoveElement only child";
    }
}

QGraphicsLayoutItem  *Layout::firstLastChildrenElement(bool first) const {
    if (first) {
        return itemAt(0);
    } else {
        return itemAt(count()-1);
    }
}
