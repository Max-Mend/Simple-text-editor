#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QStatusBar>
#include <QTabWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QSplitter>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialog>
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMainWindow mainWindow;

    QFileSystemModel *model = new QFileSystemModel(&mainWindow);
    model->setRootPath(QDir::homePath());

    QTreeView *tree = new QTreeView;
    tree->setModel(model);
    tree->setRootIndex(model->index(QDir::homePath()));
    tree->setColumnHidden(1, true);
    tree->setColumnHidden(2, true);
    tree->setColumnHidden(3, true);
    tree->setHeaderHidden(true);
    tree->setMaximumWidth(300);
    tree->setMinimumWidth(150);


    QTabWidget *tabs = new QTabWidget;
    tabs->setTabsClosable(true);
    tabs->setMovable(true);
    tabs->setTabPosition(QTabWidget::North);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(tree);
    splitter->addWidget(tabs);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 4);

    mainWindow.setCentralWidget(splitter);

    QObject::connect(tabs, &QTabWidget::tabCloseRequested, [&](int index) {
        QWidget *page = tabs->widget(index);
        tabs->removeTab(index);
        delete page;
    });

    QFont font("JetBrains Mono", 14);
    font.setFixedPitch(true);

    QFont fontMenu("JetBrains Mono", 11);
    fontMenu.setFixedPitch(true);

    auto createEditorTab = [&](const QString &title, const QString &content = "", const QString &filePath = "") {
        QPlainTextEdit *editor = new QPlainTextEdit;
        editor->setFont(font);
        editor->setPlainText(content);
        editor->setProperty("filePath", filePath);
        int index = tabs->addTab(editor, title);
        tabs->setCurrentIndex(index);
        return editor;
    };

    createEditorTab("Untitled");

    QMenuBar *menuBar = mainWindow.menuBar();
    menuBar->setFont(fontMenu);

    QStatusBar *statusBar = new QStatusBar();
    mainWindow.setStatusBar(statusBar);

    QMenu *fileMenu = menuBar->addMenu("&File");

    QAction *newFile = fileMenu->addAction("&New File");
    QObject::connect(newFile, &QAction::triggered, [&]() {
        createEditorTab("Untitled");
    });

    QAction *openFileAction = fileMenu->addAction("&Open File");
    QObject::connect(openFileAction, &QAction::triggered, [&](){
        QString fileName = QFileDialog::getOpenFileName(&mainWindow, "Open File", "", "All Files (*.*)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QFileInfo fileInfo(fileName);
                QString tabName = fileInfo.fileName();
                createEditorTab(tabName, file.readAll(), fileName);
                statusBar->showMessage("Opened: " + fileName, 5000);
            } else {
                statusBar->showMessage("Failed to open file!", 5000);
            }
        }
    });

    QAction *openFolderAction = fileMenu->addAction("&Open Folder");
    QObject::connect(openFolderAction, &QAction::triggered, [&](){
        QString folderName = QFileDialog::getExistingDirectory(&mainWindow, "Open Folder", "");
        if (!folderName.isEmpty()) {
            model->setRootPath(folderName);
            tree->setRootIndex(model->index(folderName));
            statusBar->showMessage("Opened folder: " + folderName, 5000);
        }
    });

    fileMenu->addSeparator();

    QAction *exitAction = fileMenu->addAction("E&xit");
    QObject::connect(exitAction, &QAction::triggered, &QApplication::quit);

    QMenu *editMenu = menuBar->addMenu("&Edit");

    QAction *saveFileAction = editMenu->addAction("&Save File");
    QObject::connect(saveFileAction, &QAction::triggered, [&](){
        QPlainTextEdit *currentEditor = qobject_cast<QPlainTextEdit*>(tabs->currentWidget());
        if (!currentEditor) return;

        QString filePath = currentEditor->property("filePath").toString();

        if (filePath.isEmpty()) {
            filePath = QFileDialog::getSaveFileName(&mainWindow, "Save File", "", "All Files (*.*)");
        }

        if (!filePath.isEmpty()) {
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                file.write(currentEditor->toPlainText().toUtf8());
                file.close();
                QFileInfo fileInfo(filePath);
                tabs->setTabText(tabs->currentIndex(), fileInfo.fileName());
                currentEditor->setProperty("filePath", filePath);
                statusBar->showMessage("Saved: " + filePath, 5000);
            } else {
                statusBar->showMessage("Failed to save file!", 5000);
            }
        }
    });

    QAction *saveAsAction = editMenu->addAction("Save &As...");
    QObject::connect(saveAsAction, &QAction::triggered, [&](){
        QString fileName = QFileDialog::getSaveFileName(&mainWindow, "Save File As", "", "All Files (*.*)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QPlainTextEdit *currentEditor = qobject_cast<QPlainTextEdit*>(tabs->currentWidget());
                if (currentEditor) {
                    file.write(currentEditor->toPlainText().toUtf8());
                    file.close();
                    QFileInfo fileInfo(fileName);
                    tabs->setTabText(tabs->currentIndex(), fileInfo.fileName());
                    currentEditor->setProperty("filePath", fileName);
                    statusBar->showMessage("Saved as: " + fileName, 5000);
                }
            } else {
                statusBar->showMessage("Failed to save file!", 5000);
            }
        }
    });

    QMenu *pluginsMenu = menuBar->addMenu("&Plugins");
    pluginsMenu->addAction("&Create plugin");

    QAction *openPlugin = pluginsMenu->addAction("&Open plugin");
    QObject::connect(openPlugin, &QAction::triggered, [&]() {
        QString fileName = QFileDialog::getOpenFileName(nullptr, "Open Plugin", "", "*.py");
        if (!fileName.isEmpty()) {
            qDebug() << "Opened plugin:" << fileName;
            statusBar->showMessage("Plugin functionality not yet implemented", 5000);
        }
    });

    QMenu *helpMenu = menuBar->addMenu("&Info");
    QAction *aboutAction = helpMenu->addAction("&About");
    QObject::connect(aboutAction, &QAction::triggered, [&]() {
        QDialog aboutDialog(&mainWindow);
        aboutDialog.setWindowTitle("About Chora");
        aboutDialog.resize(400, 200);

        QLabel *label = new QLabel("Chora Text Editor\n\nv1.2.0\nAuthor: Max-Mend\n\nA lightweight code editor built with Qt", &aboutDialog);
        label->setAlignment(Qt::AlignCenter);

        QVBoxLayout *layout = new QVBoxLayout(&aboutDialog);
        layout->addWidget(label);

        aboutDialog.exec();
    });

    QObject::connect(tree, &QTreeView::doubleClicked, [&](const QModelIndex &index) {
        QString filePath = model->filePath(index);
        QFileInfo fileInfo(filePath);

        if (fileInfo.isFile()) {
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                createEditorTab(fileInfo.fileName(), file.readAll(), filePath);
                statusBar->showMessage("Opened: " + filePath, 5000);
            }
        }
    });

    mainWindow.setWindowTitle("Chora");
    mainWindow.resize(1200, 800);
    mainWindow.show();

    return QApplication::exec();
}