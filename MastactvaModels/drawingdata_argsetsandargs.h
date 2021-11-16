#ifndef DRAWINGDATAARGSETSANDARGS_H
#define DRAWINGDATAARGSETSANDARGS_H


#include <memory>
#include <QVector>


class EffectArgData;
class EffectArgSetData;


class DrawingDataArgSetsAndArgs
{
public:
    std::shared_ptr<QVector<EffectArgData *>> m_effectArgsData;
    std::shared_ptr<QVector<EffectArgSetData *>> m_effectArgSetsData;

    void setArgSetIndex(int index_);
    void setObjectLocalPosition(bool local_);
    bool doAddVariableToLocalPosition() const;
    void setObjectArtefactId(int objectArtefactId_);
    void clearObjectArtefactId();
    bool find(const QString &name_) const;
    QString getValue(const QString &name_) const;

private:
    int m_argSetIndex = 0;
    bool m_addVariableToLocalPosition = false;
    int m_objectArtefactId = -1;
};


#endif // DRAWINGDATAARGSETSANDARGS_H
