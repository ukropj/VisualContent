/**
 *  CoreWindow.h
 *  Projekt 3DVisual
 */
#ifndef Window_CORE_WINDOW
#define Window_CORE_WINDOW 1

#include <QMainWindow>
#include <QToolBar>
#include <QApplication>
#include <QIcon>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QtGui>
#include <Qt/qstringlist.h>

#include "Window/OptionsWindow.h"
#include "Viewer/CoreGraph.h"
#include "Window/CheckBoxList.h"
#include "Window/ViewerQT.h"
#include "Window/MessageWindows.h"

#include "Model/FRAlgorithm.h"
#include "Core/IOManager.h"

namespace AppCore {
class IOManager;
}

namespace Window {
/**
 *  \class CoreWindow
 *  \brief Main window of aplication
 *  \author Adam Pazitnaj
 *  \date 3. 5. 2010
 */
class CoreWindow: public QMainWindow {
Q_OBJECT

public slots:

	/**
	 *  \fn public  showOptions
	 *  \brief Show the options of aplication
	 */
	void showOptions();

	/**
	 *  \fn public  playPause
	 *  \brief Play or pause the layout algorithm
	 */
	void playPause();

	/**
	 *  \fn public  noSelectClicked(bool checked
	 *  \brief No-select mode selected
	 *  \param checked    flag if button is checked
	 */
	void noSelectClicked(bool checked);

	/**
	 *  \fn public  singleSelectClicked(bool checked
	 *  \brief Single-select mode selected
	 *  \param checked   flag if button is checked
	 */
	void singleSelectClicked(bool checked);

	/**
	 *  \fn public  multiSelectClicked(bool checked
	 *  \brief Multi-select mode selected
	 *  \param  checked     flag if button is checked
	 */
	void multiSelectClicked(bool checked);

	/**
	 *  \fn public  addMetaNode
	 *  \brief Add meta node
	 */
//	void addMetaNode();

	/**
	 *  \fn public  centerView(bool checked
	 *  \brief Center the view
	 *  \param   checked     flag if button is checked
	 */
	void centerView(bool checked);

	/**
	 *  \fn public  fixNodes
	 *  \brief Fix selected nodes
	 */
	void fixNodes();

	/**
	 *  \fn public  unFixNodes
	 *  \brief Unfix all nodes
	 */
	void unFixNodes();

	/**
	 *  \fn public  removeMetaNodes
	 *  \brief Remove all meta nodes
	 */
//	void removeMetaNodes();

	/**
	 *  \fn public  loadFile
	 *  \brief Show dialog to select file which will be opened
	 */
	void loadFile();

	/**
	 *  \fn public  labelOnOff(bool checked)
	 *  \brief Show / hide labels
	 *  \param  checked flag if button is checked
	 */
	void labelOnOff(bool checked);

	/**
	 *  \fn public  sliderValueChanged(int value)
	 *  \brief Slider value is changed
	 *  \param value actual value of slider
	 */
	void sliderValueChanged(int value);

	/**
	 *  \fn public  nodeTypeComboBoxChanged(int index)
	 *  \brief Type in combobox changed
	 *  \param  index
	 */
	void nodeTypeComboBoxChanged(int index);

	/**
	 *  \fn clean
	 *  \brief Slot function that is called befoure application atempts to quit
	 */
	void clean();

private:

	/**
	 *  QApplication * application
	 *  \brief Pointer ro aplication
	 */
	QApplication * application;

	/**
	 *  QToolBar * toolBar
	 *  \brief Pointer to toolbar
	 */
	QToolBar * toolBar;

	/**
	 *  QAction * quit
	 *  \brief Action to quit aplication
	 */
	QAction * quit;

	/**
	 *  QAction * options
	 *  \brief Pointer to option dialog
	 */
	QAction * options;

	/**
	 *  QPushButton * play
	 *  \brief Action for play/pause layout
	 */
	QPushButton * play;

