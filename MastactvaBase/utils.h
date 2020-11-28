#ifndef UTILS_H
#define UTILS_H

#include <QString>


QString leftDoubleCR(const QString &str_);
qreal sqr(qreal val_);
bool isSymbol(const QChar &ch_);


static const QString g_englishLanguage = "English";
static const QString g_belarusLanguage = "Belarus";


#endif // UTILS_H
