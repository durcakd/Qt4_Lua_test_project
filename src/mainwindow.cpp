#include "mainwindow.h"

#include <QTextEdit>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>

#include <QFileInfo>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QWidget>
#include <QMessageBox>
#include <QDebug>

#include "lineeditor.h"
#include "blockscene.h"
#include "highlighter.h"
#include "parser.h"


MainWindow::MainWindow()
{
    createActions();
    createMenus();
    createToolbars();

    lineEditor = new LineEditor(this);
    highlighter = new Highlighter(lineEditor->document());


    view = new QGraphicsView();
    scene = new BlockScene(view);
    view->setScene(scene);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(lineEditor);
    layout->addWidget(view);
    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
    setWindowTitle(tr("BlockEditor"));
    setUnifiedTitleAndToolBarOnMac(true);

    _textType = "lua";
    parser = new Parser(_textType);

    QObject::connect( parser, &Parser::addElementItem,
                     scene, &BlockScene::addParserItem );
    QObject::connect( parser, &Parser::addElementLayout,
                     scene, &BlockScene::addParserLayout );

}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New"), this, SLOT(newFile()), QKeySequence::New);
    fileMenu->addAction(tr("&Open..."), this, SLOT(openFile()), QKeySequence::Open);
    //fileMenu->addAction(tr("E&xit"), qApp, SLOT(quit()), QKeySequence::Quit);

    itemMenu = menuBar()->addMenu(tr("&Item"));
    //itemMenu->addAction(deleteAction);
    itemMenu->addSeparator();

    aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(tr("&About"), this, SLOT(about()));
    //aboutMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
}

void MainWindow::createActions() { }

void MainWindow::createToolbars() { }


void MainWindow::about()
{
    QMessageBox::about(this, tr("About Block Editor"),
                       tr("<p>The <b>Block Editor</b> is prototype of hybrid text and visual editor for Lua script language, that use LPEG for syntax analysm of edited code.</p>"));
}

void MainWindow::newFile()
{
    lineEditor->clear();
}

void MainWindow::openFile(const QString &path)
{
    QString fileName = path;

    if (fileName.isNull())
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "Lua scripts (*.lua)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text)) {

            //QFileInfo fileInfo(file);

            //_textType = fileInfo.suffix();

            QTextStream in(&file);
            _text = in.readAll();
            lineEditor->setPlainText(_text);
            file.close();
            parser->parse(_text);
        }
    }
}
