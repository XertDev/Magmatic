target_sources(
		magmatic-sound
		PRIVATE
		SoundDevice.cpp
		SoundBuffer.cpp
		SoundSource.cpp
		formats/SoundLoaderVorbis.cpp
		formats/SoundLoaderOpus.cpp
)