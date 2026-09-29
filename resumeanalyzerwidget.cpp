#include "resumeanalyzerwidget.h"

#include "apiclient.h"
#include "resumecard.h"
#include "stylesheetloader.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QLabel>
#include <QLineEdit>
#include <QMimeData>
#include <QPushButton>
#include <QScrollArea>
#include <QStringList>
#include <QTextBrowser>
#include <QUrl>
#include <QVBoxLayout>

#include <functional>

namespace {

class ResumeDropZone : public QFrame
{
public:
    explicit ResumeDropZone(QWidget *parent = nullptr) : QFrame(parent)
    {
        setObjectName("resumeDropZone");
        setAcceptDrops(true);
    }

    std::function<void(const QString &)> onFileDropped;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override
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

    void dropEvent(QDropEvent *event) override
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

        if (onFileDropped)
            onFileDropped(path);
        event->acceptProposedAction();
    }
};

QString jsonValueText(const QJsonValue &value)
{
    if (value.isString())
        return value.toString();
    if (value.isDouble())
        return QString::number(value.toDouble());
    if (value.isBool())
        return value.toBool() ? QStringLiteral("Да") : QStringLiteral("Нет");
    if (value.isObject())
        return QString::fromUtf8(QJsonDocument(value.toObject()).toJson(QJsonDocument::Compact));
    return QString();
}

QString objectField(const QJsonObject &object, const QStringList &names)
{
    for (const QString &name : names) {
        const QString value = jsonValueText(object.value(name));
        if (!value.isEmpty())
            return value;
    }
    return QString();
}

QString formatItem(const QJsonValue &value, bool includeLevel)
{
    if (value.isString())
        return value.toString().toHtmlEscaped();
    if (!value.isObject())
        return jsonValueText(value).toHtmlEscaped();

    const QJsonObject object = value.toObject();
    QString name = objectField(object, {QStringLiteral("name"), QStringLiteral("skill"),
                                        QStringLiteral("language"), QStringLiteral("title")});
    QString level;
    if (includeLevel)
        level = objectField(object, {QStringLiteral("level"), QStringLiteral("proficiency_level"),
                                     QStringLiteral("proficiency"), QStringLiteral("level_name")});

    if (name.isEmpty())
        name = jsonValueText(value);
    QString result = name.toHtmlEscaped();
    if (!level.isEmpty())
        result += QStringLiteral(" — ") + level.toHtmlEscaped();
    return result;
}

QString formatArray(const QJsonValue &value, bool includeLevel)
{
    if (!value.isArray())
        return QStringLiteral("Не указано");

    QStringList values;
    for (const QJsonValue &item : value.toArray()) {
        const QString formatted = formatItem(item, includeLevel);
        if (!formatted.isEmpty())
            values.append(formatted);
    }
    return values.isEmpty() ? QStringLiteral("Не указано")
                            : values.join(QStringLiteral(" · "));
}

QString experienceLabel(const QString &value)
{
    if (value == QLatin1String("noExperience")) return QStringLiteral("Нет опыта");
    if (value == QLatin1String("between1And3")) return QStringLiteral("От 1 до 3 лет");
    if (value == QLatin1String("between3And6")) return QStringLiteral("От 3 до 6 лет");
    if (value == QLatin1String("moreThan6")) return QStringLiteral("Более 6 лет");
    return value.isEmpty() ? QStringLiteral("Не указано") : value;
}

} // namespace

