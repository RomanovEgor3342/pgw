#include "rate_limiter.h"

rate_limiter::rate_limiter(uint64_t rate_bytes_per_sec, uint64_t burst_bytes)
    : _rate(rate_bytes_per_sec),
      _burst(burst_bytes),
      _tokens(burst_bytes), // начинаем с полного ведра
      _last_update(std::chrono::steady_clock::now()) {}

bool rate_limiter::allow(uint64_t packet_size) {
    using namespace std::chrono;

    auto now = steady_clock::now();
    duration<double> elapsed = now - _last_update;
    _last_update = now;

    // добавляем токены в зависимости от прошедшего времени
    _tokens += elapsed.count() * _rate;
    if (_tokens > _burst) {
        _tokens = _burst;
    }

    if (packet_size > _burst) {
        // пакет слишком большой — никогда не пройдет
        // данная реализация не предполагает возможность передачи пакетов, размер которых превышает _burst,
        // решением могло бы стать дробление пакетов на стороне отправителя или использование асинхронной очереди для накопления токенов,
        // если в системе приемлемы задержки
        return false;
    }

    if (_tokens >= packet_size) {
        _tokens -= packet_size;
        return true;
    }

    return false;
}

uint64_t rate_limiter::tokens() const {
    return static_cast<uint64_t>(_tokens);
}