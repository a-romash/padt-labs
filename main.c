#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "polynomial.h"
#include "typeinfocomplex.h"
#include "typeinfodouble.h"

#define MAX_POLYS 16

static Polynomial polys[MAX_POLYS];
static int        used [MAX_POLYS];

static void init_slots() { memset(used, 0, sizeof(used)); }

static int find_free() {
    for (int i = 0; i < MAX_POLYS; i++) if (!used[i]) return i;
    return -1;
}

static int check_idx(int idx) {
    if (idx < 0 || idx >= MAX_POLYS || !used[idx]) {
        printf("  [!] Нет полинома с индексом %d\n", idx);
        return 0;
    }
    return 1;
}

static void print_err(const PolyError e) {
    switch (e) {
        case POLY_ERR_NULLARG:     puts("  [!] Нулевой аргумент");          break;
        case POLY_ERR_COEFF_COUNT: puts("  [!] Кол-во коэффициентов = 0");  break;
        case POLY_ERR_NOMEM:       puts("  [!] Нет памяти");                break;
        case POLY_ERR_SIZE:        puts("  [!] Несовпадение размеров");      break;
        case POLY_ERR_TYPE:        puts("  [!] Разные типы полиномов");      break;
        default:                   printf("  [!] Ошибка %d\n", (int)e);     break;
    }
}

static void list_polys() {
    int any = 0;
    for (int i = 0; i < MAX_POLYS; i++) {
        if (!used[i]) continue;

        any = 1;
        char* s = polynomialToString(&polys[i]);
        const char* tname = (polys[i].type == GetTypeInfoDouble()) ? "double" : "complex";

        printf("  [%d] (%s, deg %zu)  %s\n",
               i, tname, polys[i].coeffCount - 1, s ? s : "???");
        free(s);
    }
    if (!any) puts("  (нет полиномов)");
}

static void* read_double_coeffs(size_t n) {
    const TypeInfo* ti = GetTypeInfoDouble();
    void* arr = malloc(ti->size * n);
    if (!arr) return NULL;

    for (size_t i = 0; i < n; i++) {
        printf("    a[%zu] = ", i);
        double tmp;
        if (scanf("%lf", &tmp) != 1) { free(arr); return NULL; }

        Double* d = NewDouble(tmp);
        if (!d) { free(arr); return NULL; }
        ti->set(d, (char*)arr + i * ti->size);
        free(d);
    }
    return arr;
}

static void* read_complex_coeffs(size_t n) {
    const TypeInfo* ti = GetTypeInfoComplex();
    void* arr = malloc(ti->size * n);
    if (!arr) return NULL;

    for (size_t i = 0; i < n; i++) {
        double re, im;

        printf("    a[%zu] re = ", i); if (scanf("%lf", &re) != 1) { free(arr); return NULL; }
        printf("    a[%zu] im = ", i); if (scanf("%lf", &im) != 1) { free(arr); return NULL; }

        Complex* tmp = NewComplex(re, im);
        if (!tmp) { free(arr); return NULL; }
        ti->set(tmp, (char*)arr + i * ti->size);
        free(tmp);
    }
    return arr;
}

static void cmd_create() {
    int slot = find_free();
    if (slot < 0) { puts("  [!] Лимит полиномов достигнут"); return; }

    printf("  Тип (1=double, 2=complex): ");
    int t; if (scanf("%d", &t) != 1 || (t != 1 && t != 2)) { puts("  [!] Неверный тип"); return; }

    printf("  Степень полинома: ");
    int deg; if (scanf("%d", &deg) != 1 || deg < 0) { puts("  [!] Неверная степень"); return; }

    const TypeInfo* ti = (t == 1) ? GetTypeInfoDouble() : GetTypeInfoComplex();
    PolyError err = newPolynomial((size_t)(deg + 1), ti, &polys[slot]);
    if (err != POLY_OK) { print_err(err); return; }

    size_t n = (size_t)(deg + 1);
    printf("  Введите %zu коэффициент(ов) от a[0] до a[%d]:\n", n, deg);

    void* coeffs = (t == 1) ? read_double_coeffs(n) : read_complex_coeffs(n);
    if (!coeffs) { puts("  [!] Ошибка ввода"); free(polys[slot].coeffs); return; }

    err = fillPolynomial(&polys[slot], coeffs, n);
    free(coeffs);
    if (err != POLY_OK) { print_err(err); return; }

    used[slot] = 1;
    printf("  Создан полином [%d]\n", slot);
}

static void cmd_print() {
    printf("  Индекс полинома: ");
    int idx; if (scanf("%d", &idx) != 1) return;
    if (!check_idx(idx)) return;
    char* s = polynomialToString(&polys[idx]);
    printf("  P[%d] = %s\n", idx, s ? s : "???");
    free(s);
}

