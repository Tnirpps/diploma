**This repository is for userver v2.8 or older versions. For newer versions of userver please use
[userver-create-service](https://userver.tech/de/dab/md_en_2userver_2build_2build.html#autotoc_md177) script.**

# search_market_place

# Сервис преобразования Proto в JSON для результатов поиска Маркетплейса

## Описание проекта

Данный проект представляет из себя реализацию архитектуры поискогого сервиса с использованием userver фреймворка и технологии API Gateway.

## Функциональность

- Прием Proto-сообщений с результатами поиска по gRPC протоколу
- Преобразование данных в формат JSON с применением бизнес-правил:
  - Форматирование рейтингов в виде звезд
  - Расчет скидок и отображение оригинальных/конечных цен
  - Категоризация предпочтений пользователя
  - Маскирование личных данных пользователя
- Обработка ошибок и логирование
- Интеграция с системой мониторинга и тестирования

## Требования

- C++17 или выше
- CMake 3.12+
- userver фреймворк
- Protocol Buffers 3
- gRPC
- nlohmann/json

## Установка и настройка

### Клонирование репозитория
```bash
git clone https://github.com/Tnirpps/diploma.git
cd diploma
```

### Создание директории для сборки
```bash
mkdir build && cd build
```

### Сборка проекта
```bash
cmake ..
make
```

### Запуск сервиса
```bash
./post_search_renderer -c configs/static_config.yaml
```

## Использование

### Общая схема работы
1. Клиент отправляет Proto-сообщение с данными поиска в сервис через gRPC
2. Сервис обрабатывает сообщение, применяя необходимые трансформации
3. Результат возвращается в формате JSON через gRPC-ответ

### Пример Proto-запроса
```protobuf
message InputMessage {
  optional search.proto.output.Result result = 1;
  optional search.proto.category.Category category = 2;
  repeated search.proto.price.Price price = 3;
  repeated search.proto.price.FavouriteCategory favourite_category = 4;
  optional search.proto.passport.User user = 5;
}
```

### Пример JSON-ответа
```json
{
  "search_results": {
    "total_items": 10,
    "items": [
      {
        "id": "item1",
        "title": "Смартфон XYZ",
        "description": "Новейшая модель смартфона",
        "rating": {
          "value": 4.5,
          "stars": "★★★★★"
        }
      }
    ]
  },
  "category": {
    "id": "electronics",
    "name": "Электроника",
    "path": "/goods/electronics"
  },
  "prices": [
    {
      "amount": 200.0,
      "currency": "RUB",
      "discount_percent": 25.0,
      "discounted_amount": 150.0
    }
  ],
  "user": {
    "id": "3743102_763423000",
    "email": "user@example.com",
    "has_personalization": true
  }
}
```

## Тестирование

Проект включает комплексный набор тестов для проверки различных аспектов сервиса:

### Запуск тестов
```bash
cd build && make test
```

или используя pytest напрямую:

```bash
cd tests && python -m pytest
```

## Инструмент сравнения JSON-результатов

Для анализа различий между результатами рендеринга проект включает специальный инструмент `diff.cpp`, который позволяет:

- Сравнивать два JSON-файла и находить различия
- Игнорировать порядок элементов в массивах
- Применять допуск для числовых значений
- Игнорировать указанные пути или регулярные выражения
- Выводить статистику по найденным отличиям

### Сборка инструмента
```bash
g++ -std=c++17 -o json_diff tools/diff.cpp -lnlohmann_json
```

### Использование
```bash
./json_diff [опции] файл1.json файл2.json
```

### Опции инструмента
```
--ignore-order         Игнорировать порядок элементов в массивах
--ignore-path <путь>   Игнорировать указанный путь (можно использовать шаблоны)
--ignore-regex <regex> Игнорировать пути, соответствующие регулярному выражению
--numeric-tolerance <n> Игнорировать числовые различия меньше указанного значения
--no-color             Отключить цветной вывод
--show-all             Показывать все поля, а не только различия
--max-array-diffs <n>  Максимальное количество различий массива для показа
```