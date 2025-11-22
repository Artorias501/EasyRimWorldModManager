# WorkshopScanner API 文档

## 概述

`WorkshopScanner` 负责扫描 Steam 创意工坊的 RimWorld Mod。它会遍历创意工坊目录，解析每个 Mod 的 `About/About.xml` 文件，创建 ModItem 对象。

**类定义**: `src/data/WorkshopScanner.h`

**创意工坊路径**: 
```
{SteamPath}/steamapps/workshop/content/294100/
```

其中 `294100` 是 RimWorld 在 Steam 上的 App ID。

## 头文件引用

```cpp
#include "data/WorkshopScanner.h"
```

## 构造函数

### WorkshopScanner()
```cpp
WorkshopScanner()
```

创建扫描器实例。

### WorkshopScanner(const QString &workshopPath)
```cpp
explicit WorkshopScanner(const QString &workshopPath)
```

创建扫描器并设置创意工坊路径。

---

## 路径管理

### setWorkshopPath()
```cpp
void setWorkshopPath(const QString &workshopPath)
```

设置创意工坊根目录。

**参数**: 
- `workshopPath`: 创意工坊路径（如 `"C:/Program Files (x86)/Steam/steamapps/workshop/content/294100"`）

### getWorkshopPath()
```cpp
QString getWorkshopPath() const
```

获取当前设置的创意工坊路径。

---

## 扫描操作

### scanAllMods()
```cpp
bool scanAllMods()
```

扫描创意工坊目录下的所有 Mod。

**工作流程**:
1. 遍历创意工坊目录下的所有子文件夹
2. 在每个文件夹中查找 `About/About.xml`
3. 解析 XML 文件创建 ModItem
4. 缓存所有成功扫描的 ModItem

**返回值**: 
- `true`: 扫描成功（至少找到一个 Mod）
- `false`: 扫描失败

**示例**:
```cpp
WorkshopScanner scanner;
scanner.setWorkshopPath("C:/Program Files (x86)/Steam/steamapps/workshop/content/294100");

if (scanner.scanAllMods()) {
    qDebug() << "扫描成功，找到" << scanner.getScannedMods().size() << "个 Mod";
} else {
    qDebug() << "扫描失败";
}
```

---

## 数据访问

### getScannedMods()
```cpp
QList<ModItem *> getScannedMods() const
```

获取扫描到的所有 Mod。

**返回值**: ModItem 指针列表

**注意**: 返回的指针由 WorkshopScanner 管理，不要手动 delete。

---

## 清理

### clear()
```cpp
void clear()
```

清除所有扫描的 Mod。

**效果**: 删除所有 ModItem 对象并清空缓存。

---

## 静态方法

### detectSteamPath()
```cpp
static QString detectSteamPath()
```

自动检测 Steam 安装路径。

**检测位置** (按顺序):
1. `C:/Program Files (x86)/Steam`
2. `C:/Program Files/Steam`
3. `D:/Steam`
4. `E:/Steam`

**返回值**: 
- 找到返回 Steam 路径
- 未找到返回空字符串

**示例**:
```cpp
QString steamPath = WorkshopScanner::detectSteamPath();
if (!steamPath.isEmpty()) {
    qDebug() << "Steam 路径:" << steamPath;
} else {
    qDebug() << "未检测到 Steam";
}
```

---

## 使用示例

### 示例 1: 基本扫描

```cpp
#include "data/WorkshopScanner.h"

WorkshopScanner scanner;

// 自动检测 Steam 路径
QString steamPath = WorkshopScanner::detectSteamPath();
if (steamPath.isEmpty()) {
    qDebug() << "无法检测 Steam 路径";
    return;
}

// 设置创意工坊路径
QString workshopPath = QDir(steamPath).absoluteFilePath("steamapps/workshop/content/294100");
scanner.setWorkshopPath(workshopPath);

// 扫描所有 Mod
if (scanner.scanAllMods()) {
    QList<ModItem *> mods = scanner.getScannedMods();
    qDebug() << "找到" << mods.size() << "个创意工坊 Mod";
    
    // 显示前 5 个
    for (int i = 0; i < qMin(5, mods.size()); ++i) {
        qDebug() << mods[i]->name << "-" << mods[i]->packageId;
    }
}
```

### 示例 2: 查找特定 Mod

```cpp
WorkshopScanner scanner;
// ... 设置路径和扫描

QList<ModItem *> mods = scanner.getScannedMods();

// 查找 Harmony
ModItem *harmony = nullptr;
for (ModItem *mod : mods) {
    if (mod->packageId.compare("brrainz.harmony", Qt::CaseInsensitive) == 0) {
        harmony = mod;
        break;
    }
}

if (harmony) {
    qDebug() << "找到 Harmony:";
    qDebug() << "  名称:" << harmony->name;
    qDebug() << "  作者:" << harmony->author;
    qDebug() << "  路径:" << harmony->sourcePath;
}
```

---

## 注意事项

1. **推荐使用 ModManager**: WorkshopScanner 应该作为 ModManager 的私有服务，外部代码推荐使用 ModManager
2. **内存管理**: ModItem 对象由 WorkshopScanner 创建和管理，调用 `clear()` 会删除所有对象
3. **路径要求**: 创意工坊路径必须存在且包含 Mod 子文件夹
4. **性能**: 扫描速度取决于 Mod 数量，通常需要 1-5 秒

---

## 相关类

- [ModManager](ModManager_API.md) - 推荐使用
- [ModItem](ModItem_API.md) - Mod 数据结构
- [OfficialDLCScanner](OfficialDLCScanner_API.md) - 官方内容扫描
