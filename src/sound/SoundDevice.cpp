#include "sound/SoundDevice.hpp"
#include <AL/alc.h>
#include <spdlog/spdlog.h>
#include <AL/al.h>

void magmatic::sound::SoundDevice::ALCdeviceDeleter::operator()(ALCdevice* pointer) noexcept
{
	alcCloseDevice(pointer);
}

void magmatic::sound::SoundDevice::ALCcontextDeleter::operator()(ALCcontext* pointer) noexcept
{
	if(alcGetCurrentContext() == pointer)
	{
		alcMakeContextCurrent(nullptr);
	}
	alcDestroyContext(pointer);
}

magmatic::sound::SoundDevice::SoundDevice(const char* device)
:alc_device(alcOpenDevice(device)), alc_context(alcCreateContext(alc_device.get(), nullptr))
{
	if(!alc_device)
	{
		spdlog::error("Magmatic: Failed to open audio device");
		throw std::runtime_error("Failed to open audio device");
	}
	if(!alc_context)
	{
		spdlog::error("Magmatic: Failed to create context");
		throw std::runtime_error(" Failed to create context");
	}
	if(!alcMakeContextCurrent(alc_context.get()))
	{
		spdlog::error("Magmatic: Failed to set current context");
		throw std::runtime_error(" Failed to set current context");
	}
	alDistanceModel(AL_EXPONENT_DISTANCE);
}

std::vector<std::string> magmatic::sound::SoundDevice::enumerateDevices() noexcept
{
	const ALCchar* devices;
	devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

	const ALCchar* tmp = devices;

	std::vector<std::string> retval;

	do
	{
		retval.emplace_back(std::string(tmp));
		tmp += retval.back().size()+1;
	}
	while(*(tmp+1) != '\0');

	return retval;
}
