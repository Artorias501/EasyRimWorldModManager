# UI 实现说明

## 已完成的UI组件

### 1. 主窗口 (MainWindow)
- **文件**: `src/ui/MainWindow.ui`, `MainWindow.h`, `MainWindow.cpp`
- **功能**:
  - 三面板水平布局（使用QSplitter可调整大小）
  - 左侧：未加载的Mod列表
  - 中间：已加载的Mod列表
  - 右侧：Mod详情面板
  - 顶部菜单栏包含：
    - 文件菜单：保存到游戏(Ctrl+S)、另存为(Ctrl+Shift+S)、加载配置(Ctrl+O)、退出
    - 管理菜单：管理Mod类型、重新扫描(F5)
    - 帮助菜单：关于
  - 搜索框：每个列表都有搜索过滤功能
  - 状态栏：显示操作提示

### 2. Mod详情面板 (ModDetailPanel)
- **文件**: `src/ui/ModDetailPanel.ui`, `ModDetailPanel.h`, `ModDetailPanel.cpp`
- **功能**:
  - 显示基本信息（名称、PackageId、作者、支持版本）
  - 可编辑信息：
    - 类型：下拉框选择（来自 UserDataManager）
    - 备注：多行文本框
  - 保存更改按钮
  - 描述显示区域（支持HTML链接）
  - 依赖关系显示：
    - 依赖的Mod
    - 应在之前加载
    - 应在之后加载
    - 不兼容的Mod

### 3. 类型管理对话框 (TypeManagerDialog)
- **文件**: `src/ui/TypeManagerDialog.ui`, `TypeManagerDialog.h`, `TypeManagerDialog.cpp`
- **功能**:
  - 显示默认类型（只读）
  - 管理自定义类型：
    - 添加新类型
    - 删除选中的自定义类型
    - 确认删除提示

## UI文件详情

所有UI文件都使用Qt Designer格式（.ui文件），方便在Qt Designer或Qt Creator中可视化编辑。

### MainWindow.ui 布局说明
```
┌─────────────────────────────────────────────────────┐
│ 菜单栏: [文件] [管理] [帮助]                         │
├──────────────┬──────────────┬──────────────────────┤
│ 未加载的Mod  │ 已加载的Mod   │ Mod详情               │
│ [搜索框]     │ [搜索框]     │                      │
│ [列表]       │ [列表-可拖拽] │ [滚动区域]            │
│ [添加按钮>>] │ [上移/下移]   │  - 基本信息           │
│              │ [<<移除按钮]  │  - 可编辑(类型/备注)  │
│              │              │  - 保存按钮           │
│              │              │  - 描述              │
│              │              │  - 依赖关系           │
└──────────────┴──────────────┴──────────────────────┘
│ 状态栏: 显示操作提示                                  │
└─────────────────────────────────────────────────────┘
```

## 核心功能实现

### 显示优先级
Mod列表项的显示文本遵循优先级：
1. 备注（remark）- 如果存在
2. 名称（name）- 如果备注为空
3. PackageId - 如果名称也为空

代码实现在 `MainWindow::getModDisplayText()`

### 颜色标识
- 官方DLC：蓝色
- Core (Ludeon.RimWorld)：绿色
- 其他Mod：默认颜色

### 搜索过滤
支持模糊搜索以下字段：
- 名称
- PackageId
- 备注
- 类型

### 拖拽排序
已加载的Mod列表支持：
- 拖拽重新排序（InternalMove模式）
- 上移/下移按钮
- 自动同步到ModConfigManager

### 异步扫描
使用QtConcurrent在后台线程扫描Mod：
- 显示进度对话框
- 避免阻塞UI
- 完成后自动更新列表

## 使用的后端API

### ModManager
- `setSteamPath()` - 设置Steam路径
- `scanAll()` - 扫描所有Mod（异步执行）
- `getAllMods()` - 获取所有Mod
- `findModByPackageId()` - 查找特定Mod
- `getUserDataManager()` - 获取用户数据管理器
- `saveModsToUserData()` - 保存用户数据

### ModConfigManager
- `loadConfig()` - 加载游戏配置
- `saveConfig()` - 保存到游戏配置
- `getActiveMods()` - 获取已激活Mod列表
- `addMod()` / `removeMod()` - 添加/移除Mod
- `moveModUp()` / `moveModDown()` - 移动Mod位置
- `setActiveMods()` - 设置完整列表

### UserDataManager
- `getAllTypes()` - 获取所有类型（默认+自定义）
- `getDefaultTypes()` - 获取默认类型
- `getAllCustomTypes()` - 获取自定义类型
- `addCustomType()` / `removeCustomType()` - 管理自定义类型
- `hasType()` - 检查类型是否存在
- `saveModListToPath()` - 另存为配置文件

**注意**: UserDataManager 已整合原 ModTypeManager 的功能，统一管理类型。

## 编译配置

### CMakeLists.txt 更新
已添加必要的Qt组件：
```cmake
find_package(Qt6 COMPONENTS
    Core
    Xml
    Widgets      # 新增
    REQUIRED)

target_link_libraries(EasyRimWorldModManager
    Qt::Core
    Qt::Xml
    Qt::Widgets  # 新增
)
```

### 自动UIC配置
CMake已配置 `CMAKE_AUTOUIC ON`，会自动将 `.ui` 文件编译为 `ui_*.h` 文件。

## 如何编辑UI

### 使用Qt Designer
1. 打开Qt Designer或Qt Creator
2. 打开 `src/ui/*.ui` 文件
3. 可视化编辑布局、添加控件、调整属性
4. 保存后重新编译项目即可生效

### 使用VS Code + Qt扩展
1. 安装Qt相关扩展（如 "Qt tools"）
2. 直接编辑 `.ui` XML文件
3. 或使用扩展的可视化编辑器

## 待优化功能

以下功能已预留接口，可后续添加：

1. **自定义列表项渲染**
   - 可以实现 QStyledItemDelegate 美化显示
   - 添加图标、多行布局等

2. **依赖关系可视化**
   - 点击依赖项跳转到对应Mod
   - 高亮显示缺失的依赖

3. **批量操作**
   - 全选/反选
   - 批量添加/移除

4. **配置方案管理**
   - 快速切换不同的Mod配置
   - 配置方案对比

5. **冲突检测**
   - 自动检测不兼容的Mod组合
   - 提供加载顺序建议

## 启动说明

程序启动后会：
1. 自动检测Steam路径
2. 在后台扫描所有Mod（显示进度）
3. 加载当前游戏配置
4. 填充未加载/已加载列表
5. 准备就绪，可以开始操作

## 注意事项

1. **Core的位置**：Core (Ludeon.RimWorld) 应始终在第一位，不可移除
2. **PackageId比较**：不区分大小写
3. **保存策略**：
   - "保存到游戏"：直接覆盖游戏的ModsConfig.xml
   - "另存为"：保存到自定义位置
   - 用户数据（类型、备注）自动保存到UserData目录
4. **线程安全**：扫描操作在后台线程，UI更新在主线程

## 快捷键

- `Ctrl+S` - 保存到游戏配置
- `Ctrl+Shift+S` - 另存为
- `Ctrl+O` - 加载配置
- `F5` - 重新扫描Mod

## 错误处理

所有关键操作都有错误提示：
- 扫描失败
- 保存失败
- 加载失败
- 无效操作（如移除Core）

使用 QMessageBox 显示友好的错误信息。
