#include "Window/CoreWindow.h"

using namespace Window;

CoreWindow::CoreWindow(AppCore::IOManager* manager,
		Vwr::CoreGraph* coreGraph, QApplication* app) :
	QMainWindow(NULL) {
	//inicializacia premennych
	isPlaying = true;
	application = app;
	layout = new Model::FRAlgorithm();
	this->manager = manager;
	messageWindows = new Window::MessageWindows();

	//vytvorenie menu a toolbar-ov
	createActions();
	createMenus();
	createToolBar();

	viewerWidget = new ViewerQT(this, 0, 0, 0, coreGraph);
	viewerWidget->setSceneData(coreGraph->getScene());
	setCentralWidget(viewerWidget);
	statusBar()->showMessage("Ready");

	this->coreGraph = coreGraph;
	nodeLabelsVisible = edgeLabelsVisible = false;
}

void CoreWindow::createActions() {
	quit = new QAction("Quit", this);
	connect(quit, SIGNAL(triggered()), application, SLOT(quit()));
	connect(application, SIGNAL(aboutToQuit()), this, SLOT(clean()));

	options = new QAction("Options", this);
	connect(options, SIGNAL(triggered()), this, SLOT(showOptions()));

	load = new QAction(QIcon("img/gui/open.png"), "&Load", this);
	load->setShortcut(tr("CTRL+L"));
	connect(load, SIGNAL(triggered()), this, SLOT(loadFile()));

	play = new QPushButton();
	play->setIcon(QIcon("img/gui/pause.png"));
	play->setToolTip("&Play");
	play->setFocusPolicy(Qt::NoFocus);
	connect(play, SIGNAL(clicked()), this, SLOT(playPause()));

	addMeta = new QPushButton();
	addMeta->setIcon(QIcon("img/gui/meta.png"));
	addMeta->setToolTip("&Add meta node");
	addMeta->setFocusPolicy(Qt::NoFocus);
	addMeta->setEnabled(false);// FIXME remove
	//	connect(addMeta, SIGNAL(clicked()), this, SLOT(addMetaNode()));

	removeMeta = new QPushButton();
	removeMeta->setIcon(QIcon("img/gui/removemeta.png"));
	removeMeta->setToolTip("&Remove meta nodes");
	removeMeta->setFocusPolicy(Qt::NoFocus);
	removeMeta->setEnabled(false);//FIXME remove
	//	connect(removeMeta, SIGNAL(clicked()), this, SLOT(removeMetaNodes()));

	fix = new QPushButton();
	fix->setIcon(QIcon("img/gui/fix.png"));
	fix->setToolTip("&Fix nodes");
	fix->setFocusPolicy(Qt::NoFocus);
	connect(fix, SIGNAL(clicked()), this, SLOT(fixNodes()));

	unFix = new QPushButton();
	unFix->setIcon(QIcon("img/gui/unfix.png"));
	unFix->setToolTip("&Unfix nodes");
	unFix->setFocusPolicy(Qt::NoFocus);
	connect(unFix, SIGNAL(clicked()), this, SLOT(unFixNodes()));

	label = new QPushButton();
	label->setIcon(QIcon("img/gui/label.png"));
	label->setToolTip("&Turn on/off labels");
	label->setCheckable(true);
	label->setFocusPolicy(Qt::NoFocus);
	connect(label, SIGNAL(clicked(bool)), this, SLOT(labelOnOff(bool)));

	//mody - ziadny vyber, vyber jedneho, multi vyber centrovanie
	noSelect = new QPushButton();
	noSelect->setIcon(QIcon("img/gui/noselect.png"));
	noSelect->setToolTip("&No-select mode");
	noSelect->setCheckable(true);
	noSelect->setFocusPolicy(Qt::NoFocus);
	connect(noSelect, SIGNAL(clicked(bool)), this, SLOT(noSelectClicked(bool)));

	singleSelect = new QPushButton();
	singleSelect->setIcon(QIcon("img/gui/singleselect.png"));
	singleSelect->setToolTip("&Single-select mode");
	singleSelect->setCheckable(true);
	singleSelect->setFocusPolicy(Qt::NoFocus);
	connect(singleSelect, SIGNAL(clicked(bool)), this,
			SLOT(singleSelectClicked(bool)));

	multiSelect = new QPushButton();
	multiSelect->setIcon(QIcon("img/gui/multiselect.png"));
	multiSelect->setToolTip("&Multi-select mode");
	multiSelect->setCheckable(true);
	multiSelect->setFocusPolicy(Qt::NoFocus);
	connect(multiSelect, SIGNAL(clicked(bool)), this,
			SLOT(multiSelectClicked(bool)));

	center = new QPushButton();
	center->setIcon(QIcon("img/gui/center.png"));
	center->setToolTip("&Center view");
	center->setFocusPolicy(Qt::NoFocus);
	connect(center, SIGNAL(clicked(bool)), this, SLOT(centerView(bool)));
}

