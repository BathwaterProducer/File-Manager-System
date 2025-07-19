# Qt File System Simulator

This is a **Qt C++** desktop application that simulates a file system structure. Users can interact with a graphical interface to perform common operations such as creating files/folders, renaming, copying, pasting, drag-and-drop movement, and searching. The directory structure is saved and restored using a JSON file. Interface design learned by https://github.com/Catxiaobai/FileSystem.

## 🎯 Features

- 🗂 File tree view with custom icons and multiple file types
- 📁 Supports creating folders and common file types (e.g. `.txt`, `.doc`, `.pdf`, `.png`, etc.)
- ✏️ Rename items with duplicate name prevention
- 📋 Copy and paste files/folders (supports deep copy of subdirectories)
- 📦 Drag-and-drop movement (valid only within appropriate directory contexts)
- 🔍 File/folder name search with "Previous / Next" navigation
- 💾 Automatically save and load directory structure (`filesystem.json`)
- 📂 Double-click to open actual files (requires bound path)

## 🛠 Technical Details

- Uses `QTreeView` and `QStandardItemModel` to present a tree structure
- Implements drag-and-drop with `QDragEnterEvent` / `QDropEvent`
- Reads and writes JSON using `QJsonDocument`
- Custom right-click context menu with file operations (create, rename, delete, etc.)
- Opens files using `QDesktopServices::openUrl()`

## 📷 UI Overview

The main interface includes:

- Tree view simulating "My Computer" → Disk Drives → Folder structure
- Top label area showing current path
- Search box with "Previous / Next" buttons
- Dynamic right-click context menu (actions depend on selected item type)

## 🧪 Supported File Types

The following file types are recognized and assigned icons:

| Type           | Extension     | Icon Key            |
|----------------|---------------|---------------------|
| Text Document  | `.txt`        | `treeItem_txt`      |
| PDF Document   | `.pdf`        | `treeItem_pdf`      |
| Image File     | `.png`        | `treeItem_png`      |
| Word Document  | `.doc`        | `treeItem_doc`      |
| GIF Image      | `.gif`        | `treeItem_gif`      |
| PowerPoint     | `.ppt`        | `treeItem_ppt`      |
| Excel File     | `.xls`        | `treeItem_xls`      |
| Archive File   | `.zip`        | `treeItem_zip`      |
| Unknown Type   | Other         | `treeItem_Unknownfile` |

## ▶️ How to Use

### Build Requirements

- Qt 5 or Qt 6 (Qt 5.12+ recommended)

Open the `CMakeLists.txt` file with Qt Creator, then build and run the project.

### Runtime Instructions

1. On first launch, the app initializes a default "My Computer" structure.
2. All user actions (create, delete, rename, move, etc.) are persisted.
3. On exit, the directory structure is saved to `filesystem.json`.
4. On next launch, the app automatically loads the saved state.

## ✅ TODO

- Support dragging files to external applications
- Support drag-and-drop of files from outside the application
- Add support for multi-selection operations (copy/paste/delete)
- Monitor file changes using `QFileSystemWatcher`

## 📃 License

This project is intended for educational purposes only. It does not use any patented or commercial logic. If you plan to use it commercially, please evaluate the legal implications independently.



# Qt 文件系统模拟器

这是一个使用 **Qt C++** 开发的桌面应用程序，模拟了一个文件系统结构。用户可以通过图形界面执行诸如新建文件/文件夹、重命名、复制、粘贴、拖拽移动、搜索等常见操作，并以 JSON 文件的形式保存和恢复目录结构。
界面设计学习的这位老哥：https://github.com/Catxiaobai/FileSystem

## 🎯 功能概览

- 🗂 文件树视图，支持自定义图标和多种文件类型
- 📁 支持新建文件夹和常见类型文件（如 `.txt`, `.doc`, `.pdf`, `.png` 等）
- ✏️ 文件重命名，避免重名冲突
- 📋 文件复制/粘贴功能（支持深度复制子目录）
- 📦 拖拽移动（仅限同目录下的合法操作）
- 🔍 文件名称搜索，支持“上一个 / 下一个”定位
- 💾 自动保存和加载目录结构（`filesystem.json`）
- 📂 双击打开实际文件（需已有路径绑定）

## 🛠 技术细节

- 基于 `QTreeView` 和 `QStandardItemModel` 展示树形结构
- 使用 `QDragEnterEvent` / `QDropEvent` 实现拖拽操作
- 使用 `QJsonDocument` 实现 JSON 数据的持久化读写
- 自定义右键菜单，支持创建文件、重命名、删除等操作
- 使用 `QDesktopServices::openUrl()` 打开文件

## 📷 界面概览

主界面包含：

- 树形视图展示的“我的电脑” → 各盘符 → 文件夹结构
- 顶部路径显示区域
- 搜索框及“上一个 / 下一个”按钮
- 动态右键菜单（依据选中类型显示不同操作）

## 🧪 支持文件类型

支持以下文件类型的图标与识别：

| 类型        | 示例扩展名 | 图标键值         |
|-------------|-------------|------------------|
| 文本文档    | `.txt`     | `treeItem_txt`   |
| PDF 文档    | `.pdf`     | `treeItem_pdf`   |
| 图片文件    | `.png`     | `treeItem_png`   |
| Word 文档   | `.doc`     | `treeItem_doc`   |
| GIF 图片    | `.gif`     | `treeItem_gif`   |
| PPT 文档    | `.ppt`     | `treeItem_ppt`   |
| Excel 表格  | `.xls`     | `treeItem_xls`   |
| 压缩文件    | `.zip`     | `treeItem_zip`   |
| 未知类型    | 其他       | `treeItem_Unknownfile` |

## ▶️ 使用说明
### 编译要求
Qt 5 或 Qt 6（推荐 Qt 5.12+）

使用 Qt Creator 打开 CMakeLists.txt 工程文件后构建运行

### 运行说明
首次启动会初始化默认的“我的电脑”结构；

所有用户操作（新建、删除、重命名、移动等）会被保存；

退出程序时，目录结构将保存至 filesystem.json；

再次启动时会自动加载上次保存的状态。

## ✅ TODO
支持文件拖动至外部打开

支持从外部拖入文件并创建节点

支持多选节点复制 / 粘贴 / 删除

本地文件监控（如使用 QFileSystemWatcher）

## 📃 License
本项目仅用于学习与教学目的，未使用任何专利或商业逻辑，如需商业化请自行评估风险。
