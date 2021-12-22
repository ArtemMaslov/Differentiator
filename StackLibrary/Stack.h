#ifndef STACK_H_
#define STACK_H_


#include <stdint.h>

//#define STACK_LOGS
//#define STACK_LOG_ERRORS
//#define STACK_CRC
//#define STACK_CANARIES

//#define STACK_CANDECREASE_CAPACITY

#ifdef  STACK_LOGS

extern FILE* stackLogFile;
#else
#define stackLogFile nullptr
#endif //  StackLogs

/**
 * @brief Структура данных стек.
*/
struct Stack
{
#ifdef STACK_CANARIES
    int64_t canaryLeft;    /// Левая канарейка.
#endif // STACK_CANARIES

    size_t  elementSize;   /// Размер одного элемента.
    size_t  stackSize;     /// Текущий размер стека. Количество элементов, находящихся в стеке в данный момент.
    size_t  stackCapacity; /// Вместимость стека, максимальное число элементов, которые может хранить стек.

    void*   data;          /// Массив данных.

#ifdef STACK_CRC
    int64_t dataCRC;       /// Контрольная сумма массива данных.
    int64_t stackCRC;      /// Контрольная сумма структуры стек.
#endif // STACK_CRC
    
#ifdef STACK_CANARIES
    int64_t canaryRight;   /// Правая канарейка.
#endif // STACK_CANARIES
};

/**
 * @brief Массив строк для вывода информации об ошибках стека в логи.
*/
const char StackErrorStrings[][40] =
{
    "No errors",
    "Stack is null",
    "Stack is empty",

    "No memory",
    "Size more capacity",
    "Left canary is broken",

    "Right canary is broken",
    "Data CRC invalide",
    "Stack CRC invalide",

    "Stack isn't inited",
    "Stack element size invalide",
    "Stack data left canary is broken",
    
    "Stack data right canary is broken",
    "Trying to push null value",
};

/**
 * @brief Перечисление ошибок стека. Каждая ошибка представляет определённый бит числа. `0` означает, что ошибок нет.
*/
enum StackError
{
    STACKERR_NO_ERRORS           = 0 << 0,
    STACKERR_PTR_IS_NULL         = 1 << 0,
    STACKERR_STACK_IS_EMPTY      = 1 << 1,

    STACKERR_NO_MEMORY           = 1 << 2,
    STACKERR_SIZE_MORE_CAPACITY  = 1 << 3,
    STACKERR_LEFT_CANARY         = 1 << 4,

    STACKERR_RIGHT_CANARY        = 1 << 5,
    STACKERR_DATA_CRC            = 1 << 6,
    STACKERR_STACK_CRC           = 1 << 7,

    STACKERR_STACK_IS_NOT_INITED = 1 << 8,
    STACKERR_ELEM_SIZE_INVALIDE  = 1 << 9,
    STACKERR_DATA_LEFT_CANARY    = 1 << 10,

    STACKERR_DATA_RIGHT_CANARY   = 1 << 11,
    STACKERR_NULL_VALUE          = 1 << 12,
};

/// @brief Минимальный допустимый размер стека.
/// Стек может содержать массив данных либо нулевой длины, либо не меньшей STACK_MIN_CAPACITY.
const size_t STACK_MIN_CAPACITY = 32;
/// @brief Коэффициент расширения/сужения размера стека. Должен быть больше 1.
/// При увеличении коэффициента, уменьшается количество выделений памяти.
/// При уменьшении коэффициента, стек будет занимать меньше памяти.
/// Вместимость стека будет увеличиваться в STACK_CAPACITY_SCALE_COEFFICIENT раз, если stackSize >= stackCapacity.
/// Вместимость стека будет уменьшаться, если stackSize < newStackCapacity
/// newStackCapacity = min ( oldCapacity * (0.5 - STACK_CAPACITY_DECREASE_COEFFICIENT) ; abs(oldCapacity / 2.0 - STACK_MIN_CAPACITY) )
/// При этом, если в стеке есть элементы, то вместимость стека не может стать меньше STACK_MIN_CAPACITY.
/// Если stackSize == 0, то вместимость стека устанавливается равной 0.
const size_t STACK_CAPACITY_SCALE_COEFFICIENT = 2;
/// @brief Коэффициент уменьшения вместимости стека. Должен быть не меньше 0, не больше 0.5.
/// При увеличении коэффициента, уменьшается количество выделений памяти.
/// При уменьшении коэффициента, стек будет занимать меньше памяти.
const double STACK_CAPACITY_DECREASE_COEFFICIENT = 0.1;
/// @brief Значение левой канарейки для стека и массива данных.
#define STACK_LEFT_CANARY_VALUE 0xBAD0AAAAAAAA0BAD
/// @brief Значение правой канарейки для стека и массива данных.
#define STACK_RIGHT_CANARY_VALUE 0xBADFBBBBBBBBFBAD

