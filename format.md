### Формат архива 1.0
 * Сигнатура: ABOBA1
 * Версия: 1
 * Коды алгоритмов сжатия: 2 кода по 2 байта
 * Длина файла: 8 байт
 * Дополнительные данные: 20 байт
  

| Смещение поля (байты) | Размер поля (байты) | Описание |
| -------- | :-------: | --------- |
| 0 | 6 | Сигнатура формата ("ABOBA1") |
| 6 | 2 | Версия формата (1) |
| 8 | 2 | Код алгоритма сжатия |
| 10 | 2 | Код алгоритма коррекции ошибок |
| 12 | 8 | Исходная длина файла в байтах |
| 20 | m | Дополнительная информация |
| 20+m | n | Данные исходного файла |

### Формат архива 2.0
 * Сигнатура: ABOBA1
 * Версия: 2
 * Коды алгоритмов сжатия: 2 кода по 2 байта
 * Длина файла: 8 байт
 * Дополнительные данные: 20 байт
  

| Смещение поля (байты) | Размер поля (байты) | Описание |
| -------- | :-------: | --------- |
| 0 | 6 | Сигнатура формата ("ABOBA1") |
| 6 | 2 | Версия формата (1) |
| 8 | 2 | Код алгоритма сжатия |
| 10 | 2 | Код алгоритма коррекции ошибок |
| 12 | 8 | Кол-во вхождений в архив |
| Для каждого вхождения: |
|  | 2 | Длина пути до сущности |
|  | n | Путь до сущности |
|  | 1 | Флаг директории |
|  | 8 | Размер файла (если файл) |
|  | n | Данные файла |