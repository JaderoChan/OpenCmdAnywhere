// The "easy translate" library, in c++.
//
// Webs: https://github.com/JaderoChan/easy_translate
// You can contact me by email: c_dl_cn@outlook.com

// MIT License
//
// Copyright (c) 2024 頔珞JaderoChan

#ifndef EASY_TRANSLATE_HPP
#define EASY_TRANSLATE_HPP

#include <cstddef>              // size_t
#include <string>               // string
#include <vector>               // vector
#include <set>                  // set
#include <map>                  // map
#include <fstream>              // ifstream

#include <nlohmann/json.hpp>    // json

/// @brief Define this macro to enable easytr::updateTranslationsFiles() function.
/// @note If you define this macro, the easytr::TranslateManager::translate() function will store
/// all `Translation ID` to memory used for possible update the `Translations file`s.
// #define EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES

// Translate function
//   - Usage: EASYTR("Translation ID")
//   - Get the `Translation text` of the given `Translation ID` on current language.
//   - If the given `Translation ID` is not exist on the current language, return the `Translation ID` itself.
#define EASYTR(x) easytr::translate(x)

// The following is a sample directory structure and content structure for
// the `Languages file` and `Translations file`:
//
// languages.json (Languages file)
//   - en_US (Language ID) : en_US.json (Translations filename)
//   - zh_CN (Language ID) : zh_CN.json (Translations filename)
//   - ...
//   - ja_JP (Language ID) : ja_JP.json (Translations filename)
//   - fr_FR (Language ID) : fr_FR.json (Translations filename)
//
// en_US.json (Translations file)
//   - App.Title  (Translation ID) : Easy Translation (Translation text)
//   - App.Author (Translation ID) : JaderoChan       (Translation text)
//   - ...
//
// zh_CN.json
//   - App.Title  (Translation ID) : 轻松翻译 (Translation text)
//   - App.Author (Translation ID) : 頔珞     (Translation text)
//   - ...

// Languages
//   - Language ID : Translations filename
//   - ...
//
// Translations
//   - Translation ID : Translation text
//   -...

// Note:
// The `Languages file` (e.g. languages.json) and the `Transalations file` (e.g. en.json, zh.json) should
// use the UTF-8 encoding to save.

namespace easytr
{

class Languages
{
    friend class TranslateManager;

public:
    Languages() = default;

    Languages(const std::vector<std::pair<std::string, std::string>>& langs)
    {
        for (const auto& var : langs)
            languages_.insert({ var.first, var.second });
    }

    Languages(const std::map<std::string, std::string>& langs) : languages_(langs) {}

    /// @brief Load the `Languages` from a json string.
    /// @note If the json is invalid, the `Languages` will be empty.
    static Languages fromJson(const std::string& json)
    {
        using Json = nlohmann::json;

        Json j = Json::parse(json, nullptr, false, true);
        if (j.is_discarded())
            return Languages();

        std::map<std::string, std::string> list;
        for (const auto& var : j.items())
            list.insert({ var.key(), var.value() });

        return Languages(list);
    }

    /// @brief Load the `Languages` from a json file.
    /// @note If the json is invalid, the `Languages` will be empty.
    static Languages fromFile(const std::string& filename)
    {
        using Json = nlohmann::json;

        std::ifstream ifs(filename);
        if (!ifs.is_open())
            return Languages();

        Json j = Json::parse(ifs, nullptr, false, true);
        if (j.is_discarded())
        {
            ifs.close();
            return Languages();
        }

        std::map<std::string, std::string> list;
        ifs.close();
        for (const auto& var : j.items())
            list.insert({ var.key(), var.value() });

        return Languages(list);
    }

    /// @brief Get the json string.
    std::string toJson() const
    {
        nlohmann::json j;
        for (const auto& var : languages_)
            j[var.first] = var.second;
        return j.dump(4);
    }

