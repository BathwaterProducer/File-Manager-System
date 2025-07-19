# File-Manager-System
File-Manager-System
# Qt 文件系统模拟器

这是一个使用 **Qt C++** 开发的桌面应用程序，模拟了一个文件系统结构。用户可以通过图形界面执行诸如新建文件/文件夹、重命名、复制、粘贴、拖拽移动、搜索等常见操作，并以 JSON 文件的形式保存和恢复目录结构。

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
