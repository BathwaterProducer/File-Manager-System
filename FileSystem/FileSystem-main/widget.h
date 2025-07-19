#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QIcon>
#include <QMap>
#include <QMenu>
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMimeData>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QPainter>
#include <QMessageBox>
#include <QInputDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void on_treeView_clicked(const QModelIndex &index);
    void on_treeView_customContextMenuRequested(const QPoint &pos);
    void new_project();
    void new_file();
    void delete_project();
    void delete_file();
    void copy_file();
    void paste_file();
    QString show_path();
    void searchFile();
    void gotoNextResult();
    void gotoPrevResult();
    void rename_item();
    void openFile(const QModelIndex &index);
    void on_treeView_doubleClicked(const QModelIndex &index);

private:
    Ui::Widget *ui;
    QStandardItem *copiedItem = nullptr;
    QMap<QString, QIcon> m_publicIconMap;
    QStandardItem* deepCopyItem(QStandardItem* item);
    void startDrag(QPoint pos);
    QModelIndex findItemByName(QStandardItem* parent, const QString& name);

    void initModel();
    void saveToJson(const QString &filename);
    bool loadFromJson(const QString &filename);
    QJsonObject saveItem(QStandardItem *item);
    QList<QStandardItem*> loadItem(const QJsonObject &obj);
    bool isDropTargetValid(const QModelIndex &index);
    bool hasDuplicateName(QStandardItem *parent, const QString &name);
    QList<QModelIndex> searchResults;
    int currentResultIndex = -1;
    void focusOnCurrentResult();
    void collectMatchingItems(QStandardItem* parent, const QString& keyword);
    void new_file_with_type(const QString& suffix);
};
#endif // WIDGET_H


