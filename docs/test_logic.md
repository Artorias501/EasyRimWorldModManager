# 逻辑验证示例

## 场景说明

假设用户有以下mod：

### 从OfficialDLCScanner扫描到的：
1. `ludeon.rimworld.core` (Core)
   - `isOfficialDLC = false`
   - `type = "Core"`
   
2. `ludeon.rimworld.royalty` (Royalty DLC)
   - `isOfficialDLC = true`
   - `type = "DLC"`
   
3. `ludeon.rimworld.ideology` (Ideology DLC)
   - `isOfficialDLC = true`
   - `type = "DLC"`

### 从WorkshopScanner扫描到的：
4. `brrainz.harmony` (Harmony框架)
   - `isOfficialDLC = false`
   - `type = ""` (空或用户自定义)
   
5. `fluffy.modmanager` (Mod Manager)
   - `isOfficialDLC = false`
   - `type = ""` (空或用户自定义)

## 生成的XML配置

当调用 `ModConfigManager.setActiveModsFromList(所有5个mod)` 时：

```cpp
// 伪代码逻辑
QList<ModItem*> allMods = {core, royalty, ideology, harmony, modmanager};
modConfigManager.setActiveModsFromList(allMods);
```

### 生成结果：

```xml
<?xml version="1.0" encoding="utf-8"?>
<ModsConfigData>
  <version>1.6.4633 rev1261</version>
  
  <activeMods>
    <li>ludeon.rimworld.core</li>      <!-- ✅ Core在activeMods中 -->
    <li>ludeon.rimworld.royalty</li>   <!-- ✅ DLC在activeMods中 -->
    <li>ludeon.rimworld.ideology</li>  <!-- ✅ DLC在activeMods中 -->
    <li>brrainz.harmony</li>           <!-- ✅ 创意工坊mod在activeMods中 -->
    <li>fluffy.modmanager</li>         <!-- ✅ 创意工坊mod在activeMods中 -->
  </activeMods>
  
  <knownExpansions>
    <li>ludeon.rimworld.royalty</li>   <!-- ✅ 仅DLC (isOfficialDLC=true) -->
    <li>ludeon.rimworld.ideology</li>  <!-- ✅ 仅DLC (isOfficialDLC=true) -->
    <!-- ❌ Core不在这里 (isOfficialDLC=false) -->
    <!-- ❌ 创意工坊mod不在这里 (isOfficialDLC=false) -->
  </knownExpansions>
</ModsConfigData>
```

## 代码实现

### ModConfigManager::setActiveModsFromList 实现

```cpp
void ModConfigManager::setActiveModsFromList(const QList<ModItem*> &modList)
{
    m_activeMods.clear();
    m_knownExpansions.clear();

    for (ModItem *mod : modList)
    {
        if (!mod || !mod->isValid())
        {
            continue;
        }

        // 所有mod都加入activeMods
        m_activeMods.append(mod->packageId);

        // 只有官方DLC（isOfficialDLC=true）才加入knownExpansions
        // Core不会被加入，因为在OfficialDLCScanner中已经将其isOfficialDLC设为false
        if (mod->isOfficialDLC)
        {
            m_knownExpansions.append(mod->packageId);
        }
    }
}
```

### OfficialDLCScanner::scanDLCDirectory 实现

```cpp
ModItem *OfficialDLCScanner::scanDLCDirectory(const QString &dlcDirPath)
{
    // ... 解析About.xml ...

    // 判断是否为Core（ludeon.rimworld或ludeon.rimworld.core）
    if (dlc->packageId.compare("ludeon.rimworld", Qt::CaseInsensitive) == 0 ||
        dlc->packageId.compare("ludeon.rimworld.core", Qt::CaseInsensitive) == 0)
    {
        dlc->isOfficialDLC = false;  // ❗ Core不是DLC
        dlc->type = "Core";
    }
    else
    {
        dlc->isOfficialDLC = true;   // ✅ 其他都是DLC
        dlc->type = "DLC";
    }

    return dlc;
}
```

## 验证要点

✅ **activeMods 包含所有mod**
- Core
- 官方DLC
- 创意工坊mod

✅ **knownExpansions 只包含官方DLC**
- 有 `ludeon.rimworld.royalty`
- 有 `ludeon.rimworld.ideology`
- 没有 `ludeon.rimworld.core` (Core不是扩展包)
- 没有创意工坊mod

✅ **Core的特殊处理**
- `isOfficialDLC = false` (写死在OfficialDLCScanner中)
- 在 `activeMods` 中
- 不在 `knownExpansions` 中

✅ **版本号保留**
- 从原文件读取的 `version` 字段原样写回