void CoreWindow::createMenus() {
	file = menuBar()->addMenu("File");
	file->addAction(load);
	file->addSeparator();
	file->addAction(quit);

	edit = menuBar()->addMenu("Edit");
	edit->addAction(options);
}

void CoreWindow::createToolBar() {
	//inicializacia comboboxu typov vyberu
	nodeTypeComboBox = new QComboBox();
	nodeTypeComboBox->insertItems(0, (QStringList() << "All" << "Node"
			<< "Edge"));
	nodeTypeComboBox->setFocusPolicy(Qt::NoFocus);
	connect(nodeTypeComboBox, SIGNAL(currentIndexChanged(int)), this,
			SLOT(nodeTypeComboBoxChanged(int)));

	toolBar = new QToolBar("Tools", this);

	QFrame * frame = createHorizontalFrame();

	frame->layout()->addWidget(noSelect);
	frame->layout()->addWidget(singleSelect);
	toolBar->addWidget(frame);

	frame = createHorizontalFrame();
	toolBar->addWidget(frame);
	frame->layout()->addWidget(multiSelect);
	frame->layout()->addWidget(center);

	toolBar->addWidget(nodeTypeComboBox);
	toolBar->addSeparator();

	frame = createHorizontalFrame();

	toolBar->addWidget(frame);
	frame->layout()->addWidget(addMeta);
	frame->layout()->addWidget(removeMeta);

	frame = createHorizontalFrame();

	toolBar->addWidget(frame);
	frame->layout()->addWidget(fix);
	frame->layout()->addWidget(unFix);

	toolBar->addWidget(label);
	toolBar->addSeparator();
	toolBar->addWidget(play);
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
	if (isPlaying) {
		play->setIcon(QIcon("img/gui/play.png"));
		isPlaying = false;
		layout->pause();
		coreGraph->setNodesFreezed(true);
	} else {
		play->setIcon(QIcon("img/gui/pause.png"));
		isPlaying = true;
		coreGraph->setNodesFreezed(false);
		layout->play();
	}
}

void CoreWindow::noSelectClicked(bool checked) {
	viewerWidget->getPickHandler()->setPickMode(
			Vwr::PickHandler::PickMode::NONE);
	singleSelect->setChecked(false);
	multiSelect->setChecked(false);
	center->setChecked(false);
}

void CoreWindow::singleSelectClicked(bool checked) {
	viewerWidget->getPickHandler()->setPickMode(
			Vwr::PickHandler::PickMode::SINGLE);
	noSelect->setChecked(false);
	multiSelect->setChecked(false);
	center->setChecked(false);
}

void CoreWindow::multiSelectClicked(bool checked) {
	viewerWidget->getPickHandler()->setPickMode(
			Vwr::PickHandler::PickMode::MULTI);
	noSelect->setChecked(false);
	singleSelect->setChecked(false);
	center->setChecked(false);
}

void CoreWindow::centerView(bool checked) {
	noSelect->setChecked(false);
	singleSelect->setChecked(false);
	multiSelect->setChecked(false);

	viewerWidget->getCameraManipulator()->setCenter(
			viewerWidget->getPickHandler()->getSelectionCenter(false));
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

void CoreWindow::fixNodes() {
	viewerWidget->getPickHandler()->toggleSelectedNodesFixedState(true);
}

void CoreWindow::unFixNodes() {
	viewerWidget->getPickHandler()->toggleSelectedNodesFixedState(false);

	if (isPlaying)
		layout->play();
}

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
//	if (isPlaying)
//		layout->play();
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
		label->setChecked(edgeLabelsVisible & nodeLabelsVisible);
		break;
	case 1:
		viewerWidget->getPickHandler()->setSelectionType(
				Vwr::PickHandler::SelectionType::NODE);
		label->setChecked(nodeLabelsVisible);
		break;
	case 2:
		viewerWidget->getPickHandler()->setSelectionType(
				Vwr::PickHandler::SelectionType::EDGE);
		label->setChecked(edgeLabelsVisible);
		break;
	}
}

void CoreWindow::clean() {
	//sem kod na vycistenie pamate
	cout << "About to quit\n";
}
