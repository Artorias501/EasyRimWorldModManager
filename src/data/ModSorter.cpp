#include "ModSorter.h"
#include <QDebug>
#include <QQueue>
#include <QSet>

ModSorter::ModSorter()
{
}

QList<ModItem *> ModSorter::sortMods(const QList<ModItem *> &mods, const QStringList &typePriority)
{
    if (mods.isEmpty())
    {
        return mods;
    }

    // 1. 构建依赖关系图
    QMap<QString, QStringList> graph;
    QMap<QString, int> inDegree;
    buildDependencyGraph(mods, graph, inDegree);

    // 2. 拓扑排序
    QList<ModItem *> sorted = topologicalSort(mods, graph, inDegree);

    // 3. 在满足依赖的前提下，按类型优先级调整
    if (!typePriority.isEmpty())
    {
        sorted = stabilizeSortByTypePriority(sorted, typePriority);
    }

    return sorted;
}

void ModSorter::buildDependencyGraph(const QList<ModItem *> &mods,
                                     QMap<QString, QStringList> &outGraph,
                                     QMap<QString, int> &outInDegree)
{
    // 创建 packageId 到 ModItem 的映射
    QMap<QString, ModItem *> modMap;
    for (ModItem *mod : mods)
    {
        QString id = mod->packageId.toLower();
        modMap[id] = mod;
        outInDegree[id] = 0;
        outGraph[id] = QStringList();
    }

    // 构建依赖关系
    for (ModItem *mod : mods)
    {
        QString modId = mod->packageId.toLower();

        // 处理 dependencies（mod 依赖这些包）
        for (const QString &dep : mod->dependencies)
        {
            QString depId = dep.toLower();
            if (modMap.contains(depId))
            {
                // depId 必须在 modId 之前
                outGraph[depId].append(modId);
                outInDegree[modId]++;
            }
        }

        // 处理 loadAfter（mod 要在这些包之后加载）
        for (const QString &after : mod->loadAfter)
        {
            QString afterId = after.toLower();
            if (modMap.contains(afterId))
            {
                // afterId 必须在 modId 之前
                outGraph[afterId].append(modId);
                outInDegree[modId]++;
            }
        }

        // 处理 loadBefore（mod 要在这些包之前加载）
        for (const QString &before : mod->loadBefore)
        {
            QString beforeId = before.toLower();
            if (modMap.contains(beforeId))
            {
                // modId 必须在 beforeId 之前
                outGraph[modId].append(beforeId);
                outInDegree[beforeId]++;
            }
        }
    }
}

QList<ModItem *> ModSorter::topologicalSort(const QList<ModItem *> &mods,
                                            const QMap<QString, QStringList> &graph,
                                            QMap<QString, int> inDegree)
{
    // 创建 packageId 到 ModItem 的映射
    QMap<QString, ModItem *> modMap;
    for (ModItem *mod : mods)
    {
        modMap[mod->packageId.toLower()] = mod;
    }

    QList<ModItem *> result;
    QQueue<QString> queue;

    // 将所有入度为 0 的节点加入队列
    for (auto it = inDegree.begin(); it != inDegree.end(); ++it)
    {
        if (it.value() == 0)
        {
            queue.enqueue(it.key());
        }
    }

    // Kahn 算法
    while (!queue.isEmpty())
    {
        QString current = queue.dequeue();
        result.append(modMap[current]);

        // 遍历所有依赖 current 的节点
        for (const QString &neighbor : graph[current])
        {
            inDegree[neighbor]--;
            if (inDegree[neighbor] == 0)
            {
                queue.enqueue(neighbor);
            }
        }
    }

    // 如果结果数量不等于输入数量，说明存在循环依赖
    if (result.size() != mods.size())
    {
        qWarning() << "检测到循环依赖，无法完全排序";
        // 将未排序的节点也加入结果（虽然顺序可能不正确）
        for (ModItem *mod : mods)
        {
            if (!result.contains(mod))
            {
                result.append(mod);
            }
        }
    }

    return result;
}

