#include "sound/formats/SoundLoaderOpus.hpp"
#include <opus/opusfile.h>
#include <spdlog/spdlog.h>


bool magmatic::sound::SoundLoaderOpus::registered = magmatic::sound::Sound::register_format(
		SoundLoaderOpus::factoryName(),
		SoundLoaderOpus::createLoader
);

std::shared_ptr<magmatic::sound::SoundBuffer> magmatic::sound::SoundLoaderOpus::open(const std::filesystem::path& path)
{
	if(!std::filesystem::exists(path))
	{
		spdlog::error("Magamtic: Sound file doesn't exist: {}", path.string());
		throw std::runtime_error("Sound file doesn't exist");
	}

	int error = 0;
	OggOpusFile* ogg_file = op_open_file(path.string().c_str(), &error);

	if(error)
	{
		spdlog::error("Magmatic: Failed to decode Opus file");
		throw std::runtime_error("Failed to decode Opus file");
	}

	const auto channel_count = op_channel_count(ogg_file, -1);
	const auto pcm_length = op_pcm_total(ogg_file, -1);

	if(channel_count > 2)
	{
		spdlog::error("Magmatic: Sound files with more that 2 channels not supported");
		throw std::runtime_error("Sound file format not supported");
	}

	const auto format = channel_count == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
	size_t sample_read = 0;

	int16_t * buffer = new int16_t[pcm_length*channel_count];

	ALuint buffer_ID;
	alGenBuffers(1, &buffer_ID);
	if (auto error = alGetError(); error != AL_NO_ERROR)
	{
		spdlog::error("Magmatic: Failed to create sound buffer");
		throw std::runtime_error("Failed to create sound buffer");
	}

	while(sample_read < pcm_length)
	{
		size_t temp_readed = op_read(ogg_file, buffer + sample_read * channel_count, pcm_length * channel_count, 0);
		if(temp_readed < 0)
		{
			spdlog::error("Magmatic: Error while decoding opus file");
			throw std::runtime_error("Error while decoding opus file");
		}
		sample_read += temp_readed;
	}
	op_free(ogg_file);

	alBufferData(buffer_ID, format, buffer, sample_read * channel_count * sizeof(int16_t), 48000);
	if (auto error = alGetError(); error != AL_NO_ERROR)
	{
		spdlog::error("Magmatic: Failed to transfer audio to buffer");
		throw std::runtime_error("Failed to transfer audio to buffer");
	}

	return SoundLoader::fromID(buffer_ID);
}

std::unique_ptr<magmatic::sound::SoundLoader> magmatic::sound::SoundLoaderOpus::createLoader()
{
	return std::make_unique<SoundLoaderOpus>();
}
