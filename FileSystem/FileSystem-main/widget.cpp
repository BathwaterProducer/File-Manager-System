#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->copyName->setVisible(false);
    connect(ui->searchButton, &QPushButton::clicked, this, &Widget::searchFile);
    connect(ui->nextButton, &QPushButton::clicked, this, &Widget::gotoNextResult);
    connect(ui->prevButton, &QPushButton::clicked, this, &Widget::gotoPrevResult);

    // 启用拖放
    ui->treeView->setDragEnabled(true);
    ui->treeView->setAcceptDrops(true);
    ui->treeView->setDropIndicatorShown(true);
    ui->treeView->setDragDropMode(QAbstractItemView::InternalMove);
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    // 图标映射初始化，添加校验
    QStringList iconKeys = {"treeItem_Computer", "treeItem_Disk", "treeItem_Project",
                            "treeItem_Unknownfile", "treeItem_txt", "treeItem_gif",
                            "treeItem_pdf", "treeItem_png", "treeItem_doc",
                            "treeItem_ppt", "treeItem_zip", "treeItem_xls"};
    QStringList iconPaths = {":/Icon/Image/Computer.png", ":/Icon/Image/Disk.png",
                            ":/Icon/Image/Project.png", ":/Icon/Image/Unknownfile.png", ":/Icon/Image/txt.png",
                            ":/Icon/Image/gif.png",
                            ":/Icon/Image/pdf.png", ":/Icon/Image/png.png", ":/Icon/Image/doc.png",
                            ":/Icon/Image/ppt.png", ":/Icon/Image/zip.png", ":/Icon/Image/xls.png"};
    for (int i = 0; i < iconKeys.size(); ++i) {
        QIcon icon(iconPaths[i]);
        if (icon.isNull()) {
            qWarning("Failed to load icon: %s", qPrintable(iconPaths[i]));
        }
        m_publicIconMap[iconKeys[i]] = icon;
    }

    // 加载文件系统或初始化模型
    if (!QFile::exists("filesystem.json") || !loadFromJson("filesystem.json")) {
        initModel();
    }

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
}

Widget::~Widget()
{
    saveToJson("filesystem.json");
    delete ui;
    delete ui->treeView->model();  // 释放旧模型
}

QStandardItem* Widget::deepCopyItem(QStandardItem* item)
{
    QStandardItem* newItem = item->clone();
    for (int i = 0; i < item->rowCount(); ++i) {
        QList<QStandardItem*> row;
        for (int j = 0; j < item->columnCount(); ++j) {
            row.append(deepCopyItem(item->child(i, j)));
        }
        newItem->appendRow(row);
    }
    return newItem;
}

void Widget::copy_file()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui->treeView->model());
    copiedItem = deepCopyItem(model->itemFromIndex(currentIndex));
    ui->copyName->setText(model->itemFromIndex(currentIndex)->text());
}

void Widget::paste_file()
{
    if (!copiedItem) return;

    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui->treeView->model());
    QModelIndex currentIndex = ui->treeView->currentIndex();

    if (!isDropTargetValid(currentIndex)) {
        QMessageBox::warning(this, "无效操作", "只能粘贴到文件夹中！");
        return;
    }

    QStandardItem *currentItem = model->itemFromIndex(currentIndex);
    QString name = copiedItem->text();

    if (hasDuplicateName(currentItem, name)) {
        QMessageBox::warning(this, "命名冲突", "当前文件夹下已存在同名文件或文件夹！");
        return;
    }

    QStandardItem *newItem = deepCopyItem(copiedItem);
    QString newType = newItem->data(Qt::UserRole + 1).toString();
    QStandardItem *newTypeItem = new QStandardItem(newType);
    newTypeItem->setData(newType, Qt::UserRole + 1);
    currentItem->appendRow({newItem, newTypeItem});
    ui->treeView->setCurrentIndex(newItem->index());
}

void Widget::delete_project()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    if (!currentIndex.parent().isValid()) return;  // 避免删除根节点
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->treeView->model());
    QStandardItem* currentItem = model->itemFromIndex(currentIndex.parent());
    currentItem->removeRow(currentIndex.row());
}

void Widget::delete_file()
{
    delete_project();
}

