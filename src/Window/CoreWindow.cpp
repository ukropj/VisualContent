#include "Window/CoreWindow.h"
#include "Window/OptionsWindow.h"
#include "Window/CheckBoxList.h"
#include "Window/ViewerQT.h"
#include "Window/MessageWindows.h"
#include "Model/FRAlgorithm.h"
#include "Model/Graph.h"
#include "Core/IOManager.h"
#include "Viewer/SceneGraph.h"
#include "Viewer/OsgNode.h"
#include "osgDB/WriteFile"

using namespace Window;
CoreWindow::CoreWindow(QWidget *parent) : QMainWindow(parent) {
	createStatusBar();

	// initialize moduls
	layouter = new Model::FRAlgorithm();
	IOManager = new AppCore::IOManager();
	messageWindows = new Window::MessageWindows();
	sceneGraph = new Vwr::SceneGraph();
	viewerWidget = new ViewerQT(sceneGraph, this);
	setCentralWidget(viewerWidget);

	// create GUI
	createActions();
	createMenus();
	createToolBars();

	readSettings();
	updateRecentFileActions();

	qDebug("App initialized");
	loadFile("input/data/grid7.graphml");
}

void CoreWindow::createActions() {
	QIcon loadIcon("img/icons/load.png");

	loadAction = new QAction(loadIcon, tr("&Load"), this);
	loadAction->setShortcut(tr("CTRL+L"));
	loadAction->setToolTip(tr("Load graph from file"));
	connect(loadAction, SIGNAL(triggered()), this, SLOT(openFile()));

	optionsAction = new QAction("&Options", this);
	connect(optionsAction, SIGNAL(triggered()), this, SLOT(showOptions()));
	optionsAction->setEnabled(false);

	QIcon quitIcon("img/icons/exit.png");
	quitAction = new QAction(quitIcon, tr("&Quit"), this);
	quitAction->setToolTip(tr("Exit application"));
	connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

	QIcon playIcon;
	playIcon.addPixmap(QPixmap("img/icons/run.png"), QIcon::Normal, QIcon::Off);
	playIcon.addPixmap(QPixmap("img/icons/pause.png"), QIcon::Normal, QIcon::On);
	playAction = new QAction(playIcon, tr("&Play/pause"), this);
	playAction->setToolTip(tr("Pause or resume layouting"));
	playAction->setCheckable(true);
	playAction->setChecked(true);
	connect(playAction, SIGNAL(triggered(bool)), this, SLOT(toggleLayouting(bool)));

	QIcon fixIcon("img/icons/fix.png");
	fixAction = new QAction(fixIcon, tr("&Fix/Unfix"), this);
	fixAction->setShortcut(tr("CTRL+F"));
	fixAction->setToolTip(tr("Fix or unfix selected nodes"));
	connect(fixAction, SIGNAL(triggered()), this, SLOT(toggleFixNodes()));

	QIcon labelsIcon("img/icons/labels.png");
	labelsAction = new QAction(labelsIcon, tr("&Toggle labels"), this);
	labelsAction->setToolTip(tr("Toggle node labels"));
	labelsAction->setShortcut(tr("CTRL+T"));
	labelsAction->setCheckable(true);
	labelsAction->setChecked(false);
	connect(labelsAction, SIGNAL(triggered(bool)), this, SLOT(toggleLabels(bool)));

	QIcon randIcon("img/icons/randomize.png");
	randomizeAction = new QAction(randIcon, tr("&Radomize"), this);
	randomizeAction->setShortcut(tr("CTRL+R"));
	randomizeAction->setToolTip(tr("Randomize graph layout"));
	connect(randomizeAction, SIGNAL(triggered()), this, SLOT(randomize()));

	QIcon captureIcon("img/icons/screenshot.png");
	captureAction = new QAction(captureIcon, tr("&Capture screen"), this);
	captureAction->setShortcut(tr("CTRL+E"));
	captureAction->setToolTip(tr("Create screenshot"));
	connect(captureAction, SIGNAL(triggered()), this, SLOT(captureScreen()));

	QIcon centerIcon("img/icons/center.png");
	centerAction = new QAction(centerIcon, tr("&Center view"), this);
	centerAction->setToolTip(tr("Center view to current selection"));
	connect(centerAction, SIGNAL(triggered()), this, SLOT(centerView()));

    // recent files
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }
}

