#include "playlist_manager.h"

PlaylistManager::PlaylistManager(QObject *parent)
    : QObject(parent)
    , playlistContainer(nullptr)
    , playlistWidget(nullptr)
    , togglePlaylistButton(nullptr)
{
    QDir appDataDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!appDataDir.exists()) {
        appDataDir.mkpath(".");
    }
    defaultPlaylistFile = appDataDir.filePath("default.m3u");
}

PlaylistManager::~PlaylistManager()
{
}

void PlaylistManager::setupPlaylist(QWidget *container, QListWidget *playlist, QPushButton *toggleButton)
{
    playlistContainer = container;
    playlistWidget = playlist;
    togglePlaylistButton = toggleButton;
    
    // 连接播放列表信号
    connect(playlistWidget, &QListWidget::itemDoubleClicked,
            this, &PlaylistManager::playlistItemDoubleClicked);
    
    // 设置播放列表右键菜单
    playlistWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(playlistWidget, &QWidget::customContextMenuRequested,
            this, &PlaylistManager::showPlaylistContextMenu);
    
    // 启用拖拽排序
    playlistWidget->setDragDropMode(QAbstractItemView::InternalMove);
    playlistWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    
    // 添加播放列表项变化监听
    connect(playlistWidget->model(), &QAbstractItemModel::rowsMoved,
            this, &PlaylistManager::saveDefaultPlaylist);
    
    // 连接播放列表切换按钮信号
    if (togglePlaylistButton) {
        connect(togglePlaylistButton, &QPushButton::clicked,
                this, &PlaylistManager::togglePlaylist);
    }
    
    // 设置播放列表样式
    setupPlaylistStyle();
    
    // 加载默认播放列表
    loadDefaultPlaylist();
}

void PlaylistManager::setupPlaylistStyle()
{
    if (playlistWidget) {
        playlistWidget->setStyleSheet(R"(
            QListWidget {
                background-color: #2b2b2b;
                border: none;
            }
            QListWidget::item {
                color: #ffffff;
                padding: 4px;
                border-bottom: 1px solid #3a3a3a;
            }
            QListWidget::item:selected {
                background-color: #3a3a3a;
            }
            QListWidget::item:hover {
                background-color: #323232;
            }
        )");
    }
}

void PlaylistManager::addToPlaylist()
{
    QStringList files = QFileDialog::getOpenFileNames(nullptr,
        "添加到播放列表",
        "",
        "媒体文件 (*.mp4 *.avi *.mkv *.mp3 *.wav);;所有文件 (*.*)");

    for (const QString& file : files) {
        QListWidgetItem* item = new QListWidgetItem(QFileInfo(file).fileName());
        item->setData(Qt::UserRole, file);
        playlistWidget->addItem(item);
    }

    // 自动保存到默认播放列表
    saveDefaultPlaylist();
}

void PlaylistManager::removeFromPlaylist()
{
    QList<QListWidgetItem*> items = playlistWidget->selectedItems();
    for (QListWidgetItem* item : items) {
        delete playlistWidget->takeItem(playlistWidget->row(item));
    }

    // 自动保存到默认播放列表
    saveDefaultPlaylist();
}

// 已删除savePlaylist和loadPlaylist函数

void PlaylistManager::togglePlaylist()
{
    if (playlistContainer) {
        playlistContainer->setVisible(!playlistContainer->isVisible());
    }
}

void PlaylistManager::showPlaylistContextMenu(const QPoint &pos)
{
    QList<QListWidgetItem*> selectedItems = playlistWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        return;
    }

    QMenu contextMenu;

    // 播放选中项
    if (selectedItems.count() == 1) {
        QAction *playAction = new QAction("播放", &contextMenu);
        connect(playAction, &QAction::triggered, this, [this, selectedItems]() {
            emit playRequested(selectedItems.first()->data(Qt::UserRole).toString());
        });
        contextMenu.addAction(playAction);
        contextMenu.addSeparator();
    }

    // 删除选中项
    QAction *deleteAction = new QAction("删除选中项", &contextMenu);
    connect(deleteAction, &QAction::triggered, this, [this, selectedItems]() {
        QString message = selectedItems.count() == 1
            ? "确定要删除选中的文件吗？"
            : QString("确定要删除选中的 %1 个文件吗？").arg(selectedItems.count());

        if (QMessageBox::question(nullptr, "确认删除", message) == QMessageBox::Yes) {
            for (QListWidgetItem* item : selectedItems) {
                delete playlistWidget->takeItem(playlistWidget->row(item));
            }
            saveDefaultPlaylist();  // 保存更改
        }
    });
    contextMenu.addAction(deleteAction);

    // 添加排序菜单
    contextMenu.addSeparator();
    QMenu *sortMenu = contextMenu.addMenu("排序");

    QAction *sortByNameAct = new QAction("按名称排序", &contextMenu);
    connect(sortByNameAct, &QAction::triggered, this, [this]() {
        playlistWidget->sortItems(Qt::AscendingOrder);
        saveDefaultPlaylist();
    });
    sortMenu->addAction(sortByNameAct);

    contextMenu.exec(playlistWidget->mapToGlobal(pos));
}

QListWidgetItem* PlaylistManager::getCurrentItem() const
{
    return playlistWidget ? playlistWidget->currentItem() : nullptr;
}

int PlaylistManager::getPlaylistCount() const
{
    return playlistWidget ? playlistWidget->count() : 0;
}

QListWidgetItem* PlaylistManager::getItemAt(int index) const
{
    return playlistWidget ? playlistWidget->item(index) : nullptr;
}

void PlaylistManager::setCurrentRow(int row)
{
    if (playlistWidget) {
        playlistWidget->setCurrentRow(row);
    }
}

int PlaylistManager::getCurrentRow() const
{
    return playlistWidget ? playlistWidget->currentRow() : -1;
}

void PlaylistManager::saveDefaultPlaylist()
{
    if (!defaultPlaylistFile.isEmpty() && playlistWidget) {
        QFile file(defaultPlaylistFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (int i = 0; i < playlistWidget->count(); ++i) {
                out << playlistWidget->item(i)->data(Qt::UserRole).toString() << "\n";
            }
        }
    }
}

void PlaylistManager::loadDefaultPlaylist()
{
    if (!defaultPlaylistFile.isEmpty() && QFile::exists(defaultPlaylistFile) && playlistWidget) {
        QFile file(defaultPlaylistFile);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            playlistWidget->clear();
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString filePath = in.readLine().trimmed();
                if (!filePath.isEmpty() && QFile::exists(filePath)) {
                    QListWidgetItem* item = new QListWidgetItem(QFileInfo(filePath).fileName());
                    item->setData(Qt::UserRole, filePath);
                    playlistWidget->addItem(item);
                }
            }
        }
    }
}
