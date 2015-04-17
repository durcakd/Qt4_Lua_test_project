#include "scene/sceneeventobserver.h"

#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>

#include "scene/command/command.h"
#include "scene/command/cursormovecommand.h"
#include "scene/command/selectitemcommand.h"
#include "scene/command/changemarkedcommand.h"

SceneEventObserver::SceneEventObserver(QGraphicsItem *parent)
    : QGraphicsItem(parent) {

}


bool SceneEventObserver::sceneEventFilter ( QGraphicsItem * watched, QEvent *event ) {

    if (event->type() == QEvent::GraphicsSceneWheel) {
        qDebug() << "SEFO  MOUSE wheel EVENT";
        QGraphicsSceneWheelEvent * wheelEvent = static_cast<QGraphicsSceneWheelEvent *>(event);
        return addCommand( new ChangeMarkedCommand(watched, wheelEvent));
    }

    else if (event->type() == QEvent::GraphicsSceneMousePress) {
        qDebug() << "SEFO  MOUSE press EVENT";
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        return addCommand( new SelectItemCommand(watched, mouseEvent));
    }

    else if (event->type() == QEvent::GraphicsSceneMouseRelease) {
        //QGraphicsSceneMouseEvent * mp = static_cast<QGraphicsSceneMouseEvent *>(event);
        qDebug() << "SEFO  MOUSE release EVENT";
        return true;
    }

    else if (event->type() == QEvent::GraphicsSceneMouseMove) {
        //QGraphicsSceneMouseEvent * mp = static_cast<QGraphicsSceneMouseEvent *>(event);
        qDebug() << "SEFO  MOUSE move EVENT";
        return true;
    }

    else if (event->type() == QEvent::GraphicsSceneDragEnter) {
        //QGraphicsSceneMouseEvent * mp = static_cast<QGraphicsSceneMouseEvent *>(event);
        qDebug() << "SEFO  MOUSE drag enter EVENT";
        return true;
    }
    else if (event->type() == QEvent::GraphicsSceneDragLeave) {
        //QGraphicsSceneMouseEvent * mp = static_cast<QGraphicsSceneMouseEvent *>(event);
        qDebug() << "SEFO  MOUSE drag leave EVENT";
        return true;
    }
    else if (event->type() == QEvent::GraphicsSceneDrop) {
        qDebug() << "SEFO  MOUSE drop EVENT";
        return true;
    }

    else if (event->type() == QEvent::KeyPress) {
        qDebug() << "SEFO KEY press EVENT";
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        return keyCommand( watched, keyEvent);
    }
    return false;
}

bool SceneEventObserver::addCommand(Command *command) {
    if (NULL == command) {
        return false;
    }
    command->execute();
    return true;
}


// Key Command
bool SceneEventObserver::keyCommand(QGraphicsItem *watched, QKeyEvent *event) {
    Command *command = NULL;

    //    //if (!+Qt::ControlModifier & event->modifiers()) {
    //    AbstractElement *targed;
    //    QTextCursor cursor = textCursor();

    if ( event->key() == Qt::Key_Left  ||
         event->key() == Qt::Key_Right ||
         event->key() == Qt::Key_Up    ||
         event->key() == Qt::Key_Down ) {
        command = new CursorMoveCommand(watched, event);
    }

    return addCommand( command);
}