ResumeAnalyzerWidget::ResumeAnalyzerWidget(ApiClient *apiClient, QWidget *parent)
    : QWidget(parent),
      api_client(apiClient),
      pending_resume_title(),
      selected_file_label(nullptr),
      upload_status_label(nullptr),
      empty_resumes_label(nullptr),
      resume_name_field(nullptr),
      resume_cards_layout(nullptr),
      upload_button(nullptr),
      analyze_button(nullptr),
      analysis_browser(nullptr),
      resume_id(-1)
{
    setObjectName("resumeAnalyzerPage");
    setStyleSheet(loadStyleSheet(QStringLiteral(":/stylesheets/resumeanalyzerwidget.qss")));

    QVBoxLayout *root_layout = new QVBoxLayout(this);
    root_layout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *page_scroll_area = new QScrollArea(this);
    page_scroll_area->setObjectName("resumePageScrollArea");
    page_scroll_area->setWidgetResizable(true);
    page_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    page_scroll_area->setFrameShape(QFrame::NoFrame);
    root_layout->addWidget(page_scroll_area);

    QWidget *page_content = new QWidget;
    page_content->setObjectName("resumePageContent");
    page_scroll_area->setWidget(page_content);

    QVBoxLayout *page_layout = new QVBoxLayout(page_content);
    page_layout->setContentsMargins(38, 32, 38, 28);
    page_layout->setSpacing(20);

    QLabel *eyebrow = new QLabel(QStringLiteral("ПРОФИЛЬ КАНДИДАТА"), page_content);
    eyebrow->setObjectName("pageEyebrow");
    page_layout->addWidget(eyebrow);

    QVBoxLayout *resumes_layout = new QVBoxLayout;
    resumes_layout->setSpacing(10);

    QLabel *resumes_title = new QLabel(QStringLiteral("Ваши резюме"), page_content);
    resumes_title->setObjectName("resumesTitle");
    resumes_layout->addWidget(resumes_title);

    QScrollArea *resume_cards_scroll_area = new QScrollArea(page_content);
    resume_cards_scroll_area->setObjectName("resumeCardsScrollArea");
    resume_cards_scroll_area->setWidgetResizable(true);
    resume_cards_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    resume_cards_scroll_area->setFrameShape(QFrame::NoFrame);
    resume_cards_scroll_area->setMinimumHeight(82);
    resume_cards_scroll_area->setMaximumHeight(150);

    QWidget *resume_cards_container = new QWidget;
    QVBoxLayout *resume_cards_container_layout = new QVBoxLayout(resume_cards_container);
    resume_cards_container_layout->setContentsMargins(1, 1, 8, 1);
    resume_cards_container_layout->setSpacing(8);
    resume_cards_layout = resume_cards_container_layout;
    empty_resumes_label = new QLabel(QStringLiteral("Загруженные резюме появятся здесь"), resume_cards_container);
    empty_resumes_label->setObjectName("emptyResumesLabel");
    resume_cards_layout->addWidget(empty_resumes_label);
    resume_cards_layout->addStretch();
    resume_cards_scroll_area->setWidget(resume_cards_container);
    resumes_layout->addWidget(resume_cards_scroll_area);
    page_layout->addLayout(resumes_layout);

    QLabel *title = new QLabel(QStringLiteral("Анализ резюме"), page_content);
    title->setObjectName("pageTitle");
    page_layout->addWidget(title);

    QLabel *subtitle = new QLabel(QStringLiteral("Загрузите резюме, чтобы подготовить структурированный профиль"), page_content);
    subtitle->setObjectName("pageSubtitle");
    page_layout->addWidget(subtitle);

    QHBoxLayout *block_layout = new QHBoxLayout;
    block_layout->setSpacing(18);

    QFrame *upload_block = new QFrame(page_content);
    upload_block->setObjectName("resumePanel");
    QVBoxLayout *upload_layout = new QVBoxLayout(upload_block);
    upload_layout->setContentsMargins(22, 22, 22, 22);
    upload_layout->setSpacing(14);

    QLabel *upload_title = new QLabel(QStringLiteral("Ваше резюме"), upload_block);
    upload_title->setObjectName("panelTitle");
    upload_layout->addWidget(upload_title);

    resume_name_field = new QLineEdit(upload_block);
    resume_name_field->setObjectName("resumeNameField");
    resume_name_field->setPlaceholderText(QStringLiteral("Название резюме *"));
    upload_layout->addWidget(resume_name_field);

    QLabel *upload_hint = new QLabel(QStringLiteral("Выберите TXT-файл или перетащите его в область ниже"), upload_block);
    upload_hint->setObjectName("panelHint");
    upload_hint->setWordWrap(true);
    upload_layout->addWidget(upload_hint);

    ResumeDropZone *drop_zone = new ResumeDropZone(upload_block);
    QVBoxLayout *drop_layout = new QVBoxLayout(drop_zone);
    drop_layout->setContentsMargins(18, 20, 18, 20);
    drop_layout->setSpacing(9);

    QLabel *drop_icon = new QLabel(QStringLiteral("TXT"), drop_zone);
    drop_icon->setObjectName("dropIcon");
    drop_icon->setAlignment(Qt::AlignCenter);
    drop_layout->addWidget(drop_icon, 0, Qt::AlignHCenter);

    QLabel *drop_text = new QLabel(QStringLiteral("Перетащите файл сюда"), drop_zone);
    drop_text->setObjectName("dropTitle");
    drop_text->setAlignment(Qt::AlignCenter);
    drop_layout->addWidget(drop_text);

    QLabel *drop_subtext = new QLabel(QStringLiteral("Поддерживается формат .txt"), drop_zone);
    drop_subtext->setObjectName("panelHint");
    drop_subtext->setAlignment(Qt::AlignCenter);
    drop_layout->addWidget(drop_subtext);

    QPushButton *browse_button = new QPushButton(QStringLiteral("Выбрать файл"), drop_zone);
    browse_button->setObjectName("secondaryButton");
    browse_button->setCursor(Qt::PointingHandCursor);
    drop_layout->addWidget(browse_button, 0, Qt::AlignHCenter);
    upload_layout->addWidget(drop_zone, 1);

    selected_file_label = new QLabel(QStringLiteral("Файл не выбран"), upload_block);
    selected_file_label->setObjectName("fileStatus");
    selected_file_label->setWordWrap(true);
    upload_layout->addWidget(selected_file_label);

    upload_status_label = new QLabel(upload_block);
    upload_status_label->setObjectName("panelHint");
    upload_status_label->setWordWrap(true);
    upload_layout->addWidget(upload_status_label);

    upload_button = new QPushButton(QStringLiteral("Загрузить резюме"), upload_block);
    upload_button->setObjectName("primaryButton");
    upload_button->setCursor(Qt::PointingHandCursor);
    upload_button->setEnabled(false);
    upload_layout->addWidget(upload_button);

    QFrame *analysis_block = new QFrame(page_content);
    analysis_block->setObjectName("resumePanel");
    QVBoxLayout *analysis_layout = new QVBoxLayout(analysis_block);
    analysis_layout->setContentsMargins(22, 22, 22, 22);
    analysis_layout->setSpacing(14);

    QHBoxLayout *analysis_header = new QHBoxLayout;
    QLabel *analysis_title = new QLabel(QStringLiteral("Результат анализа"), analysis_block);
    analysis_title->setObjectName("panelTitle");
    analysis_header->addWidget(analysis_title, 1);
    analyze_button = new QPushButton(QStringLiteral("Проанализировать"), analysis_block);
    analyze_button->setObjectName("primaryButton");
    analyze_button->setCursor(Qt::PointingHandCursor);
    analyze_button->setEnabled(false);
    analysis_header->addWidget(analyze_button);
    analysis_layout->addLayout(analysis_header);

    analysis_browser = new QTextBrowser(analysis_block);
    analysis_browser->setObjectName("analysisBrowser");
    analysis_browser->setOpenExternalLinks(true);
    analysis_layout->addWidget(analysis_browser, 1);

    analysis_browser->setMinimumHeight(340);
    block_layout->addWidget(upload_block, 1);
    block_layout->addWidget(analysis_block, 1);
    page_layout->addLayout(block_layout, 1);

    connect(browse_button, &QPushButton::clicked, this, [this]() {
        const QString path = QFileDialog::getOpenFileName(
            this, QStringLiteral("Выберите резюме"), QString(), QStringLiteral("Текстовые файлы (*.txt)"));
        if (!path.isEmpty())
            selectResumeFile(path);
    });
    drop_zone->onFileDropped = [this](const QString &path) { selectResumeFile(path); };
    connect(upload_button, &QPushButton::clicked, this, &ResumeAnalyzerWidget::uploadResume);
    connect(resume_name_field, &QLineEdit::textChanged,
            this, &ResumeAnalyzerWidget::updateUploadButtonState);
    connect(analyze_button, &QPushButton::clicked, this, &ResumeAnalyzerWidget::analyzeResume);

    connect(api_client, &ApiClient::resumeUploaded, this, &ResumeAnalyzerWidget::onResumeUploaded);
    connect(api_client, &ApiClient::resumeAnalyzed, this, &ResumeAnalyzerWidget::setResumeAnalysis);
}

