#pragma once
#include <core/utility/strings.h>

// MrMAPI command line
namespace cli
{
	struct OPTIONS
	{
		int mode{};
		int optionFlags{};
		std::wstring lpszUnswitchedOption;
		// dummy function to force a vtable so we can use dynamic_cast
		virtual void __dummy() noexcept {};
	};

	class option
	{
	private:
		bool seen{false};

	public:
		LPCWSTR szSwitch{};
		int mode{};
		UINT minArgs{};
		UINT maxArgs{};
		int optionFlags{};
		std::vector<std::wstring> args;
		std::function<bool(OPTIONS& _options)> testArgs = 0;

		option() = default;

		option(
			LPCWSTR _szSwitch,
			int _mode,
			UINT _minArgs,
			UINT _maxArgs,
			int _optionFlags,
			std::function<bool(OPTIONS& _options)> _testArgs = 0)
		{
			szSwitch = _szSwitch;
			mode = _mode;
			minArgs = _minArgs;
			maxArgs = _maxArgs;
			optionFlags = _optionFlags;
			testArgs = _testArgs;
		}

		bool isSet() const noexcept { return seen; }
		bool hasArgs() const noexcept { return !args.empty(); }
		bool hasArg(size_t i) const noexcept { return args.size() > i; }
		std::wstring getArg(size_t i) const { return args.size() > i ? args[i] : std::wstring{}; }
		bool hasArgAsULONG(size_t i, int radix = 10) const
		{
			ULONG num{};
			return strings::tryWstringToUlong(num, args[i], radix, true);
		}

		ULONG getArgAsULONG(size_t i, int radix = 10) const
		{
			return args.size() > i ? strings::wstringToUlong(args[i], radix) : 0;
		}

		_Check_return_ bool
		scanArgs(std::deque<std::wstring>& args, OPTIONS& options, const std::vector<option*>& optionsArray);
	};

	enum modeEnum
	{
		cmdmodeUnknown = 0,
		cmdmodeHelp,
		cmdmodeHelpFull,
		cmdmodeFirstMode, // When extending modes, use this as the value of the first mode
	};

	enum flagsEnum
	{
		OPT_NOOPT = 0x00000,
		OPT_VERBOSE = 0x00001,
		OPT_INITMFC = 0x00002,
	};

	extern option switchInvalid;
	extern option switchHelp;
	extern option switchVerbose;

	// Checks if szArg is an option, and if it is, returns which option it is
	// We return the first match, so switches should be ordered appropriately
	// The first switch should be our "no match" switch
	option* GetOption(const std::wstring& szArg, const std::vector<option*>& _options);

	// If the mode isn't set (is 0), then we can set it to any mode
	// If the mode IS set (non 0), then we can only set it to the same mode
	// IE trying to change the mode from anything but unset will fail
	bool bSetMode(_In_ int& pMode, _In_ int targetMode);

	std::deque<std::wstring> GetCommandLine(_In_ int argc, _In_count_(argc) const char* const argv[]);

	// Parses command line arguments and fills out OPTIONS
	void ParseArgs(
		OPTIONS& options,
		std::deque<std::wstring>& args,
		const std::vector<option*>& optionsArray,
		std::function<void(OPTIONS& _options)> postParseCheck);

	void PrintArgs(_In_ const OPTIONS& ProgOpts, const std::vector<option*>& optionsArray);
} // namespace cli