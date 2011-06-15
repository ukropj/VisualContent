#ifndef Window_CORE_WINDOW
#define Window_CORE_WINDOW 1

#include <QMainWindow>
#include <QtGui>
#include <QMutex>
#include <QDebug>
#include <QProgressDialog>

namespace GraphIO {
class IOManager;
}
namespace Model {
class Graph;
class FRAlgorithm;
class Clusterer;
}
namespace Vwr {
class SceneGraph;
}

/// GUI
namespace Window {
class ViewerQT;
class ProgressManager;

/**
 *  \class CoreWindow
 *  \brief Main window of aplication.
 *  \author Adam Pazitnaj
 *  \author Jakub Ukrop
 */
class CoreWindow: public QMainWindow {
Q_OBJECT

public:
	enum StatusMsgType {
		ALG, PICK, KEYS, MAIN, NORMAL, TEMP
	};
	CoreWindow(QWidget* parent = 0);
	Model::FRAlgorithm * getLayoutThread() const {
		return layouter;
	}
	static void log(StatusMsgType type, QString msg);
signals:
	void windowResized();
private slots:
	void openFile();
	void reloadFile();
	void setDataMapping();
	void openRecentFile();
	void showOptions();
	void closeEvent(QCloseEvent *event);
	void resizeEvent(QResizeEvent *event);

	void centerView();
	void randomize();
	void toggleLabels(bool checked);
	void toggleLayouting(bool checked);
	void captureScreen();
	void setAlpha(int value);

	void toggleAutoCluster(bool checked);
	void setClusterThreshold(int value);
	void setClusteringAlg(QAction* action);

	void toggleDebug();
	void showMessageBox(QString title, QString message, bool isError);

private:
	void loadFile(QString fileName);
	void readSettings();
	void writeSettings();
	void updateRecentFileActions(QString fileName = 0);

	enum { MaxRecentFiles = 10 };

	QAction* loadAction;
	QAction* reloadAction;
	QAction* remapAction;
	QAction* quitAction;
	QAction* optionsAction;
	QAction* separatorAction;
	QAction* recentFileActions[MaxRecentFiles];

	QAction* playAction;
	QAction* randomizeAction;
	QAction* centerAction;
	QAction* labelsAction;
	QAction* captureAction;
	QSlider* slider;
	QAction* debugAction;

	QAction* autoClusterAction;
	QAction* cluster0Action;
	QAction* cluster1Action;
	QAction* cluster2Action;
	QAction* cluster3Action;
	QSlider* clusterSlider;

	QMenu* fileMenu;
	QMenu* clusteringMenu;
	QToolBar* toolBar;
	QToolBar* clusteringBar;

	Window::ViewerQT* viewerWidget;
	Model::FRAlgorithm* layouter;
	Vwr::SceneGraph* sceneGraph;
	GraphIO::IOManager* ioManager;
	Model::Clusterer* clusterer;
	QString currentFile;
	ProgressManager* pm;

	void createActions();
	void createMenus();
	void createToolBars();

	void showStatusMsg(StatusMsgType type, QString msg);
	void createStatusBar();

	static CoreWindow* instanceForStatusLog;
	QLabel* algStatus;
	QLabel* pickStatus;
	QLabel* keyStatus;
	QLabel* mainStatus;

	QProgressDialog* dialog;
};

}

#endif