    /// @brief Write the `Languages` to a json file.
    /// @return If the failed to write the file return false else return true.
    bool toFile(const std::string& filename = "languages.json") const
    {
        std::ofstream ofs(filename);
        if (!ofs.is_open())
            return false;
        ofs << toJson();
        ofs.close();
        return true;
    }

    /// @brief Get the `Translations filename` of the given `Language ID`.
    const char* at(const std::string& languageId) const
    { return languages_.at(languageId).c_str(); }

    /// @brief Get the number of the `Language ID`.
    size_t count() const { return languages_.size(); }

    /// @brief Check whether has not any `Language ID`.
    bool empty() const { return count() == 0; }

    /// @brief Check whether exists the given `Language ID`.
    bool has(const std::string& languageId) const
    { return languages_.find(languageId) != languages_.end(); }

    /// @brief Get all `Language ID`s.
    std::vector<std::string> getIds() const
    {
        std::vector<std::string> ids;
        for (const auto& var : languages_)
            ids.push_back(var.first);
        return ids;
    }

    /// @brief Add a pair of the `Language ID` and `Translations filename`.
    /// @note If the given `Language ID` already exists, do nothing.
    void add(const std::string& languageId, const std::string& translationsFilename)
    {
        if (!has(languageId))
            languages_.insert({ languageId, translationsFilename });
    }

    /// @brief Remove a `Language ID` and it corresponding `Translations filename`.
    void remove(const std::string& languageId)
    {
        if (has(languageId))
            languages_.erase(languageId);
    }

    /// @brief Remove all `Language ID`s and it corresponding `Translations filename`s.
    void clear() { languages_.clear(); }

private:
    // {Language ID : Translations filename}
    std::map<std::string, std::string> languages_;
};

class Translations
{
    friend class TranslateManager;
public:
    Translations() = default;

    Translations(const std::vector<std::pair<std::string, std::string>>& trans)
    {
        for (const auto& var : trans)
            translations_.insert({ var.first, var.second });
    }

    Translations(const std::map<std::string, std::string>& trans) : translations_(trans) {}

    /// @brief Load the `Translations` from a json string.
    /// @note If the json is invalid, the `Translations` will be empty.
    static Translations fromJson(const std::string& json)
    {
        using Json = nlohmann::json;

        Json j = Json::parse(json, nullptr, false, true);
        if (j.is_discarded())
            return Translations();

        std::map<std::string, std::string> list;
        for (const auto& var : j.items())
            list.insert({ var.key(), var.value() });

        return Translations(list);
    }

    /// @brief Load the `Translations` from a json file.
    /// @note If the json is invalid, the `Translations` will be empty.
    static Translations fromFile(const std::string& filename)
    {
        using Json = nlohmann::json;

        std::ifstream ifs(filename);
        if (!ifs.is_open())
            return Translations();

        Json j = Json::parse(ifs, nullptr, false, true);
        if (j.is_discarded())
        {
            ifs.close();
            return Translations();
        }

        std::map<std::string, std::string> list;
        ifs.close();
        for (const auto& var : j.items())
            list.insert({ var.key(), var.value() });

        return Translations(list);
    }

    /// @brief Get the json string.
    std::string toJson() const
    {
        nlohmann::json j;
        for (const auto& var : translations_)
            j[var.first] = var.second;
        return j.dump(4);
    }

    /// @brief Write the `Translations` to a json file.
    /// @return If the failed to write the file return false else return true.
    bool toFile(const std::string& filename) const
    {
        std::ofstream ofs(filename);
        if (!ofs.is_open())
            return false;
        ofs << toJson();
        ofs.close();
        return true;
    }

    /// @brief Get the `Translation text` of the given `Translation ID`.
    /// @note If the given `Translation ID` is not exist, return the `Translation ID` itself.
    const char* at(const std::string& tranId) const
    {
        if (!has(tranId))
            return tranId.c_str();
        return translations_.at(tranId).c_str();
    }

    /// @brief Get the number of the `Translation ID`.
    size_t count() const { return translations_.size(); }

    /// @brief Check whether has not any `Translation ID`.
    bool empty() const { return count() == 0; }

