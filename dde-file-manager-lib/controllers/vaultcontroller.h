/*

 * Copyright (C) 2020 ~ 2020 Deepin Technology Co., Ltd.

 *

 * Author:     lixiang

 *

 * Maintainer: lixianga@uniontech.com

 *

 * brief:

 *

 * date:    2020-05-08

 */

#ifndef VAULTCONTROLLER_H
#define VAULTCONTROLLER_H

#include "dabstractfilecontroller.h"

#include <DSecureString>

DCORE_USE_NAMESPACE

class VaultControllerPrivate;
class VaultController : public DAbstractFileController
{
    Q_OBJECT
public:
    enum VaultState {
        NotExisted,
        Encrypted,
        Unlocked,
        UnderProcess,
        Broken,
        NotAvailable
    };

    explicit VaultController(QObject *parent = nullptr);
public:

    static VaultController * getVaultController();

    const DAbstractFileInfoPointer createFileInfo(const QSharedPointer<DFMCreateFileInfoEvent> &event) const override;
    const DDirIteratorPointer createDirIterator(const QSharedPointer<DFMCreateDiriterator> &event) const override;
    DAbstractFileWatcher *createFileWatcher(const QSharedPointer<DFMCreateFileWatcherEvent> &event) const override;

    bool openFile(const QSharedPointer<DFMOpenFileEvent> &event) const override;
    bool openFiles(const QSharedPointer<DFMOpenFilesEvent> &event) const Q_DECL_OVERRIDE;
    bool deleteFiles(const QSharedPointer<DFMDeleteEvent> &event) const Q_DECL_OVERRIDE;
    DUrlList moveToTrash(const QSharedPointer<DFMMoveToTrashEvent> &event) const override;
    DUrlList pasteFile(const QSharedPointer<DFMPasteEvent> &event) const override;
    bool writeFilesToClipboard(const QSharedPointer<DFMWriteUrlsToClipboardEvent> &event) const override;
    bool renameFile(const QSharedPointer<DFMRenameEvent> &event) const override;
    /**
     * @brief shareFolder 设置文件夹共享
     * @param event       共享的信息事件
     * @return            是否共享成功
     */
    bool shareFolder(const QSharedPointer<DFMFileShareEvent> &event) const Q_DECL_OVERRIDE;

    /**
     * @brief unShareFolder 取消文件夹共享
     * @param event         关闭共享信息事件
     * @return              是否取消成功
     */
    bool unShareFolder(const QSharedPointer<DFMCancelFileShareEvent> &event) const Q_DECL_OVERRIDE;

    /**
     * @brief openInTerminal  右键菜单打开当前路径终端
     * @param event           打开终端信息事件
     * @return                是否打开终端成功
     */
    bool openInTerminal(const QSharedPointer<DFMOpenInTerminalEvent> &event) const Q_DECL_OVERRIDE;

    /**
     * @brief addToBookmark   添加当前文件夹书签
     * @param event           添加书签信息事件
     * @return                是否添加成功
     */
    bool addToBookmark(const QSharedPointer<DFMAddToBookmarkEvent> &event) const override;

    /**
     * @brief removeBookmark  移除当前文件夹书签
     * @param event           移除书签信息事件
     * @return                是否移除成功
     */
    bool removeBookmark(const QSharedPointer<DFMRemoveBookmarkEvent> &event) const override;

    /**
     * @brief createSymlink  创建快捷方式
     * @param event          创建快捷方式信息事件
     * @return               是否创建成功
     */
    bool createSymlink(const QSharedPointer<DFMCreateSymlinkEvent> &event) const Q_DECL_OVERRIDE;

    /**
     * @brief setFileTags    设置文件标记信息
     * @param event          设置文件标记信息事件
     * @return               是否标记成功
     */
    bool setFileTags(const QSharedPointer<DFMSetFileTagsEvent> &event) const Q_DECL_OVERRIDE;

    /**
     * @brief removeTagsOfFile  移除文件标记信息
     * @param event             移除文件标记信息事件
     * @return                  是否移除文件标记
     */
    bool removeTagsOfFile(const QSharedPointer<DFMRemoveTagsOfFileEvent> &event) const Q_DECL_OVERRIDE;

    /**
     * @brief getTagsThroughFiles   获取文件标记信息
     * @param event                 获取文件标记信息事件
     * @return                      标记信息列表
     */
    QList<QString> getTagsThroughFiles(const QSharedPointer<DFMGetTagsThroughFilesEvent> &event) const Q_DECL_OVERRIDE;

    bool setPermissions(const QSharedPointer<DFMSetPermissionEvent> &event) const Q_DECL_OVERRIDE;

