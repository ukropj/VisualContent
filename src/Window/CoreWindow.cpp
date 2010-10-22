#include "Window/CoreWindow.h"

using namespace Window;

CoreWindow::CoreWindow(QWidget *parent) :
	QMainWindow(parent) {
	//inicializacia premennych
	layout = new Model::FRAlgorithm();
	manager = new AppCore::IOManager();
	messageWindows = new Window::MessageWindows();
	coreGraph = new Vwr::CoreGraph();

	//vytvorenie menu a toolbar-ov
	createActions();
	createMenus();
	createToolBar();
	statusBar();

	viewerWidget = new ViewerQT(this, 0, 0, 0, coreGraph);
	viewerWidget->setSceneData(coreGraph->getScene());
	setCentralWidget(viewerWidget);

	// connect statusbar slot
	connect(viewerWidget->getPickHandler(), SIGNAL(pickMsg(QString)),
			statusBar(), SLOT(showMessage(QString)));

	nodeLabelsVisible = edgeLabelsVisible = false;
}

void CoreWindow::createActions() {
	quitAction = new QAction("Quit", this);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

	optionsAction = new QAction("Options", this);
	connect(optionsAction, SIGNAL(triggered()), this, SLOT(showOptions()));

	loadAction = new QAction(QIcon("img/gui/open.png"), "&Load", this);
	loadAction->setShortcut(tr("CTRL+L"));
	connect(loadAction, SIGNAL(triggered()), this, SLOT(loadFile()));

	playAction = new QAction(this);
	playAction->setIcon(QIcon("img/gui/pause.png"));
	playAction->setToolTip("&Play");
	connect(playAction, SIGNAL(triggered()), this, SLOT(playPause()));

	addMetaB = new QPushButton();
	addMetaB->setIcon(QIcon("img/gui/meta.png"));
	addMetaB->setToolTip("&Add meta node");
	addMetaB->setFocusPolicy(Qt::NoFocus);
	addMetaB->setEnabled(false);// FIXME remove
	//	connect(addMeta, SIGNAL(clicked()), this, SLOT(addMetaNode()));

	removeMetaB = new QPushButton();
	removeMetaB->setIcon(QIcon("img/gui/removemeta.png"));
	removeMetaB->setToolTip("&Remove meta nodes");
	removeMetaB->setFocusPolicy(Qt::NoFocus);
	removeMetaB->setEnabled(false);//FIXME remove
	//	connect(removeMeta, SIGNAL(clicked()), this, SLOT(removeMetaNodes()));

	fixB = new QPushButton();
	fixB->setIcon(QIcon("img/gui/fix.png"));
	fixB->setToolTip("&Fix nodes");
	fixB->setFocusPolicy(Qt::NoFocus);
	connect(fixB, SIGNAL(clicked()), this, SLOT(fixNodes()));

	unFixB = new QPushButton();
	unFixB->setIcon(QIcon("img/gui/unfix.png"));
	unFixB->setToolTip("&Unfix nodes");
	unFixB->setFocusPolicy(Qt::NoFocus);
	connect(unFixB, SIGNAL(clicked()), this, SLOT(unFixNodes()));

	labelsB = new QPushButton();
	labelsB->setIcon(QIcon("img/gui/label.png"));
	labelsB->setToolTip("&Turn on/off labels");
	labelsB->setCheckable(true);
	labelsB->setFocusPolicy(Qt::NoFocus);
	connect(labelsB, SIGNAL(clicked(bool)), this, SLOT(labelOnOff(bool)));

	//mody - ziadny vyber, vyber jedneho, multi vyber centrovanie
	noSelectB = new QPushButton();
	noSelectB->setIcon(QIcon("img/gui/noselect.png"));
	noSelectB->setToolTip("&No-select mode");
	noSelectB->setCheckable(true);
	noSelectB->setFocusPolicy(Qt::NoFocus);
	connect(noSelectB, SIGNAL(clicked(bool)), this, SLOT(noSelectClicked(bool)));

	singleSelectB = new QPushButton();
	singleSelectB->setIcon(QIcon("img/gui/singleselect.png"));
	singleSelectB->setToolTip("&Single-select mode");
	singleSelectB->setCheckable(true);
	singleSelectB->setFocusPolicy(Qt::NoFocus);
	connect(singleSelectB, SIGNAL(clicked(bool)), this,
			SLOT(singleSelectClicked(bool)));

	multiSelectB = new QPushButton();
	multiSelectB->setIcon(QIcon("img/gui/multiselect.png"));
	multiSelectB->setToolTip("&Multi-select mode");
	multiSelectB->setCheckable(true);
	multiSelectB->setFocusPolicy(Qt::NoFocus);
	connect(multiSelectB, SIGNAL(clicked(bool)), this,
			SLOT(multiSelectClicked(bool)));

	centerB = new QPushButton();
	centerB->setIcon(QIcon("img/gui/center.png"));
	centerB->setToolTip("&Center view");
	centerB->setFocusPolicy(Qt::NoFocus);
	connect(centerB, SIGNAL(clicked(bool)), this, SLOT(centerView(bool)));
}

