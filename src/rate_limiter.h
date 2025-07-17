#pragma once

#include <chrono>
#include <cstdint>


// ограничение объема передаваемых данных основано на использовании принципа накопления "токенов", которые
// дают право на передачу байт данных (1 токен = 1 байт)
class rate_limiter {
public:
    // rate: сколько байт добавляется в секунду
    // burst: сколько максимум может накопиться токенов
    rate_limiter(uint64_t rate_bytes_per_sec, uint64_t burst_bytes);

    // Возвращает true, если можно передать пакет нужного размера
    bool allow(uint64_t packet_size);

    // Возвращает текущее число токенов
    uint64_t tokens() const;

private:
    uint64_t _rate;         // байт/сек
    uint64_t _burst;        // макс токенов
    double _tokens;         // дробные токены для точности
    std::chrono::steady_clock::time_point _last_update;
};