    /// @brief Check whether exists the given `Translation ID`.
    bool has(const std::string& tranId) const
    { return translations_.find(tranId) != translations_.end(); }

    /// @brief Get all `Translation ID`s
    std::vector<std::string> getIds() const
    {
        std::vector<std::string> ids;
        for (const auto& var : translations_)
            ids.push_back(var.first);
        return ids;
    }

    /// @brief Add a pair of the `Translation ID` and `Translation text`.
    /// @note If the given `Translation ID` already exists, do nothing.
    void add(const std::string& tranId, const std::string& translation)
    {
        if (!has(tranId))
            translations_.insert({ tranId, translation });
    }

    /// @brief Remove a `Translation ID` and it corresponding `Translation text`.
    void remove(const std::string& tranId)
    {
        if (has(tranId))
            translations_.erase(tranId);
    }

    /// @brief Remove all `Translation ID`s and it corresponding `Translation text`s.
    void clear() { translations_.clear(); }

private:
    // {Translation ID : Translation text}
    std::map<std::string, std::string> translations_;
};

// Singleton class
class TranslateManager
{
public:
    static TranslateManager& getInstance()
    {
        static TranslateManager instance;
        return instance;
    }

    /// @brief Get the `Translation text` of the given `Translation ID` on current language.
    /// @note If the given `Translation ID` is not exist on the current language, return the `Translation ID` itself.
#ifndef EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES
    const char* translate(const std::string& tranId) const
    {
        return translations_.at(tranId);
    }
#else
    /// @brief Get the `Translation text` of the given `Translation ID` on current language.
    /// @note If the given `Translation ID` is not exist on the current language, return the `Translation ID` itself.
    const char* translate(const std::string& tranId)
    {
        tranIds_.insert(tranId);
        return translations_.at(tranId);
    }
#endif // EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES

    /// @brief Set the `Languages` and reset the current language.
    void setLanguages(const Languages& languages) { languages_ = languages; currentLanguage_.clear(); }

    /// @brief Set the `Languages` that from a json file and reset the current language.
    void setLanguages(const std::string& filename) { setLanguages(Languages::fromFile(filename)); }

    /// @brief Get the `Language ID` of the current language.
    const char* currentLanguage() const { return currentLanguage_.c_str(); }

    /// @brief Set the current language by `Language ID`.
    /// @return If success to change return true else return false.
    bool setCurrentLanguage(const std::string& languageId)
    {
        if (!hasLanguage(languageId))
            return false;

    #ifdef EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES
        bool isFirst = currentLanguage_.empty();
    #endif // !EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES
        currentLanguage_ = languageId;
        translations_ = Translations::fromFile(languages_.at(languageId));

    #ifdef EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES
        if (isFirst)
        {
            for (const auto& var : translations_.translations_)
                tranIds_.insert(var.first);
        }
    #endif // !EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES

        return true;
    }

    const Languages& languages() const { return languages_; }

    const Translations& translations() const { return translations_; }

    /// @brief Get the number of the `Language ID`.
    size_t languageCount() const { return languages_.count(); }

    /// @brief Get the number of the `Translation ID` on current language.
    size_t translationCount() const { return translations_.count(); }

    /// @brief Check whether exists the given `Language ID`.
    bool hasLanguage(const std::string& languageId) const { return languages_.has(languageId); }

    /// @brief Check whether exists the given `Translation ID`.
    bool hasTranslation(const std::string& tranId) const { return translations_.has(tranId); }