void CoreWindow::createMenus() {
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(loadAction);

    separatorAction = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActions[i]);

	fileMenu->addSeparator();
	fileMenu->addAction(quitAction);

	editMenu = menuBar()->addMenu(tr("Edit"));
	editMenu->addAction(optionsAction);
}

void CoreWindow::createToolBars() {
	toolBar = new QToolBar("Tools", this);

	toolBar->addAction(playAction);
	toolBar->addAction(randomizeAction);
	toolBar->addAction(fixAction);
	toolBar->addAction(labelsAction);
	toolBar->addAction(centerAction);
	toolBar->addSeparator();
	toolBar->addAction(captureAction);
	toolBar->addSeparator();

//	QLabel* sliderLabel = new QLabel();
//	sliderLabel->setText(QString("%1").arg(QChar(0x3b1)));
//	toolBar->addWidget(sliderLabel);
	slider = new QSlider(Qt::Vertical, this);
	slider->setTickPosition(QSlider::TicksLeft);
	slider->setTickInterval(5);
	slider->setToolTip("Layouting speed");
	slider->setValue(Util::Config::getValue("Layout.Algorithm.Alpha").toFloat() * 1000);
	connect(slider, SIGNAL(valueChanged(int)), this,
			SLOT(sliderValueChanged(int)));
	slider->setFixedHeight(150);
	toolBar->addWidget(slider);

	toolBar->setMovable(false);
	addToolBar(Qt::LeftToolBarArea, toolBar);
}

void CoreWindow::openFile() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open GraphML"),
			".", tr("GraphML Files (*.graphml)"));
	loadFile(fileName);
}

void CoreWindow::openRecentFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
    	loadFile(action->data().toString());
    }
}

void CoreWindow::loadFile(QString fileName) {
	updateRecentFileActions(fileName);

	layouter->pause();
	sceneGraph->setUpdatingNodes(false);

	messageWindows->showProgressBar();
	Model::Graph* graph = IOManager->loadGraph(fileName, messageWindows);

	if (graph == NULL) {
		messageWindows->showMessageBox("Error",
				"Could not load graph from file" + fileName, true);
	} else {
		setWindowFilePath(fileName);

		// reload
		sceneGraph->reload(graph); // deletes original scene graph
		layouter->setGraph(graph); // deletes original graph

		//reset camera
		viewerWidget->getCameraManipulator()->home(0);
		viewerWidget->getCameraManipulator()->setDistance(500);

		log(NORMAL, "Graph loaded: " + graph->toString());
	}
	messageWindows->closeProgressBar();

	// start
	sceneGraph->setUpdatingNodes(true);
	layouter->play();
}

void CoreWindow::showOptions() {
//	OptionsWindow *options = new OptionsWindow(sceneGraph, viewerWidget);
//	options->show();
}

void CoreWindow::toggleLayouting(bool checked) {
	if (!checked) {
		layouter->pause();
		sceneGraph->setUpdatingNodes(false);
	} else {
		sceneGraph->setUpdatingNodes(true);
		layouter->play();
	}
}

void CoreWindow::randomize() {
	layouter->randomize();
	sceneGraph->update(true);
	log(NORMAL, "Layout randomized");
}

void CoreWindow::centerView() {
	osg::Vec3f center;
	center = viewerWidget->getPickHandler()->getSelectionCenter();
	viewerWidget->getCameraManipulator()->setCenter(center);
}

void CoreWindow::toggleFixNodes() {
	QLinkedList<Vwr::OsgNode* > nodes = viewerWidget->getPickHandler()->getSelectedNodes();
	QLinkedList<Vwr::OsgNode* >::const_iterator i = nodes.constBegin();

	while (i != nodes.constEnd()) {
		(*i)->setFixed(!(*i)->isFixed());
		++i;
	}

	layouter->wakeUp();
}

