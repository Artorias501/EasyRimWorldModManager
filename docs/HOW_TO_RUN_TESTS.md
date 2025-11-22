# 如何运行测试

## 快速开始

### 1. 确保环境准备就绪

**必需**：
- ✅ Qt 6.10.1 已安装
- ✅ CMake 3.31 或更高版本
- ✅ Visual Studio 2022 (MSVC编译器)
- ✅ RimWorld 已安装（用于测试）
- ✅ 至少运行过一次游戏（生成配置文件）

**可选**：
- 订阅一些创意工坊mod（用于更完整的测试）

### 2. 编译项目

在项目根目录执行：

```powershell
# 配置项目
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# 编译
cmake --build build --config Debug
```

### 3. 运行测试

```powershell
# 进入构建目录
cd build\Debug

# 运行程序
.\EasyRimWorldModManager.exe
```

### 4. 查看结果

测试运行后，检查生成的文件：

```powershell
# 查看生成的配置文件
dir ..\..\UserData\ModList\

# 应该看到：
# - test_modified_config.xml
# - test_sorted_config.xml
# - test_empty_start_config.xml
```

---

## 详细步骤

### Step 1: 克隆或打开项目

```powershell
cd D:\Projects\EasyRimWorldModManager
```

### Step 2: 检查Qt路径

在 `CMakeLists.txt` 中，确认Qt路径正确：

```cmake
set(CMAKE_PREFIX_PATH "D:/Qt/6.10.1/6.10.1/msvc2022_64")
```

如果路径不同，请修改为你的Qt安装路径。

### Step 3: 配置和编译

```powershell
# 清理之前的构建（如果有）
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue

# 重新配置
cmake -B build -G "Visual Studio 17 2022" -A x64

# 编译Debug版本
cmake --build build --config Debug

# 或编译Release版本
cmake --build build --config Release
```

### Step 4: 运行测试

```powershell
# Debug版本
.\build\Debug\EasyRimWorldModManager.exe

# 或Release版本
.\build\Release\EasyRimWorldModManager.exe
```

---

## 预期输出

### 控制台输出

```
========================================
  EasyRimWorldModManager - 功能测试
========================================

本测试将执行以下操作：
  1. 扫描所有已安装的mod和DLC
  2. 读取当前游戏加载的mod列表
  3. 修改mod列表排序并添加/移除DLC
  4. 创建空白加载列表并添加mod

⚠️  注意：所有配置都保存到UserData目录
   不会修改游戏的实际配置文件！


============================================================
开始运行所有测试
============================================================
测试时间: 2025-11-22 14:30:00

============================================================
测试1：扫描所有已安装的mod和DLC
============================================================
Steam路径: C:/Program Files (x86)/Steam
游戏安装路径: C:/Program Files (x86)/Steam/steamapps/common/RimWorld

正在扫描...

✅ 扫描完成
   创意工坊mod数量: 150
   官方内容数量: 8

[... 详细输出 ...]

============================================================
所有测试完成
============================================================

生成的文件:
  - UserData/ModList/test_modified_config.xml
  - UserData/ModList/test_sorted_config.xml
  - UserData/ModList/test_empty_start_config.xml

提示：所有配置都保存到了UserData目录，没有修改游戏的实际配置。

========================================
  测试完成
========================================
```

### 生成的文件

在项目根目录下：

```
UserData/
└── ModList/
    ├── test_modified_config.xml      (测试3输出)
    ├── test_sorted_config.xml        (测试3输出)
    └── test_empty_start_config.xml   (测试4输出)
```

---

## 验证测试结果

### 1. 检查文件是否生成

```powershell
Test-Path "UserData\ModList\test_modified_config.xml"
Test-Path "UserData\ModList\test_sorted_config.xml"
Test-Path "UserData\ModList\test_empty_start_config.xml"
```

所有命令应该返回 `True`。

### 2. 检查XML内容

使用文本编辑器打开任一生成的XML文件，验证：

- ✅ 包含 `<version>` 标签
- ✅ 包含 `<activeMods>` 标签
- ✅ 包含 `<knownExpansions>` 标签
- ✅ `knownExpansions` 中只有DLC，没有Core
- ✅ XML格式正确

