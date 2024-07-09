#pragma once
#include <typeinfo>
#include <sstream>
#include <iostream>

namespace lg
{
	template <typename T>
	class Flags
	{
	public:
		using TMask = typename std::underlying_type<T>::type;

	public:
		Flags() :flags(0) {}
		Flags(T flag) : flags((TMask) flag) {}
		Flags(TMask flag) : flags(flag) {}

		void Set(T flag)
		{
			flags |= (TMask) flag;
		}
		void Unset(T flag)
		{
			flags &= !(TMask) flag;
		}
		void SetState(T flag, bool state)
		{
			if (state)
				Set(flag);
			else
				Unset(flag);
		}
		bool IsSet(T flag) const
		{
			return (flags & (TMask) flag) == (TMask) flag;
		}
		operator TMask() const { return flags; }

	private:
		TMask flags;
	};

	enum Color
	{
		ResetColor,
		White,
		Black,
		Blue,
		Green,
		Cyan,
		Red,
		Magenta,
		Yellow,
		LBlack,
		LWhite,
		LBlue,
		LGreen,
		LCyan,
		LRed,
		LMagenta,
		LYellow,
		BGWhite,
		BGBlack,
		BGBlue,
		BGGreen,
		BGCyan,
		BGRed,
		BGMagenta,
		BGYellow,
		BGLBlack,
		BGLWhite,
		BGLBlue,
		BGLGreen,
		BGLCyan,
		BGLRed,
		BGLMagenta,
		BGLYellow,
		LastColor = LYellow,
	};
	std::ostream& operator<<(std::ostream& os, const Color& rhs);

	enum FontBits
	{
		ResetFont = 0,
		Bold = 2,
		NoBold = 3,
		Underline = 4,
		NoUnderline = 5,
		CrossedOut = 8,
		NoCrossedOut = 9,
		Italic = 16,
		NoItalic = 17,
	};
	std::ostream& operator<<(std::ostream& os, const FontBits& rhs);
	typedef Flags<FontBits> Font;
	std::ostream& operator<<(std::ostream& os, const Font& rhs);

	struct Indention
	{
		char level;
		Indention(char level = 0) :level(level) {}
	};
	std::ostream& operator<<(std::ostream& os, const Indention& rhs);

	enum LogType
	{
		None = 0,
		Raw,
		Info,
		Warning,
		Error,
	};

	class Logger
	{
		LogType lastLogType = LogType::None;
		Color fontColor;
		Color backgroundColor;
		Font font;
		Indention indention;
		char lastLogged = ' ';
		bool loggedSomething;
		std::ostream* output;

	public:
		Logger(std::ostream* output = nullptr) :output(output) {}

		Color GetColor() const { return fontColor; }
		Color GetBackground() const { return backgroundColor; }
		bool GetFont() const { return font; }
		Indention GetIndention() const { return indention; }
		void SetOutput(std::ostream* target) { output = target; }

		template <typename... T>
		void operator()(const T&...o)
		{
			if (output == nullptr) output = &std::cout;

			loggedSomething = false;
			(SpecialHandling(*output, o), ...);

			if (lastLogType != LogType::None && lastLogType != LogType::Raw)
			{
				fontColor = Color::White;
				backgroundColor = Color::BGBlack;
				*output << fontColor << backgroundColor;
			}
			if (lastLogType != LogType::Raw && loggedSomething)
			{
				*output << "\n";
				lastLogged = '\n';
			}
		}

		~Logger()
		{
			*output << FontBits::ResetFont << ResetColor;
		}

	private:
		std::ostream& OutputIndention()
		{
			if (lastLogged != '\n') return *output;

			lastLogged == ' ';
			return *output << FontBits::ResetFont << indention << font;
		}
		void SetColor(Color color = Color::ResetColor)
		{
			if ((int) color <= (int) Color::LastColor)
				fontColor = color;
			else
				backgroundColor = color;

			if (color == Color::ResetColor)
			{
				fontColor = Color::White;
				backgroundColor = Color::BGBlack;
				*output << fontColor << backgroundColor;
			}
			else
				*output << color;
		}
		void SetFont(Font font = { FontBits::ResetFont })
		{
			this->font = font;
			if (font == FontBits::ResetFont)
				this->font = FontBits::ResetFont;

			*output << font << fontColor << backgroundColor;

		}

