
/*!
 * Core.cpp
 * Projekt 3DVisual
 */

#include "Core/Core.h"

AppCore::Core * AppCore::Core::core;

AppCore::Core::Core(QApplication * app)
{
    core = this;

    messageWindows = new QOSG::MessageWindows();
    this->alg = new Layout::FRAlgorithm();
    this->thr = new Layout::LayoutThread(this->alg);
    this->cg = new Vwr::CoreGraph();
    this->cw = new QOSG::CoreWindow(0, this->cg, app, this->thr);
    this->cw->resize(1024, 768);
    this->cw->show();

    app->exec();
}

AppCore::Core::~Core()
{
}

void AppCore::Core::restartLayout()
{
    // neviem preco sa tuto nejak dlho zastavi
    if(this->thr->isRunning())
    {
        this->thr->terminate();
        this->thr->wait();
    }
    delete this->thr;

    this->alg->SetGraph(Manager::GraphManager::getInstance()->getActiveGraph());
    this->alg->SetParameters(10,0.7,1,true);
    this->thr = new Layout::LayoutThread(this->alg);
    this->cw->setLayoutThread(thr);
    this->cg->reload(Manager::GraphManager::getInstance()->getActiveGraph());
    this->thr->start();
    this->thr->play();
    this->messageWindows->closeLoadingDialog();
}

AppCore::Core * AppCore::Core::getInstance(QApplication * app)
{
    if(core == NULL)
    {
        if (app != NULL)
            core = new AppCore::Core(app);
        else
        {
            qDebug() << "Internal error.";
            return NULL;
        }
    }

    return core;
}
