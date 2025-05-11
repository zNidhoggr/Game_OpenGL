#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <AL/al.h>
#include <AL/alc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <functional>

class AudioManager
{
public:
    // Estrutura para armazenar informações de fontes 3D
    struct Audio3DSource {
        ALuint sourceId;
        float posX;
        float posY;
        float posZ;
        float volume;
        int bufferId;
        bool isPlaying;
    };

    AudioManager() : device(nullptr), context(nullptr) {}

    ~AudioManager()
    {
        cleanupOpenAL();
    }

    // Carrega múltiplos arquivos de áudio de uma vez
    bool carregarAudios(std::initializer_list<std::string> listaDeAudios, const std::string &pasta = "./src/audios/")
    {
        size_t id = 0;
        bool allLoaded = true;
        
        for (const auto &nomeArquivo : listaDeAudios)
        {
            std::string caminhoCompleto = pasta + nomeArquivo;
            if (!this->loadWavFile(caminhoCompleto.c_str(), id))
            {
                std::cerr << "Falha ao carregar o áudio: " << caminhoCompleto << std::endl;
                allLoaded = false;
            }
            ++id;
        }
        return allLoaded;
    }

    // Inicializa o OpenAL
    bool initOpenAL()
    {
        // Limpar erros anteriores
        alGetError();
        
        // Abrir dispositivo
        device = alcOpenDevice(NULL);
        if (!device)
        {
            std::cerr << "Erro ao abrir dispositivo OpenAL." << std::endl;
            return false;
        }

        // Criar contexto
        context = alcCreateContext(device, NULL);
        if (!context)
        {
            std::cerr << "Erro ao criar contexto OpenAL." << std::endl;
            alcCloseDevice(device);
            return false;
        }
        alcMakeContextCurrent(context);

        // Verificar por erros após inicialização
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro durante inicialização do OpenAL: " << error << std::endl;
            cleanupOpenAL();
            return false;
        }