QJsonObject Widget::saveItem(QStandardItem *item)
{
    QJsonObject obj;
    obj["name"] = item->text();
    obj["type"] = item->data(Qt::UserRole + 1).toString();

    // 保存路径（用于文件双击打开）
    QString path = item->data(Qt::UserRole + 2).toString();
    if (!path.isEmpty()) {
        obj["path"] = path;
    }

    // 保存图标 key
    for (auto it = m_publicIconMap.constBegin(); it != m_publicIconMap.constEnd(); ++it) {
        if (it.value().cacheKey() == item->icon().cacheKey()) {
            obj["icon"] = it.key();
            break;
        }
    }

    // 保存子项（仅第0列递归）
    if (item->rowCount() > 0) {
        QJsonArray children;
        for (int i = 0; i < item->rowCount(); ++i) {
            children.append(saveItem(item->child(i, 0)));
        }
        obj["children"] = children;
    }

    return obj;
}

QList<QStandardItem*> Widget::loadItem(const QJsonObject &obj)
{
    QString name = obj["name"].toString();
    QString type = obj["type"].toString();
    QString iconName = obj["icon"].toString();
    QString path = obj["path"].toString();  // 从 JSON 中读取路径字段

    QStandardItem *item = new QStandardItem(name);
    item->setData(type, Qt::UserRole + 1);

    // 恢复路径数据（用于双击打开）
    if (!path.isEmpty()) {
        item->setData(path, Qt::UserRole + 2);
    }

    if (!iconName.isEmpty() && m_publicIconMap.contains(iconName)) {
        item->setIcon(m_publicIconMap[iconName]);
    }

    QStandardItem *typeItem = new QStandardItem(type);
    typeItem->setData(type, Qt::UserRole + 1);

    // 加载子项（仅第0列递归）
    if (obj.contains("children")) {
        QJsonArray children = obj["children"].toArray();
        for (const QJsonValue &child : children) {
            QList<QStandardItem*> childRow = loadItem(child.toObject());
            item->appendRow(childRow);
        }
    }

    return { item, typeItem };
}

void Widget::saveToJson(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QJsonObject root;
    QJsonArray items;
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui->treeView->model());

    for (int i = 0; i < model->rowCount(); ++i) {
        items.append(saveItem(model->item(i, 0)));
    }

    root["items"] = items;
    file.write(QJsonDocument(root).toJson());
    file.close();
}

bool Widget::loadFromJson(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) return false;

    delete ui->treeView->model();
    QStandardItemModel *model = new QStandardItemModel(ui->treeView);
    model->setHorizontalHeaderLabels(QStringList() << "名称" << "类型");

    QJsonArray items = doc.object()["items"].toArray();
    for (const QJsonValue &val : items) {
        QList<QStandardItem*> rowItems = loadItem(val.toObject());
        model->appendRow(rowItems);
    }

    ui->treeView->setModel(model);
    ui->treeView->header()->resizeSection(0, 300);
    return true;
}


void Widget::initModel()
{
    QStandardItemModel* model = new QStandardItemModel(ui->treeView);
    model->setHorizontalHeaderLabels(QStringList() << "名称" << "类型");

    QVector<QString> myDisks = {"C盘", "D盘", "E盘"};

    QStandardItem* myComputer = new QStandardItem(m_publicIconMap["treeItem_Computer"], "我的电脑");
    myComputer->setData("system", Qt::UserRole + 1);
    QStandardItem* myComputerType = new QStandardItem("system");
    myComputerType->setData("system", Qt::UserRole + 1);
    model->appendRow({myComputer, myComputerType});

    for (int i = 0; i < myDisks.size(); i++) {
        QStandardItem* myDisk = new QStandardItem(m_publicIconMap["treeItem_Disk"], myDisks[i]);
        myDisk->setData("驱动器", Qt::UserRole + 1);
        QStandardItem* myDiskType = new QStandardItem("驱动器");
        myDiskType->setData("驱动器", Qt::UserRole + 1);
        myComputer->appendRow({myDisk, myDiskType});

        for (int j = 1; j < 3; j++) {
            QStandardItem* myProject = new QStandardItem(m_publicIconMap["treeItem_Project"], "文件夹" + QString::number(j));
            myProject->setData("文件夹", Qt::UserRole + 1);
            QStandardItem* myProjectType = new QStandardItem("文件夹");
            myProjectType->setData("文件夹", Qt::UserRole + 1);
            myDisk->appendRow({myProject, myProjectType});

            QString fileName = "文件.txt";
            QStringList strList = fileName.split(".");
            QString fileType = (strList.size() < 2 || strList[1] != "txt") ? "未知文件" : "txt文件";
            QIcon fileIcon = (fileType == "未知文件") ? m_publicIconMap["treeItem_Unknownfile"] : m_publicIconMap["treeItem_txt"];

            QStandardItem* myFile = new QStandardItem(fileIcon, fileName);
            myFile->setData(fileType, Qt::UserRole + 1);
            QStandardItem* myFileType = new QStandardItem(fileType);
            myFileType->setData(fileType, Qt::UserRole + 1);
            myProject->appendRow({myFile, myFileType});
        }
    }

    ui->treeView->setModel(model);
    ui->treeView->header()->resizeSection(0, 300);
    ui->treeView->update();
}