    /// @brief Update all `Translations file`s. (add pairs of the new `Translation ID` and empty `Translation text`)
    /// @return The number of updated files.
    /// @note - The new `Translation ID` is from all `Translation ID` that passed as #translate() function argument in programs.
    /// @note - This function can help you to easy get the all `Translation ID` that need to translate.
    /// @attention - Make sure to call this function after you already call all #translate() function,
    /// if not you will get incomplete `Translation ID` list.
    /// @attention - This function is not effect when undefine the macro \ref EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES.
    size_t updateTranslationsFiles() const
    {
    #ifndef EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES
        return 0;
    #else
        using Json = nlohmann::json;

        size_t updated = 0;
        for (const auto& languageId : languages_.getIds())
        {
            std::string filename = languages_.at(languageId);
            std::ifstream ifs(filename);
            Json j;
            if (!ifs.is_open())
            {
                for (const auto& tranId : tranIds_)
                    j[tranId] = "";
            }
            else
            {
                j = Json::parse(ifs, nullptr, false, true);
                if (j.is_discarded())
                {
                    j = Json();
                    for (const auto& tranId : tranIds_)
                        j[tranId] = "";
                }
                else
                {
                    std::map<std::string, std::string> map; // For sort
                    for (const auto& tranId : tranIds_)
                        j.contains(tranId) ? map.insert({ tranId, j[tranId] }) : map.insert({ tranId, "" });

                    j.clear();
                    for (const auto& var : map)
                        j[var.first] = var.second;
                }

                ifs.close();
            }

            std::ofstream ofs(filename);
            if (!ofs.is_open())
                continue;

            ofs << j.dump(4);
            ofs.close();
            updated++;
        }

        return updated;
    #endif // EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES
    }

private:
    TranslateManager() = default;

    ~TranslateManager() = default;

    TranslateManager(const TranslateManager&) = delete;

    TranslateManager& operator=(const TranslateManager&) = delete;

#ifdef EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES
    std::set<std::string> tranIds_;
#endif // !EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES
    std::string currentLanguage_;
    Languages languages_;
    Translations translations_;
};

// For convenience

inline TranslateManager& getTranslateManager()
{ return TranslateManager::getInstance(); }

/// @brief Get the `Translation text` of the given `Translation ID` on current language.
/// @note If the given `Translation ID` is not exist on the current language, return the `Translation ID` itself.
inline const char* translate(const std::string& tranId)
{ return getTranslateManager().translate(tranId); }

/// @brief Set the `Languages`.
inline void setLanguages(const Languages& langs)
{ getTranslateManager().setLanguages(langs); }

/// @brief Set the `Languages` that from a json file.
inline void setLanguages(const std::string& filename)
{ getTranslateManager().setLanguages(filename); }

inline const char* currentLanguage()
{ return getTranslateManager().currentLanguage(); }

/// @brief Set the current language by `Language ID`.
/// @return If success to change return true else return false.
inline bool setCurrentLanguage(const std::string& languageId)
{ return getTranslateManager().setCurrentLanguage(languageId); }

/// @brief Get the number of the `Language ID`.
inline size_t languageCount()
{ return getTranslateManager().languageCount(); }

/// @brief Get the number of the `Translation ID` on current language.
inline size_t translationCount()
{ return getTranslateManager().translationCount(); }

/// @brief Check whether exists the given `Language ID`.
inline bool hasLanguage(const std::string& languageId)
{ return getTranslateManager().hasLanguage(languageId); }

/// @brief Check whether exists the given `Translation ID`.
inline bool hasTranslation(const std::string& tranId)
{ return getTranslateManager().hasTranslation(tranId); }

inline const Languages& languages()
{ return getTranslateManager().languages(); }

inline const Translations& translations()
{ return getTranslateManager().translations(); }

/// @brief Update all `Translations file`s. (add pairs of the new `Translation ID` and empty `Translation text`)
/// @return The number of updated files.
/// @note - The new `Translation ID` is from all `Translation ID` that passed as #tr() function argument in programs.
/// @note - This function can help you to easy get the all `Translation ID` that need to translate.
/// @attention - Make sure to call this function after you already call all #tr() function,
/// if not you will get incomplete `Translation ID` list.
/// @attention - This function is not effect when undefine the macro \ref EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES.
inline size_t updateTranslationsFiles()
{ return getTranslateManager().updateTranslationsFiles(); }

} // namespace easytr

#endif // !EASY_TRANSLATE_HPP
