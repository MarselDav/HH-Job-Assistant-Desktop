#include "resumecard.h"

ResumeCard::ResumeCard(int resumeId,
                       const QString &resumeName,
                       const QJsonObject &resumeAnalysis,
                       QWidget *parent)
    : QFrame(parent),
      resume_id(resumeId),
      resume_name(resumeName),
      resume_name_label(new QLabel(resumeName, this)),
      analyze_button(new QPushButton(QStringLiteral("Проанализировать"), this)),
      brief_description_label(new QLabel(this)),
      topics_scroll_area(new QScrollArea(this)),
      topics_container(new QWidget),
      topics_layout(new QHBoxLayout(topics_container)),
      delete_button(new QToolButton(this))
{
    setObjectName("resumeCard");
    setStyleSheet(loadStyleSheet(QStringLiteral(":/stylesheets/resumecard.qss")));

    QVBoxLayout *card_layout = new QVBoxLayout(this);
    card_layout->setContentsMargins(16, 12, 16, 12);
    card_layout->setSpacing(8);

    QHBoxLayout *header_layout = new QHBoxLayout;
    header_layout->setSpacing(10);
    resume_name_label->setObjectName("resumeCardName");
    resume_name_label->setWordWrap(true);
    header_layout->addWidget(resume_name_label, 1);

    delete_button->setObjectName("deleteResumeButton");
    delete_button->setIcon(QIcon(QStringLiteral(":/icons/images/bin.png")));
    delete_button->setIconSize(QSize(17, 17));
    delete_button->setToolTip(QStringLiteral("Удалить резюме"));
    delete_button->setCursor(Qt::PointingHandCursor);
    header_layout->addWidget(delete_button, 0, Qt::AlignTop);
    card_layout->addLayout(header_layout);

    analyze_button->setObjectName("analyzeResumeButton");
    analyze_button->setCursor(Qt::PointingHandCursor);
    card_layout->addWidget(analyze_button, 0, Qt::AlignLeft);

    brief_description_label->setObjectName("resumeBriefDescription");
    brief_description_label->setWordWrap(true);
    brief_description_label->setTextFormat(Qt::PlainText);
    brief_description_label->hide();
    card_layout->addWidget(brief_description_label);

    topics_scroll_area->setObjectName("resumeTopicsScrollArea");
    topics_scroll_area->setWidgetResizable(false);
    topics_scroll_area->setFrameShape(QFrame::NoFrame);
    topics_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    topics_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    topics_scroll_area->setFixedHeight(46);
    topics_layout->setContentsMargins(1, 1, 1, 1);
    topics_layout->setSpacing(8);
    topics_scroll_area->setWidget(topics_container);
    topics_scroll_area->hide();
    card_layout->addWidget(topics_scroll_area);

    connect(delete_button, &QToolButton::clicked, this, [this]() {
        emit deleteResume(resume_id);
    });
    connect(analyze_button, &QPushButton::clicked, this, [this]() {
        emit resumeAnalyzeButtonClicked(resume_id);
    });

    setResumeAnalysis(resumeAnalysis);
}

QString ResumeCard::resumeName() const
{
    return resume_name;
}

void ResumeCard::setResumeAnalysis(const QJsonObject &resumeAnalysis)
{
    QJsonObject analysis = resumeAnalysis;
    if (analysis.value(QStringLiteral("resume_analysis")).isObject())
        analysis = analysis.value(QStringLiteral("resume_analysis")).toObject();

    const bool hasAnalysis = !analysis.isEmpty();
    analyze_button->setVisible(!hasAnalysis);
    brief_description_label->clear();
    brief_description_label->hide();

    while (QLayoutItem *item = topics_layout->takeAt(0)) {
        delete item->widget();
        delete item;
    }
    topics_scroll_area->hide();

    if (!hasAnalysis)
        return;

    brief_description_label->setText(
        analysis.value(QStringLiteral("brief_description")).toString());
    brief_description_label->setVisible(!brief_description_label->text().isEmpty());

    addTopic(analysis.value(QStringLiteral("profession_name")).toString());
    addTopic(JsonFormatter::experienceLabel(
        analysis.value(QStringLiteral("experience")).toString()));

    const QStringList skills = JsonFormatter::formatArray(
        analysis.value(QStringLiteral("skills_sorted_by_level")), false);
    for (const QString &skill : skills)
        addTopic(skill);

    const bool hasTopics = topics_layout->count() > 0;
    topics_container->adjustSize();
    topics_scroll_area->setVisible(hasTopics);
}

void ResumeCard::addTopic(const QString &text)
{
    if (text.trimmed().isEmpty())
        return;

    QLabel *topic = new QLabel(text, topics_container);
    topic->setObjectName("resumeTopic");
    topic->setTextFormat(Qt::PlainText);
    topic->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    topics_layout->addWidget(topic);
}