//鼠标点击事件
void Widget::on_treeView_clicked(const QModelIndex &index)
{
    ui->label->setText(show_path());
}

QString Widget::show_path()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    if (!currentIndex.isValid()) return "";

    QStringList pathList;

    // 只从第0列取名称（第1列是类型）
    QModelIndex index = currentIndex.sibling(currentIndex.row(), 0);
    while (index.isValid()) {
        QString name = ui->treeView->model()->data(index, Qt::DisplayRole).toString();
        pathList.prepend(name);
        index = index.parent();
    }

    QString fullPath = pathList.join(" / ");
    return fullPath;
}


void Widget::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex currentIndex = ui->treeView->indexAt(pos);
    if (!currentIndex.isValid()) return;

    // 直接从模型中获取类型信息
    QVariant typeData = ui->treeView->model()->data(currentIndex.sibling(currentIndex.row(), 1), Qt::DisplayRole);
    QString currentInfo = typeData.isValid() ? typeData.toString() : "";

    if (currentInfo.isEmpty() || currentInfo == "system") {
        return;
    }

    QMenu menu(ui->treeView);

    // 只允许在文件夹中右键创建
    if (currentInfo == "文件夹") {
        menu.addAction("新建文件夹", this, &Widget::new_project);

        // 新建文件的子菜单
        QMenu* newFileMenu = new QMenu("新建文件", &menu);

        newFileMenu->addAction("文本文档 (.txt)", this, [=]() { new_file_with_type("txt"); });
        newFileMenu->addAction("PDF文档 (.pdf)", this, [=]() { new_file_with_type("pdf"); });
        newFileMenu->addAction("图片文件 (.png)", this, [=]() { new_file_with_type("png"); });
        newFileMenu->addAction("Word文档 (.doc)", this, [=]() { new_file_with_type("doc"); });
        newFileMenu->addAction("GIF图片 (.gif)", this, [=]() { new_file_with_type("gif"); });
        newFileMenu->addAction("PPT文档 (.ppt)", this, [=]() { new_file_with_type("ppt"); });
        newFileMenu->addAction("Excel文档 (.xls)", this, [=]() { new_file_with_type("xls"); });
        newFileMenu->addAction("压缩文件 (.zip)", this, [=]() { new_file_with_type("zip"); });

        menu.addMenu(newFileMenu);  // 添加到主菜单中

        menu.addAction("删除", this, &Widget::delete_project);

        QAction* pasteAction = menu.addAction("粘贴", this, &Widget::paste_file);
        menu.addAction("重命名", this, &Widget::rename_item);

        pasteAction->setEnabled(!ui->copyName->text().isEmpty());

    } else {
        menu.addAction("新建文件夹", this, &Widget::new_project);

        QMenu* newFileMenu = new QMenu("新建文件", &menu);
        newFileMenu->addAction("文本文档 (.txt)", this, [=]() { new_file_with_type("txt"); });
        newFileMenu->addAction("PDF文档 (.pdf)", this, [=]() { new_file_with_type("pdf"); });
        newFileMenu->addAction("图片文件 (.png)", this, [=]() { new_file_with_type("png"); });
        newFileMenu->addAction("Word文档 (.doc)", this, [=]() { new_file_with_type("doc"); });
        newFileMenu->addAction("GIF图片 (.gif)", this, [=]() { new_file_with_type("gif"); });
        newFileMenu->addAction("PPT文档 (.ppt)", this, [=]() { new_file_with_type("ppt"); });
        newFileMenu->addAction("Excel文档 (.xls)", this, [=]() { new_file_with_type("xls"); });
        newFileMenu->addAction("压缩文件 (.zip)", this, [=]() { new_file_with_type("zip"); });
        menu.addMenu(newFileMenu);

        menu.addAction("删除", this, &Widget::delete_file);
        menu.addAction("复制", this, &Widget::copy_file);

        QAction* pasteAction = menu.addAction("粘贴", this, &Widget::paste_file);
        menu.addAction("重命名", this, &Widget::rename_item);

        pasteAction->setEnabled(!ui->copyName->text().isEmpty());
    }

    menu.exec(ui->treeView->viewport()->mapToGlobal(pos));

}

