# threadpool

Пул потоков с возможностью указания приоритета выполнения поставленной задачи, позволяющий:
* изменять число рабочих потоков в процессе выполнения;
* ограничивать количество поставленных задач, приводя к ожиданию освобождения места для задачи клиентами пула;
*  останавливаться, дожидаясь завершения всех поставленных задач;
* останавливаться форсированно, не дожидаясь выполнения всех задач.

