#include "resumedropzone.h"

ResumeDropZone::ResumeDropZone(QWidget *parent) : QFrame(parent)
{
    setObjectName("resumeDropZone");
    setAcceptDrops(true);
}

void ResumeDropZone::dragEnterEvent(QDragEnterEvent *event)
{
    if (!event->mimeData()->hasUrls()) {
        event->ignore();
        return;
    }

    const QList<QUrl> urls = event->mimeData()->urls();
    if (!urls.isEmpty() && urls.first().isLocalFile()
        && QFileInfo(urls.first().toLocalFile()).suffix().compare(QStringLiteral("txt"), Qt::CaseInsensitive) == 0) {
        event->acceptProposedAction();
        return;
    }

    event->ignore();
}

void ResumeDropZone::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty() || !urls.first().isLocalFile()) {
        event->ignore();
        return;
    }

    const QString path = urls.first().toLocalFile();
    if (QFileInfo(path).suffix().compare(QStringLiteral("txt"), Qt::CaseInsensitive) != 0) {
        event->ignore();
        return;
    }

    emit fileDropped(path);
    event->acceptProposedAction();
}