void Widget::new_project()
{
    qint64 timestamp = QDateTime::currentSecsSinceEpoch();
    QString time = QString::number(timestamp);
    QString folderName = "新建文件夹" + time;

    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->treeView->model());
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem* currentItem = model->itemFromIndex(currentIndex);

    // 如果当前不是文件夹，添加到其父项
    if (currentItem->data(Qt::UserRole + 1).toString() != "文件夹") {
        currentItem = model->itemFromIndex(currentIndex.parent());
    }

    if (hasDuplicateName(currentItem, folderName)) {
        QMessageBox::warning(this, "命名冲突", "当前文件夹下已存在同名文件夹！");
        return;
    }

    QStandardItem* myProject = new QStandardItem(m_publicIconMap["treeItem_Project"], folderName);
    myProject->setData("文件夹", Qt::UserRole + 1);
    QStandardItem* typeItem = new QStandardItem("文件夹");
    typeItem->setData("文件夹", Qt::UserRole + 1);
    currentItem->appendRow({myProject, typeItem});
    ui->treeView->setCurrentIndex(myProject->index());
}

void Widget::new_file()
{
    qint64 timestamp = QDateTime::currentSecsSinceEpoch();
    QString time = QString::number(timestamp);
    QString fileName = "新建文件" + time + ".txt";  // 默认是 txt，可改为其他扩展名测试

    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->treeView->model());
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem* currentItem = model->itemFromIndex(currentIndex);

    // 如果当前项不是文件夹，则添加到其父项
    if (currentItem->data(Qt::UserRole + 1).toString() != "文件夹") {
        currentItem = model->itemFromIndex(currentIndex.parent());
    }

    if (hasDuplicateName(currentItem, fileName)) {
        QMessageBox::warning(this, "命名冲突", "当前文件夹下已存在同名文件！");
        return;
    }

    // 根据后缀判断文件类型
    QStringList strList = fileName.split(".");
    QString suffix = (strList.size() >= 2) ? strList[1].toLower() : "";
    QString fileType;
    QString iconKey;

    if (suffix == "txt") {
        fileType = "txt文件";
        iconKey = "treeItem_txt";
    } else if (suffix == "pdf") {
        fileType = "pdf文档";
        iconKey = "treeItem_pdf";
    } else if (suffix == "png") {
        fileType = "png文件";
        iconKey = "treeItem_png";
    } else if (suffix == "doc") {
        fileType = "Word文档";
        iconKey = "treeItem_doc";
    } else if (suffix == "gif") {
        fileType = "gif文件";
        iconKey = "treeItem_gif";
    } else if (suffix == "ppt") {
        fileType = "ppt文档";
        iconKey = "treeItem_ppt";
    } else if (suffix == "xls") {
        fileType = "xls文档";
        iconKey = "treeItem_xls";
    } else if (suffix == "zip") {
        fileType = "zip文件";
        iconKey = "treeItem_zip";
    }else {
        fileType = "未知文件";
        iconKey = "treeItem_Unknownfile";
    }

    QIcon fileIcon = m_publicIconMap.value(iconKey, m_publicIconMap["treeItem_Unknownfile"]);

    QStandardItem* myFile = new QStandardItem(fileIcon, fileName);
    myFile->setData(fileType, Qt::UserRole + 1);
    QStandardItem* typeItem = new QStandardItem(fileType);
    typeItem->setData(fileType, Qt::UserRole + 1);
    currentItem->appendRow({myFile, typeItem});
    ui->treeView->setCurrentIndex(myFile->index());
}



