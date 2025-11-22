# ModManager API 文档

## 概述

`ModManager` 是 EasyRimWorldModManager 的**中心管理类**，推荐作为外部代码与后端交互的唯一入口。它整合了 `WorkshopScanner`、`OfficialDLCScanner` 和 `UserDataManager`，提供统一的数据管理和访问接口。

**类定义**: `src/data/ModManager.h`

## 特性

- ✅ **统一接口**: 所有 Mod 相关操作通过 ModManager 完成
- ✅ **自动缓存**: 扫描结果自动缓存，避免重复扫描
- ✅ **数据同步**: 自动同步用户数据（备注、类型）
- ✅ **内存管理**: 自动管理 ModItem 对象的生命周期
- ✅ **简化使用**: 无需直接操作扫描器和数据管理器

## 头文件引用

```cpp
#include "data/ModManager.h"
```

## 构造函数

### ModManager()
```cpp
ModManager()
```

创建默认的 ModManager 实例。

**特性**:
- 自动创建 WorkshopScanner、OfficialDLCScanner 和 UserDataManager
- 自动初始化用户数据目录结构
- 自动加载用户数据（备注和类型）

**示例**:
```cpp
ModManager manager;
// 准备就绪，可以设置路径并扫描
```

---

### ModManager(const QString &steamPath)
```cpp
explicit ModManager(const QString &steamPath)
```

创建 ModManager 并设置 Steam 路径。

**参数**:
- `steamPath`: Steam 安装路径（如 `"C:/Program Files (x86)/Steam"`）

**特性**:
- 自动推断创意工坊路径: `{steamPath}/steamapps/workshop/content/294100`
- 自动推断游戏路径: `{steamPath}/steamapps/common/RimWorld`
- 自动设置 DLC 扫描路径

**示例**:
```cpp
ModManager manager("C:/Program Files (x86)/Steam");
// 路径已设置，可以直接扫描
manager.scanAll();
```

---

## 路径管理

### setSteamPath()
```cpp
void setSteamPath(const QString &steamPath)
```

设置 Steam 安装路径。

**参数**:
- `steamPath`: Steam 根目录路径

**效果**:
- 更新创意工坊扫描路径
- 更新游戏安装路径（假设在 Steam 标准位置）
- 更新 DLC 扫描路径

**示例**:
```cpp
ModManager manager;

// 手动设置路径
manager.setSteamPath("C:/Program Files (x86)/Steam");

// 或使用自动检测
QString steamPath = WorkshopScanner::detectSteamPath();
if (!steamPath.isEmpty()) {
    manager.setSteamPath(steamPath);
}
```

---

### getSteamPath()
```cpp
QString getSteamPath() const
```

获取当前设置的 Steam 路径。

**返回值**: Steam 路径字符串

---

### setGameInstallPath()
```cpp
void setGameInstallPath(const QString &gameInstallPath)
```

设置游戏安装路径（覆盖默认推断）。

**参数**:
- `gameInstallPath`: RimWorld 游戏根目录

**用途**: 
- 游戏不在 Steam 标准位置
- 使用非 Steam 版本

**示例**:
```cpp
manager.setGameInstallPath("D:/Games/RimWorld");
```

---

### getGameInstallPath()
```cpp
QString getGameInstallPath() const
```

获取当前设置的游戏安装路径。

**返回值**: 游戏路径字符串

---

## Mod 扫描和缓存

### scanAll()
```cpp
bool scanAll()
```

扫描所有 Mod 和 DLC（包括创意工坊和官方内容）。

**工作流程**:
1. 清除旧缓存
2. 调用 `scanWorkshopMods()`
3. 调用 `scanOfficialDLCs()`
4. 自动调用 `loadUserDataToMods()` 加载用户数据

**返回值**: 
- `true`: 至少有一个扫描器成功
- `false`: 所有扫描器都失败

