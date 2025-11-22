# ModManager 重构文档

## 重构目标

将 `ModManager` 重构为项目的**中心管理类**，整合 `OfficialDLCScanner`、`WorkshopScanner` 和 `UserDataManager`，实现统一的数据管理和访问接口。

## 重构内容

### 1. 架构调整

**重构前：**
```
外部代码
  ├── 直接使用 OfficialDLCScanner
  ├── 直接使用 WorkshopScanner  
  ├── 直接使用 UserDataManager
  └── 使用 ModManager（功能有限）
```

**重构后：**
```
外部代码
  └── 只使用 ModManager
       ├── 内部管理 OfficialDLCScanner（私有）
       ├── 内部管理 WorkshopScanner（私有）
       ├── 内部管理 UserDataManager
       └── 提供统一的数据访问接口
```

### 2. ModManager 新增功能

#### 2.1 缓存机制

新增两个缓存字段：
```cpp
QList<ModItem *> m_cachedWorkshopMods;   // 缓存的工坊Mod列表
QList<ModItem *> m_cachedOfficialDLCs;   // 缓存的官方DLC列表
```

**工作流程：**
1. 扫描时，调用扫描器获取数据
2. 将扫描结果缓存到 ModManager
3. 所有 `getAllMods()`、`getWorkshopMods()`、`getOfficialDLCs()` 都从缓存返回

#### 2.2 UserDataManager 联动

新增 UserDataManager 实例：
```cpp
UserDataManager *m_userDataManager;
```

**新增方法：**

##### `loadUserDataToMods()`
从 UserDataManager 加载备注和类型到所有缓存的 ModItem：
```cpp
void ModManager::loadUserDataToMods()
{
    QList<ModItem *> allMods = getAllMods();
    
    for (ModItem *mod : allMods)
    {
        // 从UserDataManager加载类型
        QString type = m_userDataManager->getModType(mod->packageId);
        if (!type.isEmpty())
            mod->type = type;
        
        // 从UserDataManager加载备注
        QString remark = m_userDataManager->getModRemark(mod->packageId);
        if (!remark.isEmpty())
            mod->remark = remark;
    }
}
```

##### `saveModsToUserData()`
将所有缓存的 ModItem 的备注和类型保存到 UserDataManager：
```cpp
void ModManager::saveModsToUserData()
{
    QList<ModItem *> allMods = getAllMods();
    
    for (ModItem *mod : allMods)
    {
        // 保存类型
        if (!mod->type.isEmpty())
            m_userDataManager->setModType(mod->packageId, mod->type);
        
        // 保存备注
        if (!mod->remark.isEmpty())
            m_userDataManager->setModRemark(mod->packageId, mod->remark);
    }
    
    // 持久化到文件
    m_userDataManager->saveModData();
}
```

#### 2.3 自动数据同步

**在构造函数中：**
```cpp
ModManager::ModManager()
{
    // ...
    m_userDataManager = new UserDataManager();
    
    // 初始化用户数据目录
    UserDataManager::initializeDirectories();
    
    // 自动加载用户数据
    m_userDataManager->loadAll();
}
```

**在 scanAll() 中：**
```cpp
bool ModManager::scanAll()
{
    // 扫描工坊Mod和官方DLC
    scanWorkshopMods();
    scanOfficialDLCs();
    
    // 自动从UserDataManager加载备注和类型
    loadUserDataToMods();
    
    return true;
}
```

**在析构函数中：**
```cpp
ModManager::~ModManager()
{
    // 自动保存用户数据
    saveModsToUserData();
    m_userDataManager->saveAll();
    
    // 清理资源
    delete m_workshopScanner;
    delete m_dlcScanner;
    delete m_userDataManager;
}
```

### 3. 扫描器职责调整

#### 3.1 OfficialDLCScanner 和 WorkshopScanner

**重构后的定位：**
- 仅作为 ModManager 的**私有服务**
- 只负责扫描文件系统，返回 ModItem 对象
- 不再直接暴露给外部代码使用

**注意：**
- 扫描器仍然是 public 类（因为需要在头文件中声明）
- 但在使用上，应该只被 ModManager 调用
- WorkshopScanner::detectSteamPath() 这样的静态工具方法除外

### 4. 测试代码更新

所有测试函数都已更新为通过 ModManager 访问：

**测试1：扫描所有已安装的mod和DLC**
```cpp
ModManager modManager;
modManager.setSteamPath(steamPath);
modManager.scanAll();  // 自动缓存并加载用户数据

QList<ModItem *> workshopMods = modManager.getWorkshopMods();
QList<ModItem *> officialDLCs = modManager.getOfficialDLCs();
```

**测试3/4：使用 ModManager 的 UserDataManager**
```cpp
UserDataManager *userDataManager = modManager.getUserDataManager();
userDataManager->saveModListToPath(...);
```

## 使用示例

### 基本用法

