#ifndef IMAGEDATAINITIALIZER_H
#define IMAGEDATAINITIALIZER_H


#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/quizimagedrawingdata.h"
#include "../MastactvaBase/utils.h"


/*
 * simple IFileSource implementation
 * (for test purpose)
*/
class MapFileSource : public drawingdata::IFileSource
{
public:
    MapFileSource() = default;
    ~MapFileSource() override = default;

    bool containsText(const QString &filename_) const;
    bool containsImage(const QString &filename_) const;
    bool load(const QString &filename_);
    void addText(const QString &filename_, const QString &text_);
    void addImage(const QString &filename_, const QImage &image_);

    QString getText(const FileSource &filename_) const override;
    QImage getImage(const FileSource &filename_) const override;

private:
    QHash<QString, QString> m_texts;
    QHash<QString, QImage> m_images;
};


class ImageDataInitializer : public IDefaultData<drawing_data::QuizImageObjects>
{
public:
    ImageDataInitializer();
    //void setInfo(const QString& filenameA_, const QString& filenameB_, ... );
    void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
    std::pair<const char *, const char *> getDescription() const override;

private:
    std::shared_ptr<MapFileSource> m_filesource;
    const QPair<QString,QString> m_filenames;
    const QPair<std::vector<QVector4D>, std::vector<QVector4D>> m_coordinates;
};

#endif // IMAGEDATAINITIALIZER_H
