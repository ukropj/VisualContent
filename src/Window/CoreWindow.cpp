#include "Window/CoreWindow.h"
#include "Window/OptionsWindow.h"
#include "Window/CheckBoxList.h"
#include "Window/ViewerQT.h"
#include "Window/MessageWindows.h"
#include "Model/FRAlgorithm.h"
#include "Core/IOManager.h"
#include "Viewer/SceneGraph.h"

using namespace Window;

CoreWindow::CoreWindow(QWidget *parent) :
	QMainWindow(parent) {

	createStatusBar();
	//inicializacia premennych
	layout = new Model::FRAlgorithm();
	manager = new AppCore::IOManager();
	messageWindows = new Window::MessageWindows();
	sceneGraph = new Vwr::SceneGraph(new Model::Graph("empty", 0));

	//vytvorenie menu a toolbar-ov
	createActions();
	createMenus();
	createToolBar();

	viewerWidget = new ViewerQT(sceneGraph, this);
	setCentralWidget(viewerWidget);

	nodeLabelsVisible = edgeLabelsVisible = false;

	// testing only:
	Model::Graph* graph = manager->loadGraph("input/data/grid7.graphml",
			messageWindows);
	layout->setGraph(graph);
	layout->setParameters(10, 0.7, 1, false);
	sceneGraph->reload(graph);
	layout->play();

	noSelectB->click();
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
	playAction->setToolTip(tr("Play"));
	connect(playAction, SIGNAL(triggered()), this, SLOT(playPause()));

	addMetaB = new QPushButton();
	addMetaB->setIcon(QIcon("img/gui/meta.png"));
	addMetaB->setToolTip("&Add meta node");
	addMetaB->setFocusPolicy(Qt::NoFocus);
	addMetaB->setEnabled(false);// FIXME remove
	//	connect(addMeta, SIGNAL(clicked()), this, SLOT(addMetaNode()));

	removeMetaB = new QPushButton();
	removeMetaB->setIcon(QIcon("img/gui/removemeta.png"));
	removeMetaB->setToolTip(tr("Remove meta nodes"));
	removeMetaB->setFocusPolicy(Qt::NoFocus);
	removeMetaB->setEnabled(false);//FIXME remove
	//	connect(removeMeta, SIGNAL(clicked()), this, SLOT(removeMetaNodes()));

	fixB = new QPushButton();
	fixB->setIcon(QIcon("img/gui/fix.png"));
	fixB->setToolTip(tr("Fix nodes"));
	fixB->setFocusPolicy(Qt::NoFocus);
	fixB->setShortcut(tr("CTRL+F"));
	connect(fixB, SIGNAL(clicked()), this, SLOT(fixNodes()));

	unFixB = new QPushButton();
	unFixB->setIcon(QIcon("img/gui/unfix.png"));
	unFixB->setToolTip(tr("Unfix nodes"));
	unFixB->setFocusPolicy(Qt::NoFocus);
	unFixB->setShortcut(tr("CTRL+U"));
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
	noSelectB->setShortcut(tr("CTRL+Z"));
	noSelectB->setToolTip(tr("Explore mode"));
	noSelectB->setCheckable(true);
	noSelectB->setFocusPolicy(Qt::NoFocus);
	connect(noSelectB, SIGNAL(clicked(bool)), this, SLOT(noSelectClicked(bool)));

	singleSelectB = new QPushButton();
	singleSelectB->setIcon(QIcon("img/gui/singleselect.png"));
	singleSelectB->setShortcut(tr("CTRL+X"));
	singleSelectB->setToolTip(tr("Select mode"));
	singleSelectB->setCheckable(true);
	singleSelectB->setFocusPolicy(Qt::NoFocus);
	connect(singleSelectB, SIGNAL(clicked(bool)), this,
			SLOT(singleSelectClicked(bool)));

	multiSelectB = new QPushButton();
	multiSelectB->setIcon(QIcon("img/gui/multiselect.png"));
	multiSelectB->setToolTip(tr("Randomize"));
	multiSelectB->setFocusPolicy(Qt::NoFocus);
	connect(multiSelectB, SIGNAL(clicked()), this, SLOT(randomize()));

	centerB = new QPushButton();
	centerB->setIcon(QIcon("img/gui/center.png"));
	centerB->setToolTip(tr("Center view"));
	centerB->setFocusPolicy(Qt::NoFocus);
	connect(centerB, SIGNAL(clicked(bool)), this, SLOT(centerView(bool)));
}

