#include <iostream>
#include <Windows.h>

#define NUM_OF_TOOLS 5
struct TrackingState {
	double Trs[3]; // X, Y, Z
	double Quat[4]; // W, X, Y, Z

	bool bTracked; // true : Tracked, false : Untracked
};

struct TrackingMessage {
	TrackingState TrackingStates[NUM_OF_TOOLS];
	bool bLocked = false;
};

HANDLE MemoryMapper = NULL;
HANDLE MemoryMapPtr = NULL;
TrackingMessage* Message = nullptr;

bool InitSharedMemory()
{
	MemoryMapper = CreateFileMapping(
		INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
		sizeof(TrackingMessage), L"TrackingStates"
	);

	if (!MemoryMapper) 
	{
		std::cout << "Failed CreateFileMapping" << std::endl;
		return false;
	}

	MemoryMapPtr = (BYTE*)MapViewOfFile(MemoryMapper, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	
	if (!MemoryMapPtr)
	{
		CloseHandle(MemoryMapper);
		std::cout << "Failed MapViewOfFile" << std::endl;
		return false;
	}

	Message = (TrackingMessage*)MemoryMapPtr;
	return true;
}

void CloseSharedMemory()
{
	if (MemoryMapPtr)
	{
		UnmapViewOfFile(MemoryMapPtr);
	}

	if (MemoryMapper)
	{
		CloseHandle(MemoryMapper);
	}
}

int main(int argc, char** argv)
{
	std::cout << "MemoryWriter" << std::endl;
	InitSharedMemory();

	while (true)
	{
#if false
		char Key;
		std::cin >> Key;


		if (Key == 'q')
		{
			break;
		}
		else if (Key == 's')
		{
			static int Cnt = 0;
			Cnt++;

			for (int Index = 0; Index < NUM_OF_TOOLS; Index++)
			{
				TrackingStateList[Index].Trs[0] = 1.0 * Cnt;
				TrackingStateList[Index].Trs[1] = 2.0 * Cnt;
				TrackingStateList[Index].Trs[2] = 3.0 * Cnt;
			}
		}
#endif
		static int Cnt = 0;
		if (Message->bLocked == false)
		{
			Message->bLocked = true;
			std::cout << "Count : " << Cnt << std::endl;
			Cnt++;
		}
		else
		{
			std::cout << "Locked" << std::endl;
			continue;
		}

		for (int Index = 0; Index < NUM_OF_TOOLS; Index++)
		{
			Message->TrackingStates[Index].Trs[0] = 1.0 * Cnt;
			Message->TrackingStates[Index].Trs[1] = 1.0 * Cnt;
			Message->TrackingStates[Index].Trs[2] = 1.0 * Cnt;
		}
		Message->bLocked = false;

		Sleep(100);
	}
	CloseSharedMemory();

	return 0;
}