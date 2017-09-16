#pragma once


#include "AudioApi.h"


class AudioDuplicator {

	CComPtr<IAudioClient> pDefaultAudioClient;
	CComPtr<IAudioClient> pSecondaryAudioClient;
	CComPtr<IAudioCaptureClient> pCaptureClient;
	CComPtr<IAudioRenderClient> pRenderClient;
	CComHeapPtr<WAVEFORMATEX> pwfx;
	UINT32 bufferFrameCount;
	volatile bool stop = false;
public:
	AudioDuplicator() {

		

		HRESULT hr;

		// Common init 
		hr = CoInitialize(NULL);
		EXIT_ON_ERROR(hr);

		CComPtr<IMMDeviceEnumerator> pEnumerator;
		hr = CoCreateInstance(
			CLSID_MMDeviceEnumerator, NULL,
			CLSCTX_ALL, IID_IMMDeviceEnumerator,
			(void**)&pEnumerator
		);
		EXIT_ON_ERROR(hr);


		// render 'hook' init

		CComPtr<IMMDevice> pDefaultDevice;
		hr = pEnumerator->GetDefaultAudioEndpoint(
			eRender, eConsole, &pDefaultDevice);
		EXIT_ON_ERROR(hr);


		//CComPtr<IAudioClient> pAudioClient;
		hr = pDefaultDevice->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&pDefaultAudioClient);
		EXIT_ON_ERROR(hr);


		// TODO ezt amugy releaselni k�ne... de h�t a faszom...
		//WAVEFORMATEX* pwfx = nullptr;
		hr = pDefaultAudioClient->GetMixFormat(&pwfx);
		EXIT_ON_ERROR(hr);


		REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
		hr = pDefaultAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, hnsRequestedDuration, 0, pwfx, NULL);
		EXIT_ON_ERROR(hr);


		// Get the size of the allocated buffer.
		hr = pDefaultAudioClient->GetBufferSize(&bufferFrameCount);
		EXIT_ON_ERROR(hr);


		hr = pDefaultAudioClient->GetService(
			IID_IAudioCaptureClient,
			(void**)&pCaptureClient);
		EXIT_ON_ERROR(hr);

		// init renderer instance


		CComPtr<IMMDevice> secondaryDevice;
		hr = SelectDevice(pEnumerator, secondaryDevice);
		EXIT_ON_ERROR(hr);

		hr = secondaryDevice->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&pSecondaryAudioClient);
		EXIT_ON_ERROR(hr);

		// TODO a n�gyes csodakonstanssal valamit kezdeni
		DWORD audclnt_streamflags = 0;
		hr = pSecondaryAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, audclnt_streamflags, hnsRequestedDuration * 4, 0, pwfx, NULL);
		//hr = pSecondaryAudioClient->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE, audclnt_streamflags, hnsRequestedDuration * 4, 0, pwfx, NULL);
		EXIT_ON_ERROR(hr);

		hr = pSecondaryAudioClient->GetService(
			IID_IAudioRenderClient,
			(void**)&pRenderClient);
		EXIT_ON_ERROR(hr);
	}



	// TODO lehetne hozz� gui-t �rni
	HRESULT SelectDevice(CComPtr<IMMDeviceEnumerator> pEnumerator, CComPtr<IMMDevice>& device) {
		CString preferredSecondaryDeviceName = L"SAMSUNG";

		HRESULT hr = S_OK;

		CComPtr<IMMDevice> pDefaultDevice;
		hr = pEnumerator->GetDefaultAudioEndpoint(
			eRender, eConsole, &pDefaultDevice);
		EXIT_ON_ERROR(hr);




		//CComHeapPtr<wchar_t> pDefaultDeviceId;
		//hr = pDefaultDevice->GetId(&pDefaultDeviceId);
		//EXIT_ON_ERROR(hr);

		//CComPtr<IPropertyStore> defaultDeviceProperties;
		//hr = pDefaultDevice->OpenPropertyStore(STGM_READ, &defaultDeviceProperties);
		//EXIT_ON_ERROR(hr);
		//
		//
		////PROPVARIANT varFriendlyName;
		//CPropVariant varFriendlyName;
		//PropVariantInit(&varFriendlyName);
		////hr = ppProperties->GetValue(PKEY_DeviceInterface_FriendlyName, &varFriendlyName);
		//hr = defaultDeviceProperties->GetValue(PKEY_Device_FriendlyName, &varFriendlyName);
		//EXIT_ON_ERROR(hr);

		//CString deafultDevicefriendlyName = varFriendlyName.pwszVal;
		CString deafultDevicefriendlyName = GetFriendlyName(pDefaultDevice);



		if (deafultDevicefriendlyName.Find(preferredSecondaryDeviceName) >= 0) {
			// m�r alapb�l a kivanatos volt kivalasztva deafultnak, igy semmi ertelme
			// ILLETVE meg lehet csin�lni, hogy ilyenkor a f�les legyen a secondary
			//  VAGY GUI n lehessen kiv�lasztani
			//hr = ERROR_ALREADY_EXISTS;
			//return hr;
			preferredSecondaryDeviceName = L"Speakers";
		}


		CComPtr<IMMDeviceCollection> deviceCollection;
		hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollection);
		EXIT_ON_ERROR(hr);


		UINT deviceCount;
		hr = deviceCollection->GetCount(&deviceCount);
		EXIT_ON_ERROR(hr);

		//CSimpleDialog

		for (int i = 0; i < int(deviceCount); ++i) {
			device.Release();
			hr = deviceCollection->Item(i, &device);
			EXIT_ON_ERROR(hr);

			//CComHeapPtr<wchar_t> pDeviceId;
			//hr = device->GetId(&pDeviceId);
			//EXIT_ON_ERROR(hr);
			//CString deviceId = static_cast<wchar_t*>(pDeviceId);
			CString devicefriendlyName = GetFriendlyName(device);

			if (devicefriendlyName.Find(preferredSecondaryDeviceName) >= 0) {
				ASSERT(hr == S_OK);
				return hr;
			}

		}
		hr = ERROR_NOT_FOUND;
		return hr;

	}


	void Run() {

		HRESULT hr;

		// Notify the audio sink which format to use.
		//hr = pMySink->SetFormat(pwfx);
		//EXIT_ON_ERROR(hr);



		// Calculate the actual duration of the allocated buffer.
		//REFERENCE_TIME hnsActualDuration = REFERENCE_TIME(double(REFTIMES_PER_SEC) * bufferFrameCount / pwfx->nSamplesPerSec);
		DWORD hnsActualDuration = DWORD(double(REFTIMES_PER_SEC) * bufferFrameCount / pwfx->nSamplesPerSec);

		//hr = pAudioClient->Start();  // Start recording.
		hr = pDefaultAudioClient->Start();
		EXIT_ON_ERROR(hr);

		hr = pSecondaryAudioClient->Start();
		EXIT_ON_ERROR(hr);



		// Each loop fills about half of the shared buffer.
		while (!stop) {
			// Sleep for half the buffer duration.
			Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);

			UINT32 packetLength = 0;
			hr = pCaptureClient->GetNextPacketSize(&packetLength);
			EXIT_ON_ERROR(hr);



			while (packetLength != 0 && !stop) {

				// Get the available data in the shared buffer.
				UINT32 numFramesAvailable;
				DWORD flags;
				BYTE* pCaptureData;
				BYTE* pRenderData;

				hr = pCaptureClient->GetBuffer(
					&pCaptureData,
					&numFramesAvailable,
					&flags,
					NULL,
					NULL
				);
				EXIT_ON_ERROR(hr);

				BOOL bufferIsSilent = (flags & AUDCLNT_BUFFERFLAGS_SILENT);
				//if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
				//	pCaptureData = NULL;  // Tell CopyData to write silence.
				//}

				if (!bufferIsSilent) {



					hr = pRenderClient->GetBuffer(numFramesAvailable, &pRenderData);
					EXIT_ON_ERROR(hr);


					memcpy(pRenderData, pCaptureData, numFramesAvailable * pwfx->nBlockAlign);
					// Load the initial data into the shared buffer.
					//hr = pMySource->LoadData(bufferFrameCount, pData, &flags);
					//EXIT_ON_ERROR(hr);

					DWORD renderFlags = 0;
					hr = pRenderClient->ReleaseBuffer(numFramesAvailable, renderFlags);
					EXIT_ON_ERROR(hr);
					//pRenderClient->
					// Copy the available capture data to the audio sink.
					//hr = pMySink->CopyData(pData, numFramesAvailable, &bDone);
					//EXIT_ON_ERROR(hr);

				}


				hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
				EXIT_ON_ERROR(hr);


				hr = pCaptureClient->GetNextPacketSize(&packetLength);
				EXIT_ON_ERROR(hr);
			}
		}

		hr = pDefaultAudioClient->Stop();  // Stop recording.
		EXIT_ON_ERROR(hr);


		hr = pSecondaryAudioClient->Stop();
		EXIT_ON_ERROR(hr);



	}

	void Stop() {
		stop = true;
	}


	//public:
	//	static shared_ptr<Recorder> Create() {
	//		try {
	//			return shared_ptr<Recorder>();
	//		} catch(RecorderException) {
	//
	//		}
	//		return nullptr;
	//		
	//	}
};



static void DoEverythingLofasz() {
	try {
		shared_ptr<AudioDuplicator> recorder = make_shared<AudioDuplicator>();


		std::thread myThread([&recorder] {
			recorder->Run();

		});

		MessageBox(NULL, L"Press ok to stop", 0, MB_ICONASTERISK);
		recorder->Stop();
		myThread.join();


	}
	catch (AudioApiException) {
		MessageBox(0, L"Recorder exception", 0, 0);
	}



}





