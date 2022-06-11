#ifndef DRAWINGDATAARGSETSANDARGS_H
#define DRAWINGDATAARGSETSANDARGS_H


#include <memory>
#include <QVector>


class EffectArgData;
class EffectArgSetData;
class EffectArgValueData;


/*
 * the helper class for algorithm of processing effect data to the effect objects
 * the holder class to pass drawing data arg sets and arg data to the functions
*/
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
    bool find(const QString &name_);
    QString getValue() const;

private:
    int m_argSetIndex = 0;
    bool m_addVariableToLocalPosition = false;
    int m_objectArtefactId = -1;
    const EffectArgValueData *m_value = nullptr;
};


#endif // DRAWINGDATAARGSETSANDARGS_H