void CoreWindow::createMenus() {
	fileMenu = menuBar()->addMenu("File");
	fileMenu->addAction(loadAction);
	fileMenu->addSeparator();
	fileMenu->addAction(quitAction);

	editMenu = menuBar()->addMenu("Edit");
	editMenu->addAction(optionsAction);
}

void CoreWindow::createToolBar() {
	//inicializacia comboboxu typov vyberu
	nodeTypeCB = new QComboBox();
	nodeTypeCB->insertItems(0, (QStringList() << "All" << "Node" << "Edge"));
	nodeTypeCB->setFocusPolicy(Qt::NoFocus);
	connect(nodeTypeCB, SIGNAL(currentIndexChanged(int)), this,
			SLOT(nodeTypeComboBoxChanged(int)));

	toolBar = new QToolBar("Tools", this);

	QFrame * frame = createHorizontalFrame();

	frame->layout()->addWidget(noSelectB);
	frame->layout()->addWidget(singleSelectB);
	toolBar->addWidget(frame);

	frame = createHorizontalFrame();
	toolBar->addWidget(frame);
	frame->layout()->addWidget(multiSelectB);
	frame->layout()->addWidget(centerB);

	toolBar->addWidget(nodeTypeCB);
	toolBar->addSeparator();

	frame = createHorizontalFrame();

	toolBar->addWidget(frame);
	frame->layout()->addWidget(addMetaB);
	frame->layout()->addWidget(removeMetaB);

	frame = createHorizontalFrame();

	toolBar->addWidget(frame);
	frame->layout()->addWidget(fixB);
	frame->layout()->addWidget(unFixB);

	toolBar->addWidget(labelsB);
	toolBar->addSeparator();
	toolBar->addAction(playAction);
	toolBar->addSeparator();

	//inicializacia slideru
	slider = new QSlider(Qt::Vertical, this);
	slider->setTickPosition(QSlider::TicksAbove);
	slider->setTickInterval(5);
	slider->setValue(5);
	slider->setFocusPolicy(Qt::NoFocus);
	connect(slider, SIGNAL(valueChanged(int)), this,
			SLOT(sliderValueChanged(int)));

	frame = createHorizontalFrame();
	frame->setMaximumHeight(100);
	frame->layout()->setAlignment(Qt::AlignHCenter);
	toolBar->addWidget(frame);
	frame->layout()->addWidget(slider);

	addToolBar(Qt::LeftToolBarArea, toolBar);
	toolBar->setMovable(false);
}

QFrame* CoreWindow::createHorizontalFrame() {
	QFrame * frame = new QFrame();
	QHBoxLayout * layout = new QHBoxLayout();
	frame->setLayout(layout);
	layout->setMargin(0);
	layout->setSpacing(0);

	return frame;
}

void CoreWindow::showOptions() {
	OptionsWindow *options = new OptionsWindow(coreGraph, viewerWidget);
	options->show();
}

void CoreWindow::playPause() {
	if (layout->isPlaying()) {
		playAction->setIcon(QIcon("img/gui/play.png"));
		layout->pause();
//		coreGraph->setNodesFreezed(true);
	} else {
		playAction->setIcon(QIcon("img/gui/pause.png"));
//		coreGraph->setNodesFreezed(false);
		layout->play();
	}
}

void CoreWindow::noSelectClicked(bool checked) {
	viewerWidget->getPickHandler()->setPickMode(
			Vwr::PickHandler::PickMode::NONE);
	singleSelectB->setChecked(false);
	multiSelectB->setChecked(false);
	centerB->setChecked(false);
}

void CoreWindow::singleSelectClicked(bool checked) {
	viewerWidget->getPickHandler()->setPickMode(
			Vwr::PickHandler::PickMode::SINGLE);
	noSelectB->setChecked(false);
	multiSelectB->setChecked(false);
	centerB->setChecked(false);
}

void CoreWindow::multiSelectClicked(bool checked) {
	viewerWidget->getPickHandler()->setPickMode(
			Vwr::PickHandler::PickMode::MULTI);
	noSelectB->setChecked(false);
	singleSelectB->setChecked(false);
	centerB->setChecked(false);
}

void CoreWindow::centerView(bool checked) {
	noSelectB->setChecked(false);
	singleSelectB->setChecked(false);
	multiSelectB->setChecked(false);

	viewerWidget->getCameraManipulator()->setCenter(
			viewerWidget->getPickHandler()->getSelectionCenter(false));
}

