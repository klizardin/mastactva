#include "mergeid.h"
#include "QRandomGenerator"


inline
QString getRandomString(QRandomGenerator *rnd_, int size_)
{
   static const QString s_chars("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

   QString result;
   result.reserve(size_ + 1);
   for(int i=0; i<size_; ++i)
   {
       const int index = rnd_->generate() % s_chars.length();
       const QChar nextChar = s_chars.at(index);
       result.append(nextChar);
   }
   return result;
}


MergeId::MergeId()
{
    reset();
}

void MergeId::set(const QString &val_)
{
    static_cast<QString &>(*this) = val_;
}

void MergeId::reset()
{
    QString rnd = getRandomString(QRandomGenerator::global(), 36);
    swap(rnd);
}
