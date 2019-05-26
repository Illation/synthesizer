#pragma once

#include <windows.h>

enum LogLevel
{
	Info = 0x1,
	Warning = 0x2,
	Error = 0x4,
	FixMe = 0x8, 
	Verbose = 0x10
};

class Logger
{
public:
	typedef std::pair<int32, int32> T_CursorPos;

	static void Log(const std::string& msg, LogLevel level = LogLevel::Info,
		bool timestamp = false, T_CursorPos cursorPos = std::make_pair(-1, -1));

	static T_CursorPos GetCursorPosition();

	static void StartFileLogging(const std::string& filename);
	static void StopFileLogging();

	static void UseTimestampDate(bool val) { m_TimestampDate = val; }

	static bool IsInitialized() { return m_IsInitialized; }

	static void Initialize();
	static void Release();
private:

	static void CheckBreak(LogLevel level);

private:
	class AbstractLogger
	{
	protected:
		std::ostream* m_os;
	public:
		AbstractLogger() {}
		virtual ~AbstractLogger() {}

		virtual void Log(const std::string& message)
		{
			(*m_os) << message;
			m_os->flush();
		}
		virtual void SetCursorPosition(T_CursorPos cursorPos) { UNUSED(cursorPos); }
	};

	class FileLogger : public AbstractLogger
	{
		std::string m_filename;
	public:
		explicit FileLogger(const std::string& fileName)
			: m_filename(fileName)
		{
			m_os = new std::ofstream(m_filename.c_str());
		}
		~FileLogger()
		{
			if (m_os)
			{
				std::ofstream* of = static_cast<std::ofstream*>(m_os);
				of->close();
				delete m_os;
			}
		}
	};

	class ConsoleLogger : public AbstractLogger
	{
	public:
		ConsoleLogger();
		enum class Color
		{
			WHITE,
			RED,
			GREEN,
			YELLOW,
			MAGENTA
		};
		void SetColor(ConsoleLogger::Color color);
		void SetCursorPosition(T_CursorPos cursorPos) override;
		T_CursorPos GetCursorPosition();
	private:
#ifdef PLATFORM_Win
		HANDLE m_ConsoleHandle;
#endif
	};

	class DebugLogger : public AbstractLogger
	{
	public:
		DebugLogger() {}
		virtual ~DebugLogger() {}
		void Log(const std::string& message)override;
	};

	static ConsoleLogger* m_ConsoleLogger;
	static FileLogger* m_FileLogger;
	static DebugLogger* m_DebugLogger;

	static uint8 m_BreakBitField;
	static bool m_TimestampDate;
	static bool m_IsInitialized;

private:
	//Disable default constructor and destructor
	Logger() = default;
	~Logger() = default;
	Logger(const Logger &obj);
	Logger& operator=(const Logger& obj);
};