示例内容：
```xml
<?xml version="1.0" encoding="utf-8"?>
<ModsConfigData>
  <version>1.6.4633 rev1261</version>
  <activeMods>
    <li>ludeon.rimworld.core</li>
    <li>brrainz.harmony</li>
    <li>ludeon.rimworld.royalty</li>
  </activeMods>
  <knownExpansions>
    <li>ludeon.rimworld.royalty</li>
  </knownExpansions>
</ModsConfigData>
```

### 3. 验证knownExpansions逻辑

在生成的配置文件中：
- `ludeon.rimworld.core` 应该在 `activeMods` 中
- `ludeon.rimworld.core` **不应该**在 `knownExpansions` 中
- 所有 `knownExpansions` 中的项都应该在 `activeMods` 中

---

## 常见问题

### Q: 编译失败 - 找不到Qt

**错误信息**：
```
CMake Error: Could not find Qt6
```

**解决方案**：
1. 确认Qt 6.10.1已正确安装
2. 修改 `CMakeLists.txt` 中的 `CMAKE_PREFIX_PATH`
3. 重新运行cmake配置

### Q: 运行时找不到DLL

**错误信息**：
```
无法启动程序，缺少 Qt6Core.dll
```

**解决方案**：
CMakeLists.txt已配置自动复制DLL，如果仍有问题：

```powershell
# 手动复制Qt DLL到构建目录
Copy-Item "D:\Qt\6.10.1\6.10.1\msvc2022_64\bin\Qt6Core.dll" "build\Debug\"
Copy-Item "D:\Qt\6.10.1\6.10.1\msvc2022_64\bin\Qt6Xml.dll" "build\Debug\"
```

### Q: 测试说找不到ModsConfig.xml

**错误信息**：
```
❌ 配置文件不存在
   请先运行一次游戏以生成配置文件
```

**解决方案**：
1. 运行一次RimWorld游戏
2. 确保游戏至少加载到主菜单
3. 配置文件会自动生成在：
   `C:\Users\{你的用户名}\AppData\LocalLow\Ludeon Studios\RimWorld by Ludeon Studios\Config\ModsConfig.xml`

### Q: 扫描失败，找不到Steam路径

**错误信息**：
```
❌ 扫描失败
```

**解决方案**：
修改 `test_functions.cpp` 中的Steam路径：

```cpp
// 在test_ScanInstalledModsAndDLCs()函数中
QString steamPath = "你的Steam安装路径";  // 例如 "D:/Steam"
```

### Q: 没有扫描到任何创意工坊mod

**原因**：未订阅任何mod

**不影响测试**：测试仍会正常运行，只是显示mod数量为0

**可选**：在Steam创意工坊订阅一些mod以获得更完整的测试

---

## 调试建议

### 启用详细输出

在 `test_functions.cpp` 的开头添加：

```cpp
void runAllTests()
{
    // 设置详细日志
    qSetMessagePattern("[%{time}] %{message}");
    
    // ... 其余代码
}
```

### 单独运行某个测试

修改 `main.cpp`：

```cpp
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    // 只运行测试1
    test_ScanInstalledModsAndDLCs();
    
    return 0;
}
```

### 添加断点调试

在Visual Studio中：
1. 打开 `test_functions.cpp`
2. 在关键行设置断点（F9）
3. 按F5开始调试

---

## 下一步

测试通过后，你可以：

1. **查看生成的配置文件**
   - 用XML编辑器或文本编辑器打开
   - 验证knownExpansions逻辑是否正确

2. **修改测试**
   - 在 `test_functions.cpp` 中添加自己的测试逻辑
   - 测试更多边界情况

3. **开发UI**
   - 使用Qt Widgets或Qt Quick
   - 基于现有的后端功能

4. **扩展功能**
   - 添加mod冲突检测
   - 实现依赖关系排序
   - 添加mod搜索和过滤

---

## 清理

如果需要重新开始：

```powershell
# 删除构建目录
Remove-Item -Recurse -Force build

# 删除生成的测试文件
Remove-Item -Recurse -Force UserData

# 重新编译
cmake -B build
cmake --build build --config Debug
```

---

## 支持

如果遇到问题：

1. 检查 `docs/test_guide.md` - 完整测试指南
2. 查看 `docs/test_quick_reference.md` - 快速参考
3. 阅读 `README.md` - 项目概述

---

**祝测试顺利！** 🎉
