#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace ssge
{
	class IniFile
	{
	protected:
		struct Item
		{
			std::string key;
			std::string value;
			Item() = default;
			Item(std::string k, std::string v)
				: key(std::move(k)), value(std::move(v)) {}
		};
		struct Section
		{
			std::string caption;
			std::vector<Item> items;
			Section() = default;
			Section(std::string c)
				: caption(std::move(c)) {}
		};

		std::vector<Section> sections;
		std::string errorLog;

		// ---------- small helpers ----------
		static inline void ltrim(std::string& s)
		{
			size_t i = 0u;
			while (i < s.size() && std::isspace((unsigned char)s[i]))
				i++;

			s.erase(0, i);
		}
		static inline void rtrim(std::string& s)
		{
			if (s.empty()) return;
			size_t i = s.size();
			while (i > 0 && std::isspace((unsigned char)s[i - 1]))
				i--;
			
			s.erase(i);
		}
		static inline std::string trim(std::string s)
		{
			ltrim(s);
			rtrim(s);
			return s;
		}

		static inline std::string lower(std::string s)
		{
			for (size_t i = 0; i < s.size(); i++)
				s[i] = (char)std::tolower((unsigned char)s[i]);

			return s;
		}

		inline void ensureSection(const std::string& caption, size_t& secIdx)
		{
			for (size_t i = 0; i < sections.size(); i++)
			{
				if (sections[i].caption == caption)
				{
					secIdx = i; return;
				}
			}
			sections.push_back(Section(caption));
			secIdx = sections.size() - 1;
		}

		inline bool findFirstItem(
			const std::string& caption,
			const std::string& key,
			size_t& secIdx,
			size_t& itemIdx) const
		{
			for (size_t s = 0; s < sections.size(); ++s)
			{
				if (sections[s].caption == caption)
				{
					for (size_t i = 0; i < sections[s].items.size(); ++i)
					{
						if (sections[s].items[i].key == key)
						{
							secIdx = s;
							itemIdx = i;
							return true;
						}
					}
					return false; // section found, key not present
				}
			}
			return false; // no section
		}

	public:
		IniFile() = default;

		// Log an error
		void logError(std::string error)
		{
			errorLog += "Ini error: " + error + "\n";
		}
		
		// Get all errors
		const std::string& getErrorLog() const
		{
			return errorLog;
		}
		
		// Clear the error log
		void clearErrorLog()
		{
			errorLog.clear();
		}

		// --------- basic string get (first match), with fallback ----------
		std::string getValue(
			const std::string& caption,
			const std::string& key,
			const std::string& fallback = "") const
		{
			for (const auto& sec : sections)
			{
				if (sec.caption == caption)
				{
					for (const auto& it : sec.items)
					{
						if (it.key == key)
						{
							return it.value;
						}
					}
					return fallback;
				}
			}
			return fallback;
		}

		// (legacy) numeric key accessor (first match), with fallback = ""
		std::string getValue(const std::string& caption, int numericKey) const
		{
			for (const auto& sec : sections)
			{
				if (sec.caption == caption)
				{
					for (const auto& it : sec.items)
					{
						try
						{
							if (std::stoi(it.key) == numericKey)
								return it.value;
						}
						catch (...) { /* ignore */ }
					}
					return "";
				}
			}
			return "";
		}

		// -------- exists / set / delete (strings) ----------
		bool existsString(const std::string& caption,
			const std::string& key) const
		{
			size_t s, i; return findFirstItem(caption, key, s, i);
		}

		// setString: update first match or append if missing
		bool setString(const std::string& caption, const std::string& key,
			const std::string& value)
		{
			size_t s, i;
			if (findFirstItem(caption, key, s, i))
			{
				sections[s].items[i].value = value;
				return true;
			}
			size_t secIdx = 0; ensureSection(caption, secIdx);
			sections[secIdx].items.push_back(Item(key, value));
			return true;
		}

		// deleteString: remove ALL duplicates of key in that section.
		// returns true if removed at least one; false if none existed.
		bool deleteString(const std::string& caption, const std::string& key)
		{
			for (auto& sec : sections)
			{
				if (sec.caption == caption)
				{
					const size_t before = sec.items.size();

					sec.items.erase(
						std::remove_if(
							sec.items.begin(),
							sec.items.end(),
							[&](const Item& it)
								{
									return it.key == key;
								}
							),
						sec.items.end()
					);

					return sec.items.size() != before;
				}
			}
			return false;
		}

		// -------- int ----------
		int getInt(
			const std::string& caption,
			const std::string& key,
			int fallback = 0) const
		{
			const std::string v = getValue(caption, key, "");
			if (v.empty()) return fallback;
			try { return std::stoi(trim(v)); }
			catch (...) { /* keep fallback */ }
			return fallback;
		}

		bool setInt(
			const std::string& caption,
			const std::string& key,
			int value)
		{
			return setString(caption, key, std::to_string(value));
		}

		bool existsInt(
			const std::string& caption,
			const std::string& key) const
		{
			return existsString(caption, key);
		}

		bool deleteInt(const std::string& caption, const std::string& key)
		{
			return deleteString(caption, key);
		}

		// -------- float (uses std::stof) ----------
		float getFloat(
			const std::string& caption,
			const std::string& key,
			float fallback = 0.0f) const
		{
			const std::string v = getValue(caption, key, "");
			if (v.empty()) return fallback;
			try { return std::stof(trim(v)); }
			catch (...) {}
			return fallback;
		}

		bool setFloat(
			const std::string& caption,
			const std::string& key,
			float value)
		{
			std::ostringstream ss; ss << value;
			return setString(caption, key, ss.str());
		}

		bool existsFloat(
			const std::string& caption,
			const std::string& key) const
		{
			return existsString(caption, key);
		}

		bool deleteFloat(const std::string& caption, const std::string& key)
		{
			return deleteString(caption, key);
		}

		// -------- bool ----------
		// Default true/false words (case-insensitive)
		static inline const char* const DEFAULT_TRUE_WORDS[6] =
			{ "1","true","yes","on","y","t" };

		static inline const char* const DEFAULT_FALSE_WORDS[6] =
			{ "0","false","no","off","n","f" };

		// Customizable parser
		bool getBool(const std::string& caption,
			const std::string& key,
			bool fallback,
			const std::vector<std::string>& trueWords,
			const std::vector<std::string>& falseWords) const
		{
			std::string v = trim(lower(getValue(caption, key, "")));
			if (v.empty()) return fallback;

			for (const auto& w : trueWords)  if (v == lower(w))  return true;
			for (const auto& w : falseWords) if (v == lower(w))  return false;
			return fallback;
		}

		// Default parser
		bool getBool(const std::string& caption, const std::string& key,
			bool fallback = false) const
		{
			std::vector<std::string> t, f;
			for (const char* s : DEFAULT_TRUE_WORDS)  if (s) t.push_back(s);
			for (const char* s : DEFAULT_FALSE_WORDS) if (s) f.push_back(s);
			return getBool(caption, key, fallback, t, f);
		}

		bool setBool(const std::string& caption, const std::string& key,
			bool value)
		{
			// write canonical "true"/"false"
			return setString(caption, key, value ? "true" : "false");
		}

		bool existsBool(const std::string& caption,
			const std::string& key) const
		{
			return existsString(caption, key);
		}

		bool deleteBool(const std::string& caption, const std::string& key)
		{
			return deleteString(caption, key);
		}

		// -------- load ----------
		bool loadIni(const char* path)
		{
			std::ifstream file(path, std::ios::in | std::ios::binary);
			if (!file)
			{
				logError("Cannot open INI file for read.");
				return false;
			}

			sections.clear();

			// UTF-8 BOM skip
			{
				char bom[3] = { 0,0,0 };
				file.read(bom, 3);
				bool hasBOM = (file.gcount() == 3 &&
					(unsigned char)bom[0] == 0xEF &&
					(unsigned char)bom[1] == 0xBB &&
					(unsigned char)bom[2] == 0xBF);
				if (!hasBOM) { file.clear(); file.seekg(0, std::ios::beg); }
			}

			std::string line;
			Section* current = nullptr;

			while (std::getline(file, line))
			{
				// normalize line endings, trim
				line = trim(line);
				if (line.empty()) continue;

				// comment lines (first non-space ; or #)
				if (line[0] == ';' || line[0] == '#') continue;

				// section?
				if (line.front() == '[' && line.back() == ']')
				{
					const std::string caption = trim(
						line.substr(1, line.size() - 2)
					);

					sections.push_back(Section(caption));
					current = &sections.back();
					continue;
				}

				// key=value (within a section)
				if (current)
				{
					const size_t eq = line.find('=');
					if (eq == std::string::npos) continue;
					std::string key = trim(line.substr(0, eq));
					std::string val = line.substr(eq + 1);
					current->items.push_back(Item(key, val));
				}
			}
			return true;
		}

		// -------- save (overwrite) ----------
		bool saveIni(const char* path)
		{
			std::ofstream file(
				path, std::ios::out | std::ios::trunc | std::ios::binary);

			if (!file)
			{
				logError("Cannot open INI file for write.");
				return false;
			}

			for (size_t s = 0; s < sections.size(); ++s)
			{
				const auto& sec = sections[s];
				file << "[" << sec.caption << "]\n";
				for (const auto& it : sec.items)
				{
					file << it.key << "=" << it.value << "\n";
				}
				if (s + 1 < sections.size())
					file << "\n";
			}
			// ofstream closes on dtor; return success if stream ok
			if (!file.good())
			{
				logError("Write failed while saving INI.");
				return false;
			}

			return true;
		}
	};
}