void CoreWindow::fixNodes() {
	viewerWidget->getPickHandler()->toggleSelectedNodesFixedState(true);
}

void CoreWindow::unFixNodes() {
	viewerWidget->getPickHandler()->toggleSelectedNodesFixedState(false);
	layout->wakeUp();
}

//void CoreWindow::addMetaNode() {
//	Graph::Graph * currentGraph = manager->getActiveGraph();
//
//	if (currentGraph != NULL) {
//		osg::Vec3 position =
//				viewerWidget->getPickHandler()->getSelectionCenter(true);
//
//		osg::ref_ptr<Graph::Node> metaNode = currentGraph->addNode("metaNode",
//				currentGraph->getNodeMetaType(), position);
//		QLinkedList<osg::ref_ptr<Graph::Node> > * selectedNodes =
//				viewerWidget->getPickHandler()->getSelectedNodes();
//
//		QLinkedList<osg::ref_ptr<Graph::Node> >::const_iterator i =
//				selectedNodes->constBegin();
//
//		while (i != selectedNodes->constEnd()) {
//			Graph::Edge * e = currentGraph->addEdge("metaEdge", (*i), metaNode,
//					currentGraph->getEdgeMetaType(), true);
//			e->setCamera(viewerWidget->getCamera());
//			++i;
//		}
//
//		if (isPlaying)
//			layout->play();
//	}
//}

//void CoreWindow::removeMetaNodes() {
//	QLinkedList<osg::ref_ptr<Graph::Node> > * selectedNodes =
//			viewerWidget->getPickHandler()->getSelectedNodes();
//	Graph::Graph * currentGraph = manager->getActiveGraph();
//
//	QLinkedList<osg::ref_ptr<Graph::Node> >::const_iterator i =
//			selectedNodes->constBegin();
//
//	while (i != selectedNodes->constEnd()) {
//		if ((*i)->getType()->isMeta())
//			currentGraph->removeNode((*i));
//		++i;
//	}
//
//		layout->wakeUp();
//}

void CoreWindow::loadFile() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open GraphML"),
			".", tr("GraphML Files (*.graphml)"));

	layout->pause();

	Model::Graph* graph = manager->loadGraph(fileName, messageWindows);

	if (graph == NULL) {
		messageWindows->showMessageBox("Chyba",
				"Zvoleny subor nie je validny GraphML subor.", true);
		layout->play();
		return;
	}

	// TODO fix loading

	if (layout->isRunning()) {
		layout->stop();
		layout->wait();
	}
	delete layout;

	layout = new Model::FRAlgorithm(graph);
	layout->setParameters(10, 0.7, 1, true);
	coreGraph->reload(graph);

	layout->start();
	layout->play();

	messageWindows->closeLoadingDialog();

	viewerWidget->getCameraManipulator()->home(0);
	statusBar()->showMessage("Graph loaded");
}

void CoreWindow::labelOnOff(bool) {
	if (viewerWidget->getPickHandler()->getSelectionType()
			== Vwr::PickHandler::SelectionType::EDGE) {
		edgeLabelsVisible = !edgeLabelsVisible;
		coreGraph->setEdgeLabelsVisible(edgeLabelsVisible);
	} else if (viewerWidget->getPickHandler()->getSelectionType()
			== Vwr::PickHandler::SelectionType::NODE) {
		nodeLabelsVisible = !nodeLabelsVisible;
		coreGraph->setNodeLabelsVisible(nodeLabelsVisible);
	} else if (viewerWidget->getPickHandler()->getSelectionType()
			== Vwr::PickHandler::SelectionType::ALL) {
		bool state = edgeLabelsVisible & nodeLabelsVisible;

		nodeLabelsVisible = edgeLabelsVisible = !state;

		coreGraph->setEdgeLabelsVisible(!state);
		coreGraph->setNodeLabelsVisible(!state);
	}
}

void CoreWindow::sliderValueChanged(int value) {
	layout->setAlphaValue((float) value * 0.001);
}

void CoreWindow::nodeTypeComboBoxChanged(int index) {
	switch (index) {
	case 0:
		viewerWidget->getPickHandler()->setSelectionType(
				Vwr::PickHandler::SelectionType::ALL);
		labelsB->setChecked(edgeLabelsVisible & nodeLabelsVisible);
		break;
	case 1:
		viewerWidget->getPickHandler()->setSelectionType(
				Vwr::PickHandler::SelectionType::NODE);
		labelsB->setChecked(nodeLabelsVisible);
		break;
	case 2:
		viewerWidget->getPickHandler()->setSelectionType(
				Vwr::PickHandler::SelectionType::EDGE);
		labelsB->setChecked(edgeLabelsVisible);
		break;
	}
}

void CoreWindow::closeEvent(QCloseEvent *event) {
	qDebug("About to quit\n");
	event->accept();
}

