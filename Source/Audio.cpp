#include "Audio.h"

Audio* Audio::instance = nullptr; // definition class variable

//OpenAL error checking
#define OpenAL_ErrorCheck(message)\
{\
	ALenum error = alGetError();\
	if( error != AL_NO_ERROR)\
	{\
		std::cerr << "OpenAL Error: " << error << " with call for " << #message << std::endl;\
	}\
}

#define alec(FUNCTION_CALL)\
FUNCTION_CALL;\
OpenAL_ErrorCheck(FUNCTION_CALL)

// Credits To: https://github.com/matts-dev/OpenAL_TestProject/blob/master/TestOpenAL/src/no_gpl_dependency/main_nogpl.cpp

Audio::Audio() {
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// find the default audio device
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const ALCchar* defaultDeviceString = alcGetString(/*device*/nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
	device = alcOpenDevice(defaultDeviceString);
	if (!device)
	{
		std::cerr << "failed to get the default device for OpenAL" << std::endl;
		return;
	}
	std::cout << "OpenAL Device: " << alcGetString(device, ALC_DEVICE_SPECIFIER) << std::endl;
	//OpenAL_ErrorCheck(device);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create an OpenAL audio context from the device
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	context = alcCreateContext(device, /*attrlist*/ nullptr);
	//OpenAL_ErrorCheck(context);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Activate this context so that OpenAL state modifications are applied to the context
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (!alcMakeContextCurrent(context))
	{
		std::cerr << "failed to make the OpenAL context the current context" << std::endl;
		return;
	}
	//OpenAL_ErrorCheck("Make context current");

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create a listener in 3d space (ie the player); (there always exists as listener, you just configure data on it)
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	alec(alListener3f(AL_POSITION, 0.f, 0.f, 0.f));
	alec(alListener3f(AL_VELOCITY, 0.f, 0.f, 0.f));
	ALfloat forwardAndUpVectors[] = {
		/*forward = */ 1.f, 0.f, 0.f,
		/* up = */ 0.f, 1.f, 0.f
	};
	alec(alListenerfv(AL_ORIENTATION, forwardAndUpVectors));

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create buffers that hold our sound data; these are shared between contexts and ar defined at a device level
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	struct ReadWavData
	{
		unsigned int channels = 0;
		unsigned int sampleRate = 0;
		drwav_uint64 totalPCMFrameCount = 0;
		std::vector<uint16_t> pcmData;
		drwav_uint64 getTotalSamples() { return totalPCMFrameCount * channels; }
	};
	ReadWavData monoData;
	{
		drwav_int16* pSampleData = drwav_open_file_and_read_pcm_frames_s16("sounds/grunt-mono.wav", &monoData.channels, &monoData.sampleRate, &monoData.totalPCMFrameCount, nullptr);
		if (pSampleData == NULL) {
			std::cerr << "failed to load audio file" << std::endl;
			drwav_free(pSampleData, nullptr); //todo use raii to clean this up
			return;
		}
		if (monoData.getTotalSamples() > drwav_uint64(std::numeric_limits<size_t>::max()))
		{
			std::cerr << "too much data in file for 32bit addressed vector" << std::endl;
			drwav_free(pSampleData, nullptr);
			return;
		}
		monoData.pcmData.resize(size_t(monoData.getTotalSamples()));
		std::memcpy(monoData.pcmData.data(), pSampleData, monoData.pcmData.size() * /*twobytes_in_s16*/2);
		drwav_free(pSampleData, nullptr);
	}

	alec(alGenBuffers(1, &monoSoundBuffer));
	alec(alBufferData(monoSoundBuffer, monoData.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, monoData.pcmData.data(), monoData.pcmData.size() * 2 /*two bytes per sample*/, monoData.sampleRate));

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// load a stereo file into a buffer
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ReadWavData stereoData;
	{
		drwav_int16* pSampleData = drwav_open_file_and_read_pcm_frames_s16("sounds/levelup-stereo.wav", &stereoData.channels, &stereoData.sampleRate, &stereoData.totalPCMFrameCount, nullptr);
		if (pSampleData == NULL) {
			std::cerr << "failed to load audio file" << std::endl;
			return;
		}
		if (stereoData.getTotalSamples() > drwav_uint64(std::numeric_limits<size_t>::max()))
		{
			std::cerr << "too much data in file for 32bit addressed vector" << std::endl;
			return;
		}
		stereoData.pcmData.resize(size_t(stereoData.getTotalSamples()));
		std::memcpy(stereoData.pcmData.data(), pSampleData, stereoData.pcmData.size() * /*twobytes_in_s15*/2);
		drwav_free(pSampleData, nullptr);
	}

	alec(alGenBuffers(1, &stereoSoundBuffer));
	alec(alBufferData(stereoSoundBuffer, stereoData.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, stereoData.pcmData.data(), stereoData.pcmData.size() * 2 /*two bytes per sample*/, stereoData.sampleRate));

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// create a sound source that plays our mono sound (from the sound buffer)
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	alec(alGenSources(1, &monoSource));
	alec(alSource3f(monoSource, AL_POSITION, 1.f, 0.f, 0.f));
	alec(alSource3f(monoSource, AL_VELOCITY, 0.f, 0.f, 0.f));
	alec(alSourcef(monoSource, AL_PITCH, 1.f));
	alec(alSourcef(monoSource, AL_GAIN, 1.f));
	alec(alSourcei(monoSource, AL_LOOPING, AL_FALSE));
	alec(alSourcei(monoSource, AL_BUFFER, monoSoundBuffer));


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// create a sound source for our stereo sound; note 3d positioning doesn't work with stereo files because
	// stereo files are typically used for music. stereo files come out of both ears so it is hard to know
	// what the sound should be doing based on 3d position data.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	alec(alGenSources(1, &stereoSource));
	//alec(alSource3f(stereoSource, AL_POSITION, 0.f, 0.f, 1.f)); //NOTE: this does not work like mono sound positions!
	//alec(alSource3f(stereoSource, AL_VELOCITY, 0.f, 0.f, 0.f)); 
	alec(alSourcef(stereoSource, AL_PITCH, 1.f));
	alec(alSourcef(stereoSource, AL_GAIN, 1.f));
	alec(alSourcei(stereoSource, AL_LOOPING, AL_FALSE));
	alec(alSourcei(stereoSource, AL_BUFFER, stereoSoundBuffer));

	/*
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// play the mono sound source
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	alec(alSourcePlay(monoSource));
	ALint sourceState;
	alec(alGetSourcei(monoSource, AL_SOURCE_STATE, &sourceState));
	while (sourceState == AL_PLAYING)
	{
		//basically loop until we're done playing the mono sound source
		alec(alGetSourcei(monoSource, AL_SOURCE_STATE, &sourceState));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// play the stereo sound source after the mono!
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	alec(alSourcePlay(stereoSource));
	alec(alGetSourcei(stereoSource, AL_SOURCE_STATE, &sourceState));
	while (sourceState == AL_PLAYING)
	{
		//basically loop until we're done playing the mono sound source
		alec(alGetSourcei(stereoSource, AL_SOURCE_STATE, &sourceState));
	}
	*/

	threadStopped = false;
	audioThread = std::thread(&Audio::pollAudio, this);

}

void Audio::playAudio(int x) {
	if (x == 0) {
		mixerMutex.lock();
		alec(alSourcePlay(monoSource));
		mixer.push_back(0);
		mixerMutex.unlock();
	}
	else if (x == 1) {
		mixerMutex.lock();
		alec(alSourcePlay(stereoSource));
		mixer.push_back(1);
		mixerMutex.unlock();
	}
}

void Audio::pollAudio() {
	while (!threadStopped) {
		mixerMutex.lock();
		std::erase_if(mixer, [this](const int x) -> bool {
			ALint source_state = AL_PLAYING;
			ALuint soundSource;
			if (x == 0) {
				soundSource = monoSource;
			}
			else {
				soundSource = stereoSource;
			}
			alGetSourcei(stereoSource, AL_SOURCE_STATE, &source_state);
			if (source_state != AL_STOPPED)
				return false;

			return true;
		});
		mixerMutex.unlock();

		static constexpr int UPDATES_PER_SECOND = 200;
		static constexpr int UPDATE_FRAME_MS = 1000 / UPDATES_PER_SECOND;
		std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_FRAME_MS));
	}
}

Audio::~Audio() {
	threadStopped = true;
	audioThread.join();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// clean up our resources!
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	alec(alDeleteSources(1, &monoSource));
	alec(alDeleteSources(1, &stereoSource));
	alec(alDeleteBuffers(1, &monoSoundBuffer));
	alec(alDeleteBuffers(1, &stereoSoundBuffer));
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(context);
	alcCloseDevice(device);
}


