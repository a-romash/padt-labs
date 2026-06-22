# ЛР 3 – GUI (ImGui + SFML)

## Воспроизводимость

Все зависимости собираются из исходников через CMake FetchContent:

| Зависимость | Версия |
|-------------|--------|
| SFML        | 3.0.2  |
| Dear ImGui  | v1.91.9 |
| imgui-sfml  | v3.0   |

## Сборка

```bash
cd gui
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/lab3_gui
```

## Что умеет

Выбор типа элемента (int / double / string / char / Student / Teacher) и все
операции дека: Append, Prepend, PopFirst, PopLast, Get по индексу, InsertAt,
GetSubsequence, GetFirst/GetLast, плюс операнд-дек **B** для Concat (A+B) и
FindSubsequence (поиск B в A). Результаты и ошибки пишутся в лог внизу окна.


## Скриншот:

![img](img.png)