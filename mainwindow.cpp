#include <QStandardPaths>
#include <QFileDialog>
#include <QTextStream>
#include <QStringConverter>
#include <QFile>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <memory>
#include <iterator>
#include <algorithm>
#include <array>
#include <string>

using std::unique_ptr;
using std::make_unique;
using std::find;
using std::find_if;
using std::copy_if;
using std::back_inserter;
using std::distance;
using std::string;
using std::to_string;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    for (auto widget: {ui->textEdit, ui->mushEdit}) {
        QFont font = widget->document()->defaultFont();
        font.setFamily("Courier New");
        widget->document()->setDefaultFont(font);
    }
    connect(ui->textEdit, &QPlainTextEdit::textChanged, this, &MainWindow::textUpdated);
    connect(ui->actionQuit, &QAction::triggered, []() { QApplication::quit(); });
    connect(ui->actionLoad, &QAction::triggered, this, &MainWindow::loadText);
    connect(ui->actionSave_MUSHtext, &QAction::triggered, this, &MainWindow::saveText);
}

void MainWindow::saveText()
{
    auto contents = ui->mushEdit->document()->toPlainText();
    if (contents.size() <= 0) return;

    auto docpath = QStandardPaths::displayName(QStandardPaths::DocumentsLocation);
    auto filename = QFileDialog::getSaveFileName(this,
                                                 "Save File",
                                                 docpath,
                                                 "Text files (*.txt *.asc)");
    if (filename.size() <= 0) return;

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << ui->mushEdit->document()->toPlainText();
    file.close();
}

void MainWindow::loadText()
{
    auto docpath = QStandardPaths::displayName(QStandardPaths::DocumentsLocation);
    auto filename = QFileDialog::getOpenFileName(this,
                                                 "Open File",
                                                 docpath,
                                                 "Text files (*.txt *.asc)");
    if (filename.size() <= 0) return;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    auto data = file.readAll().toStdString();
    file.close();

    string contents;
    copy_if(data.cbegin(), data.cend(), back_inserter(contents),
            [](char ch) {
       return (ch == 10 || ch == 13 || (ch >= 32 && ch < 127));
    });
    ui->textEdit->document()->clear();
    auto cursor = make_unique<QTextCursor>(ui->textEdit->document());
    cursor->insertText(contents.c_str());
    textUpdated();
}

void MainWindow::textUpdated()
{
    static long int dist;
    static string::iterator end;

    ui->mushEdit->document()->clear();
    auto origtext = ui->textEdit->document()->toPlainText().toStdString();
    string newtext = "";
    auto iter = origtext.begin();

    while (iter < origtext.end()) {
        switch (*iter) {
        case '\\':
            newtext += "\\\\";
            break;
        case '%':
            newtext += "%%";
            break;
        case '\n':
            newtext += "%r";
            break;
        case '\t':
            newtext += "%t";
            break;
        case ' ':
            end = find_if(iter + 1, origtext.end(),
                          [](char ch) { return ch != ' ';  });
            dist = distance(iter, end);
            if (dist == 1) newtext += " ";
            else newtext += "[space(" + to_string(dist) + ")]";
            iter = end - 1;
            break;
        default:
            newtext += *iter;
            break;
        }
        ++iter;
    }
    if (newtext.cend() != find_if(newtext.cbegin(), newtext.cend(),
                                  [](char ch) { return (ch < 32 || ch >= 127); }))
        newtext = "Input contains non-ASCII text.";
    auto cursor = make_unique<QTextCursor>(ui->mushEdit->document());
    cursor->insertText(newtext.c_str());
}

MainWindow::~MainWindow()
{
    delete ui;
}
