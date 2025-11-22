# 界面优化说明

## 已完成的改进

### 1. TokyoNight 主题样式

基于 TokyoNight Storm 配色方案创建了完整的 QSS 样式表。

#### 主要颜色
- **背景色**: #24283b (深蓝灰)
- **次级背景**: #1f2335 (更深的蓝灰)
- **前景色**: #c0caf5 (浅蓝白)
- **注释色**: #565f89 (灰蓝)
- **强调色**: #7aa2f7 (亮蓝)
- **成功色**: #9ece6a (绿)
- **警告色**: #e0af68 (橙黄)
- **错误色**: #f7768e (红)
- **边框色**: #3b4261 (中蓝灰)

#### 样式化的组件
- ✅ 主窗口和对话框
- ✅ 菜单栏和菜单项
- ✅ 状态栏
- ✅ 按钮（普通/悬停/按下/禁用状态）
- ✅ 输入框和文本编辑器
- ✅ 列表控件
- ✅ 下拉框
- ✅ 滚动条
- ✅ 分组框
- ✅ 分割器
- ✅ 工具提示

### 2. Mod 详情面板优化

#### 列表自适应高度
实现了 `adjustListHeight()` 方法，让依赖关系列表根据内容自动调整高度：

- **最小高度**: 40px（即使没有内容也保持可见）
- **最大高度**: 200px（超过后显示滚动条）
- **动态计算**: 根据列表项数量自动调整

应用于：
- 依赖的 Mod 列表
- 应在之前加载列表
- 应在之后加载列表
- 不兼容的 Mod 列表

#### 备注框优化
- **最大高度**: 80px
- **自适应内容**: 使用 AdjustToContents 策略

### 3. 资源系统

创建了 Qt 资源文件系统：
- `resources/styles/tokyonight.qss` - 样式表文件
- `resources/resources.qrc` - 资源配置文件
- 在 `main.cpp` 中自动加载样式表
- 支持从资源文件或文件系统加载（开发时方便调试）

## 使用方法

### 应用样式表
样式表在程序启动时自动加载，无需额外操作。

### 修改样式
1. 编辑 `resources/styles/tokyonight.qss`
2. 重新编译项目（CMake 会自动处理 .qrc 文件）
3. 运行程序查看效果

### 临时测试样式（不重新编译）
1. 修改 `resources/styles/tokyonight.qss`
2. 确保 `main.cpp` 中的备用加载路径正确
3. 运行程序会从文件系统加载

## 技术细节

### QSS 选择器
```css
/* 通用控件 */
QWidget { }

/* 特定控件 */
QPushButton { }
QPushButton:hover { }
QPushButton:pressed { }

/* ID 选择器 */
#unloadedModsLabel { }

/* 状态选择器 */
QListWidget::item:selected { }
```

### 动态高度计算
```cpp
int itemHeight = listWidget->sizeHintForRow(0);
int totalHeight = count * itemHeight + 8; // 8px 边距
totalHeight = qMin(totalHeight, maxHeight);
totalHeight = qMax(totalHeight, minHeight);
listWidget->setMaximumHeight(totalHeight);
```

## 扩展建议

### 添加多主题支持
可以创建多个 QSS 文件，让用户选择主题：
- `tokyonight.qss`（当前）
- `tokyonight-light.qss`（浅色版）
- `dracula.qss`
- `nord.qss`

### 自定义颜色
在 `UserDataManager` 中添加主题配置：
```json
{
  "theme": "tokyonight",
  "customColors": {
    "accent": "#7aa2f7"
  }
}
```

### 图标系统
添加 SVG 图标以匹配主题：
```xml
<file>icons/add.svg</file>
<file>icons/remove.svg</file>
<file>icons/sort.svg</file>
```

## 参考

- [TokyoNight 配色方案](https://github.com/tokyo-night/tokyo-night-vscode-theme)
- [Qt Style Sheets Reference](https://doc.qt.io/qt-6/stylesheet-reference.html)
- [Qt Resource System](https://doc.qt.io/qt-6/resources.html)
