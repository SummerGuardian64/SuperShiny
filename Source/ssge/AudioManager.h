#pragma once
#include <string>
#include <unordered_map>
#include <SDL.h>
#include <SDL_mixer.h>
#include "PassKey.h"

namespace ssge
{
    class AudioManager
    {
        int masterVol = 100; // 0..100
        int musicVol = 100; // 0..100
        int sfxVol = 100; // 0..100

        Mix_Music* currentMusic = nullptr;
        std::string currentMusicPath;

        // tiny cache (good enough for the spree)
        std::unordered_map<std::string, Mix_Music*> musics;
        std::unordered_map<std::string, Mix_Chunk*> chunks;

        static int toSDL(int pct)
        { // 0..100 -> 0..128
            if (pct <= 0) return 0;
            if (pct >= 100) return MIX_MAX_VOLUME;
            return (pct * MIX_MAX_VOLUME) / 100;
        }
        void applyVolumes()
        {
            // music volume = master * music
            int m = (toSDL(masterVol) * musicVol) / 100;
            Mix_VolumeMusic(m);

            // sfx volume = master * sfx
            int s = (toSDL(masterVol) * sfxVol) / 100;
            // set default channel volume; we also set per-chunk when (re)loaded
            Mix_Volume(-1, s);
            for (auto& kv : chunks) Mix_VolumeChunk(kv.second, s);
        }

    public:
        AudioManager(PassKey<Engine>pk) {};
        AudioManager(const AudioManager& toCopy) = delete;
        AudioManager(AudioManager&& toMove) = delete;
        ~AudioManager()
        {
            shutdown();
        }

        bool init(PassKey<Engine>pk)
        {
            // Keep it simple: OGG/WAV are safest across XP+.
            // (If you ship MP3, make sure the required DLLs are present.)
            int want = MIX_INIT_OGG; // add MIX_INIT_MP3 if you ship mp3 DLLs
            if ((Mix_Init(want) & want) != want) {
                SDL_Log("Mix_Init: %s", Mix_GetError());
                // not fatal if WAV-only, continue
            }
            if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
                SDL_Log("Mix_OpenAudio: %s", Mix_GetError());
                return false;
            }
            // A few channels for SFX
            Mix_AllocateChannels(16);
            applyVolumes();
            return true;
        }

        void shutdown()
        {
            Mix_HaltChannel(-1);
            Mix_HaltMusic();
            for (auto& kv : chunks) Mix_FreeChunk(kv.second);
            chunks.clear();
            for (auto& kv : musics) Mix_FreeMusic(kv.second);
            musics.clear();
            Mix_CloseAudio();
            Mix_Quit();
        }

        void setMasterVolume(int v) { masterVol = SDL_clamp(v, 0, 100); applyVolumes(); }
        void setMusicVolume(int v) { musicVol = SDL_clamp(v, 0, 100); applyVolumes(); }
        void setSfxVolume(int v) { sfxVol = SDL_clamp(v, 0, 100); applyVolumes(); }

        int  getMasterVolume() const { return masterVol; }
        int  getMusicVolume() const { return musicVol; }
        int  getSfxVolume() const { return sfxVol; }

        // Music
        bool playMusic(const std::string& path, int loops = -1)
        {
            Mix_Music* m = nullptr;
            auto it = musics.find(path);
            if (it == musics.end())
            {
                m = Mix_LoadMUS(path.c_str());
                if (!m)
                {
                    SDL_Log("Mix_LoadMUS %s: %s", path.c_str(), Mix_GetError());
                    return false;
                }
                musics[path] = m;
            }
            else
            {
                m = it->second;
            }
            currentMusic = m;
            currentMusicPath = path;
            applyVolumes();
            if (Mix_PlayMusic(m, loops) < 0)
            {
                SDL_Log("Mix_PlayMusic: %s", Mix_GetError());
                return false;
            }
            return true;
        }
        void stopMusic() { Mix_HaltMusic(); }
        void pauseMusic() { Mix_PauseMusic(); }
        void resumeMusic() { Mix_ResumeMusic(); }

        // SFX
        bool loadSfx(const std::string& path)
        {
            if (chunks.count(path)) return true;
            Mix_Chunk* c = Mix_LoadWAV(path.c_str());
            if (!c)
            {
                SDL_Log("Mix_LoadWAV %s: %s", path.c_str(), Mix_GetError());
                return false;
            }
            // honor current sfx volume right away
            Mix_VolumeChunk(c, (toSDL(masterVol) * sfxVol) / 100);
            chunks[path] = c;
            return true;
        }

        int playSfx(const std::string& path, int loops = 0)
        {
            if (!loadSfx(path))
                return -1;

            return Mix_PlayChannel(-1, chunks[path], loops);
        }

        // Is anything currently playing (not paused)?
        bool isMusicPlaying() const
        {
            return Mix_PlayingMusic() == 1 && Mix_PausedMusic() == 0;
        }

        // Is THIS track playing (and not paused)?
        bool isMusicPlaying(const std::string& path) const
        {
            return isMusicPlaying() && (path == currentMusicPath);
        }

        // Play only if not already playing this track
        bool playMusicIfNotPlaying(const std::string& path, int loops = -1)
        {
            if (isMusicPlaying(path))
                return true;

            return playMusic(path, loops);
        }
    };

} // namespace ssge
