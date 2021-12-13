#include <iostream>
#include <windows.h>
#include <filesystem>
#include <shlobj.h>
#include <thread>
#include <vector>
#include <string>

#pragma intrinsic(__rdtsc)

#define MAXMODULE 50

typedef void(__cdecl* cfunc)(const char*);
typedef void(__cdecl* cwfunc)(const wchar_t*);

void CalcPartFxn(int x, int i, double& result)
{
	static int k = 3;

	double sum = 0;
	for (int j = 1; j <= i; j++)
	{
		sum += (j + pow(x + j, 1 / k)) / (2 * i * j - 1);
	}

	result += 1 / sum;
}

void FxnPart(int x, int start, int n, double& result)
{
	std::time_t startSeconds = std::time(nullptr);
	int nNextOutput = n / 10;
	int percents = 0;

	for (int i = start; i <= n; i++)
	{
		CalcPartFxn(x, i, result);

		if (nNextOutput == i)
		{
			nNextOutput += n / 10;
			percents += 10;
			std::cout << "Current percent: " << percents << std::endl;
			std::time_t curSeconds = std::time(nullptr);
			int nSecondsLods = curSeconds - startSeconds;
			std::cout << "Seconds lost: " << nSecondsLods << std::endl;
			double dSecondsLeft = round((double(100 - percents) / percents) * nSecondsLods);
			std::cout << "Seconds left: " << dSecondsLeft << std::endl;
		}
	}
}

void Fxn(int x, int n, double& result)
{
	FxnPart(x, 1, n, result);
}

void ThreadFxn(int x, int n, double& result)
{
	static int nThreads = 5;
	std::vector<double> vPartsFxn;
	std::vector<std::thread> vThreads;
	vPartsFxn.resize(nThreads);

	for (int nThread = 1; nThread <= nThreads; nThread++)
	{
		int nStart = round(((nThread - 1) * double(n / nThreads)) + 1);
		int nEnd = round(double(n / nThreads) * nThread);
		vThreads.push_back(std::thread(FxnPart, x, nStart, nEnd, std::ref(vPartsFxn[nThread - 1])));
	}

	for (int nThread = 1; nThread <= nThreads; nThread++)
	{
		vThreads[nThread - 1].join();
	}

	for (int nThread = 1; nThread <= nThreads; nThread++)
	{
		result += vPartsFxn[nThread - 1];
	}
}

int main()
{
	unsigned __int64 nTacts, nTactsThread;
	nTacts = __rdtsc();
	double dResult = 0;
	Fxn(1000, 500, dResult);
	nTacts = __rdtsc() - nTacts;
	std::cout << "Tacts to standard Fxn: " << nTacts << std::endl;

	nTactsThread = __rdtsc();
	double dResultThread = 0;
	ThreadFxn(1000, 500, dResultThread);
	nTactsThread = __rdtsc() - nTactsThread;
	std::cout << "Tacts to threaded Fxn: " << nTactsThread << std::endl;

	cwfunc SetLogFileName;
	cfunc SetLogFormat;
	cfunc LogPrintError;
	cfunc LogPrintDebug;
	cfunc LogPrintTrace;

	// Get a handle to the DLL module.
	HINSTANCE hinstLib = LoadLibrary(L"logger.dll");
	BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

	// If the handle is valid, try to get the function address.
	if (hinstLib != NULL)
	{
		SetLogFileName = (cwfunc)GetProcAddress(hinstLib, "SetLoggerFileName");
		SetLogFormat = (cfunc)GetProcAddress(hinstLib, "SetLoggerFormat");
		LogPrintError = (cfunc)GetProcAddress(hinstLib, "LoggerPrintError");
		LogPrintDebug = (cfunc)GetProcAddress(hinstLib, "LoggerPrintDebug");
		LogPrintTrace = (cfunc)GetProcAddress(hinstLib, "LoggerPrintTrace");

		// If the function address is valid, call the function.

		if ((SetLogFileName != NULL) && (SetLogFormat != NULL) && (LogPrintError != NULL)
			&& (LogPrintDebug != NULL) && (LogPrintTrace != NULL))
		{
			fRunTimeLinkSuccess = TRUE;

			SetLogFormat("%%time%% %data%\t*prior*\t**message**");

			SetLogFileName(L"result.log");
			std::string sResult = "Tacts to standard Fxn: " + std::to_string(nTacts) + ". Tacts to threaded Fxn: " + std::to_string(nTactsThread);
			LogPrintTrace(sResult.c_str());
		}
		// Free the DLL module.

		fFreeResult = FreeLibrary(hinstLib);
	}

	// If unable to call the DLL function, use an alternative.
	if (!fRunTimeLinkSuccess)
		MessageBox(nullptr, L"Unable to call the DLL function.", L"Error", MB_OK | MB_ICONERROR);

	return 0;
}
