/*
 * Copyright (C) 2019 Deepin Technology Co., Ltd.
 *
 * Author:     Mike Chen <kegechen@gmail.com>
 *
 * Maintainer: Mike Chen <chenke_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "dfmtagwidget.h"
#include "dfileservices.h"
#include "app/define.h"
#include "controllers/pathmanager.h"
#include "singleton.h"
#include <dfilemenumanager.h>
#include <shutil/desktopfile.h>
#include <tag/tagmanager.h>
#include <QLabel>
#include <QVBoxLayout>
#include <dabstractfilewatcher.h>

DFM_BEGIN_NAMESPACE

class DFMCrumbEdit : public DCrumbEdit{
public:
    DFMCrumbEdit(QWidget *parent = nullptr):DCrumbEdit(parent){}
    bool isEditing(){
        return m_isEditByDoubleClick;
    }
protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override{
        m_isEditByDoubleClick = true;
        DCrumbEdit::mouseDoubleClickEvent(event);
        m_isEditByDoubleClick = false;
    }

private:
    bool m_isEditByDoubleClick{false};
};

class DFMTagWidgetPrivate : public QSharedData
{
public:
    explicit DFMTagWidgetPrivate(DFMTagWidget *qq, const DUrl& url);
    virtual ~DFMTagWidgetPrivate();

private:
    DUrl    m_url;
    QLabel      *m_tagLable{nullptr};
    QLabel      *m_tagLeftLable{nullptr};
    QVBoxLayout *m_mainLayout{nullptr};
    DFMCrumbEdit  *m_tagCrumbEdit{ nullptr };
    DTagActionWidget *m_tagActionWidget{ nullptr };
    DAbstractFileWatcher *m_devicesWatcher{ nullptr };

    DFMTagWidget *q_ptr{ nullptr };
    Q_DECLARE_PUBLIC(DFMTagWidget)
};


DFMTagWidgetPrivate::DFMTagWidgetPrivate(DFMTagWidget *qq, const DUrl &url)
    : m_url(url)
    , q_ptr(qq)
{

}

DFMTagWidgetPrivate::~DFMTagWidgetPrivate()
{

}

DFMTagWidget::DFMTagWidget(DUrl url, QWidget *parent/*=nullptr*/)
    : QFrame (parent)
    , d_private(new DFMTagWidgetPrivate(this, url))
{
    initUi();
    initConnection();
}

DFMTagWidget::~DFMTagWidget()
{

}

void DFMTagWidget::initUi()
{
    Q_D(DFMTagWidget);
    d->m_mainLayout = new QVBoxLayout;
    setLayout(d->m_mainLayout);

    d->m_tagLable = new QLabel(tr("Tag"), this);
    d->m_mainLayout->addWidget(d->m_tagLable);
    d->m_tagLeftLable = new QLabel(tr("Tag"), this);

    d->m_tagActionWidget =  new DTagActionWidget(this);
    d->m_tagActionWidget->setMaximumHeight(20);
    d->m_tagActionWidget->setObjectName("tagActionWidget");
    QHBoxLayout *tagActionLayout = new QHBoxLayout;
    tagActionLayout->addWidget(d->m_tagLeftLable);
    tagActionLayout->addWidget(d->m_tagActionWidget);
    d->m_mainLayout->addLayout(tagActionLayout);
    d->m_tagLeftLable->setHidden(true);

    d->m_tagCrumbEdit = new DFMCrumbEdit(this);
    d->m_tagCrumbEdit->setObjectName("tagCrumbEdit");
    d->m_tagCrumbEdit->setFrameShape(QFrame::Shape::NoFrame);
    d->m_tagCrumbEdit->viewport()->setBackgroundRole(QPalette::NoRole);
    d->m_mainLayout->addWidget(d->m_tagCrumbEdit);

    loadTags(d->m_url);
}

