#include "Window/CoreWindow.h"
#include "Window/ViewerQT.h"
#include "Model/FRAlgorithm.h"
#include "Model/Graph.h"
#include "Model/Clusterer.h"
#include "GraphIO/IOManager.h"
#include "Viewer/SceneGraph.h"
#include "Viewer/PickHandler.h"
#include "Viewer/CameraManipulator.h"
#include "Util/Config.h"

#include <QTime>

#include "osgDB/WriteFile"

using namespace Window;
CoreWindow::CoreWindow(QWidget *parent) : QMainWindow(parent) {
	createStatusBar();
//	pm = new ProgressManager(this); // not used

	// initialize moduls
	layouter = new Model::FRAlgorithm();
	ioManager = new GraphIO::IOManager();
	clusterer = new Model::Clusterer();
	sceneGraph = new Vwr::SceneGraph();
	viewerWidget = new ViewerQT(sceneGraph, this);
	setCentralWidget(viewerWidget);
	connect(this, SIGNAL(windowResized()), viewerWidget->getPickHandler(), SLOT(windowResized()));
	currentFile = "";

	// create GUI
	createActions();
	createMenus();
	createToolBars();

	readSettings();
	updateRecentFileActions();

	qDebug("App initialized");
//	loadFile("input/data/line.graphml");
}

void CoreWindow::createActions() {
	QIcon loadIcon("img/icons/load.png");
	loadAction = new QAction(loadIcon, tr("&Load"), this);
	loadAction->setShortcut(tr("CTRL+L"));
	loadAction->setToolTip(tr("Load graph from file"));
	connect(loadAction, SIGNAL(triggered()), this, SLOT(openFile()));

	reloadAction = new QAction(tr("&Reload"), this);
	reloadAction->setToolTip(tr("Reload currently loaded graph"));
	connect(reloadAction, SIGNAL(triggered()), this, SLOT(reloadFile()));

	QIcon mapIcon("img/icons/map.png");
	remapAction = new QAction(mapIcon, tr("&Set mapping"), this);
	remapAction->setShortcut(tr("CTRL+M"));
	remapAction->setToolTip(tr("Set data mapping"));
	connect(remapAction, SIGNAL(triggered()), this, SLOT(setDataMapping()));
	remapAction->setEnabled(false);

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

	QIcon modeIcon("img/icons/debug.png");
	debugAction = new QAction(modeIcon, tr("&Debug"), this);
	debugAction->setShortcut(tr("CTRL+D"));
	debugAction->setCheckable(true);
	debugAction->setChecked(false);
	connect(debugAction, SIGNAL(triggered()), this, SLOT(toggleDebug()));

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

	QIcon clusterIcon("img/icons/autocluster.png");
	autoClusterAction = new QAction(clusterIcon, tr("Toggle auto-&clustering"), this);
	autoClusterAction->setToolTip(tr("Toggle automatic clustering"));
	autoClusterAction->setCheckable(true);
	autoClusterAction->setChecked(true);
	connect(autoClusterAction, SIGNAL(triggered(bool)), this, SLOT(toggleAutoCluster(bool)));

	cluster0Action = new QAction(tr("No clustering"), this);
	cluster0Action->setCheckable(true);
	cluster0Action->setData(QVariant(0));
	cluster1Action = new QAction(tr("Cluster by adjacency"), this);
	cluster1Action->setCheckable(true);
	cluster1Action->setData(QVariant(1));
	cluster2Action = new QAction(tr("Cluster leafs"), this);
	cluster2Action->setCheckable(true);
	cluster2Action->setData(QVariant(2));
	cluster3Action = new QAction(tr("Cluster neighbors (pseudorandom)"), this);
	cluster3Action->setCheckable(true);
	cluster3Action->setData(QVariant(3));

	QActionGroup* clusteringTypes = new QActionGroup(this);
	clusteringTypes->addAction(cluster0Action);
	clusteringTypes->addAction(cluster1Action);
	clusteringTypes->addAction(cluster2Action);
	clusteringTypes->addAction(cluster3Action);
	connect(clusteringTypes, SIGNAL(triggered(QAction*)), this, SLOT(setClusteringAlg(QAction*)));
	cluster1Action->trigger();

	// recent files
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
        		this, SLOT(openRecentFile()));
    }

	dialog = new QProgressDialog("", "", 0, 10, this, Qt::Dialog);
	dialog->setWindowTitle("Loading");
	dialog->setCancelButtonText("Abort");
	Qt::WindowFlags flags = dialog->windowFlags();
	flags = flags & (~Qt::WindowContextHelpButtonHint);
	dialog->setWindowFlags(flags);
	dialog->setModal(true);
	dialog->setMinimumDuration(400);
	dialog->setAutoReset(false);
}

