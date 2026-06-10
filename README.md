# LazySequence

Учебный проект на C++, посвящённый ленивым последовательностям.
Работу выполнил Ястребов Владимир Б25-507

## Описание

Проект реализует набор модулей на C++ для работы с ленивыми последовательностями, ординалами и конечными автоматами. Включает класс LazySequence для конечных и бесконечных последовательностей, модели ординальных и кардинальных чисел для описания типов порядка, потоки чтения/записи (файловые и поверх последовательностей), а также конечный автомат турникета с запуском по набору событий.

## Установка

```bash
cd ~
git clone https://github.com/IastrebovVladimir/Lab_4.git
cd PiecewiseFunction
git clone https://github.com/google/googletest.git
```

## Запуск тестов

```bash
g++ -g -O0 -std=c++20 tests/testLazy.cpp tests/testGenerator.cpp tests/testStreams.cpp tests/testMachines.cpp tests/testTransfinite.cpp -I. -isystem googletest/googletest/include -Igoogletest/googletest googletest/googletest/src/gtest-all.cc -pthread -o LazyTests
./LazyTests
```

## Запуск LazySequence

```bash
g++ -g -O0 -std=c++20 menu/main_menu_lazy.cpp Sequence/types.cpp -o Lab_4
./Lab_4
```


```text
Lab4/
├─ FileStream/
│  ├─ FileReadOnlyStream.h      # Поток чтения из файла
│  └─ FileWriteOnlyStream.h     # Поток записи в файл
│
├─ LazySequence/
│  ├─ Generator.h               # Генераторы элементов и операций над ленивой последовательностью
│  └─ LazySequence.h            # Основной класс LazySequence<T>
│
├─ menu/
│  ├─ LazyMenuTraits.h          # Правила построения бесконечных последовательностей для int и double
│  ├─ main_menu_lazy.cpp        # Точка входа консольного меню
│  ├─ menu_lazy.h               # Объявления функций меню
│  └─ menu_lazy.hpp             # Реализация меню
│
├─ Sequence                     # Последовательность (из ЛР2, ArraySequence.h, types.h, types.cpp)
├─ SequenceStream/
│  ├─ SequenceReadOnlyStream.h  # Поток чтения из последовательности
│  └─ SequenceWriteOnlyStream.h # Поток записи в последовательность
│
├─ StateMachine/
│  ├─ MachineRunner.h           # Запуск конечного автомата на потоке событий
│  ├─ StateMachine.h            # Базовый интерфейс конечного автомата
│  └─ TurnstileStateMachine.h   # Автомат турникет
│
├─ Stream/
│  ├─ ReadOnlyStream.h          # Базовый интерфейс потока чтения
│  └─ WriteOnlyStream.h         # Базовый интерфейс потока записи
│
├─ TransfiniteNumbers/
│  ├─ Cardinal.h                # Кардинальные числа
│  └─ Ordinal.h                 # Ординальные числв
│
└─ tests/
   ├─ testGenerator.cpp         # Тесты генераторов
   ├─ testLazy.cpp              # Тесты LazySequence
   ├─ testMachines.cpp          # Тесты конечных автоматов
   ├─ testMain.cpp              # Точка входа тестов GoogleTest
   ├─ testStreams.cpp           # Тесты потоков записи/чтения последовательностей и файлов
   └─ testTransfinite.cpp       # Тесты ординалов
```