void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-qstandarditemmodeldatalist")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void Widget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-qstandarditemmodeldatalist")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void Widget::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasFormat("application/x-qstandarditemmodeldatalist")) {
        event->ignore();
        return;
    }
    QModelIndex dropIndex = ui->treeView->indexAt(event->position().toPoint());
    if (!dropIndex.isValid()) {
        event->ignore();
        return;
    }

    // 判断拖放目标是否为“文件夹”类型
    QString dropType = ui->treeView->model()->data(dropIndex.sibling(dropIndex.row(), 1)).toString();
    if (dropType != "文件夹") {
        // 拖到文件上，拒绝
        event->ignore();
        return;
    }

    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui->treeView->model());
    QStandardItem *dropItem = model->itemFromIndex(dropIndex);

    if (!copiedItem) {
        event->ignore();
        return;
    }

    // 防止把一个项拖入它自己
    QModelIndex dragIndex = ui->treeView->currentIndex();
    if (dragIndex == dropIndex || dragIndex.parent() == dropIndex) {
        event->ignore();
        return;
    }

    // 拷贝拖拽项并插入
    QStandardItem *newItem = deepCopyItem(copiedItem);
    QString newType = newItem->data(Qt::UserRole + 1).toString();
    QStandardItem *newTypeItem = new QStandardItem(newType);
    newTypeItem->setData(newType, Qt::UserRole + 1);

    dropItem->appendRow({newItem, newTypeItem});
    ui->treeView->setCurrentIndex(newItem->index());

    event->acceptProposedAction();
}

void Widget::startDrag(QPoint pos)
{
    QModelIndex index = ui->treeView->indexAt(pos);
    if (!index.isValid()) return;

    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui->treeView->model());
    copiedItem = model->itemFromIndex(index);

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    // 使用自定义数据格式
    mimeData->setData("application/x-qstandarditemmodeldatalist", QByteArray());
    drag->setMimeData(mimeData);

    // 设置拖拽时的预览图像
    QPixmap pixmap(ui->treeView->visualRect(index).size());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, model->itemFromIndex(index)->text());
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));

    drag->exec(Qt::MoveAction);
}

bool Widget::isDropTargetValid(const QModelIndex &index)
{
    if (!index.isValid()) return false;
    QString type = ui->treeView->model()->data(index.sibling(index.row(), 1), Qt::DisplayRole).toString();
    return (type == "文件夹");
}

bool Widget::hasDuplicateName(QStandardItem *parent, const QString &name) {
    if (!parent) return false;
    for (int i = 0; i < parent->rowCount(); ++i) {
        if (parent->child(i, 0)->text() == name) {
            return true;
        }
    }
    return false;
}

void Widget::searchFile()
{
    QString keyword = ui->searchEdit->text().trimmed();
    if (keyword.isEmpty()) return;

    searchResults.clear();
    currentResultIndex = -1;

    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->treeView->model());
    collectMatchingItems(model->invisibleRootItem(), keyword);

    if (searchResults.isEmpty()) {
        QMessageBox::information(this, "未找到", "未找到匹配的文件或文件夹！");
        return;
    }

    currentResultIndex = 0;
    focusOnCurrentResult();
}

QModelIndex Widget::findItemByName(QStandardItem* parent, const QString& name)
{
    for (int i = 0; i < parent->rowCount(); ++i) {
        QStandardItem* child = parent->child(i, 0);  // 第0列是名称
        if (child->text().contains(name, Qt::CaseInsensitive)) {
            return child->index();
        }
        QModelIndex found = findItemByName(child, name);
        if (found.isValid()) {
            return found;
        }
    }
    return QModelIndex();
}

void Widget::collectMatchingItems(QStandardItem* parent, const QString& keyword)
{
    for (int i = 0; i < parent->rowCount(); ++i) {
        QStandardItem* child = parent->child(i, 0);  // 只看第0列
        if (child->text().contains(keyword, Qt::CaseInsensitive)) {
            searchResults.append(child->index());
        }
        collectMatchingItems(child, keyword);  // 递归查找
    }
}

void Widget::focusOnCurrentResult()
{
    if (currentResultIndex < 0 || currentResultIndex >= searchResults.size()) return;
    QModelIndex index = searchResults[currentResultIndex];
    ui->treeView->setCurrentIndex(index);
    ui->treeView->scrollTo(index);
}

void Widget::gotoNextResult()
{
    if (searchResults.isEmpty()) return;
    currentResultIndex = (currentResultIndex + 1) % searchResults.size();  // 循环
    focusOnCurrentResult();
}

void Widget::gotoPrevResult()
{
    if (searchResults.isEmpty()) return;
    currentResultIndex = (currentResultIndex - 1 + searchResults.size()) % searchResults.size();  // 循环
    focusOnCurrentResult();
}


