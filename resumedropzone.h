#ifndef RESUMEDROPZONE_H
#define RESUMEDROPZONE_H

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFrame>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>

class ResumeDropZone : public QFrame
{
    Q_OBJECT

public:
    explicit ResumeDropZone(QWidget *parent = nullptr);

signals:
    void fileDropped(const QString &path);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // RESUMEDROPZONE_H
