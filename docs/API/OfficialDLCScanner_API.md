# OfficialDLCScanner API 文档

## 概述

`OfficialDLCScanner` 负责扫描 RimWorld 的官方内容，包括游戏核心（Core）和官方 DLC（如 Royalty、Ideology、Biotech 等）。

**类定义**: `src/data/OfficialDLCScanner.h`

**扫描路径**: 
```
{GamePath}/Data/
├── Core/           # 游戏核心
├── Royalty/        # 官方DLC
├── Ideology/       # 官方DLC
└── Biotech/        # 官方DLC
```

## 头文件引用

```cpp
#include "data/OfficialDLCScanner.h"
```

## 构造函数

### OfficialDLCScanner()
```cpp
OfficialDLCScanner()
```

创建扫描器实例。

### OfficialDLCScanner(const QString &dataPath)
```cpp
explicit OfficialDLCScanner(const QString &dataPath)
```

创建扫描器并设置 Data 目录路径。

---

## 路径管理

### setDataPath()
```cpp
void setDataPath(const QString &dataPath)
```

设置游戏 Data 目录路径。

**参数**: 
- `dataPath`: 游戏的 Data 目录（如 `"C:/Program Files (x86)/Steam/steamapps/common/RimWorld/Data"`）

### getDataPath()
```cpp
QString getDataPath() const
```

获取当前设置的 Data 目录路径。

---

## 扫描操作

### scanAllDLCs()
```cpp
bool scanAllDLCs()
```

扫描 Data 目录下的所有官方内容（Core + DLC）。

**工作流程**:
1. 遍历 Data 目录下的所有子文件夹
2. 在每个文件夹中查找 `About/About.xml`
3. 解析 XML 创建 ModItem
4. 识别 Core 和 DLC
   - Core: `packageId == "Ludeon.RimWorld"`, `isOfficialDLC = false`, `type = "Core"`
   - DLC: 其他官方内容, `isOfficialDLC = true`, `type = "DLC"`

**返回值**: 
- `true`: 扫描成功
- `false`: 扫描失败

**示例**:
```cpp
OfficialDLCScanner scanner;
scanner.setDataPath("C:/Program Files (x86)/Steam/steamapps/common/RimWorld/Data");

if (scanner.scanAllDLCs()) {
    qDebug() << "扫描成功";
    qDebug() << "找到" << scanner.getScannedDLCs().size() << "个官方内容";
} else {
    qDebug() << "扫描失败";
}
```

---

## 数据访问

### getScannedDLCs()
```cpp
QList<ModItem *> getScannedDLCs() const
```

获取扫描到的所有官方内容（包括 Core 和 DLC）。

**返回值**: ModItem 指针列表

**注意**: 
- Core 的 `isOfficialDLC` 为 `false`，但包含在此列表中
- 实际 DLC 的 `isOfficialDLC` 为 `true`

---

## 清理

### clear()
```cpp
void clear()
```

清除所有扫描的官方内容。

---

## 静态方法

### getDefaultDataPath()
```cpp
static QString getDefaultDataPath(const QString &gameInstallPath)
```

根据游戏安装路径获取 Data 目录路径。

**参数**: 
- `gameInstallPath`: 游戏根目录

**返回值**: 
- Data 目录的完整路径

**示例**:
```cpp
QString gamePath = "C:/Program Files (x86)/Steam/steamapps/common/RimWorld";
QString dataPath = OfficialDLCScanner::getDefaultDataPath(gamePath);
// 返回: "C:/Program Files (x86)/Steam/steamapps/common/RimWorld/Data"
```

---

## 使用示例

### 示例 1: 基本扫描

```cpp
#include "data/OfficialDLCScanner.h"
#include <QDebug>

OfficialDLCScanner scanner;

QString gamePath = "C:/Program Files (x86)/Steam/steamapps/common/RimWorld";
QString dataPath = OfficialDLCScanner::getDefaultDataPath(gamePath);
scanner.setDataPath(dataPath);

if (scanner.scanAllDLCs()) {
    QList<ModItem *> dlcs = scanner.getScannedDLCs();
    
    qDebug() << "=== 官方内容 ===";
    for (ModItem *dlc : dlcs) {
        qDebug() << dlc->name;
        qDebug() << "  PackageId:" << dlc->packageId;
        qDebug() << "  类型:" << dlc->type;
        qDebug() << "  是否DLC:" << (dlc->isOfficialDLC ? "是" : "否");
        qDebug() << "";
    }
}
```

### 示例 2: 区分 Core 和 DLC

```cpp
OfficialDLCScanner scanner;
// ... 设置路径和扫描

QList<ModItem *> allContent = scanner.getScannedDLCs();

ModItem *core = nullptr;
QList<ModItem *> dlcs;

for (ModItem *item : allContent) {
    if (item->packageId.compare("Ludeon.RimWorld", Qt::CaseInsensitive) == 0) {
        core = item;  // 这是 Core
    } else if (item->isOfficialDLC) {
        dlcs.append(item);  // 这是 DLC
    }
}

if (core) {
    qDebug() << "游戏核心:" << core->name;
}

qDebug() << "\n已安装的官方 DLC:";
for (ModItem *dlc : dlcs) {
    qDebug() << " -" << dlc->name;
}
```

### 示例 3: 检查特定 DLC

```cpp
bool hasRoyalty(OfficialDLCScanner &scanner) {
    for (ModItem *item : scanner.getScannedDLCs()) {
        if (item->packageId.compare("ludeon.rimworld.royalty", 
                                    Qt::CaseInsensitive) == 0) {
            return true;
        }
    }
    return false;
}

// 使用
if (hasRoyalty(scanner)) {
    qDebug() << "已安装 Royalty DLC";
}
```

---

## Core 识别规则

Core 的识别基于以下规则：

1. **PackageId**: 精确匹配 `"Ludeon.RimWorld"`（不区分大小写）
2. **标记**: 
   - `isOfficialDLC = false` (Core 不是 DLC)
   - `type = "Core"`
3. **名称回退**: 如果 About.xml 中没有 `<name>` 标签，使用 `"RimWorld Core"` 作为默认名称

```cpp
// 正确的 Core 识别
if (mod->packageId.compare("Ludeon.RimWorld", Qt::CaseInsensitive) == 0) {
    // 这是 Core
    // mod->isOfficialDLC == false
    // mod->type == "Core"
}
```

**注意**: 不要使用 `contains("core")` 或类似方法，因为 PackageId 中不包含 "core" 字符串。

---

## 官方 DLC 识别规则

官方 DLC 的识别规则：

1. **位置**: 在 `{GamePath}/Data/` 目录下
2. **非 Core**: PackageId 不是 `"Ludeon.RimWorld"`
3. **标记**:
   - `isOfficialDLC = true`
   - `type = "DLC"`
4. **名称回退**: 如果没有 `<name>` 标签，使用 `packageId` 作为名称

---

## 注意事项

1. **推荐使用 ModManager**: OfficialDLCScanner 应该作为 ModManager 的私有服务
2. **Core 的特殊性**: Core 的 `isOfficialDLC` 为 `false`，需要特别处理
3. **PackageId 大小写**: 比较时应使用不区分大小写的方式
4. **内存管理**: ModItem 由 OfficialDLCScanner 管理，调用 `clear()` 会删除
5. **路径要求**: Data 目录必须存在且包含 Core 文件夹

---

## 相关类

- [ModManager](ModManager_API.md) - 推荐使用
- [ModItem](ModItem_API.md) - Mod 数据结构
- [WorkshopScanner](WorkshopScanner_API.md) - 创意工坊扫描
