# Qt UI 文件快速参考

## UI文件列表

| 文件 | 类型 | 用途 |
|------|------|------|
| `MainWindow.ui` | QMainWindow | 主窗口，包含三面板布局和菜单栏 |
| `ModDetailPanel.ui` | QWidget | Mod详情编辑面板 |
| `TypeManagerDialog.ui` | QDialog | Mod类型管理对话框 |

## 主要控件对象名称

### MainWindow.ui

#### 列表和搜索
- `unloadedModsList` - QListWidget - 未加载的Mod列表
- `loadedModsList` - QListWidget - 已加载的Mod列表（支持拖拽）
- `unloadedSearchEdit` - QLineEdit - 未加载Mod搜索框
- `loadedSearchEdit` - QLineEdit - 已加载Mod搜索框

#### 按钮
- `addSelectedButton` - QPushButton - 添加选中的Mod
- `removeButton` - QPushButton - 移除Mod
- `moveUpButton` - QPushButton - 上移
- `moveDownButton` - QPushButton - 下移

#### 布局
- `splitter` - QSplitter - 三面板分割器（水平）
- `scrollArea` - QScrollArea - 详情面板的滚动区域

#### 菜单和动作
- `actionSaveToGame` - 保存到游戏配置 (Ctrl+S)
- `actionSaveAs` - 另存为 (Ctrl+Shift+S)
- `actionLoadConfig` - 加载配置 (Ctrl+O)
- `actionManageTypes` - 管理Mod类型
- `actionRescan` - 重新扫描 (F5)
- `actionAbout` - 关于

### ModDetailPanel.ui

#### 只读信息
- `nameValue` - QLabel - 显示Mod名称
- `packageIdValue` - QLabel - 显示PackageId
- `authorValue` - QLabel - 显示作者
- `versionValue` - QLabel - 显示支持版本

#### 可编辑信息
- `typeComboBox` - QComboBox - 类型下拉框
- `remarkTextEdit` - QTextEdit - 备注文本框
- `saveButton` - QPushButton - 保存更改按钮

#### 描述和依赖
- `descriptionBrowser` - QTextBrowser - 显示Mod描述
- `dependenciesList` - QListWidget - 依赖的Mod
- `loadBeforeList` - QListWidget - 应在之前加载
- `loadAfterList` - QListWidget - 应在之后加载
- `incompatibleList` - QListWidget - 不兼容的Mod

### TypeManagerDialog.ui

#### 列表
- `defaultTypesList` - QListWidget - 默认类型（只读）
- `customTypesList` - QListWidget - 自定义类型

#### 输入和按钮
- `newTypeEdit` - QLineEdit - 新类型输入框
- `addButton` - QPushButton - 添加类型按钮
- `deleteButton` - QPushButton - 删除类型按钮

## 信号槽连接

### MainWindow

```cpp
// 菜单动作
actionSaveToGame -> triggered() -> MainWindow::onSaveToGame()
actionSaveAs -> triggered() -> MainWindow::onSaveAs()
actionLoadConfig -> triggered() -> MainWindow::onLoadConfig()
actionManageTypes -> triggered() -> MainWindow::onManageTypes()
actionRescan -> triggered() -> MainWindow::onRescan()
actionAbout -> triggered() -> MainWindow::onAbout()

// 按钮
addSelectedButton -> clicked() -> MainWindow::onAddSelected()
removeButton -> clicked() -> MainWindow::onRemoveMod()
moveUpButton -> clicked() -> MainWindow::onMoveUp()
moveDownButton -> clicked() -> MainWindow::onMoveDown()

// 列表选择
unloadedModsList -> itemClicked() -> MainWindow::onUnloadedModSelected()
loadedModsList -> itemClicked() -> MainWindow::onLoadedModSelected()

// 搜索
unloadedSearchEdit -> textChanged() -> MainWindow::onUnloadedSearchChanged()
loadedSearchEdit -> textChanged() -> MainWindow::onLoadedSearchChanged()

// 拖拽排序
loadedModsList->model() -> rowsMoved() -> MainWindow::onLoadedListOrderChanged()
```

### ModDetailPanel

