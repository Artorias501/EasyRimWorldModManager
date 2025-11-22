#ifndef MODSORTER_H
#define MODSORTER_H

#include "ModItem.h"
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

/**
 * @brief Mod 排序器
 *
 * 实现基于依赖关系和类型优先级的 Mod 排序算法
 * 排序优先级：
 * 1. Mod 之间的依赖关系（dependencies/loadBefore/loadAfter）
 * 2. 用户设置的类型优先级
 */
class ModSorter
{
public:
    ModSorter();

    /**
     * @brief 对 Mod 列表进行排序
     *
     * @param mods 要排序的 Mod 列表
     * @param typePriority 类型优先级列表（从高到低）
     * @return 排序后的 Mod 列表
     *
     * 算法流程：
     * 1. 构建依赖关系图
     * 2. 拓扑排序解决依赖约束
     * 3. 在满足依赖的前提下，按类型优先级调整顺序
     */
    static QList<ModItem *> sortMods(const QList<ModItem *> &mods, const QStringList &typePriority);

    /**
     * @brief 检查是否存在循环依赖
     *
     * @param mods Mod 列表
     * @return 如果存在循环依赖返回 true
     */
    static bool hasCircularDependency(const QList<ModItem *> &mods);

    /**
     * @brief 获取循环依赖的详细信息
     *
     * @param mods Mod 列表
     * @return 循环依赖的 Mod ID 列表
     */
    static QStringList getCircularDependencies(const QList<ModItem *> &mods);

private:
    /**
     * @brief 拓扑排序（Kahn算法）
     *
     * @param mods Mod 列表
     * @param graph 依赖图（key依赖value中的所有mod）
     * @param inDegree 入度表
     * @return 排序后的 Mod 列表
     */
    static QList<ModItem *> topologicalSort(const QList<ModItem *> &mods,
                                            const QMap<QString, QStringList> &graph,
                                            QMap<QString, int> inDegree);

    /**
     * @brief 构建依赖关系图
     *
     * @param mods Mod 列表
     * @param outGraph 输出：依赖图
     * @param outInDegree 输出：入度表
     */
    static void buildDependencyGraph(const QList<ModItem *> &mods,
                                     QMap<QString, QStringList> &outGraph,
                                     QMap<QString, int> &outInDegree);

    /**
     * @brief 获取类型优先级
     *
     * @param type Mod 类型
     * @param typePriority 类型优先级列表
     * @return 优先级值（越小越优先，未设置的为最大值）
     */
    static int getTypePriority(const QString &type, const QStringList &typePriority);

    /**
     * @brief 在满足依赖的前提下，按类型优先级稳定排序
     *
     * @param mods 已经拓扑排序的 Mod 列表
     * @param typePriority 类型优先级列表
     * @return 优化后的 Mod 列表
     */
    static QList<ModItem *> stabilizeSortByTypePriority(const QList<ModItem *> &mods,
                                                        const QStringList &typePriority);

    /**
     * @brief 检查能否安全交换两个相邻 Mod 的位置
     *
     * @param mod1 前面的 Mod
     * @param mod2 后面的 Mod
     * @return 如果可以交换返回 true
     */
    static bool canSwap(ModItem *mod1, ModItem *mod2);
};

#endif // MODSORTER_H