void ResumeAnalyzerWidget::selectResumeFile(const QString &path)
{
    if (QFileInfo(path).suffix().compare(QStringLiteral("txt"), Qt::CaseInsensitive) != 0)
        return;

    selected_resume_path = path;
    selected_file_label->setText(QFileInfo(path).fileName());
    upload_status_label->clear();
    updateUploadButtonState();
}

void ResumeAnalyzerWidget::updateUploadButtonState()
{
    const bool hasFile = !selected_resume_path.isEmpty();
    const bool hasName = resume_name_field && !resume_name_field->text().trimmed().isEmpty();
    upload_button->setEnabled(hasFile && hasName);
}

void ResumeAnalyzerWidget::uploadResume()
{
    const QString resumeName = resume_name_field->text().trimmed();
    if (selected_resume_path.isEmpty() || resumeName.isEmpty())
        return;

    pending_resume_title = resumeName;
    upload_status_label->setText(QStringLiteral("Загружаем резюме…"));
    upload_button->setEnabled(false);
    api_client->uploadResume(selected_resume_path, pending_resume_title);
}


void ResumeAnalyzerWidget::onResumeUploaded(const QJsonObject &resumeReply)
{
    resume_id = resumeReply["resume_id"].toInt();
    analyze_button->setEnabled(true);
    upload_status_label->setText(QStringLiteral("Резюме загружено"));

    if (empty_resumes_label) {
        resume_cards_layout->removeWidget(empty_resumes_label);
        delete empty_resumes_label;
        empty_resumes_label = nullptr;
    }

    const int cardIndex = qMax(0, resume_cards_layout->count() - 1);
    resume_cards_layout->insertWidget(cardIndex, new ResumeCard(pending_resume_title));
    pending_resume_title.clear();
    selected_resume_path.clear();
    selected_file_label->setText(QStringLiteral("Файл не выбран"));
    resume_name_field->clear();
    updateUploadButtonState();
}