    static DUrl makeVaultUrl(QString path = "", QString host = "files");
    static DUrl localUrlToVault(const DUrl &vaultUrl);
    static DUrl localToVault(QString localPath);
    static QString vaultToLocal(const DUrl &vaultUrl);
    static DUrl vaultToLocalUrl(const DUrl &vaultUrl);
    static DUrlList vaultToLocalUrls(DUrlList vaultUrls);

    /**
     * @brief state         获取当前保险箱状态
     * @param lockBaseDir   保险箱加密文件夹
     * @return              返回VaultState枚举值
     */
    VaultState state(QString lockBaseDir = "");

public slots:

    /**
     * @brief createVault       创建保险箱
     * @param lockBaseDir       保险箱加密文件夹 默认值内部自动创建
     * @param unlockFileDir     保险箱解密文件夹 默认值内部自动创建
     * @param passWord          保险箱密码

     */
    void createVault(const DSecureString &password, QString lockBaseDir = "", QString unlockFileDir = "");

    /**
     * @brief unlockVault       解锁保险箱
     * @param lockBaseDir       保险箱加密文件夹 默认值内部自动创建
     * @param unlockFileDir     保险箱解密文件夹 默认值内部自动创建
     * @param passWord          保险箱密码
     */
    void unlockVault(const DSecureString &password, QString lockBaseDir = "", QString unlockFileDir = "");

    /**
     * @brief lockVault         加锁保险箱
     * @param lockBaseDir       保险箱加密文件夹 默认值内部自动创建
     * @param unlockFileDir     保险箱解密文件夹 默认值内部自动创建
     */
    void lockVault(QString lockBaseDir = "", QString unlockFileDir = "");

    /**
     * @brief makeVaultLocalPath    创建本地路径
     * @param path                  子目录(文件或文件路径)
     * @param base                  父目录
     * @return                      返回新路径
     */
    static QString makeVaultLocalPath(QString path = "", QString base = "");

    /**
     * @brief vaultLockPath   返回默认保险箱加密文件夹路径，如路径是外部传入暂时无法获取
     * @return                默认保险箱加密文件夹路径
     */
    static QString vaultLockPath();

    /**
     * @brief vaultLockPath   返回默认保险箱解密文件夹路径，如路径是外部传入暂时无法获取
     * @return                默认保险箱解密文件夹路径
     */
    static QString vaultUnlockPath();

signals:
    /**
     * @brief readError 错误输出
     * @param error     错误信息
     */
    void signalReadError(QString error);

    /**
     * @brief signalReadOutput  标准输出
     * @param msg               输出信息
     */
    void signalReadOutput(QString msg);

    /**
     * @brief signalCreateVault 创建保险箱是否成功的信号
     * @param state             返回ErrorCode枚举值
     */
    void signalCreateVault(int state);

    /**
     * @brief singalUnlockVault 解锁保险箱是否成功的信号
     * @param state             返回ErrorCode枚举值
     */
    void signalUnlockVault(int state);

    /**
     * @brief signalLockVault   加锁保险箱是否成功的信号
     * @param state             返回ErrorCode枚举值
     */
    void signalLockVault(int state);

    /**
     * @brief signalCalculationVaultFinish  计算保险箱大小完成
     */
    void signalCalculationVaultFinish() const;

signals:
    /**
    * @brief 下列信号为本类内部使用，请勿外用
    */

    /**
     * @brief sigCreateVault    创建保险箱信号
     * @param lockBaseDir       保险箱加密文件夹
     * @param unlockFileDir     保险箱解密文件夹
     * @param passWord          保险箱密码
     */
    void sigCreateVault(QString lockBaseDir, QString unlockFileDir, QString passWord);

    /**
     * @brief sigUnlockVault    解锁保险箱信号
     * @param lockBaseDir       保险箱加密文件夹
     * @param unlockFileDir     保险箱解密文件夹
     * @param passWord          保险箱密码
     */
    void sigUnlockVault(QString lockBaseDir, QString unlockFileDir, QString passWord);

    /**
     * @brief sigLockVault      加锁保险箱信号
     * @param unlockFileDir     保险箱解密文件夹
     */
    void sigLockVault(QString unlockFileDir);

private:
    VaultControllerPrivate * d_ptr;

    static VaultController * cryfs;

    Q_DECLARE_PRIVATE(VaultController)
};


class VaultCalculation : public QObject
{
    Q_OBJECT
private:
    VaultCalculation(QObject * parent = nullptr);

public:

    static VaultCalculation * Initialize();

    /**
     * @brief calculationVault 计算保险箱大小
     */
    void calculationVault();

public:
    bool m_flg;

private:
    static VaultCalculation * m_vaultCalculation;


};

#endif