void CoreWindow::createMenus() {
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(loadAction);
	fileMenu->addAction(reloadAction);
	fileMenu->addAction(remapAction);
	fileMenu->addAction(captureAction);

    separatorAction = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActions[i]);

	fileMenu->addSeparator();
	fileMenu->addAction(quitAction);

	clusteringMenu = menuBar()->addMenu(tr("&Clustering"));
	clusteringMenu->addAction(autoClusterAction);
	clusteringMenu->addSeparator();
	clusteringMenu->addAction(cluster0Action);
	clusteringMenu->addAction(cluster1Action);
	clusteringMenu->addAction(cluster2Action);
	clusteringMenu->addAction(cluster3Action);
}

void CoreWindow::createToolBars() {
	toolBar = new QToolBar("Tools", this);

	toolBar->addAction(playAction);
	toolBar->addAction(randomizeAction);
	toolBar->addAction(labelsAction);
	toolBar->addAction(centerAction);
	toolBar->addSeparator();
	toolBar->addAction(remapAction);
	toolBar->addAction(captureAction);
	toolBar->addSeparator();

	slider = new QSlider(Qt::Vertical, this);
	slider->setMinimum(1);
	slider->setMaximum(100);
	slider->setTickPosition(QSlider::TicksLeft);
	slider->setTickInterval(5);
	slider->setSingleStep(5);
	slider->setToolTip("Layouting speed");
	slider->setValue(Util::Config::getValue("Layout.Algorithm.Alpha").toFloat() * 1000);
	connect(slider, SIGNAL(valueChanged(int)), this,
			SLOT(setAlpha(int)));
	slider->setFixedHeight(180);
	toolBar->addWidget(slider);

//	toolBar->addSeparator();
//	toolBar->addAction(debugAction);

	toolBar->setMovable(false);
	addToolBar(Qt::LeftToolBarArea, toolBar);

	clusteringBar = new QToolBar("Clustering", this);
	clusteringBar->addAction(autoClusterAction);

	clusterSlider = new QSlider(Qt::Vertical, this);
	clusterSlider->setMinimum(0);
	clusterSlider->setMaximum(100);
	clusterSlider->setTickPosition(QSlider::TicksRight);
	clusterSlider->setTickInterval(5);
	clusterSlider->setSingleStep(5);
	clusterSlider->setToolTip("Auto-clustering threshold");
	clusterSlider->setValue(Util::Config::getValue("Viewer.Clustering.ClusterThreshold").toFloat() * 100);
	connect(clusterSlider, SIGNAL(valueChanged(int)), this,
			SLOT(setClusterThreshold(int)));
	clusterSlider->setFixedHeight(200);
	clusteringBar->addWidget(clusterSlider);

	clusteringBar->setMovable(false);
	addToolBar(Qt::RightToolBarArea, clusteringBar);
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

void CoreWindow::reloadFile() {
    loadFile(currentFile);
}

void CoreWindow::setDataMapping() {
	layouter->pause();
	sceneGraph->setUpdatingNodes(false);
	viewerWidget->setRendering(false);

	sceneGraph->setDataMapping();

	viewerWidget->setRendering(true);
	sceneGraph->setUpdatingNodes(true);
	if (labelsAction->isChecked())
		sceneGraph->setNodeLabelsVisible(true);
	if (playAction->isChecked())
		layouter->play();
}

void CoreWindow::loadFile(QString filePath) {
	if (filePath.isEmpty())
		return;

	QTime t;

	QFile file(filePath);
	updateRecentFileActions(filePath);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(NULL, "Error",
				QString("Cannot read file %1:\n%2.")
				.arg(filePath)
				.arg(file.errorString()));
		return;
	}

	layouter->pause();
	sceneGraph->setUpdatingNodes(false);
	viewerWidget->setRendering(false);

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	t.start();
	Model::Graph* graph = ioManager->loadGraph(&file, dialog);
	file.close();
	qDebug() << "Parsing: " << t.elapsed();

	if (graph == NULL) {
		if (!dialog->wasCanceled())
			showMessageBox("Error", "Could not load graph from file" + filePath, true);
	} else {
		int origNodes = graph->getNodes()->size();
		qDebug() << "GraphML parsed successfully.";
		// cluster
		t.restart();
		clusterer->cluster(graph, dialog);
		qDebug() << "Clustering: " << t.elapsed();

		// reload
		viewerWidget->getPickHandler()->reset();
		t.restart();
		sceneGraph->reload(graph, origNodes, dialog);	// deletes original scene graph
		qDebug() << "Drawing: " << t.elapsed();
		layouter->setGraph(graph); 	// deletes original graph

		if (!dialog->wasCanceled()) {
			setWindowFilePath(filePath);
			remapAction->setEnabled(true);
			currentFile = filePath;
		} else {
			setWindowFilePath("");
			remapAction->setEnabled(false);
			currentFile = "";
			// NOTE: graph was still loaded into layouter
		}
		dialog->reset();

		//reset camera
		viewerWidget->getCameraManipulator()->home(0);
		viewerWidget->getCameraManipulator()->setDistance(500);

		qDebug() << "Orig nodes: " << origNodes;
		log(NORMAL, "Graph loaded: " + graph->toString());
	}
	QApplication::restoreOverrideCursor();

	// start
	viewerWidget->setRendering(true);
	sceneGraph->setUpdatingNodes(true);
	if (labelsAction->isChecked())
		sceneGraph->setNodeLabelsVisible(true);
	if (playAction->isChecked())
		layouter->play();
	if (autoClusterAction->isChecked())
		setClusterThreshold(clusterSlider->value());
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

void CoreWindow::toggleDebug() {
	viewerWidget->getPickHandler()->changeMode();
}

void CoreWindow::toggleLabels(bool checked) {
	sceneGraph->setNodeLabelsVisible(checked);
	sceneGraph->setEdgeLabelsVisible(checked);
}

void CoreWindow::setAlpha(int value) {
	layouter->setAlphaValue(value / 1000.0f); // don't allow 0
	layouter->wakeUp();
}

void CoreWindow::setClusterThreshold(int value) {
	viewerWidget->setRendering(false);
	sceneGraph->setClusterThreshold(value / 100.0f);
	layouter->wakeUp();
	viewerWidget->setRendering(true);
}

void CoreWindow::toggleAutoCluster(bool checked) {
	clusterSlider->setEnabled(checked);
	if (checked) {
		sceneGraph->setClusterThreshold((clusterSlider->value()) / 100.0f);
	} else {
		sceneGraph->setClusterThreshold(-1);
	}
}

void CoreWindow::setClusteringAlg(QAction* action) {
	bool changed = clusterer->setClusteringAlg(action->data().toInt());
	if (changed && !currentFile.isEmpty()) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, tr("Clustering type changed"),
				"<p>Change of clustering algorithm will take effect<br>"
				"only after reloading graph.</p><p>Reload now?</p>",
				QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes) {
			reloadFile();
		}
	}
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

	viewerWidget->close();
	viewerWidget->deleteLater();
	delete ioManager;
//	delete layouter;
	event->accept();
}

void CoreWindow::resizeEvent (QResizeEvent * event) {
	emit windowResized();
}

Window::CoreWindow* CoreWindow::instanceForStatusLog = NULL;

void CoreWindow::log(StatusMsgType type, QString msg) {
	if (instanceForStatusLog == NULL) {
		qWarning() << "Statusbar not created yet!";
		return;
	}
	instanceForStatusLog->showStatusMsg(type, msg);
}

void CoreWindow::showMessageBox(QString title, QString message, bool isError) {
	QMessageBox msgBox;
	msgBox.setText(message);
	msgBox.setWindowTitle(title);
	if (isError) {
		msgBox.setIcon(QMessageBox::Warning);
	} else {
		msgBox.setIcon(QMessageBox::Information);
	}
	msgBox.exec();
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

void CoreWindow::readSettings() {
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    if (settings.value("maximized", false).toBool())
        showMaximized();
}

void CoreWindow::writeSettings() {
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    settings.setValue("maximized", isMaximized());
}