static void cmd_sum() {
    printf("  Индекс первого полинома: ");  int a; if (scanf("%d", &a) != 1) return;
    printf("  Индекс второго полинома: "); int b; if (scanf("%d", &b) != 1) return;
    if (!check_idx(a) || !check_idx(b)) return;

    int slot = find_free();
    if (slot < 0) { puts("  [!] Нет свободных слотов"); return; }

    polys[slot].coeffs = NULL;
    PolyError err = sumPolynomial(&polys[a], &polys[b], &polys[slot]);
    if (err != POLY_OK) { print_err(err); return; }

    polys[slot].type = polys[a].type;
    used[slot] = 1;
    printf("  Результат сложения → [%d]\n", slot);
    char* s = polynomialToString(&polys[slot]);
    printf("  P[%d] = %s\n", slot, s ? s : "???");
    free(s);
}

static void cmd_mult() {
    printf("  Индекс первого полинома: ");  int a; if (scanf("%d", &a) != 1) return;
    printf("  Индекс второго полинома: "); int b; if (scanf("%d", &b) != 1) return;
    if (!check_idx(a) || !check_idx(b)) return;

    int slot = find_free();
    if (slot < 0) { puts("  [!] Нет свободных слотов"); return; }

    polys[slot].coeffs = NULL;
    PolyError err = multPolynomial(&polys[a], &polys[b], &polys[slot]);
    if (err != POLY_OK) { print_err(err); return; }

    polys[slot].type = polys[a].type;
    used[slot] = 1;
    printf("  Результат умножения → [%d]\n", slot);
    char* s = polynomialToString(&polys[slot]);
    printf("  P[%d] = %s\n", slot, s ? s : "???");
    free(s);
}

static void cmd_scalar_mult() {
    printf("  Индекс полинома: ");
    int idx; if (scanf("%d", &idx) != 1) return;
    if (!check_idx(idx)) return;

    void* scalar = NULL;
    if (polys[idx].type == GetTypeInfoDouble()) {
        double v; printf("  Скаляр: "); if (scanf("%lf", &v) != 1) return;
        scalar = NewDouble(v);
    } else {
        double re, im;
        printf("  Скаляр re: "); if (scanf("%lf", &re) != 1) return;
        printf("  Скаляр im: "); if (scanf("%lf", &im) != 1) return;
        scalar = NewComplex(re, im);
    }
    if (!scalar) return;

    PolyError err = multPolynomialByScalar(&polys[idx], scalar);
    free(scalar);
    if (err != POLY_OK) { print_err(err); return; }

    char* s = polynomialToString(&polys[idx]);
    printf("  P[%d] = %s\n", idx, s ? s : "???");
    free(s);
}

static void cmd_solve() {
    printf("  Индекс полинома: ");
    int idx; if (scanf("%d", &idx) != 1) return;
    if (!check_idx(idx)) return;

    void* x      = NULL;
    void* result = NULL;

    if (polys[idx].type == GetTypeInfoDouble()) {
        double v;
        printf("  x = ");
        if (scanf("%lf", &v) != 1) return;

        x      = NewDouble(v);
        result = NewDouble(0.0);

    } else {
        double re, im;
        printf("  x re = "); if (scanf("%lf", &re) != 1) return;
        printf("  x im = "); if (scanf("%lf", &im) != 1) return;
        x      = NewComplex(re, im);
        result = NewComplex(0.0, 0.0);
    }
    if (!x || !result) { free(x); free(result); return; }

    PolyError err = computePolynomial(&polys[idx], x, result);
    if (err != POLY_OK) { print_err(err); }
    else {
        char xbuf[128], buf[128];
        polys[idx].type->toString(x,      xbuf, sizeof(xbuf));
        polys[idx].type->toString(result, buf,  sizeof(buf));
        printf("  P(%s) = %s\n", xbuf, buf);
    }
    free(x); free(result);
}

static void cmd_delete() {
    printf("  Индекс полинома: ");
    int idx; if (scanf("%d", &idx) != 1) return;
    if (!check_idx(idx)) return;
    if (polys[idx].coeffs) { free(polys[idx].coeffs); polys[idx].coeffs = NULL; }
    used[idx] = 0;
    printf("  Полином [%d] удалён\n", idx);
}

static void print_menu() {
    puts("\n┌─────────────────────────────────────┐");
    puts("│        Калькулятор полиномов        │");
    puts("├─────────────────────────────────────┤");
    puts("│  1  Создать полином                 │");
    puts("│  2  Показать полином                │");
    puts("│  3  Список всех полиномов           │");
    puts("│  4  Сложить два полинома            │");
    puts("│  5  Умножить два полинома           │");
    puts("│  6  Умножить полином на скаляр      │");
    puts("│  7  Вычислить полином в точке       │");
    puts("│  8  Удалить полином                 │");
    puts("│  0  Выход                           │");
    puts("└─────────────────────────────────────┘");
    printf("  > ");
}
int main() {
    init_slots();

    int choice;

    print_menu();
    while (1) {
        if (scanf("%d", &choice) != 1) { /* очистка мусора в stdin */
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        switch (choice) {
            case 0: puts("  Пока!"); return 0;
            case 1: cmd_create();      break;
            case 2: cmd_print();       break;
            case 3: list_polys();      break;
            case 4: cmd_sum();         break;
            case 5: cmd_mult();        break;
            case 6: cmd_scalar_mult(); break;
            case 7: cmd_solve();       break;
            case 8: cmd_delete();      break;
            default: puts("  [!] Неизвестная команда");
        }

        print_menu();
    }
}