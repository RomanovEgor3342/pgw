
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

- n — количество чисел в выражении 

- connections — количество параллельных TCP-соединений

- server_addr — IP-адрес сервера (например, 127.0.0.1)

- server_port — порт, на котором слушает сервер
