#ifndef MODDETAILPANEL_H
#define MODDETAILPANEL_H

#include "../data/ModItem.h"
#include "../data/ModManager.h"
#include <QWidget>
#include <QListWidget>

QT_BEGIN_NAMESPACE

namespace Ui {
    class ModDetailPanel;
}

QT_END_NAMESPACE

class ModDetailPanel : public QWidget {
    Q_OBJECT

public:
    explicit ModDetailPanel(QWidget *parent = nullptr);

    ~ModDetailPanel();

    void setModItem(ModItem *mod, ModManager *manager);

    void clearDisplay();

    void refreshTypeComboBox();

signals:
    void modDetailsChanged();

private slots:
    void onSaveClicked();

private:
    Ui::ModDetailPanel *ui;
    ModItem *currentMod;
    ModManager *modManager;

    void loadModTypes();

    void displayModInfo();

    void displayDependencies();

    void adjustListHeight(QListWidget *listWidget);
};

#endif // MODDETAILPANEL_H
