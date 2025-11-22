#include "ModItem.h"

void ModItem::addDependency(const QString &dependency) {
    if (!dependency.isEmpty() && !dependencies.contains(dependency)) {
        dependencies.append(dependency);
    }
}

void ModItem::addLoadBefore(const QString &modId) {
    if (!modId.isEmpty() && !loadBefore.contains(modId)) {
        loadBefore.append(modId);
    }
}

void ModItem::addLoadAfter(const QString &modId) {
    if (!modId.isEmpty() && !loadAfter.contains(modId)) {
        loadAfter.append(modId);
    }
}

void ModItem::addIncompatibleWith(const QString &modId) {
    if (!modId.isEmpty() && !incompatibleWith.contains(modId)) {
        incompatibleWith.append(modId);
    }
}

void ModItem::addSupportedVersion(const QString &version) {
    if (!version.isEmpty() && !supportedVersions.contains(version)) {
        supportedVersions.append(version);
    }
}

bool ModItem::isValid() const {
    return !identifier.isEmpty();
}