```cpp
saveButton -> clicked() -> ModDetailPanel::onSaveClicked()
```

### TypeManagerDialog

```cpp
addButton -> clicked() -> TypeManagerDialog::onAddType()
deleteButton -> clicked() -> TypeManagerDialog::onDeleteType()
newTypeEdit -> returnPressed() -> TypeManagerDialog::onAddType()
```

## 自定义信号

### ModDetailPanel

```cpp
signals:
    void modDetailsChanged();  // Mod详情已修改
```

## 编辑UI文件的常见任务

### 添加新的菜单项
1. 在 Qt Designer 中打开 `MainWindow.ui`
2. 右键点击菜单栏，选择"添加菜单"或"添加动作"
3. 设置动作的 text 和 objectName
4. 在 MainWindow.h 添加槽函数声明
5. 在 MainWindow.cpp 实现槽函数
6. 在 setupConnections() 中连接信号

### 修改布局
1. 打开对应的 `.ui` 文件
2. 调整控件的大小、位置
3. 修改布局属性（margins, spacing等）
4. 保存并重新编译

### 添加新控件
1. 在 Qt Designer 中拖拽控件到界面
2. 设置 objectName（用于代码访问）
3. 在对应的 .h 文件中通过 `ui->objectName` 访问

### 修改控件属性
常用属性：
- `text` - 显示文本
- `placeholderText` - 占位符文本（输入框）
- `font` - 字体
- `enabled` - 是否启用
- `visible` - 是否可见
- `maximumHeight/Width` - 最大尺寸
- `wordWrap` - 自动换行
- `selectionMode` - 选择模式（列表）
- `dragDropMode` - 拖拽模式（列表）

## 样式表 (可选)

如果需要自定义样式，可以在代码中设置：

```cpp
// 示例：设置按钮样式
ui->addSelectedButton->setStyleSheet(
    "QPushButton {"
    "    background-color: #0078d4;"
    "    color: white;"
    "    border-radius: 4px;"
    "    padding: 5px 15px;"
    "}"
    "QPushButton:hover {"
    "    background-color: #106ebe;"
    "}"
);
```

或在 `.ui` 文件的 styleSheet 属性中设置。

## 布局管理

### 使用的布局
- `QVBoxLayout` - 垂直布局
- `QHBoxLayout` - 水平布局
- `QFormLayout` - 表单布局（标签-值对）
- `QSplitter` - 可调整大小的分割器

### 布局技巧
- 使用 Spacer 填充空白区域
- 设置 sizePolicy 控制控件伸缩
- 使用 stretch factor 控制分割器比例
- 设置 minimumSize/maximumSize 限制尺寸

## 图标和资源 (可选扩展)

如需添加图标：
1. 创建 `resources.qrc` 文件
2. 添加图标资源
3. 在 CMakeLists.txt 中配置 AUTORCC
4. 在 UI 中使用 `:/icons/name.png`

## 国际化 (可选扩展)

如需支持多语言：
1. 在 UI 文件中使用 tr() 函数
2. 使用 lupdate 提取翻译字符串
3. 翻译 .ts 文件
4. 使用 lrelease 生成 .qm 文件
5. 在程序中加载翻译

## 调试UI

### 查看控件树
使用Qt的调试工具：
```cpp
// 输出所有子控件
qDebug() << this->findChildren<QWidget*>();
```

### 运行时修改属性
```cpp
// 动态修改样式
ui->someWidget->setProperty("class", "highlight");
ui->someWidget->style()->unpolish(ui->someWidget);
ui->someWidget->style()->polish(ui->someWidget);
```

## 性能优化

- 使用 `setUpdatesEnabled(false)` 在批量修改时禁用更新
- 列表项过多时考虑使用 Model/View 架构
- 避免频繁的 clear() 和 addItem()，考虑更新现有项

## 访问UI控件

在代码中访问UI控件：

```cpp
// 在构造函数中
ui = new Ui::MainWindow;
ui->setupUi(this);

// 访问控件
ui->unloadedModsList->count();
ui->typeComboBox->currentText();
ui->remarkTextEdit->toPlainText();

// 析构时清理
delete ui;
```