void DFMTagWidget::initConnection()
{
    Q_D(DFMTagWidget);
    if (!d->m_tagCrumbEdit || !d->m_tagActionWidget)
        return;

    QObject::connect(d->m_tagCrumbEdit, &DCrumbEdit::crumbListChanged, d->m_tagCrumbEdit, [=](){
        if (!d->m_tagCrumbEdit->isEditing() && !d->m_tagCrumbEdit->property("LoadFileTags").toBool()) {
            DFileService::instance()->makeTagsOfFiles(nullptr, {d->m_url}, d->m_tagCrumbEdit->crumbList());
        }
    });

    QObject::connect(d->m_tagActionWidget, &DTagActionWidget::checkedColorChanged, d->m_tagActionWidget, [=](const QColor &color){
        const QStringList tag_name_list = TagManager::instance()->getTagsThroughFiles({d->m_url});
        QMap<QString, QColor> nameColors = TagManager::instance()->getTagColor({tag_name_list});
        DUrlList urlList{d->m_url};
        QList<QColor> checkedcolors{ d->m_tagActionWidget->checkedColorList() };
        QSet<QString> dirtyTagFilter = TagManager::instance()->allTagOfDefaultColors();

        QSet<QString> sameColors = nameColors.keys(color).toSet();
        // 当有多个相同颜色名字不同的tag时， 取消tag优先取消默认颜色
        if (sameColors.count()>0 && !checkedcolors.contains(color) && !sameColors.intersects(dirtyTagFilter)) {
            dirtyTagFilter << *sameColors.begin();
        }

        QStringList new_tagNames;
        for (const QColor &color : checkedcolors) {
            QString tag_name =  nameColors.key(color);
            if (tag_name.isEmpty())
                tag_name = TagManager::instance()->getTagNameThroughColor(color);

            if (tag_name.isEmpty()) {
                continue;
            }
            new_tagNames << tag_name;
        }

        DFileService::instance()->makeTagsOfFiles(nullptr, urlList, new_tagNames, dirtyTagFilter);
        loadTags(d->m_url);
    });
}

void DFMTagWidget::loadTags(const DUrl& url)
{
    Q_D(DFMTagWidget);
    if (!d->m_tagCrumbEdit || !d->m_tagActionWidget || !shouldShow(url))
        return;

    const QStringList tag_name_list = TagManager::instance()->getTagsThroughFiles({url});
    QMap<QString, QColor> nameColors = TagManager::instance()->getTagColor({tag_name_list});
    QList<QColor>  selectColors;

    d->m_tagCrumbEdit->setProperty("LoadFileTags", true);
    d->m_tagCrumbEdit->clear();
    for(auto it = nameColors.begin();it != nameColors.end(); ++it) {
        DCrumbTextFormat format = d->m_tagCrumbEdit->makeTextFormat();
        format.setText(it.key());
        selectColors << it.value();
        format.setBackground(QBrush(it.value()));
        format.setBackgroundRadius(5);
        d->m_tagCrumbEdit->insertCrumb(format, 0);
    }
    d->m_tagCrumbEdit->setProperty("LoadFileTags", false);

    d->m_tagActionWidget->setCheckedColorList(selectColors);

    if (!d->m_devicesWatcher || d->m_url != url) {
        d->m_url = url;

        if (d->m_devicesWatcher) {
            d->m_devicesWatcher->stopWatcher();
            d->m_devicesWatcher->deleteLater();
        }

        d->m_devicesWatcher = DFileService::instance()->createFileWatcher(this, d->m_url, this);
        if (d->m_devicesWatcher) {
            d->m_devicesWatcher->startWatcher();

            connect(d->m_devicesWatcher, &DAbstractFileWatcher::fileAttributeChanged, this, [=](const DUrl &url) {
                if (url == d->m_url){
                    loadTags(d->m_url);
                }
            });
        }
    }
}

QWidget *DFMTagWidget::tagTitle()
{
    Q_D(DFMTagWidget);
    return d->m_tagLable;
}

QWidget *DFMTagWidget::tagLeftTitle()
{
    Q_D(DFMTagWidget);
    return d->m_tagLeftLable;
}

DTagActionWidget *DFMTagWidget::tagActionWidget()
{
    Q_D(DFMTagWidget);
    return d->m_tagActionWidget;
}

DCrumbEdit *DFMTagWidget::tagCrumbEdit()
{
    Q_D(DFMTagWidget);
    return d->m_tagCrumbEdit;
}

bool DFMTagWidget::shouldShow(const DUrl& url)
{
    const DAbstractFileInfoPointer &fileInfo = DFileService::instance()->createFileInfo(nullptr, url);
    if (!fileInfo || fileInfo->isVirtualEntry())
        return false;

    bool isComputerOrTrash = false;
    DUrl realTargetUrl = fileInfo->fileUrl();
    if (fileInfo && fileInfo->isSymLink()) {
        realTargetUrl = fileInfo->rootSymLinkTarget();
    }

    if (realTargetUrl.toLocalFile().endsWith(QString(".") + "desktop")) {
        DesktopFile df(realTargetUrl.toLocalFile());
        isComputerOrTrash = (df.getDeepinId() == "dde-trash" || df.getDeepinId() == "dde-computer");
    }

    bool showTags = !systemPathManager->isSystemPath(url.path()) &&
             !isComputerOrTrash && DFileMenuManager::whetherShowTagActions({url});

    return showTags;
}

DFM_END_NAMESPACE