#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QStatusBar>
#include <QTabWidget>
#include <QTreeView>
#include <QSplitter>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialog>
#include <QDir>
#include <QDebug>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QPainter>
#include <QFileSystemModel>

#include "core/codeeditor.h"

class CodeEditor;

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
        CodeEditor *editor = new CodeEditor;
        editor->setFont(font);
        editor->setPlainText(content);
        editor->setProperty("filePath", filePath);

        editor->setLineWrapMode(QPlainTextEdit::NoWrap);

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

    editMenu->addAction("Run");

    editMenu->addSeparator();

    QAction *saveFileAction = editMenu->addAction("&Save File");
    QObject::connect(saveFileAction, &QAction::triggered, [&](){
        CodeEditor *currentEditor = dynamic_cast<CodeEditor*>(tabs->currentWidget());
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
                CodeEditor *currentEditor = dynamic_cast<CodeEditor*>(tabs->currentWidget());
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

    QAction *preferencesAction = menuBar->addAction("&Settings");
    QObject::connect(preferencesAction, &QAction::triggered, [&]() {
        QDialog settingsDialog(&mainWindow);
        settingsDialog.setWindowTitle("Settings");
        settingsDialog.resize(600, 400);

        QVBoxLayout *mainLayout = new QVBoxLayout(&settingsDialog);

        QGroupBox *interfaceGroup = new QGroupBox("Interface", &settingsDialog);
        QVBoxLayout *interfaceLayout = new QVBoxLayout(interfaceGroup);

        QCheckBox *treeViewCheckBox = new QCheckBox("Show File Tree", interfaceGroup);
        treeViewCheckBox->setChecked(!tree->isHidden());
        interfaceLayout->addWidget(treeViewCheckBox);

        CodeEditor *currentEditor = dynamic_cast<CodeEditor*>(tabs->currentWidget());

        QCheckBox *lineNumbersCheckBox = new QCheckBox("Show Line Numbers", interfaceGroup);
        lineNumbersCheckBox->setChecked(currentEditor ? currentEditor->areLineNumbersVisible() : true);
        interfaceLayout->addWidget(lineNumbersCheckBox);

        QCheckBox *statusBarCheckBox = new QCheckBox("Show Status Bar", interfaceGroup);
        statusBarCheckBox->setChecked(!statusBar->isHidden());
        interfaceLayout->addWidget(statusBarCheckBox);

        mainLayout->addWidget(interfaceGroup);

        QGroupBox *editorGroup = new QGroupBox("Editor", &settingsDialog);
        QVBoxLayout *editorLayout = new QVBoxLayout(editorGroup);

        QHBoxLayout *fontLayout = new QHBoxLayout();
        fontLayout->addWidget(new QLabel("Font Size:", editorGroup));
        QSpinBox *fontSizeSpinBox = new QSpinBox(editorGroup);
        fontSizeSpinBox->setRange(8, 24);
        fontSizeSpinBox->setValue(currentEditor ? currentEditor->font().pointSize() : 14);
        fontLayout->addWidget(fontSizeSpinBox);
        fontLayout->addStretch();
        editorLayout->addLayout(fontLayout);

        QCheckBox *wordWrapCheckBox = new QCheckBox("Word Wrap", editorGroup);
        wordWrapCheckBox->setChecked(currentEditor ? currentEditor->lineWrapMode() != QPlainTextEdit::NoWrap : false);
        editorLayout->addWidget(wordWrapCheckBox);

        mainLayout->addWidget(editorGroup);

        mainLayout->addStretch();

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addStretch();

        QPushButton *okButton = new QPushButton("OK", &settingsDialog);
        QPushButton *cancelButton = new QPushButton("Cancel", &settingsDialog);
        QPushButton *applyButton = new QPushButton("Apply", &settingsDialog);

        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addWidget(applyButton);

        mainLayout->addLayout(buttonLayout);

        auto applySettings = [&]() {
            if (treeViewCheckBox->isChecked()) {
                tree->show();
            } else {
                tree->hide();
            }

            if (statusBarCheckBox->isChecked()) {
                statusBar->show();
            } else {
                statusBar->hide();
            }

            for (int i = 0; i < tabs->count(); ++i) {
                CodeEditor *editor = dynamic_cast<CodeEditor*>(tabs->widget(i));
                if (editor) {
                    editor->setLineNumbersVisible(lineNumbersCheckBox->isChecked());

                    QFont editorFont = editor->font();
                    editorFont.setPointSize(fontSizeSpinBox->value());
                    editor->setFont(editorFont);

                    if (wordWrapCheckBox->isChecked()) {
                        editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
                    } else {
                        editor->setLineWrapMode(QPlainTextEdit::NoWrap);
                    }
                }
            }
        };

        QObject::connect(okButton, &QPushButton::clicked, [&]() {
            applySettings();
            settingsDialog.accept();
        });

        QObject::connect(cancelButton, &QPushButton::clicked, &settingsDialog, &QDialog::reject);

        QObject::connect(applyButton, &QPushButton::clicked, applySettings);

        settingsDialog.exec();
    });

    QMenu *helpMenu = menuBar->addMenu("&Info");
    QAction *aboutAction = helpMenu->addAction("&About");
    QObject::connect(aboutAction, &QAction::triggered, [&]() {
        QDialog aboutDialog(&mainWindow);
        aboutDialog.setWindowTitle("About Chora");
        aboutDialog.resize(400, 200);

        QLabel *label = new QLabel("Chora Text Editor\n\nv1.4.0\nAuthor: Max-Mend\n\nA lightweight code editor built with Qt", &aboutDialog);
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
    mainWindow.setWindowIcon(QIcon("../assets/logo.svg"));
    mainWindow.resize(1200, 800);
    mainWindow.show();

    return QApplication::exec();
}