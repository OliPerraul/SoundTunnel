#pragma once


#include "AudioApi.h"

#include <Utils/Semaphore.h>

#include <thread>


class AudioDuplicator {
	CComPtr<IMMDevice> sourceDevice;
	CComPtr<IMMDevice> sinkDevice;


	volatile bool stop = false;
	Semaphore uninitSemaphore;
public:
	AudioDuplicator();

	
	HRESULT SetSourceDevice(CComPtr<IMMDevice> device);
	HRESULT SetSinkDevice(CComPtr<IMMDevice> device);

	HRESULT Run();
	HRESULT RunAsync();
	void Stop();
	void WaitForDestroy();
private:
	thread backgroundThread;
private:
	HRESULT SelectDevice(CComPtr<IMMDeviceEnumerator> pEnumerator, const vector<CString>& preferredDevices, _Out_ CComPtr<IMMDevice>& device);
	HRESULT Init();
	HRESULT InitDefaultDevices();
};