**示例**:
```cpp
ModManager manager;
manager.setSteamPath("C:/Program Files (x86)/Steam");

if (manager.scanAll()) {
    qDebug() << "扫描成功";
    qDebug() << "工坊 Mod:" << manager.getWorkshopMods().size();
    qDebug() << "官方 DLC:" << manager.getOfficialDLCs().size();
} else {
    qDebug() << "扫描失败";
}
```

---

### scanWorkshopMods()
```cpp
bool scanWorkshopMods()
```

只扫描 Steam 创意工坊 Mod。

**返回值**: 
- `true`: 扫描成功
- `false`: 扫描失败

**注意**: 不会自动加载用户数据，需要手动调用 `loadUserDataToMods()`

---

### scanOfficialDLCs()
```cpp
bool scanOfficialDLCs()
```

只扫描官方 DLC 和 Core。

**返回值**: 
- `true`: 扫描成功
- `false`: 扫描失败

**注意**: 不会自动加载用户数据，需要手动调用 `loadUserDataToMods()`

---

## 缓存数据访问

### getAllMods()
```cpp
QList<ModItem *> getAllMods() const
```

获取所有 Mod（官方 DLC + 创意工坊 Mod）。

**返回值**: ModItem 指针列表（官方内容在前）

**示例**:
```cpp
QList<ModItem *> allMods = manager.getAllMods();

for (ModItem *mod : allMods) {
    qDebug() << mod->name << "-" << mod->packageId;
}
```

---

### getWorkshopMods()
```cpp
QList<ModItem *> getWorkshopMods() const
```

获取所有创意工坊 Mod。

**返回值**: 创意工坊 ModItem 指针列表

---

### getOfficialDLCs()
```cpp
QList<ModItem *> getOfficialDLCs() const
```

获取所有官方内容（DLC，不包括 Core）。

**返回值**: 官方 DLC ModItem 指针列表

**注意**: Core 的 `isOfficialDLC` 为 `false`，不会出现在此列表中

---

### findModByPackageId()
```cpp
ModItem *findModByPackageId(const QString &packageId) const
```

根据 PackageId 查找 Mod。

**参数**:
- `packageId`: Mod 的唯一标识符（不区分大小写）

**返回值**: 
- 找到: ModItem 指针
- 未找到: `nullptr`

**示例**:
```cpp
ModItem *royalty = manager.findModByPackageId("ludeon.rimworld.royalty");
if (royalty) {
    qDebug() << "找到:" << royalty->name;
} else {
    qDebug() << "未安装 Royalty DLC";
}
```

---

### isOfficialDLC()
```cpp
bool isOfficialDLC(const QString &packageId) const
```

检查指定 PackageId 是否为官方 DLC。

**参数**:
- `packageId`: Mod 的 PackageId

**返回值**: 
- `true`: 是官方 DLC
- `false`: 不是官方 DLC 或不存在

**示例**:
```cpp
if (manager.isOfficialDLC("ludeon.rimworld.ideology")) {
    qDebug() << "这是官方 DLC";
}
```

---

## UserDataManager 联动

### getUserDataManager()
```cpp
UserDataManager* getUserDataManager()
const UserDataManager* getUserDataManager() const
```

获取 UserDataManager 实例。

**返回值**: UserDataManager 指针

**用途**: 
- 直接访问用户数据功能
- 保存 Mod 列表到文件
- 管理自定义类型

**示例**:
```cpp
UserDataManager *userMgr = manager.getUserDataManager();

// 设置 Mod 类型
userMgr->setModType("some.packageid", "功能性");

// 添加备注
userMgr->setModRemark("some.packageid", "我的备注");

// 保存数据
userMgr->saveAll();
```

---

### loadUserDataToMods()
```cpp
void loadUserDataToMods()
```

从 UserDataManager 加载备注和类型到所有缓存的 ModItem。

**工作流程**:
1. 遍历所有缓存的 ModItem
2. 从 UserDataManager 读取类型和备注
3. 写入 ModItem 的 `type` 和 `remark` 字段