	/**
	 *  QPushButton * addMeta
	 *  \brief Action for adding meta node
	 */
	QPushButton * addMeta;

	/**
	 *  QPushButton * fix
	 *  \brief Action for fix selected nodes
	 */
	QPushButton * fix;

	/**
	 *  QPushButton * unFix
	 *  \brief Action for unfix nodes
	 */
	QPushButton * unFix;

	/**
	 *  QPushButton * noSelect
	 *  \brief Action for no-select mode
	 */
	QPushButton * noSelect;

	/**
	 *  QPushButton * singleSelect
	 *  \brief Action for single-select mode
	 */
	QPushButton * singleSelect;

	/**
	 *  QPushButton * multiSelect
	 *  \brief Action for multi-select mode
	 */
	QPushButton * multiSelect;

	/**
	 *  QPushButton * center
	 *  \brief Action for center view
	 */
	QPushButton * center;

	/**
	 *  QPushButton * removeMeta
	 *  \brief Action for removing meta nodes
	 */
	QPushButton * removeMeta;

	/**
	 *  QAction * load
	 *  \brief Action for loading file
	 */
	QAction * load;

	/**
	 *  QPushButton * label
	 *  \brief Pointer to labelOn/labelOff button
	 */
	QPushButton * label;

	/**
	 *  QSlider * slider
	 *  \brief Pointer to slider
	 */
	QSlider * slider;

	/**
	 *  QMenu * file
	 *  \brief Pointer to file menu
	 */
	QMenu * file;

	/**
	 *  QMenu * edit
	 *  \brief Pointer to edit menu
	 */
	QMenu * edit;

	/**
	 *  Window::ViewerQT * viewerWidget
	 *  \brief Ponter to viewer widget
	 */
	ViewerQT * viewerWidget;

	/**
	 *  Layout::LayoutThread * layout
	 *  \brief Pointer to layout thread
	 */
	Model::FRAlgorithm* layout;

	/**
	 *  QComboBox * nodeTypeComboBox
	 *  \brief Pointer to comobox of node types
	 */
	QComboBox * nodeTypeComboBox;

	/**
	 *  int isPlaying
	 *  \brief Flag if layout is running
	 */
	bool isPlaying;

	/**
	 *  \fn private  createActions
	 *  \brief Initialize all actions of aplication
	 */
	void createActions();

	/**
	 *  \fn private  createMenus
	 *  \brief Create menu of aplication
	 */
	void createMenus();

	/**
	 *  \fn private  createToolBar
	 *  \brief Create toolBar
	 */
	void createToolBar();

	/**
	 *  \fn private  createHorizontalFrame
	 *  \brief Crate frame with horizontal label
	 *  \return QFrame * created frame
	 */
	QFrame * createHorizontalFrame();

	/**
	 *  Vwr::CoreGraph * coreGraph
	 *  \brief Pointer to CoreGraph
	 */
	Vwr::CoreGraph * coreGraph;

	/**
	 *  bool edgeLabelsVisible
	 *  \brief Flag if edges are visible
	 */
	bool edgeLabelsVisible;

	/**
	 *  bool nodeLabelsVisible
	 *  \brief Flag if labels are visible
	 */
	bool nodeLabelsVisible;

	AppCore::IOManager *manager;	// TODO refactor to MVC
	Window::MessageWindows *messageWindows;

public:

	/*!
	 *
	 * \param parent
	 * Rodic okna.
	 *
	 * \param coreGraph
	 * Graf, ktory bude zobrazovany.
	 *
	 * \param app
	 * Qt aplikacia.
	 *
	 * Constructor
	 *
	 */
	CoreWindow(AppCore::IOManager* manager, Vwr::CoreGraph* coreGraph,
			QApplication* app);

	/**
	 *  \fn inline public constant  getLayoutThread
	 *  \brief Get the layout thread
	 *  \return Layout::LayoutThread *
	 */
	Model::FRAlgorithm * getLayoutThread() const {
		return layout;
	}

};
}

#endif
