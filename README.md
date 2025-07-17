
## Сборка

Проект использует `CMake`.

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Структура проекта

- control_plane — управление PDN Connection, Bearer и APN.

- pdn_connection — информация о подключении абонента к PDN.

- bearer — Bearer для передачи данных.

- data_plane — обработка uplink/downlink трафика, интеграция с ограничителем скорости.

- rate_limiter — реализация token bucket для ограничения скорости.

- Тесты с использованием GoogleTest.

## Запуск тестов
Из папки build
```bash
test/simple_pgw_tests
```