		template<typename T>
		constexpr std::ostream& SpecialHandling(std::ostream& os, const T& val)
		{
			if constexpr (std::is_same<T, Indention>::value)
			{
				indention = val;
				return os;
			}
			else if constexpr (std::is_same<T, FontBits>::value)
			{
				if (val == FontBits::ResetFont)
				{
					SetFont(0);
					return os;
				}
				int bit = val / 2;
				bool reset = val % 2;
				font.SetState((FontBits) bit, !reset);
				SetFont(font);
				return os;
			}
			else if constexpr (std::is_same<T, Font>::value)
			{
				SetFont(val);
				return os;
			}
			else if constexpr (std::is_same<T, Color>::value)
			{
				SetColor(val);
				return os;
			}
			else if constexpr (std::is_same<T, LogType>::value)
			{
				Font f;
				switch (val)
				{
				case LogType::None: lastLogType = val; return os;
				case LogType::Raw: lastLogType = val; return os;
				case LogType::Info: lastLogType = val; loggedSomething = true;  OutputIndention(); f = font; SetFont({ (FontBits::Bold / 2) }); SetColor(Color::Green); os << "Info: "; SetFont(f); lastLogged = ' '; return os;
				case LogType::Warning: lastLogType = val; loggedSomething = true;  OutputIndention(); f = font; SetFont({ (FontBits::Bold / 2) }); SetColor(Color::Yellow); os << "Warning: "; SetFont(f); lastLogged = ' '; return os;
				case LogType::Error: lastLogType = val; loggedSomething = true; OutputIndention(); f = font; SetFont({ (FontBits::Bold / 2) }); SetColor(Color::LRed); os << "Error: "; SetFont(f); lastLogged = ' '; return os;
				default:return os;
				}
			}
			else
			{
				if (lastLogged != '\n' && lastLogged != ' ' && lastLogType != LogType::Raw)
				{
					os << ' ';
					lastLogged = ' ';
				}

				std::ostringstream oss;
				oss << val;
				OutputIndention();
				for (auto&& c : oss.str())
				{
					lastLogged = c;
					os << c;
					OutputIndention();
				}
				loggedSomething = true;

				return os;
			}
		}
	};
	inline Logger Log;

	std::ostream& operator<<(std::ostream& os, const Color& rhs)
	{
		if (&os != &std::cout) return os;
		switch (rhs)
		{
		case Color::White: return os << "\033[37m";
		case Color::Black: return os << "\033[30m";
		case Color::Blue: return os << "\033[34m";
		case Color::Green: return os << "\033[32m";
		case Color::Cyan: return os << "\033[36m";
		case Color::Red: return os << "\033[31m";
		case Color::Magenta: return os << "\033[35m";
		case Color::Yellow: return os << "\033[33m";
		case Color::LBlack: return os << "\033[90m";
		case Color::LWhite: return os << "\033[97m";
		case Color::LBlue: return os << "\033[94m";
		case Color::LGreen: return os << "\033[92m";
		case Color::LCyan: return os << "\033[96m";
		case Color::LRed: return os << "\033[91m";
		case Color::LMagenta: return os << "\033[95m";
		case Color::LYellow: return os << "\033[93m";
		case Color::BGWhite: return os << "\033[47m";
		case Color::BGBlack: return os << "\033[40m";
		case Color::BGBlue: return os << "\033[44m";
		case Color::BGGreen: return os << "\033[42m";
		case Color::BGCyan: return os << "\033[46m";
		case Color::BGRed: return os << "\033[41m";
		case Color::BGMagenta: return os << "\033[45m";
		case Color::BGYellow: return os << "\033[43m";
		case Color::BGLBlack: return os << "\033[100m";
		case Color::BGLWhite: return os << "\033[47m";
		case Color::BGLBlue: return os << "\033[104m";
		case Color::BGLGreen: return os << "\033[102m";
		case Color::BGLCyan: return os << "\033[106m";
		case Color::BGLRed: return os << "\033[101m";
		case Color::BGLMagenta: return os << "\033[105m";
		case Color::BGLYellow: return os << "\033[103m";
		default:return os;
		}
	}
	std::ostream& operator<<(std::ostream& os, const Font& rhs)
	{
		if (&os != &std::cout) return os;
		os << FontBits::ResetFont;
		for (int i = 2; i < 32; i += 2)
		{
			if (rhs.IsSet((FontBits) (i / 2)))
				os << (FontBits) i;
		}
		return os;
	}
	std::ostream& operator<<(std::ostream& os, const FontBits& rhs)
	{
		if (&os != &std::cout) return os;
		switch (rhs)
		{
		case FontBits::ResetFont: return os << "\033[0m";
		case FontBits::Bold: return os << "\033[1m";
		case FontBits::Underline: return os << "\033[4m";
		case FontBits::CrossedOut: return os << "\033[9m";
		case FontBits::Italic: return os << "\033[3m";
		default:return os;
		}
	}
	std::ostream& operator<<(std::ostream& os, const Indention& rhs)
	{
		return os << std::string(rhs.level, ' ');
	}
}