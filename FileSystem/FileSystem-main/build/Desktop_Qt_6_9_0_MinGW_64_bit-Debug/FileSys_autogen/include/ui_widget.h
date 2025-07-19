/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QTreeView *treeView;
    QLabel *label;
    QLabel *name;
    QLabel *copyName;
    QLineEdit *searchEdit;
    QPushButton *searchButton;
    QPushButton *prevButton;
    QPushButton *nextButton;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(639, 600);
        treeView = new QTreeView(Widget);
        treeView->setObjectName("treeView");
        treeView->setGeometry(QRect(40, 120, 561, 441));
        label = new QLabel(Widget);
        label->setObjectName("label");
        label->setGeometry(QRect(100, 10, 511, 21));
        name = new QLabel(Widget);
        name->setObjectName("name");
        name->setGeometry(QRect(40, 10, 71, 20));
        copyName = new QLabel(Widget);
        copyName->setObjectName("copyName");
        copyName->setGeometry(QRect(80, 40, 81, 20));
        searchEdit = new QLineEdit(Widget);
        searchEdit->setObjectName("searchEdit");
        searchEdit->setGeometry(QRect(150, 70, 241, 31));
        searchButton = new QPushButton(Widget);
        searchButton->setObjectName("searchButton");
        searchButton->setGeometry(QRect(410, 70, 61, 31));
        prevButton = new QPushButton(Widget);
        prevButton->setObjectName("prevButton");
        prevButton->setGeometry(QRect(510, 50, 61, 24));
        nextButton = new QPushButton(Widget);
        nextButton->setObjectName("nextButton");
        nextButton->setGeometry(QRect(510, 90, 61, 24));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        label->setText(QCoreApplication::translate("Widget", "\346\210\221\347\232\204\347\224\265\350\204\221", nullptr));
        name->setText(QCoreApplication::translate("Widget", "\345\275\223\345\211\215\350\267\257\345\276\204\357\274\232", nullptr));
        copyName->setText(QCoreApplication::translate("Widget", "\345\267\262\345\244\215\345\210\266\346\226\207\344\273\266", nullptr));
        searchButton->setText(QCoreApplication::translate("Widget", "\346\220\234\347\264\242", nullptr));
#if QT_CONFIG(shortcut)
        searchButton->setShortcut(QCoreApplication::translate("Widget", "Return", nullptr));
#endif // QT_CONFIG(shortcut)
        prevButton->setText(QCoreApplication::translate("Widget", "\344\270\212\344\270\200\344\270\252", nullptr));
#if QT_CONFIG(shortcut)
        prevButton->setShortcut(QCoreApplication::translate("Widget", "Up", nullptr));
#endif // QT_CONFIG(shortcut)
        nextButton->setText(QCoreApplication::translate("Widget", "\344\270\213\344\270\200\344\270\252", nullptr));
#if QT_CONFIG(shortcut)
        nextButton->setShortcut(QCoreApplication::translate("Widget", "Down", nullptr));
#endif // QT_CONFIG(shortcut)
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
