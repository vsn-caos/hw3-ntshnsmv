#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Программе на стандартный поток ввода задается арифметическое выражение
// в синтаксисе языка python3. Необходимо вычислить это выражение и вывести результат.
// Использовать дополнительные процессы запрещено — нужно использовать exec.

int main(void) {
    // TODO: прочитайте выражение из stdin,
    //       затем вызовите execvp/execlp для запуска python3,
    //       который вычислит и выведет результат.
    //       Подсказка: python3 -c "print(<выражение>)"

    char *expr = NULL;
    size_t capacity = 0;

    ssize_t len = getline(&expr, &capacity, stdin);
    if (len == -1) {
        free(expr);
        return 1;
    }

    while (len > 0 && (expr[len - 1] == '\n' || expr[len - 1] == '\r')) {
        expr[--len] = '\0';
    }

    const char *prefix = "print(";
    const char *suffix = ")";

    size_t code_len = strlen(prefix) + strlen(expr) + strlen(suffix) + 1;
    char *code = malloc(code_len);
    if (code == NULL) {
        free(expr);
        return 1;
    }

    snprintf(code, code_len, "%s%s%s", prefix, expr, suffix);

    execlp("python3", "python3", "-c", code, NULL);

    perror("execlp");
    free(code);
    free(expr);
    return 1;
}