        std::cout << "OpenAL inicializado com sucesso." << std::endl;
        return true;
    }

    // Libera recursos do OpenAL
    void cleanupOpenAL()
    {
        // Liberar todas as fontes e buffers ativos
        releaseAllAudio();

        // Liberar contexto e dispositivo
        alcMakeContextCurrent(NULL);
        if (context)
        {
            alcDestroyContext(context);
            context = nullptr;
        }
        if (device)
        {
            alcCloseDevice(device);
            device = nullptr;
        }
        std::cout << "Recursos OpenAL liberados." << std::endl;
    }

    // Carrega um arquivo WAV
    bool loadWavFile(const char *filename, int id)
    {
        // Limpar erros anteriores do OpenAL
        alGetError();

        FILE *file = fopen(filename, "rb");
        if (!file)
        {
            std::cerr << "Erro ao abrir arquivo WAV: " << filename << std::endl;
            return false;
        }

        // Estruturas para cabeçalhos WAV
        struct {
            char chunkID[4];
            unsigned int chunkSize;
            char format[4];
        } header;

        struct {
            char subChunkID[4];
            unsigned int subChunkSize;
            unsigned short audioFormat;
            unsigned short numChannels;
            unsigned int sampleRate;
            unsigned int byteRate;
            unsigned short blockAlign;
            unsigned short bitsPerSample;
        } fmtChunk;

        struct {
            char subChunkID[4];
            unsigned int subChunkSize;
        } dataChunk;

        // Lendo o cabeçalho RIFF
        if (fread(&header, sizeof(header), 1, file) != 1)
        {
            std::cerr << "Erro ao ler cabeçalho RIFF: " << filename << std::endl;
            fclose(file);
            return false;
        }

        // Verificando se é um arquivo RIFF WAVE válido
        if (strncmp(header.chunkID, "RIFF", 4) != 0 || strncmp(header.format, "WAVE", 4) != 0)
        {
            std::cerr << "Arquivo não é um WAV válido: " << filename << std::endl;
            fclose(file);
            return false;
        }

        // Procurar chunk "fmt "
        bool foundFmt = false;
        while (!foundFmt)
        {
            if (fread(&fmtChunk.subChunkID, sizeof(fmtChunk.subChunkID), 1, file) != 1)
            {
                std::cerr << "Erro ao procurar chunk fmt: " << filename << std::endl;
                fclose(file);
                return false;
            }

            if (strncmp(fmtChunk.subChunkID, "fmt ", 4) == 0)
            {
                foundFmt = true;

                // Ler tamanho do chunk
                if (fread(&fmtChunk.subChunkSize, sizeof(fmtChunk.subChunkSize), 1, file) != 1)
                {
                    std::cerr << "Erro ao ler tamanho do chunk fmt: " << filename << std::endl;
                    fclose(file);
                    return false;
                }

                // Ler restante do chunk fmt
                if (fread(&fmtChunk.audioFormat, fmtChunk.subChunkSize, 1, file) != 1)
                {
                    std::cerr << "Erro ao ler dados do chunk fmt: " << filename << std::endl;
                    fclose(file);
                    return false;
                }
            }
            else
            {
                // Pular chunk desconhecido
                unsigned int skipSize;
                if (fread(&skipSize, sizeof(skipSize), 1, file) != 1)
                {
                    std::cerr << "Erro ao ler tamanho de chunk desconhecido: " << filename << std::endl;
                    fclose(file);
                    return false;
                }
                fseek(file, skipSize, SEEK_CUR);
            }
        }

        // Verificar formato de áudio (deve ser PCM = 1)
        if (fmtChunk.audioFormat != 1)
        {
            std::cerr << "Formato de áudio não suportado (apenas PCM linear): " << filename << std::endl;
            fclose(file);
            return false;
        }

        // Procurar chunk "data"
        bool foundData = false;
        while (!foundData)
        {
            if (fread(&dataChunk.subChunkID, sizeof(dataChunk.subChunkID), 1, file) != 1)
            {
                std::cerr << "Erro ao procurar chunk data: " << filename << std::endl;
                fclose(file);
                return false;
            }

            if (strncmp(dataChunk.subChunkID, "data", 4) == 0)
            {
                foundData = true;

                // Ler tamanho dos dados
                if (fread(&dataChunk.subChunkSize, sizeof(dataChunk.subChunkSize), 1, file) != 1)
                {
                    std::cerr << "Erro ao ler tamanho do chunk data: " << filename << std::endl;
                    fclose(file);
                    return false;
                }
            }
            else
            {
                // Pular chunk desconhecido
                unsigned int skipSize;
                if (fread(&skipSize, sizeof(skipSize), 1, file) != 1)
                {
                    std::cerr << "Erro ao ler tamanho de chunk desconhecido: " << filename << std::endl;
                    fclose(file);
                    return false;
                }
                fseek(file, skipSize, SEEK_CUR);
            }
        }

        // Alocar buffer para dados de áudio usando smart pointer
        std::unique_ptr<unsigned char[]> data(new unsigned char[dataChunk.subChunkSize]);
        if (!data)
        {
            std::cerr << "Erro ao alocar memória para dados de áudio: " << filename << std::endl;
            fclose(file);
            return false;
        }

        // Ler dados de áudio
        if (fread(data.get(), 1, dataChunk.subChunkSize, file) != dataChunk.subChunkSize)
        {
            std::cerr << "Erro ao ler dados de áudio: " << filename << std::endl;
            fclose(file);
            return false;
        }

        fclose(file);

        // Determinar formato do áudio para OpenAL
        ALenum format;
        if (fmtChunk.numChannels == 1)
        {
            format = (fmtChunk.bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
        }
        else if (fmtChunk.numChannels == 2)
        {
            format = (fmtChunk.bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
        }
        else
        {
            std::cerr << "Número de canais não suportado (" << fmtChunk.numChannels << "): " << filename << std::endl;
            return false;
        }

        // Verificar se o buffer já existe e deletá-lo se necessário
        if (buffers.find(id) != buffers.end())
        {
            alDeleteBuffers(1, &buffers[id]);
            buffers.erase(id);
        }

        // Gerar novo buffer e carregar dados
        ALuint buffer;
        alGenBuffers(1, &buffer);

        // Verificar erro após criar buffer
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao gerar buffer de áudio: " << error << std::endl;
            return false;
        }

        // Carregar dados no buffer
        alBufferData(buffer, format, data.get(), dataChunk.subChunkSize, fmtChunk.sampleRate);

        // Verificar erro após carregar dados
        error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao carregar buffer de áudio (" << filename << "): " << error << std::endl;
            alDeleteBuffers(1, &buffer);
            return false;
        }

        // Armazenar buffer
        buffers[id] = buffer;

        std::cout << "Áudio carregado com sucesso: " << filename 
                  << " (Canais: " << fmtChunk.numChannels 
                  << ", Bits: " << fmtChunk.bitsPerSample 
                  << ", Taxa: " << fmtChunk.sampleRate << " Hz)" << std::endl;
        return true;
    }

    // Reproduzir áudio uma vez
    void playAudio(int id, float volume = 1.0f)
    {
        if (buffers.find(id) == buffers.end())
        {
            std::cerr << "Áudio com ID " << id << " não encontrado." << std::endl;
            return;
        }

        // Limpar erros anteriores
        alGetError();

        // Reutilizar fonte existente se disponível
        if (sources.find(id) != sources.end())
        {
            ALuint source = sources[id];
            alSourceStop(source);
            alSourcef(source, AL_GAIN, volume);
            alSourcePlay(source);

            // Verificar erros
            ALenum error = alGetError();
            if (error != AL_NO_ERROR)
            {
                std::cerr << "Erro ao reproduzir áudio (ID " << id << "): " << error << std::endl;
            }
            return;
        }

        // Criar nova fonte
        ALuint source;
        alGenSources(1, &source);
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao gerar fonte de áudio (ID " << id << "): " << error << std::endl;
            return;
        }

        alSourcei(source, AL_BUFFER, buffers[id]);
        alSourcef(source, AL_GAIN, volume);
        alSourcePlay(source);

        error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao configurar/reproduzir fonte (ID " << id << "): " << error << std::endl;
            alDeleteSources(1, &source);
            return;
        }

        sources[id] = source;
    }

    // Reproduzir áudio em repetição
    void playAudioRepeter(int id, float volume = 1.0f)
    {
        if (buffers.find(id) == buffers.end())
        {
            std::cerr << "Áudio com ID " << id << " não encontrado." << std::endl;
            return;
        }

        // Limpar erros anteriores
        alGetError();

        // Verificar se já existe uma fonte para este ID em repeatingAudio
        if (repeatingAudio.find(id) != repeatingAudio.end())
        {
            ALuint source = repeatingAudio[id];
            // Atualizar volume da fonte existente
            alSourcef(source, AL_GAIN, volume);

            // Verificar se já está tocando
            ALint state;
            alGetSourcei(source, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING)
            {
                alSourcePlay(source);

                // Verificar erros
                ALenum error = alGetError();
                if (error != AL_NO_ERROR)
                {
                    std::cerr << "Erro ao reproduzir áudio em loop (ID " << id << "): " << error << std::endl;
                }
            }
            return;
        }

        // Se não existe, criar nova fonte se não exceder o limite
        if (repeatingAudio.size() >= MAX_SOURCES)
        {
            std::cerr << "Limite de fontes de repetição atingido. Som descartado." << std::endl;
            return;
        }

        ALuint source;
        alGenSources(1, &source);
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao gerar fonte de áudio em loop (ID " << id << "): " << error << std::endl;
            return;
        }

        alSourcei(source, AL_BUFFER, buffers[id]);
        alSourcef(source, AL_GAIN, volume);     // Definir o volume
        alSourcei(source, AL_LOOPING, AL_TRUE); // Ativar repetição automática
        alSourcePlay(source);

        error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao configurar/reproduzir fonte em loop (ID " << id << "): " << error << std::endl;
            alDeleteSources(1, &source);
            return;
        }

        repeatingAudio[id] = source;
        activeSources.push_back(source);
    }

    // Reproduzir áudio em 3D
    ALuint playAudio3D(int id, float volume, float posX, float posY, float posZ)
    {
        if (buffers.find(id) == buffers.end())
        {
            std::cerr << "Áudio com ID " << id << " não encontrado." << std::endl;
            return 0;
        }

        // Limpar erros anteriores
        alGetError();

        ALuint source;
        alGenSources(1, &source);

        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao gerar fonte de áudio 3D: " << error << std::endl;
            return 0;
        }

        alSourcei(source, AL_BUFFER, buffers[id]);
        alSourcef(source, AL_GAIN, volume);

        // Definindo a posição 3D
        alSource3f(source, AL_POSITION, posX, posY, posZ);

        // Configurar atenuação de distância para sons 3D
        alSourcef(source, AL_REFERENCE_DISTANCE, 1.0f); // Distância onde o som é mais alto
        alSourcef(source, AL_MAX_DISTANCE, 20.0f);      // Distância onde o som não é mais ouvido
        alSourcef(source, AL_ROLLOFF_FACTOR, 1.0f);     // Fator de queda de volume

        alSourcePlay(source);

        error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao reproduzir áudio 3D (ID " << id << "): " << error << std::endl;
            alDeleteSources(1, &source);
            return 0;
        }
        
        // Armazenar informações da fonte 3D para atualização futura
        Audio3DSource sourceInfo = {
            source,
            posX, posY, posZ,
            volume,
            id,
            true
        };
        
        audio3DSources[source] = sourceInfo;
        activeSources.push_back(source);
        
        return source; // Retorna o ID da fonte para permitir atualizações
    }

    // Nova função para atualizar a posição de um áudio 3D existente
    bool updateAudio3DPosition(ALuint sourceId, float posX, float posY, float posZ)
    {
        if (audio3DSources.find(sourceId) == audio3DSources.end())
        {
            std::cerr << "Fonte de áudio 3D com ID " << sourceId << " não encontrada." << std::endl;
            return false;
        }
        
        // Limpar erros anteriores
        alGetError();
        
        // Atualizar a posição da fonte
        alSource3f(sourceId, AL_POSITION, posX, posY, posZ);
        
        // Verificar erros
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao atualizar posição do áudio 3D (ID " << sourceId << "): " << error << std::endl;
            return false;
        }
        
        // Atualizar informações armazenadas
        audio3DSources[sourceId].posX = posX;
        audio3DSources[sourceId].posY = posY;
        audio3DSources[sourceId].posZ = posZ;
        
        return true;
    }

    // Verifica se uma fonte de áudio 3D específica está tocando
    bool isAudio3DPlaying(ALuint sourceId) const
    {
        if (audio3DSources.find(sourceId) == audio3DSources.end())
        {
            return false;
        }
        
        ALint state;
        alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }

    // Parar áudio em repetição
    void stopAudioRepeter(int id)
    {
        if (repeatingAudio.find(id) == repeatingAudio.end())
        {
            return;
        }

        ALuint source = repeatingAudio[id];
        alSourceStop(source);

        // Verificar erros
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao parar áudio em loop (ID " << id << "): " << error << std::endl;
        }
    }

    // Parar áudio normal
    void stopAudio(int id)
    {
        if (sources.find(id) == sources.end())
        {
            return;
        }
        ALuint source = sources[id];
        alSourceStop(source);

        // Verificar erros
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao parar áudio (ID " << id << "): " << error << std::endl;
        }
    }

    // Parar áudio 3D específico
    void stopAudio3D(ALuint sourceId)
    {
        if (audio3DSources.find(sourceId) == audio3DSources.end())
        {
            return;
        }
        
        alSourceStop(sourceId);
        
        // Verificar erros
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao parar áudio 3D (ID " << sourceId << "): " << error << std::endl;
        }
    }

    // Verificar se um áudio está tocando
    bool isAudioPlaying(int id) const
    {
        auto sourceIt = sources.find(id);
        if (sourceIt != sources.end())
        {
            ALint state;
            alGetSourcei(sourceIt->second, AL_SOURCE_STATE, &state);
            return state == AL_PLAYING;
        }

        auto repeatingIt = repeatingAudio.find(id);
        if (repeatingIt != repeatingAudio.end())
        {
            ALint state;
            alGetSourcei(repeatingIt->second, AL_SOURCE_STATE, &state);
            return state == AL_PLAYING;
        }

        return false;
    }

    // Parar todos os áudios
    void stopAllAudio()
    {
        alGetError(); // Limpar erros anteriores

        for (auto &pair : sources)
        {
            alSourceStop(pair.second);
        }

        for (auto &pair : repeatingAudio)
        {
            alSourceStop(pair.second);
        }
        
        for (auto &pair : audio3DSources)
        {
            alSourceStop(pair.first);
        }

        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao parar todos os áudios: " << error << std::endl;
        }
    }

    // Liberar todos os recursos de áudio
    void releaseAllAudio()
    {
        // Limpar erros anteriores
        alGetError();

        // Parar e liberar fontes normais
        for (auto &pair : sources)
        {
            alSourceStop(pair.second);
            alDeleteSources(1, &pair.second);
        }
        sources.clear();

        // Parar e liberar fontes em repetição
        for (auto &pair : repeatingAudio)
        {
            alSourceStop(pair.second);
            alDeleteSources(1, &pair.second);
        }
        repeatingAudio.clear();
        
        // Parar e liberar fontes 3D
        for (auto &pair : audio3DSources)
        {
            alSourceStop(pair.first);
            alDeleteSources(1, &pair.first);
        }
        audio3DSources.clear();
        
        activeSources.clear();

        // Limpar buffers
        for (auto &pair : buffers)
        {
            alDeleteBuffers(1, &pair.second);
        }
        buffers.clear();

        // Verificar erros
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao liberar recursos de áudio: " << error << std::endl;
        }
        else
        {
            std::cout << "Todos os recursos de áudio foram liberados com sucesso." << std::endl;
        }
    }

    // Definir volume de áudio
    void setVolume(int id, float volume)
    {
        // Limpar erros anteriores
        alGetError();

        // Limitar volume entre 0.0 e 1.0
        volume = std::max(0.0f, std::min(volume, 1.0f));

        // Verificar em fontes normais
        if (sources.find(id) != sources.end())
        {
            alSourcef(sources[id], AL_GAIN, volume);
            ALenum error = alGetError();
            if (error != AL_NO_ERROR)
            {
                std::cerr << "Erro ao ajustar volume (ID " << id << "): " << error << std::endl;
            }
            return;
        }

        // Verificar em fontes de repetição
        if (repeatingAudio.find(id) != repeatingAudio.end())
        {
            alSourcef(repeatingAudio[id], AL_GAIN, volume);
            ALenum error = alGetError();
            if (error != AL_NO_ERROR)
            {
                std::cerr << "Erro ao ajustar volume em loop (ID " << id << "): " << error << std::endl;
            }
            return;
        }

        std::cerr << "Áudio com ID " << id << " não encontrado para ajuste de volume." << std::endl;
    }
    
    // Definir volume de áudio 3D
    void setAudio3DVolume(ALuint sourceId, float volume)
    {
        if (audio3DSources.find(sourceId) == audio3DSources.end())
        {
            std::cerr << "Fonte de áudio 3D com ID " << sourceId << " não encontrada." << std::endl;
            return;
        }
        
        // Limpar erros anteriores
        alGetError();
        
        // Limitar volume entre 0.0 e 1.0
        volume = std::max(0.0f, std::min(volume, 1.0f));
        
        alSourcef(sourceId, AL_GAIN, volume);
        
        // Verificar erros
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao ajustar volume do áudio 3D (ID " << sourceId << "): " << error << std::endl;
            return;
        }
        
        // Atualizar informações armazenadas
        audio3DSources[sourceId].volume = volume;
    }
    
    // Configurar ouvinte 3D (posição do jogador)
    void setListener3DPosition(float posX, float posY, float posZ, 
                               float forwardX, float forwardY, float forwardZ, 
                               float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f)
    {
        // Posição do ouvinte
        alListener3f(AL_POSITION, posX, posY, posZ);
        
        // Orientação do ouvinte (frente e cima)
        float orientation[6] = {forwardX, forwardY, forwardZ, upX, upY, upZ};
        alListenerfv(AL_ORIENTATION, orientation);
        
        // Verificar erros
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "Erro ao configurar ouvinte 3D: " << error << std::endl;
        }
    }

    // Limpar fontes não utilizadas para gerenciar recursos
    void cleanupUnusedSources()
    {
        // Limpar erros anteriores
        alGetError();
        
        // Verificar fontes normais
        for (auto it = sources.begin(); it != sources.end();)
        {
            ALint state;
            alGetSourcei(it->second, AL_SOURCE_STATE, &state);
            
            if (state != AL_PLAYING)
            {
                alDeleteSources(1, &it->second);
                it = sources.erase(it);
            }
            else
            {
                ++it;
            }
        }
        
        // Verificar fontes 3D
        for (auto it = audio3DSources.begin(); it != audio3DSources.end();)
        {
            ALint state;
            alGetSourcei(it->first, AL_SOURCE_STATE, &state);
            
            if (state != AL_PLAYING)
            {
                alDeleteSources(1, &it->first);
                it = audio3DSources.erase(it);
            }
            else
            {
                ++it;
            }
        }
        
        // Atualizar lista de fontes ativas
        activeSources.clear();
        for (auto &pair : sources) activeSources.push_back(pair.second);
        for (auto &pair : repeatingAudio) activeSources.push_back(pair.second);
        for (auto &pair : audio3DSources) activeSources.push_back(pair.first);
    }

private:
    ALCdevice *device;
    ALCcontext *context;
    std::unordered_map<int, ALuint> buffers;             // Mapeia IDs para buffers
    std::unordered_map<int, ALuint> sources;             // Mapeia IDs para fontes normais
    std::unordered_map<int, ALuint> repeatingAudio;      // Mapeia IDs para fontes em repetição
    std::unordered_map<ALuint, Audio3DSource> audio3DSources; // Mapeia fontes 3D pelo ID da fonte
    std::vector<ALuint> activeSources;                   // Lista de fontes ativas para rastreamento
    const int MAX_SOURCES = 32;                          // Limite de fontes ativas (aumentado)
};

#endif // AUDIOMANAGER_HPP