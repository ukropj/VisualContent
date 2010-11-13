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
#include <QDebug>
#include <Qt/qstringlist.h>

namespace AppCore {
class IOManager;
}
namespace Model {
class FRAlgorithm;
}
namespace Vwr {
class SceneGraph;
}

namespace Window {
class ViewerQT;
class MessageWindows;

/**
 *  \class CoreWindow
 *  \brief Main window of aplication
 *  \author Adam Pazitnaj
 *  \date 3. 5. 2010
 */
class CoreWindow: public QMainWindow {
Q_OBJECT

public:

	enum StatusMsgType {
		ALG, PICK, KEYS, MAIN, NORMAL, TEMP
	};

	CoreWindow(QWidget* parent = 0);

	Model::FRAlgorithm * getLayoutThread() const {
		return layout;
	}

	static void log(StatusMsgType type, QString msg);

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
	 *  \fn public  addMetaNode
	 *  \brief Add meta node
	 */
	//	void addMetaNode();

	/**
	 *  \fn public  centerView(bool checked
	 *  \brief Center the view
	 *  \param   checked     flag if button is checked
	 */
	void centerView();

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

	void randomize();

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

protected:
	void closeEvent(QCloseEvent *event);
	void setSelectedFixed(bool fixed);

private:

	QToolBar * toolBar;

	QAction * quitAction;
	QAction * optionsAction;
	QAction * playAction;

	/**
	 *  QPushButton * addMeta
	 *  \brief Action for adding meta node
	 */
	QPushButton * addMetaB;

	/**
	 *  QPushButton * fix
	 *  \brief Action for fix selected nodes
	 */
	QPushButton * fixB;

	/**
	 *  QPushButton * unFix
	 *  \brief Action for unfix nodes
	 */
	QPushButton * unFixB;

	/**
	 *  QPushButton * noSelect
	 *  \brief Action for no-select mode
	 */
	QPushButton * noSelectB;

	/**
	 *  QPushButton * singleSelect
	 *  \brief Action for single-select mode
	 */
	QPushButton * singleSelectB;

	/**
	 *  QPushButton * multiSelect
	 *  \brief Action for multi-select mode
	 */
	QPushButton * randomizeB;

	/**
	 *  QPushButton * center
	 *  \brief Action for center view
	 */
	QPushButton * centerB;

	/**
	 *  QPushButton * removeMeta
	 *  \brief Action for removing meta nodes
	 */
	QPushButton * removeMetaB;

	/**
	 *  QAction * load
	 *  \brief Action for loading file
	 */
	QAction * loadAction;

	/**
	 *  QPushButton * label
	 *  \brief Pointer to labelOn/labelOff button
	 */
	QPushButton * labelsB;

	/**
	 *  QSlider * slider
	 *  \brief Pointer to slider
	 */
	QSlider * slider;

	/**
	 *  QMenu * file
	 *  \brief Pointer to file menu
	 */
	QMenu * fileMenu;

	/**
	 *  QMenu * edit
	 *  \brief Pointer to edit menu
	 */
	QMenu * editMenu;

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
	QComboBox * nodeTypeCB;

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
	Vwr::SceneGraph * sceneGraph;

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

	AppCore::IOManager *manager;
	MessageWindows *messageWindows;

	void showStatusMsg(StatusMsgType type, QString msg);
	void createStatusBar();

	static CoreWindow* instanceForStatusLog;
	QLabel *algStatus;
	QLabel *pickStatus;
	QLabel *keyStatus;
	QLabel *mainStatus;
};
}

#endif