/**
 * @brief             Конструктор стека.
 * @param stack       Указатель на стек.
 * @param elementSize Размер одного элемента в массиве data.
 * @param Capacity    Начальная вместимость стека. По умолчанию 0. Если значение больше 0, то под данные будет выделена память.
 * @return            Код ошибки, 0 - в случае успешного завершения.
*/
int   StackConstructor(Stack *stack, const size_t elementSize, const size_t Capacity = 0);

/**
 * @brief       Деструктор стека.
 * @param stack Указатель на стек.
 * @return      Код ошибки, 0 - в случае успешного завершения.
*/
int   StackDestructor(Stack *stack);

/**
 * @brief       Добавляет элемент в конец стека.
 * @param stack Указатель на стек.
 * @param value Указатель на значение, которое нужно добавить в стек.
 * @return      Код ошибки, 0 - в случае успешного завершения.
*/
int   StackPush(Stack *stack, const void *data);

/**
 * @brief       Удаляет элемент из конца стека и возвращает его.
 * @param stack Указатель на стек.
 * @param error Если не nullptr, то этой переменной будут присвоены коды ошибок, которые произошли во время работы функции.
 * @return      Значение удалённого из стека элемента, nullptr - в случае ошибки.
*/
void* StackPop(Stack *stack, int *error = nullptr);

/**
 * @brief       Проверяет стек на наличии ошибок.
 * @param stack Указатель на стек.
 * @return      Возвращает коды ошибок текущего состояния стека.
*/
int   ValidateStack(const Stack *stack);

void* StackGetElemAt(const Stack* stack, const size_t index);

//#define StackDump(stack, file, programm_function_name, programm_file, programm_line) \
//StackDump_(stack, file, #stack, __FUNCTION__, __FILE__, __LINE__, programm_function_name, programm_file, programm_line);

/**
 * @brief       Выводит детальную информацию о текущем состоянии стека.
 * @param stack Указатель на стек.
 * @param file  Указатель на поток вывода
*/
#define StackDump(stack, file) \
StackDump_(stack, file, #stack, __FUNCTION__, __FILE__, __LINE__, "", "", -1);

/**
 * @brief                        Внутренняя функция. Используйте StackDump взамен.
 * @param stack                  Указатель на стек.
 * @param file                   Указатель на поток вывода.
 * @param stack_variable_name    Имя переменной в библиотеке StackLibrary.
 * @param stack_function_name    Имя функции в библиотеке StackLibrary.
 * @param stack_file             Файл библиотеки StackLibrary.
 * @param stack_line             Номер строки кода в библиотеке StackLibrary.
 * @param programm_function_name Имя функции в программе, использующей StackLibrary.
 * @param programm_file          Имя файла в программе, использующей StackLibrary.
 * @param programm_line          Номер строки кода в программе, использующей StackLibrary.
*/
void StackDump_(const Stack *stack, FILE *file,
    const char *stack_variable_name,
    const char *stack_function_name,
    const char *stack_file,
    const int   stack_line,
    const char *programm_function_name,
    const char *programm_file,
    const int   programm_line);

/**
 * @brief      Конструктор файла логов стека.
 * @param file Указатель на поток вывода.
*/
void StackLogConstructor(FILE* file);


#endif