void CoreWindow::toggleLabels(bool checked) {
	sceneGraph->setNodeLabelsVisible(checked);
}

void CoreWindow::sliderValueChanged(int value) {
	layouter->setAlphaValue((float) value * 0.001);
	layouter->wakeUp();
}

void CoreWindow::captureScreen() {
	 QString fileName = QFileDialog::getSaveFileName(this, "Save image", QString(), "*.png");
	 if (fileName.isEmpty())
	        return;
	 if (!fileName.endsWith(".png", Qt::CaseInsensitive))
		 fileName += ".png";

	 // Qt API doesn't work here:
	 //	 QPixmap snapshot = QPixmap::grabWindow(viewerWidget->winId());
	 //	 snapshot.save(fileName, "PNG");

	 int x = viewerWidget->getCamera()->getViewport()->x();
	 int y = viewerWidget->getCamera()->getViewport()->y();
	 int w = viewerWidget->getCamera()->getViewport()->width();
	 int h = viewerWidget->getCamera()->getViewport()->height();

	 osg::ref_ptr<osg::Image> image = new osg::Image;
	 image->readPixels(x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE);
	 osgDB::writeImageFile(*image, fileName.toStdString());
}

void CoreWindow::closeEvent(QCloseEvent *event) {
	qDebug("About to quit\n");
	layouter->stop();
	layouter->wait();
	writeSettings();
	event->accept();
}

Window::CoreWindow* CoreWindow::instanceForStatusLog = NULL;

void CoreWindow::log(StatusMsgType type, QString msg) {
	if (instanceForStatusLog == NULL) {
		qWarning() << "Statusbar not created yet!";
		return;
	}
	instanceForStatusLog->showStatusMsg(type, msg);
}

void CoreWindow::showStatusMsg(StatusMsgType type, QString msg) {
	switch (type) {
	case ALG:
		algStatus->setText(msg);
		break;
	case PICK:
		//		pickStatus->setText(msg);
		break;
	case KEYS:
		//		keyStatus->setText(msg);
		break;
	case TEMP:
		statusBar()->showMessage(msg, 3000); // 3 sec
		break;
	case NORMAL:
	default:
		statusBar()->showMessage(msg);
		break;
	}
}

void CoreWindow::createStatusBar() {
	statusBar();
	algStatus = new QLabel(" --- ");
	pickStatus = new QLabel(" --- ");
	keyStatus = new QLabel("");
	//	statusBar()->addPermanentWidget(pickStatus);
	//	statusBar()->addPermanentWidget(keyStatus);
	statusBar()->addPermanentWidget(algStatus);
	instanceForStatusLog = this;
}

void CoreWindow::updateRecentFileActions(QString fileName) {
	QSettings settings(QApplication::organizationName(), QApplication::applicationName());
	QStringList files = settings.value("recentFileList").toStringList();

	if (fileName != NULL) {
		fileName = QFileInfo(fileName).canonicalFilePath();
		files.removeAll(fileName);
		if (!fileName.isEmpty() && QFile::exists(fileName)) {
			files.prepend(fileName);
			while (files.size() > MaxRecentFiles)
				files.removeLast();
			settings.setValue("recentFileList", files);
		} else {
			return;
		}
	}

    foreach (QString fileName, files) {
        if (fileName.isEmpty() || !QFile::exists(fileName))
            files.removeOne(fileName);
    }

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        recentFileActions[i]->setText(text);
        recentFileActions[i]->setData(files[i]);
        recentFileActions[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActions[j]->setVisible(false);

    separatorAction->setVisible(numRecentFiles > 0);
}

void CoreWindow::readSettings()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    if (settings.value("maximized", false).toBool())
        showMaximized();
}

void CoreWindow::writeSettings()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    settings.setValue("maximized", isMaximized());
}

