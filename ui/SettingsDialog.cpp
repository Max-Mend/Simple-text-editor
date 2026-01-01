#include "SettingsDialog.h"
#include "MainWindow.h"
#include "../core/codeeditor.h"
#include <QObject>

#include "TerminalWidget.h"

SettingsDialog::SettingsDialog(QWidget *parent, QTabWidget *tabs,
                               QTreeView *tree, QStatusBar *statusBar)
    : QDialog(parent), tabWidget(tabs), treeView(tree), statusBar(statusBar)
{
    mainWindow = qobject_cast<MainWindow*>(parent);

    setWindowTitle("Settings");
    resize(600, 450);
    setupUI();
}

SettingsDialog::~SettingsDialog() = default;

void SettingsDialog::setupUI() {
    mainLayout = new QVBoxLayout(this);

    setupInterfaceGroup();
    setupEditorGroup();

    mainLayout->addStretch();
    setupButtons();
}

void SettingsDialog::setupInterfaceGroup() {
    interfaceGroup = new QGroupBox("Interface", this);
    QVBoxLayout *interfaceLayout = new QVBoxLayout(interfaceGroup);

    treeViewCheckBox = new QCheckBox("Show File Tree", interfaceGroup);
    treeViewCheckBox->setChecked(!treeView->isHidden());
    interfaceLayout->addWidget(treeViewCheckBox);

    CodeEditor *currentEditor = dynamic_cast<CodeEditor*>(tabWidget->currentWidget());

    lineNumbersCheckBox = new QCheckBox("Show Line Numbers", interfaceGroup);
    lineNumbersCheckBox->setChecked(currentEditor ? currentEditor->areLineNumbersVisible() : true);
    interfaceLayout->addWidget(lineNumbersCheckBox);

    statusBarCheckBox = new QCheckBox("Show Status Bar", interfaceGroup);
    statusBarCheckBox->setChecked(!statusBar->isHidden());
    interfaceLayout->addWidget(statusBarCheckBox);

    terminalCheckBox = new QCheckBox("Show Terminal", interfaceGroup);
    if (mainWindow && mainWindow->terminal) {
        terminalCheckBox->setChecked(!mainWindow->terminal->isHidden());
    } else {
        terminalCheckBox->setChecked(true);
    }
    interfaceLayout->addWidget(terminalCheckBox);

    mainLayout->addWidget(interfaceGroup);
}

void SettingsDialog::setupEditorGroup() {
    editorGroup = new QGroupBox("Editor", this);
    QVBoxLayout *editorLayout = new QVBoxLayout(editorGroup);

    QHBoxLayout *fontLayout = new QHBoxLayout();
    fontLayout->addWidget(new QLabel("Font Size:", editorGroup));

    fontSizeSpinBox = new QSpinBox(editorGroup);
    fontSizeSpinBox->setRange(8, 24);

    CodeEditor *currentEditor = dynamic_cast<CodeEditor*>(tabWidget->currentWidget());
    fontSizeSpinBox->setValue(currentEditor ? currentEditor->font().pointSize() : 14);

    fontLayout->addWidget(fontSizeSpinBox);
    fontLayout->addStretch();
    editorLayout->addLayout(fontLayout);

    wordWrapCheckBox = new QCheckBox("Word Wrap", editorGroup);
    wordWrapCheckBox->setChecked(currentEditor ?
        currentEditor->lineWrapMode() != QPlainTextEdit::NoWrap : false);
    editorLayout->addWidget(wordWrapCheckBox);

    autoSaveCheckBox = new QCheckBox("Auto Save", editorGroup);
    if (mainWindow) {
        autoSaveCheckBox->setChecked(mainWindow->autoSaveEnabled);
    }
    editorLayout->addWidget(autoSaveCheckBox);

    QHBoxLayout *autoSaveLayout = new QHBoxLayout();
    autoSaveLayout->addWidget(new QLabel("Auto Save Interval (seconds):", editorGroup));

    autoSaveIntervalSpinBox = new QSpinBox(editorGroup);
    autoSaveIntervalSpinBox->setRange(1, 60);
    if (mainWindow) {
        autoSaveIntervalSpinBox->setValue(mainWindow->autoSaveInterval);
    } else {
        autoSaveIntervalSpinBox->setValue(3);
    }

    autoSaveLayout->addWidget(autoSaveIntervalSpinBox);
    autoSaveLayout->addStretch();
    editorLayout->addLayout(autoSaveLayout);

    mainLayout->addWidget(editorGroup);
}

void SettingsDialog::setupButtons() {
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);
    applyButton = new QPushButton("Apply", this);

    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(applyButton);

    mainLayout->addLayout(buttonLayout);

    QObject::connect(okButton, &QPushButton::clicked, [this]() {
        applySettings();
        accept();
    });

    QObject::connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    QObject::connect(applyButton, &QPushButton::clicked, this, &SettingsDialog::applySettings);
}

void SettingsDialog::applySettings() {
    if (treeViewCheckBox->isChecked()) {
        treeView->show();
    } else {
        treeView->hide();
    }

    if (statusBarCheckBox->isChecked()) {
        statusBar->show();
    } else {
        statusBar->hide();
    }

    if (mainWindow && mainWindow->terminal) {
        if (terminalCheckBox->isChecked()) {
            mainWindow->terminal->show();
        } else {
            mainWindow->terminal->hide();
        }
    }

    if (mainWindow) {
        mainWindow->autoSaveEnabled = autoSaveCheckBox->isChecked();
        mainWindow->autoSaveInterval = autoSaveIntervalSpinBox->value();
    }

    for (int i = 0; i < tabWidget->count(); ++i) {
        CodeEditor *editor = dynamic_cast<CodeEditor*>(tabWidget->widget(i));
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
}