**何时调用**:
- `scanAll()` 会自动调用
- 手动调用 `scanWorkshopMods()` 或 `scanOfficialDLCs()` 后需要手动调用

**示例**:
```cpp
manager.scanWorkshopMods();
manager.scanOfficialDLCs();
manager.loadUserDataToMods();  // 手动加载用户数据
```

---

### saveModsToUserData()
```cpp
void saveModsToUserData()
```

将所有缓存的 ModItem 的备注和类型保存到 UserDataManager。

**工作流程**:
1. 遍历所有缓存的 ModItem
2. 将非空的 `type` 和 `remark` 写入 UserDataManager
3. 调用 `UserDataManager::saveModData()` 持久化

**何时调用**:
- ModManager 析构时会自动调用
- 修改 ModItem 后想立即保存时手动调用

**示例**:
```cpp
// 修改 Mod 数据
ModItem *mod = manager.findModByPackageId("some.packageid");
mod->remark = "这是我最喜欢的 Mod";
mod->type = "功能性";

// 立即保存
manager.saveModsToUserData();
```

---

## 清理

### clear()
```cpp
void clear()
```

清除所有缓存数据。

**效果**:
- 清空 WorkshopScanner 和 OfficialDLCScanner（删除 ModItem 对象）
- 清空 ModManager 的缓存列表
- 清空 PackageId 索引

**示例**:
```cpp
manager.clear();
// 缓存已清空，需要重新扫描
manager.scanAll();
```

---

## 完整使用示例

### 示例 1: 基本扫描和显示

```cpp
#include "data/ModManager.h"
#include "data/WorkshopScanner.h"
#include <QDebug>

int main() {
    // 创建 ModManager
    ModManager manager;
    
    // 自动检测 Steam 路径
    QString steamPath = WorkshopScanner::detectSteamPath();
    if (steamPath.isEmpty()) {
        qDebug() << "无法检测 Steam 路径";
        return 1;
    }
    
    manager.setSteamPath(steamPath);
    
    // 扫描所有 Mod
    if (!manager.scanAll()) {
        qDebug() << "扫描失败";
        return 1;
    }
    
    // 显示所有 Mod
    qDebug() << "=== 官方内容 ===";
    for (ModItem *dlc : manager.getOfficialDLCs()) {
        qDebug() << dlc->name << "-" << dlc->packageId;
    }
    
    qDebug() << "\n=== 创意工坊 Mod (前 10 个) ===";
    QList<ModItem *> workshopMods = manager.getWorkshopMods();
    for (int i = 0; i < qMin(10, workshopMods.size()); ++i) {
        qDebug() << workshopMods[i]->name;
    }
    
    return 0;
}
```

### 示例 2: 用户数据管理

```cpp
#include "data/ModManager.h"

void manageUserData() {
    ModManager manager;
    manager.setSteamPath(WorkshopScanner::detectSteamPath());
    manager.scanAll();
    
    // 查找特定 Mod
    ModItem *mod = manager.findModByPackageId("some.packageid");
    if (!mod) {
        qDebug() << "Mod 未安装";
        return;
    }
    
    // 设置备注和类型
    mod->remark = "核心前置，必须加载";
    mod->type = "前置框架";
    
    // 保存到 UserDataManager
    manager.saveModsToUserData();
    
    qDebug() << "用户数据已保存";
    
    // 清除并重新扫描，测试数据恢复
    manager.clear();
    manager.scanAll();
    
    mod = manager.findModByPackageId("some.packageid");
    qDebug() << "重新扫描后:";
    qDebug() << "  备注:" << mod->remark;
    qDebug() << "  类型:" << mod->type;
}
```

### 示例 3: 与 ModConfigManager 配合

