/************************************************
* @file MyLanguageData.cpp
* @brief My Language Data,
* I check variables before setting them to prevent triggers.
* @author Jeffrey Scott Flesher <Jeffrey.Scott.Flesher@>
 ***********************************************/
#include "MyLanguageData.h"
/*****************************************************************************/
/**
 * @brief MyLanguageData::MyLanguageData
 * @param thisName
 * @param thisNameTranslated
 */
MyLanguageData::MyLanguageData(const QString &thisName, const QString &thisNameTranslated) : myName(thisName), myNameTranslated(thisNameTranslated)
{
}
/*****************************************************************************/
/**
 * @brief MyLanguageData::name
 * @return myName
 */
QString MyLanguageData::name() const
{
    return myName;
}
/*****************************************************************************/
/**
 * @brief MyLanguageData::nameTranslated
 * @return myNameTranslated
 */
QString MyLanguageData::nameTranslated() const
{
    return myNameTranslated;
}
/******************************* End of File *********************************/