```cpp
// 1. 创建 ModManager（会自动加载用户数据）
ModManager modManager;
modManager.setSteamPath("C:/Program Files (x86)/Steam");

// 2. 扫描所有mod（会自动缓存并同步用户数据）
modManager.scanAll();

// 3. 从缓存获取数据
QList<ModItem *> allMods = modManager.getAllMods();
QList<ModItem *> workshopMods = modManager.getWorkshopMods();
QList<ModItem *> officialDLCs = modManager.getOfficialDLCs();

// 4. 修改ModItem的备注和类型
workshopMods[0]->remark = "这是我最喜欢的mod";
workshopMods[0]->type = "功能性";

// 5. 保存到UserDataManager
modManager.saveModsToUserData();

// 6. 清除并重新扫描（会自动恢复用户数据）
modManager.clear();
modManager.scanAll();
```

### 访问 UserDataManager

```cpp
ModManager modManager;

// 获取 UserDataManager 实例
UserDataManager *userDataManager = modManager.getUserDataManager();

// 保存 Mod 列表
userDataManager->saveModListToPath(
    "UserData/ModList/my_list.xml",
    activeMods,
    knownExpansions,
    version
);

// 获取自定义类型列表
QStringList customTypes = userDataManager->getAllCustomTypes();
```

## 数据流图

```
┌─────────────────────────────────────────────────────┐
│                    ModManager                        │
│  (中心管理类，协调所有组件)                          │
├─────────────────────────────────────────────────────┤
│  缓存:                                               │
│  • m_cachedWorkshopMods    (工坊Mod列表)            │
│  • m_cachedOfficialDLCs    (官方DLC列表)            │
│  • m_packageIdMap          (PackageId索引)          │
├─────────────────────────────────────────────────────┤
│  组件:                                               │
│  • WorkshopScanner         (扫描Steam创意工坊)      │
│  • OfficialDLCScanner      (扫描官方DLC)            │
│  • UserDataManager         (用户数据持久化)         │
└─────────────────────────────────────────────────────┘
           │                    │                 │
           ▼                    ▼                 ▼
    ┌─────────────┐     ┌─────────────┐   ┌──────────────┐
    │ 文件系统     │     │ 文件系统     │   │ JSON 文件     │
    │ Workshop/    │     │ Data/       │   │ UserData/    │
    └─────────────┘     └─────────────┘   └──────────────┘
```

## 工作流程

### 扫描流程

```
1. modManager.scanAll()
   ├─ 调用 scanWorkshopMods()
   │   ├─ m_workshopScanner->scanAllMods()
   │   └─ 缓存到 m_cachedWorkshopMods
   │
   ├─ 调用 scanOfficialDLCs()
   │   ├─ m_dlcScanner->scanAllDLCs()
   │   └─ 缓存到 m_cachedOfficialDLCs
   │
   └─ 自动调用 loadUserDataToMods()
       └─ 从 UserDataManager 加载备注和类型到 ModItem
```

### 数据同步流程

```
用户修改 ModItem.remark 或 ModItem.type
   │
   ▼
modManager.saveModsToUserData()
   │
   ├─ 遍历所有 ModItem
   ├─ 调用 UserDataManager.setModType()
   ├─ 调用 UserDataManager.setModRemark()
   └─ 调用 UserDataManager.saveModData()
       └─ 持久化到 JSON 文件
```

### 数据恢复流程

```
modManager.scanAll()
   │
   └─ loadUserDataToMods()
       │
       ├─ 遍历所有 ModItem
       ├─ 调用 UserDataManager.getModType()
       ├─ 调用 UserDataManager.getModRemark()
       └─ 写入 ModItem.type 和 ModItem.remark
```

## 优势

### 1. 统一接口
- 外部代码只需要与 ModManager 交互
- 降低了代码耦合度
- 简化了使用流程

### 2. 自动数据同步
- 扫描后自动加载用户数据
- 销毁前自动保存用户数据
- 无需手动管理数据同步

### 3. 缓存机制
- 避免重复扫描文件系统
- 提高性能
- 数据访问更快速

### 4. 职责清晰
- **OfficialDLCScanner**: 只负责扫描官方DLC
- **WorkshopScanner**: 只负责扫描创意工坊Mod
- **UserDataManager**: 只负责用户数据持久化
- **ModManager**: 协调所有组件，提供统一接口

### 5. 易于扩展
- 未来新增功能只需在 ModManager 中实现
- 不影响现有的扫描器和数据管理器
- 符合单一职责原则

## 注意事项

### 1. 内存管理
- ModItem 对象由扫描器创建和管理
- ModManager 的缓存只存储指针
- 调用 `clear()` 时，扫描器会删除 ModItem 对象

### 2. 数据一致性
- 修改 ModItem 后，需要调用 `saveModsToUserData()` 保存
- 或者等到 ModManager 析构时自动保存

### 3. 外部依赖
- WorkshopScanner::detectSteamPath() 仍然是公开的静态方法
- 可以在创建 ModManager 前调用以检测路径

## 未来改进

### 1. 异步扫描
```cpp
QFuture<bool> ModManager::scanAllAsync();
```

### 2. 增量更新
```cpp
bool ModManager::updateChangedMods();
```

### 3. 事件通知
```cpp
signal void modsScanned(int count);
signal void userDataLoaded();
```

### 4. 过滤和排序
```cpp
QList<ModItem *> ModManager::getModsByType(const QString &type);
QList<ModItem *> ModManager::getModsSortedByName();
```

---

**重构日期**: 2025年11月22日  
**重构版本**: v1.0  
**重构人员**: GitHub Copilot (Claude Sonnet 4.5)