```cpp
#include "data/ModManager.h"
#include "data/ModConfigManager.h"

void buildModList() {
    // 1. 扫描可用 Mod
    ModManager manager;
    manager.setSteamPath(WorkshopScanner::detectSteamPath());
    manager.scanAll();
    
    // 2. 构建 Mod 列表
    QList<ModItem *> selectedMods;
    
    // 添加 Core
    ModItem *core = manager.findModByPackageId("Ludeon.RimWorld");
    if (core) selectedMods.append(core);
    
    // 添加一些工坊 Mod
    for (ModItem *mod : manager.getWorkshopMods()) {
        if (mod->type == "前置框架") {
            selectedMods.append(mod);
        }
    }
    
    // 添加所有 DLC
    selectedMods.append(manager.getOfficialDLCs());
    
    // 3. 应用到配置
    ModConfigManager config;
    config.setActiveModsFromList(selectedMods);
    
    // 4. 保存到自定义位置
    UserDataManager *userMgr = manager.getUserDataManager();
    userMgr->saveModListToPath(
        "UserData/ModList/my_list.xml",
        config.getActiveMods(),
        config.getKnownExpansions(),
        config.getVersion()
    );
    
    qDebug() << "Mod 列表已保存";
}
```

### 示例 4: 批量设置类型

```cpp
void categorizeAllMods(ModManager &manager) {
    // 假设已经扫描完成
    QList<ModItem *> allMods = manager.getAllMods();
    
    for (ModItem *mod : allMods) {
        // 根据名称或描述自动分类
        QString name = mod->name.toLower();
        
        if (name.contains("harmony") || name.contains("hugslib")) {
            mod->type = "前置框架";
        } else if (name.contains("ui") || name.contains("interface")) {
            mod->type = "界面增强";
        } else if (name.contains("weapon") || name.contains("armor")) {
            mod->type = "装备";
        } else {
            mod->type = "其他";
        }
    }
    
    // 保存所有更改
    manager.saveModsToUserData();
    qDebug() << "已为" << allMods.size() << "个 Mod 设置类型";
}
```

---

## 性能考虑

### 缓存机制

- **首次扫描**: 需要遍历文件系统，可能需要 1-5 秒
- **后续访问**: 从内存缓存读取，毫秒级响应
- **内存占用**: 约 100-500 个 ModItem，每个 ~2KB，总计 ~1MB

### 优化建议

1. **避免频繁扫描**: 扫描一次后使用缓存
2. **异步扫描**: 在后台线程调用 `scanAll()`（注意线程安全）
3. **增量更新**: 只扫描变化的 Mod（未来功能）

---

## 注意事项

### 1. 内存管理

- ❌ **不要** 手动 `delete` 返回的 ModItem 指针
- ✅ **应该** 让 ModManager 管理 ModItem 的生命周期
- ✅ **应该** 在不再需要时调用 `clear()`

### 2. 线程安全

- ⚠️ ModManager 不是线程安全的
- 如需多线程，为每个线程创建独立的 ModManager 实例
- 或使用互斥锁保护访问

### 3. 数据持久化

- ✅ ModManager 析构时**自动**保存用户数据
- ✅ 也可以手动调用 `saveModsToUserData()` 立即保存
- ⚠️ 不保存 ModsConfig.xml，使用 ModConfigManager 管理

### 4. 路径要求

- Steam 路径必须包含 `steamapps/workshop/content/294100`
- 游戏路径必须包含 `Data` 文件夹
- 路径不存在时，扫描会失败但不会崩溃

---

## 相关类

- [ModItem](ModItem_API.md) - Mod 数据结构
- [ModConfigManager](ModConfigManager_API.md) - 配置文件管理
- [UserDataManager](UserDataManager_API.md) - 用户数据管理
- [WorkshopScanner](WorkshopScanner_API.md) - 创意工坊扫描
- [OfficialDLCScanner](OfficialDLCScanner_API.md) - 官方内容扫描

---

## 版本历史

- **v1.0** (2025-11-22): 初始版本，重构为中心管理类
  - 添加缓存机制
  - 整合 UserDataManager
  - 自动数据同步
