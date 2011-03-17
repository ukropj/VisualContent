/**
 *  CoreWindow.h
 *  Projekt 3DVisual
 */
#ifndef Window_CORE_WINDOW
#define Window_CORE_WINDOW 1

#include <QMainWindow>
#include <QtGui>
#include <QDebug>

namespace AppCore {
class IOManager;
}
namespace Model {
class Graph;
class FRAlgorithm;
}
namespace Vwr {
class SceneGraph;
}

namespace Window {
class ViewerQT;

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
		return layouter;
	}
	static void log(StatusMsgType type, QString msg);
signals:
	void windowResized();
private slots:
	void openFile();
	void setDataMapping();
	void openRecentFile();
	void showOptions();
	void closeEvent(QCloseEvent *event);
	void resizeEvent(QResizeEvent *event);

	void centerView();
	void randomize();
	void toggleLabels(bool checked);
	void toggleLayouting(bool checked);
	void toggleFixNodes();
	void captureScreen();
	void sliderValueChanged(int value);

	void toggleDebug();
	void showMessageBox(QString title, QString message, bool isError);
private:
	void loadFile(QString fileName);
	void readSettings();
	void writeSettings();
	void updateRecentFileActions(QString fileName = 0);

	enum { MaxRecentFiles = 10 };

	QAction* loadAction;
	QAction* remapAction;
	QAction* quitAction;
	QAction* optionsAction;
	QAction* separatorAction;
	QAction* recentFileActions[MaxRecentFiles];

	QAction* playAction;
	QAction* fixAction;
	QAction* randomizeAction;
	QAction* centerAction;
	QAction* labelsAction;
	QAction* captureAction;
	QSlider* slider;
	QAction* debugAction;

	QMenu* fileMenu;
	QMenu* editMenu;
	QToolBar* toolBar;

	Window::ViewerQT* viewerWidget;
	Model::FRAlgorithm* layouter;
	Vwr::SceneGraph* sceneGraph;
	AppCore::IOManager *ioManager;
	QString currentFile;

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

	QProgressDialog* progressBar;
};

}

#endif