int ModSorter::getTypePriority(const QString &type, const QStringList &typePriority)
{
    int index = typePriority.indexOf(type);
    if (index >= 0)
    {
        return index;
    }
    // 未在优先级列表中的类型，优先级最低
    return 99999;
}

QList<ModItem *> ModSorter::stabilizeSortByTypePriority(const QList<ModItem *> &mods,
                                                        const QStringList &typePriority)
{
    QList<ModItem *> result = mods;

    // 使用冒泡排序的思想，但只交换不破坏依赖关系的相邻元素
    bool swapped = true;
    while (swapped)
    {
        swapped = false;
        for (int i = 0; i < result.size() - 1; i++)
        {
            ModItem *mod1 = result[i];
            ModItem *mod2 = result[i + 1];

            int priority1 = getTypePriority(mod1->type, typePriority);
            int priority2 = getTypePriority(mod2->type, typePriority);

            // 如果 mod2 优先级更高且可以安全交换，则交换
            if (priority2 < priority1 && canSwap(mod1, mod2))
            {
                result.swapItemsAt(i, i + 1);
                swapped = true;
            }
        }
    }

    return result;
}

bool ModSorter::canSwap(ModItem *mod1, ModItem *mod2)
{
    QString id1 = mod1->packageId.toLower();
    QString id2 = mod2->packageId.toLower();

    // 检查 mod1 是否依赖 mod2
    for (const QString &dep : mod1->dependencies)
    {
        if (dep.toLower() == id2)
            return false;
    }
    for (const QString &after : mod1->loadAfter)
    {
        if (after.toLower() == id2)
            return false;
    }

    // 检查 mod2 是否必须在 mod1 之前
    for (const QString &before : mod1->loadBefore)
    {
        if (before.toLower() == id2)
            return false;
    }

    // 检查 mod2 是否依赖 mod1（如果是，不能交换）
    for (const QString &dep : mod2->dependencies)
    {
        if (dep.toLower() == id1)
            return false;
    }
    for (const QString &before : mod2->loadBefore)
    {
        if (before.toLower() == id1)
            return false;
    }
    for (const QString &after : mod2->loadAfter)
    {
        if (after.toLower() == id1)
            return false;
    }

    return true;
}

bool ModSorter::hasCircularDependency(const QList<ModItem *> &mods)
{
    QMap<QString, QStringList> graph;
    QMap<QString, int> inDegree;
    buildDependencyGraph(mods, graph, inDegree);

    QList<ModItem *> sorted = topologicalSort(mods, graph, inDegree);
    return sorted.size() != mods.size();
}

QStringList ModSorter::getCircularDependencies(const QList<ModItem *> &mods)
{
    QMap<QString, QStringList> graph;
    QMap<QString, int> inDegree;
    buildDependencyGraph(mods, graph, inDegree);

    // 创建副本用于修改
    QMap<QString, int> inDegreeCopy = inDegree;
    QSet<QString> visited;
    QQueue<QString> queue;

    // 将所有入度为 0 的节点加入队列
    for (auto it = inDegreeCopy.begin(); it != inDegreeCopy.end(); ++it)
    {
        if (it.value() == 0)
        {
            queue.enqueue(it.key());
        }
    }

    // 移除所有可以拓扑排序的节点
    while (!queue.isEmpty())
    {
        QString current = queue.dequeue();
        visited.insert(current);

        for (const QString &neighbor : graph[current])
        {
            inDegreeCopy[neighbor]--;
            if (inDegreeCopy[neighbor] == 0)
            {
                queue.enqueue(neighbor);
            }
        }
    }

    // 剩下的节点就是循环依赖的节点
    QStringList circular;
    for (auto it = inDegreeCopy.begin(); it != inDegreeCopy.end(); ++it)
    {
        if (it.value() > 0)
        {
            circular.append(it.key());
        }
    }

    return circular;
}
