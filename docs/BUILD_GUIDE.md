# 构建和运行指南

## 构建步骤

### 方法1: 使用Visual Studio

1. 打开 `build/EasyRimWorldModManager.sln`
2. 选择 Debug 或 Release 配置
3. 构建解决方案 (Ctrl+Shift+B)
4. 运行程序 (F5)

### 方法2: 使用CMake命令行

```powershell
# 进入build目录
cd d:\Projects\EasyRimWorldModManager\build

# 配置项目（如果还没配置）
cmake ..

# 编译
cmake --build . --config Debug

# 运行
.\Debug\EasyRimWorldModManager.exe
```

### 方法3: 重新生成构建文件

如果遇到问题，可以清理并重新生成：

```powershell
# 删除build目录
Remove-Item -Recurse -Force build

# 创建新的build目录
mkdir build
cd build

# 配置CMake
cmake ..

# 编译
cmake --build . --config Debug
```

## UI文件编译

`.ui` 文件会在编译时自动通过 `uic` 工具转换为 `ui_*.h` 文件：

- `MainWindow.ui` → `ui_MainWindow.h`
- `ModDetailPanel.ui` → `ui_ModDetailPanel.h`
- `TypeManagerDialog.ui` → `ui_TypeManagerDialog.h`

这些文件会生成在：
```
build/EasyRimWorldModManager_autogen/include/
```

## 常见问题

### Q: IntelliSense报错找不到 ui_*.h 文件
A: 这是正常的，这些文件在首次编译时才会生成。编译一次后错误会消失。

### Q: 找不到Qt头文件
A: 确保 CMakeLists.txt 中的 Qt 路径正确：
```cmake
set(CMAKE_PREFIX_PATH "D:/Qt/6.10.1/6.10.1/msvc2022_64")
```

### Q: 运行时缺少DLL
A: CMakeLists.txt 已配置自动复制必需的Qt DLL到输出目录。如果仍然缺少，检查Qt安装路径。

### Q: 如何编辑UI文件
A: 
1. 安装 Qt Designer (随Qt一起安装)
2. 使用Qt Creator打开项目
3. 或直接编辑 .ui XML文件

## 依赖项

确保已安装：
- Qt 6.10.1 (或更高版本)
  - Qt::Core
  - Qt::Xml
  - Qt::Widgets
  - Qt::Gui
- Visual Studio 2022 (MSVC编译器)
- CMake 3.31+

## 调试

### Visual Studio调试
1. 在VS中打开解决方案
2. 设置断点
3. 按 F5 开始调试

### 输出日志
程序使用 `qDebug()` 输出日志，可以查看：
- 控制台窗口（Debug模式）
- Visual Studio 输出窗口

## 运行时路径

程序运行时会创建/使用以下目录：

```
工作目录/
├── UserData/
│   ├── ModData/
│   │   ├── mod_data.json        # Mod类型和备注
│   │   └── custom_types.json    # 自定义类型
│   └── ModList/
│       └── *.xml                # 保存的配置
└── plugins/
    └── platforms/
        └── qwindows.dll         # Qt平台插件
```

## 首次运行

首次运行时，程序会：
1. 自动检测Steam路径
2. 扫描所有Mod（可能需要几秒钟）
3. 加载当前游戏配置
4. 显示主界面

如果Steam路径检测失败，可能需要手动配置游戏路径（功能待添加）。

## 性能提示

- Mod扫描会在后台线程执行，不会阻塞UI
- 扫描约100-500个Mod通常需要1-5秒
- 后续操作都是即时的（缓存机制）

## 更新UI

如果修改了 `.ui` 文件：
1. 保存文件
2. 重新编译项目
3. `uic` 会自动重新生成 `ui_*.h`
4. 程序会使用新的布局

## 清理生成的文件

```powershell
# 清理编译产物
cmake --build . --target clean

# 完全清理（需要重新配置CMake）
cd ..
Remove-Item -Recurse -Force build
```
