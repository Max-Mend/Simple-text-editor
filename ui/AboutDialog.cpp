#include "AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("About Chora Spatium");
    resize(400, 200);
    setupUI();
}

AboutDialog::~AboutDialog() = default;

void AboutDialog::setupUI() {
    QLabel *label = new QLabel(
        "Chora Spatium\n\n"
        "v1.5.0\n"
        "Author: Max-Mend\n\n"
        "A lightweight code editor built with Qt", 
        this
    );
    label->setAlignment(Qt::AlignCenter);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
}