void ResumeAnalyzerWidget::analyzeResume()
{
    if (resume_id < 0)
    {
        qDebug() << "Резюме не было загружено, анализ невозможен";
    }

    // api_client->analyzeResume(resume_id);

    api_client->analyzeResume(1); // для тестирования, анализ с таким id уже существует и не тратит ресурсы
}

void ResumeAnalyzerWidget::setResumeAnalysis(const QJsonObject &resumeAnalysis)
{
    QFile template_file(QStringLiteral(":/templates/resume_analysis_patter.html"));
    if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        analysis_browser->clear();
        return;
    }

    QJsonObject analysis = resumeAnalysis.value("resume_analysis").toObject();

    QString html = QString::fromUtf8(template_file.readAll());
    template_file.close();

    const QString brief = analysis.value(QStringLiteral("brief_description")).toString();
    const QString profession = analysis.value(QStringLiteral("profession_name")).toString();
    const QString experience = experienceLabel(analysis.value(QStringLiteral("experience")).toString());

    html.replace(QStringLiteral("{{profession_name}}"), profession.toHtmlEscaped());
    html.replace(QStringLiteral("{{experience}}"), experience.toHtmlEscaped());
    html.replace(QStringLiteral("{{brief_description}}"), brief.toHtmlEscaped());
    html.replace(QStringLiteral("{{skills}}"), formatArray(analysis.value(QStringLiteral("skills_sorted_by_level")), true));
    html.replace(QStringLiteral("{{languages}}"), formatArray(analysis.value(QStringLiteral("languages_with_level")), true));
    html.replace(QStringLiteral("{{work_formats}}"), formatArray(analysis.value(QStringLiteral("work_formats")), false));
    html.replace(QStringLiteral("{{work_schedule_by_days}}"), formatArray(analysis.value(QStringLiteral("work_schedule_by_days")), false));
    html.replace(QStringLiteral("{{working_hours}}"), formatArray(analysis.value(QStringLiteral("working_hours")), false));

    analysis_browser->setHtml(html);
}