void CoreWindow::createMenus() {
	fileMenu = menuBar()->addMenu(tr("File"));
	fileMenu->addAction(loadAction);
	fileMenu->addSeparator();
	fileMenu->addAction(quitAction);

	editMenu = menuBar()->addMenu(tr("Edit"));
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

	//	frame = createHorizontalFrame();
	//	toolBar->addWidget(frame);
	//	frame->layout()->addWidget(addMetaB);
	//	frame->layout()->addWidget(removeMetaB);

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
	OptionsWindow *options = new OptionsWindow(sceneGraph, viewerWidget);
	options->show();
}

void CoreWindow::playPause() {
	if (layout->isPlaying()) {
		playAction->setIcon(QIcon("img/gui/play.png"));
		layout->pause();
		sceneGraph->setNodesFreezed(true);
		log(NORMAL, "Layout paused");
	} else {
		playAction->setIcon(QIcon("img/gui/pause.png"));
		sceneGraph->setNodesFreezed(false);
		layout->play();
		log(NORMAL, "Layout unpaused");
	}
}

void CoreWindow::randomize() {
	layout->randomize();
	log(NORMAL, "Layout randomized");
}

void CoreWindow::noSelectClicked(bool checked) {
	viewerWidget->getPickHandler()->setPickMode(Vwr::PickHandler::NONE);
	singleSelectB->setChecked(false);
	multiSelectB->setChecked(false);
	centerB->setChecked(false);
}

void CoreWindow::singleSelectClicked(bool checked) {
	viewerWidget->getPickHandler()->setPickMode(Vwr::PickHandler::SELECT);
	noSelectB->setChecked(false);
	multiSelectB->setChecked(false);
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
	NodeList nodes = viewerWidget->getPickHandler()->getSelectedNodes();
	NodeList::const_iterator i = nodes.constBegin();

	while (i != nodes.constEnd()) {
//		(*i)->setTargetPosition((*i)->getCurrentPosition() / graphScale);
		(*i)->setFixed(true);
		++i;
	}
}

void CoreWindow::unFixNodes() {
	NodeList nodes = viewerWidget->getPickHandler()->getSelectedNodes();
	NodeList::const_iterator i = nodes.constBegin();

	while (i != nodes.constEnd()) {
		(*i)->setFixed(false);
		++i;
	}
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

	Model::Graph* graph = manager->loadGraph(fileName, messageWindows);

	if (graph == NULL) {
		messageWindows->showMessageBox("Chyba",
				"Zvoleny subor nie je validny GraphML subor.", true);
		return;
	}

	layout->pause();
	layout->setGraph(graph);
	layout->setParameters(10, 0.7, 1, false);

	sceneGraph->reload(graph);

	messageWindows->closeLoadingDialog();

	layout->play();

	viewerWidget->getCameraManipulator()->home(0);
	log(NORMAL, "Graph loaded");
}

void CoreWindow::labelOnOff(bool) {
	bool state;
	switch (viewerWidget->getPickHandler()->getSelectionType()) {
	case Vwr::PickHandler::EDGE:
		edgeLabelsVisible = !edgeLabelsVisible;
		sceneGraph->setEdgeLabelsVisible(edgeLabelsVisible);
		break;
	case Vwr::PickHandler::NODE:
		nodeLabelsVisible = !nodeLabelsVisible;
		sceneGraph->setNodeLabelsVisible(nodeLabelsVisible);
		break;
	case Vwr::PickHandler::ALL:
		state = edgeLabelsVisible & nodeLabelsVisible;
		nodeLabelsVisible = edgeLabelsVisible = !state;
		sceneGraph->setEdgeLabelsVisible(!state);
		sceneGraph->setNodeLabelsVisible(!state);
	default:
		break;
	}
}

void CoreWindow::sliderValueChanged(int value) {
	layout->setAlphaValue((float) value * 0.001);
}

void CoreWindow::nodeTypeComboBoxChanged(int index) {
	switch (index) {
	case 0:
		viewerWidget->getPickHandler()->setSelectionType(Vwr::PickHandler::ALL);
		labelsB->setChecked(edgeLabelsVisible & nodeLabelsVisible);
		break;
	case 1:
		viewerWidget->getPickHandler()->setSelectionType(Vwr::PickHandler::NODE);
		labelsB->setChecked(nodeLabelsVisible);
		break;
	case 2:
		viewerWidget->getPickHandler()->setSelectionType(Vwr::PickHandler::EDGE);
		labelsB->setChecked(edgeLabelsVisible);
		break;
	}
}

void CoreWindow::closeEvent(QCloseEvent *event) {
	qDebug("About to quit\n");
	layout->stop();
	layout->wait();
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
	case MAIN:
		mainStatus->setText(msg);
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
	mainStatus = new QLabel("Application ready");
	statusBar()->addWidget(mainStatus);
	//	statusBar()->addPermanentWidget(pickStatus);
	//	statusBar()->addPermanentWidget(keyStatus);
	statusBar()->addPermanentWidget(algStatus);
	instanceForStatusLog = this;
}