void Widget::new_file_with_type(const QString& suffix)
{
    qint64 timestamp = QDateTime::currentSecsSinceEpoch();
    QString time = QString::number(timestamp);
    QString fileName = "新建文件" + time + "." + suffix;

    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->treeView->model());
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem* currentItem = model->itemFromIndex(currentIndex);

    if (currentItem->data(Qt::UserRole + 1).toString() != "文件夹") {
        currentItem = model->itemFromIndex(currentIndex.parent());
    }

    if (!currentItem) return;

    if (hasDuplicateName(currentItem, fileName)) {
        QMessageBox::warning(this, "命名冲突", "当前文件夹下已存在同名文件！");
        return;
    }

    // 后缀类型识别
    QString fileType;
    QString iconKey;
    QString lowerSuffix = suffix.toLower();

    if (lowerSuffix == "txt") {
        fileType = "txt文件";
        iconKey = "treeItem_txt";
    } else if (lowerSuffix == "pdf") {
        fileType = "pdf文件";
        iconKey = "treeItem_pdf";
    } else if (lowerSuffix == "png") {
        fileType = "png文件";
        iconKey = "treeItem_png";
    } else if (lowerSuffix == "doc") {
        fileType = "doc文档";
        iconKey = "treeItem_doc";
    } else if (lowerSuffix == "gif") {
        fileType = "gif文件";
        iconKey = "treeItem_gif";
    } else if (lowerSuffix == "ppt") {
        fileType = "ppt文档";
        iconKey = "treeItem_ppt";
    } else if (lowerSuffix == "xls") {
        fileType = "xls文档";
        iconKey = "treeItem_xls";
    } else if (lowerSuffix == "zip") {
        fileType = "zip文件";
        iconKey = "treeItem_zip";
    } else {
        fileType = "未知文件";
        iconKey = "treeItem_Unknownfile";
    }

    QIcon fileIcon = m_publicIconMap.value(iconKey, m_publicIconMap["treeItem_Unknownfile"]);

    // 实际文件路径（你可以改路径）
    QString filePath = QDir::currentPath() + "/" + fileName;

    // 如果文件不存在，则创建空文件
    QFile file(filePath);
    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
        }
    }

    // 创建项目节点
    QStandardItem* myFile = new QStandardItem(fileIcon, fileName);
    myFile->setData(fileType, Qt::UserRole + 1);
    myFile->setData(filePath, Qt::UserRole + 2);
    QStandardItem* typeItem = new QStandardItem(fileType);
    typeItem->setData(fileType, Qt::UserRole + 1);
    currentItem->appendRow({myFile, typeItem});
    ui->treeView->setCurrentIndex(myFile->index());
}

void Widget::rename_item()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    if (!currentIndex.isValid()) return;

    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->treeView->model());
    QStandardItem* currentItem = model->itemFromIndex(currentIndex.sibling(currentIndex.row(), 0));  // 只取第0列名称
    QStandardItem* parentItem = model->itemFromIndex(currentIndex.parent());

    QString oldName = currentItem->text();

    // 弹出输入框
    bool ok;
    QString newName = QInputDialog::getText(this, "重命名", "请输入新名称：", QLineEdit::Normal, oldName, &ok);

    if (!ok || newName.trimmed().isEmpty() || newName == oldName)
        return;

    // 校验是否重名
    if (hasDuplicateName(parentItem, newName)) {
        QMessageBox::warning(this, "命名冲突", "当前文件夹下已存在同名文件！");
        return;
    }

    currentItem->setText(newName);
}

void Widget::on_treeView_doubleClicked(const QModelIndex &index)
{
    QString type = index.sibling(index.row(), 1).data(Qt::DisplayRole).toString();
    if (type == "文件夹" || type == "驱动器" || type == "system")
        return;

    QString filePath = index.sibling(index.row(), 0).data(Qt::UserRole + 2).toString();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "打开失败", "该文件没有绑定路径，无法打开！");
        return;
    }

    bool ok = QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    if (!ok) {
        QMessageBox::warning(this, "打开失败", QString("无法打开文件：\n%1").arg(filePath));
    }
}

void Widget::openFile(const QModelIndex &index)
{
    // 判断是否为文件（排除 文件夹 / 驱动器 / 系统 根节点）
    QString type = index.sibling(index.row(), 1)
                       .data(Qt::DisplayRole).toString();
    if (type == "文件夹" || type == "驱动器" || type == "system") {
        return; // 不处理这些类型
    }

    // 获取路径（UserRole + 2 存储）
    QString filePath = index.sibling(index.row(), 0)
                           .data(Qt::UserRole + 2).toString();

    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "打开失败", "该文件没有绑定路径，无法打开！");
        return;
    }

    // 尝试用系统默认程序打开
    bool ok = QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    if (!ok) {
        QMessageBox::warning(this, "打开失败", QString("无法打开文件：\n%1").arg(filePath));
    }
}
